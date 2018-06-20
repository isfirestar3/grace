#include "nsp.h"
#include "nis.h"
#include "tst.h"
#include "proto.h"
#include "xml_loaded.h"
#include "vehicle.h"
#include "var.h"
#include "avltree.h"
#include "navigation.h"
#include "logger.h"
#include "hash.h"
#include "clist.h"
#include "error.h"
#include "posix_wait.h"
#include "posix_naos.h"
#include "posix_atomic.h"
#include "args.h"
#include "elmo.h"
#include "vartypes.h"
#include "moons.h"
#include "regist_event.h"
#include "posix_string.h"

extern
int encrypt(const unsigned char* ori, int oricb, const unsigned char* key, int keycb, unsigned char** out, int* outcb);

#define NSP_ACCESS_MASK_READ   (1)
#define NSP_ACCESS_MASK_WRITE   (2)
#define NSP_ACCESS_MASK_ACCEPT   (4)

#pragma pack(push, 1)

typedef struct {
    struct avltree_node_t index_;
    HTCPLINK link_;
    enum nsp__controlor_type_t link_type_;
    char local_ipv4_[16];
    uint16_t local_port_;
    uint32_t remote_ipv4_;
    uint16_t remote_port_;
    int access_;
    tst_t template_;
    struct list_head lst_connected_;
    struct avltree_node_t *index_client_bylink_;
    posix__pthread_mutex_t locker_;
    int keepalive_join_; // 心跳线程退出
    posix__waitable_handle_t keepalive_sync_; // 心跳包检查线程的同步/超时事件
    posix__pthread_t keepalive_th_;
    int keepalive_chkcnt_; // 心跳检测计数, 超过 NSP_MAXIMUM_KEEPALIVE_CHECKING_COUNT 即被踢
    unsigned char key_[PROTO_SESSION_KEY_LENGTH]; // 链接的安全密钥
} nsp__tcp_object_t;

typedef struct {
	HUDPLINK link_;
	char local_ipv4_[16];
	uint16_t local_port_;
	tst_t template_;
} nsp__udp_object_t;

#pragma pack(pop)

static nsp__tcp_object_t *__tcp_server = NULL;
static nsp__udp_object_t *__udp_server = NULL;

static
int __session_timeout_tick = 0;
static
int __session_timeout_times = 0;
static
int __atomic_pktid = ATOMIC_PKTID;


static
int nspi__loaded_handler(xmlNodePtr element) {

    xmlNodePtr node;
    xmlAttrPtr attr;

    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"local", element->name)) {
            node = element->xmlChildrenNode;
            while (node) {
                if (0 == xmlStrcmp(BAD_CAST"tcp", node->name) && __tcp_server) {
                    attr = node->properties;
                    while (attr) {
                        if (attr->type == XML_ATTRIBUTE_NODE) {
                            /* 配置信息不能覆盖启动参数的设置 */
                            if ((0 == xmlStrcmp(BAD_CAST"ipv4", attr->name)) && (0 == strlen(__tcp_server->local_ipv4_))) {
                                covert_first_text_value_s(attr->xmlChildrenNode, __tcp_server->local_ipv4_, cchof(__tcp_server->local_ipv4_));
                            } else if ((0 == xmlStrcmp(BAD_CAST"port", attr->name)) && (0 == __tcp_server->local_port_)) {
                                __tcp_server->local_port_ = covert_first_text_value_i(attr->xmlChildrenNode);
                            }
                        }

                        attr = attr->next;
                    }
                }
				if (0 == xmlStrcmp(BAD_CAST"udp", node->name) && __udp_server) {
					attr = node->properties;
					while (attr) {
						if (attr->type == XML_ATTRIBUTE_NODE) {
							/* 配置信息不能覆盖启动参数的设置 */
							if ((0 == xmlStrcmp(BAD_CAST"ipv4", attr->name)) && (0 == strlen(__udp_server->local_ipv4_))) {
								covert_first_text_value_s(attr->xmlChildrenNode, __udp_server->local_ipv4_, cchof(__udp_server->local_ipv4_));
							} else if ((0 == xmlStrcmp(BAD_CAST"port", attr->name)) && (0 == __udp_server->local_port_)) {
								__udp_server->local_port_ = covert_first_text_value_i(attr->xmlChildrenNode);
							}
						}

						attr = attr->next;
					}
				}

                node = node->next;
            }
        }

        if (0 == xmlStrcmp(BAD_CAST"timeout", element->name)) {
            attr = element->properties;
            while (attr) {
                if (attr->type == XML_ATTRIBUTE_NODE) {
                    if (0 == xmlStrcmp(BAD_CAST"tick", attr->name)) {
                        __session_timeout_tick = covert_first_text_value_i(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(BAD_CAST"times", attr->name)) {
                        __session_timeout_times = covert_first_text_value_i(attr->xmlChildrenNode);
                    }
                }
                attr = attr->next;
            }
        }

        element = element->next;
    }
	if (0 == strlen(__udp_server->local_ipv4_)) {
		// 如果xml中没配置UDP的地址和端口，那么和TCP保持一致 
		posix__strcpy(__udp_server->local_ipv4_, posix__ipv4_length, __tcp_server->local_ipv4_);
		__udp_server->local_port_ = __tcp_server->local_port_;
	}
    return 0;
}

static
int nspi__compare_bylink(const void *left, const void *right) {
    nsp__tcp_object_t *pleft = (nsp__tcp_object_t *) left;
    nsp__tcp_object_t *pright = (nsp__tcp_object_t *) right;

    if (pleft->link_ < pright->link_) {
        return 1;
    } else {
        if (pleft->link_ > pright->link_) {
            return -1;
        }
    }

    return 0;
}

static
int nspi__on_tcp_accepted(HTCPLINK accepted) {
    nsp__tcp_object_t *client;
    int i;
    nsp__pre_login_t pkt_pre_login;

    client = (nsp__tcp_object_t *) malloc(sizeof ( nsp__tcp_object_t));
    if (!client) {
        return -1;
    }
    memset(client, 0, sizeof ( nsp__tcp_object_t));

    client->link_ = accepted;
    tcp_getaddr(accepted, LINK_ADDR_REMOTE, &client->remote_ipv4_, &client->remote_port_);

    // tst
    client->template_.builder_ = __tcp_server->template_.builder_;
    client->template_.parser_ = __tcp_server->template_.parser_;
    client->template_.cb_ = __tcp_server->template_.cb_;
    tcp_settst(accepted, &client->template_);

    // 生成属于这个链接的随机密钥
    for (i = 0; i < PROTO_SESSION_KEY_LENGTH; i++) {
        client->key_[i] = rand() % 0xFE;
    }

    /* 链接后的信息插入服务管理链表和索引表 */
    posix__pthread_mutex_lock(&__tcp_server->locker_);
    list_add_tail(&client->lst_connected_, &__tcp_server->lst_connected_);
    __tcp_server->index_client_bylink_ = avlinsert(__tcp_server->index_client_bylink_, &client->index_, &nspi__compare_bylink);
    posix__pthread_mutex_unlock(&__tcp_server->locker_);
	
	log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "tcp link 0x%08X accepted.", accepted);

    /* 将密钥信息通过 pre login 投递给连接客户端 */
    if (run__if_must_login()) {
        memset(&pkt_pre_login, 0, sizeof ( pkt_pre_login));
        pkt_pre_login.head_.type_ = PKTTYPE_PRE_LOGIN;
        pkt_pre_login.head_.size_ = sizeof ( nsp__pre_login_t);
        pkt_pre_login.size_ = sizeof ( pkt_pre_login.key_);
        memcpy(pkt_pre_login.key_, client->key_, sizeof ( pkt_pre_login.key_));
        return tcp_write(accepted, pkt_pre_login.head_.size_, &nsp__packet_maker, &pkt_pre_login);
    }

    return 0;
}

static
void nspi__on_tcp_closed(HTCPLINK link) {
    nsp__tcp_object_t target, *removed;
    var__error_handler_t *err;

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "tcp link 0x%08X has been closed.", link);
	
	nspi__unregist_cycle_event(link, NULL, 0);

    posix__pthread_mutex_lock(&__tcp_server->locker_);
    if (link == __tcp_server->link_) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "tcp service link has been destoryed");

        err = var__get_error_handler();
        if (err) {
            if (err->tcp_master_object_destoryed_as_fatal_) {
                var__mark_framwork_error(kVarFixedObject_MotionTemplateFramwork, var__make_error_code(kVarType_MotionTemplateFramwork, kFramworkFatal_TcpMasterServicesHalted));
            }
            var__release_object_reference(err);
        }
    } else {
        target.link_ = link;
        removed = NULL;
        __tcp_server->index_client_bylink_ = avlremove(
                __tcp_server->index_client_bylink_, &target.index_, (struct avltree_node_t **) &removed, &nspi__compare_bylink);
        if (removed) {

            /* 从服务端的链路中删除当前节点 */
            if (!list_empty(&removed->lst_connected_)) {
                list_del(&removed->lst_connected_);
                INIT_LIST_HEAD(&removed->lst_connected_);
            }

            /* 如果遥控身份离线，则应该将将控制模式置为导航， 并且合速度设0 */
            if (kControlorType_Manual & removed->link_type_) {
                var__vehicle_t *veh = var__get_vehicle();
				if (veh) {
					veh->control_mode_ = kVehicleControlMode_Navigation;
					veh->manual_velocity_.x_ = veh->manual_velocity_.y_ = veh->manual_velocity_.w_ = 0.0;
					var__release_object_reference(veh);
				}
            }

            char ipstr[16];
            posix__ipv4tos(removed->remote_ipv4_, ipstr, cchof(ipstr));
            log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, 
					"tcp object %s:%u has been closed.", ipstr, removed->remote_port_);
            free(removed);
        }
    }
    posix__pthread_mutex_unlock(&__tcp_server->locker_);
}

static
void STD_CALL nspi__tcp_callback(const nis_event_t *naio_event, const void *pParam2) {
	int retval;
    tcp_data_t *tcp_data = (tcp_data_t *) pParam2;

	retval = 0;
    switch (naio_event->Event) {
        case EVT_TCP_ACCEPTED:
			retval = nspi__on_tcp_accepted(tcp_data->e.Accept.AcceptLink);
            break;
        case EVT_RECEIVEDATA:
			retval = nsp__on_tcp_recvdata(naio_event->Ln.Tcp.Link, tcp_data->e.Packet.Data, tcp_data->e.Packet.Size);
            break;
        case EVT_CLOSED:
            nspi__on_tcp_closed(tcp_data->e.LinkOption.OptionLink);
            break;
        case EVT_DEBUG_LOG:
            log__save("nshost", kLogLevel_Info, kLogTarget_Filesystem, "%s", tcp_data->e.DebugLog.logstr);
            break;
        default:
            break;
    }
	
	if ( retval < 0) {
		tcp_destroy(naio_event->Ln.Tcp.Link);
	}
}

// UDP 链接对象的回调处理函数
static void STD_CALL nspi__udp_callback(const nis_event_t *naio_event, const void *pParam2) {
	udp_data_t *udp_data = (udp_data_t *)pParam2;

	switch (naio_event->Event) {
		case EVT_RECEIVEDATA:
			nsp__on_udp_recvdata(naio_event->Ln.Udp.Link, udp_data->e.Packet.RemoteAddress, udp_data->e.Packet.RemotePort, \
				 udp_data->e.Packet.Data, udp_data->e.Packet.Size);
			break;
		default:
			break;
	}
}

int STD_CALL nsp__packet_maker(void *data, int cb, void *context) {
    if (data && cb > 0 && context) {
        memcpy(data, context, cb);
        return 0;
    }
    return -1;
}

void nsp__refurbish_keepalive_status(HTCPLINK lnk) {
    nsp__tcp_object_t node, *object;
    struct avltree_node_t *target;

    node.link_ = lnk;

    posix__pthread_mutex_lock(&__tcp_server->locker_);
    target = avlsearch(__tcp_server->index_client_bylink_, &node.index_, &nspi__compare_bylink);
    if (target) {
        object = (nsp__tcp_object_t *) target;
        object->keepalive_chkcnt_ = 0;
    }
    posix__pthread_mutex_unlock(&__tcp_server->locker_);
}

static
void *nspi__keepalive_checking_routine(void *parameter) {
    struct list_head *pos, *cursor;
    nsp__tcp_object_t *object;

    while (__tcp_server->keepalive_join_ < 0) {
        if (posix__waitfor_waitable_handle(&__tcp_server->keepalive_sync_, __session_timeout_tick) < 0) {
            break;
        }

        posix__pthread_mutex_lock(&__tcp_server->locker_);

        list_for_each_safe(pos, cursor, &__tcp_server->lst_connected_) {
            object = containing_record(pos, nsp__tcp_object_t, lst_connected_);
            if (object->keepalive_chkcnt_++ > __session_timeout_times) {

                log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout,
							"client link 0x%08X alive timeout checking failed.this connection will be destory.", object->link_);
                list_del(pos);
                INIT_LIST_HEAD(pos);

                // on_closed 阶段可以处理链表/搜索树/内存 的销毁
                tcp_destroy(object->link_);
            }
        }
        posix__pthread_mutex_unlock(&__tcp_server->locker_);
    }

    return NULL;
}

static
int nspi__init_tcp_server_object() {
    if (__tcp_server) {
        return -1;
    }

    __tcp_server = (nsp__tcp_object_t *) malloc(sizeof ( nsp__tcp_object_t));
    if (!__tcp_server) {
        return -1;
    }

    memset(__tcp_server, 0, sizeof ( nsp__tcp_object_t));
    __tcp_server->link_ = INVALID_HTCPLINK;
    __tcp_server->access_ = NSP_ACCESS_MASK_ACCEPT;

    /* 优先从启动参数中获取地址信息 */
    run__getarg_tcphost(__tcp_server->local_ipv4_);
    __tcp_server->local_port_ = run__getarg_tcpport();

	if (posix__init_synchronous_waitable_handle(&__tcp_server->keepalive_sync_) < 0) {
		return -1;
	}
	
	posix__pthread_mutex_init(&__tcp_server->locker_);
    INIT_LIST_HEAD(&__tcp_server->lst_connected_);

    return 0;
}

static
void nspi__uninit_tcp_server_object() {
    struct list_head *pos, *cursor;
    nsp__tcp_object_t *object;

    if (!__tcp_server) {
        return;
    }

    /* 中止心跳监视线程 */
    __tcp_server->keepalive_join_ = 1;
    posix__pthread_join(&__tcp_server->keepalive_th_, NULL);
    posix__uninit_waitable_handle(&__tcp_server->keepalive_sync_);

    /* 关闭服务 */
    if (INVALID_HTCPLINK != __tcp_server->link_) {
        tcp_destroy(__tcp_server->link_);
        __tcp_server->link_ = INVALID_HTCPLINK;
    }

    /* 释放所有已经连接的对象 */
    posix__pthread_mutex_lock(&__tcp_server->locker_);

    list_for_each_safe(pos, cursor, &__tcp_server->lst_connected_) {
        object = containing_record(pos, nsp__tcp_object_t, lst_connected_);

        // 先删除链表节点，并后移循环节
        list_del(pos);
        INIT_LIST_HEAD(pos);

        // on_closed 阶段可以处理链表/搜索树/内存 的销毁
        tcp_destroy(object->link_);
    }
    INIT_LIST_HEAD(&__tcp_server->lst_connected_);
    posix__pthread_mutex_unlock(&__tcp_server->locker_);

    /* 释放服务安全锁 */
    posix__pthread_mutex_release(&__tcp_server->locker_);

    /* 释放服务对象 */
    free(__tcp_server);
    __tcp_server = NULL;
}

static
int nspi__init_udp_server_object() {
	if (__udp_server) {
		return -1;
	}

	__udp_server = (nsp__udp_object_t *)malloc(sizeof (nsp__udp_object_t));
	if (!__udp_server) {
		return -1;
	}

	memset(__udp_server, 0, sizeof (nsp__udp_object_t));
	__udp_server->link_ = INVALID_HUDPLINK;

	/* 优先从启动参数中获取地址信息 */
	run__getarg_tcphost(__udp_server->local_ipv4_);
	__udp_server->local_port_ = run__getarg_udpport();

    return 0;
}

static
void nspi__uninit_udp_server_object() {
	if (!__udp_server) {
		return;
	}
	/* 关闭服务 */
	if (INVALID_HUDPLINK != __udp_server->link_) {
		udp_destroy(__udp_server->link_);
		__udp_server->link_ = INVALID_HUDPLINK;
	}

	/* 释放服务对象 */
	free(__udp_server);
	__udp_server = NULL;
}

void nsp__uninit_network() {
    /* 反初始化服务对象 */
    nspi__uninit_tcp_server_object();
    nspi__uninit_udp_server_object();
    tcp_uninit();
    udp_uninit();
}

int nsp__init_network() {
    if ((nspi__init_tcp_server_object() < 0) || (nspi__init_udp_server_object() < 0)) {
        nsp__uninit_network();
        return -1;
    }

    if (xml__read_data(NULL, "nsp.xml", "nsp", nspi__loaded_handler) < 0) {
        return -1;
    }

    // 创建 TCP 链接对象
    __tcp_server->link_ = tcp_create(&nspi__tcp_callback, __tcp_server->local_ipv4_, __tcp_server->local_port_);
    if (INVALID_HTCPLINK == __tcp_server->link_) {
        return -1;
    }

    // 指定 TCP 下层协议模板
    __tcp_server->template_.parser_ = &nsp__tst_parser;
    __tcp_server->template_.builder_ = &nsp__tst_builder;
    __tcp_server->template_.cb_ = sizeof ( nsp__tst_head_t);
    tcp_settst(__tcp_server->link_, &__tcp_server->template_);

    // 启动 TCP 监听
    if (tcp_listen(__tcp_server->link_, 5) < 0) {
        tcp_destroy(__tcp_server->link_);
        __tcp_server->link_ = INVALID_HTCPLINK;
        return -1;
    }

    // 启动 TCP 对客户端的心跳校验常驻线程
    if (__session_timeout_tick > 0 && __session_timeout_times > 0) {
        __tcp_server->keepalive_join_ = -1;
        posix__pthread_create(&__tcp_server->keepalive_th_, &nspi__keepalive_checking_routine, NULL);
    }

	// 创建 UDP 链接对象
	__udp_server->link_ = udp_create(&nspi__udp_callback, __udp_server->local_ipv4_, __udp_server->local_port_, 0);
	if (INVALID_HTCPLINK == __udp_server->link_) {
		return -1;
	}

	// 指定 UDP 下层协议模板
	__udp_server->template_.parser_ = &nsp__tst_parser;
	__udp_server->template_.builder_ = &nsp__tst_builder;
	__udp_server->template_.cb_ = sizeof (nsp__tst_head_t);
	tcp_settst(__udp_server->link_, &__udp_server->template_);

	return 0;
}

// 设置链接类型， 登入阶段发生
int nsp__on_login(HTCPLINK link,
        enum nsp__controlor_type_t type,
        int access,
        const unsigned char *login_buffer,
        int login_buffer_size,
        const unsigned char *login_md5) {
    struct avltree_node_t *found;
    nsp__tcp_object_t node;
    int retval = -1;
    char ipstr[16];
    unsigned char *out;
    int outcb;
    MD5_CTX md5ctx;
    unsigned char digest[16];
    struct list_head *loop;
    nsp__tcp_object_t *check;

    if (type > kControlorType_MaximumFunctions || type < kControlorType_Unknown || !login_buffer || !login_md5) {
        return -EINVAL;
    }

    node.link_ = link;

    posix__pthread_mutex_lock(&__tcp_server->locker_);

    /* 不允许同时存在两个遥控或者Customer身份的终端 */
    if (type & kControlorType_Manual || type & kControlorType_Customer) {
        list_for_each(loop, &__tcp_server->lst_connected_) {
            check = containing_record(loop, nsp__tcp_object_t, lst_connected_);
            if (check->link_type_ & kControlorType_Manual || check->link_type_ & kControlorType_Customer) {
                posix__pthread_mutex_unlock(&__tcp_server->locker_);
                return -EEXIST;
            }
        }
    }

    found = avlsearch(__tcp_server->index_client_bylink_, &node.index_, &nspi__compare_bylink);
    if (found) {
        nsp__tcp_object_t *tcp_object = (nsp__tcp_object_t *) found;

        // secure check by encrypt
        if (encrypt(login_buffer, login_buffer_size, tcp_object->key_, sizeof ( tcp_object->key_), &out, &outcb) >= 0) {
            MD5__Init(&md5ctx);
            MD5__Update(&md5ctx, out, outcb);
            MD5__Final(&md5ctx, digest);
            if (0 != (retval = memcmp(digest, login_md5, sizeof ( digest)))) {
                log__save("motion_template", kLogLevel_Warning, kLogTarget_Filesystem | kLogTarget_Stdout,
					"0x%08X from %s:%u access decline.", type, posix__ipv4tos(tcp_object->remote_ipv4_, ipstr, cchof(ipstr)), tcp_object->remote_port_);
                retval = -EFAULT;
            }
            free(out);
        }

        if (retval >= 0) {

            // type of this link
            tcp_object->link_type_ = type;

            // creat access
            tcp_object->access_ = NSP_ACCESS_MASK_READ | NSP_ACCESS_MASK_WRITE;

            // hint of login information
            log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
					"0x%08X from %s:%u login successful.link=0x%08X",
                    type, posix__ipv4tos(tcp_object->remote_ipv4_, ipstr, cchof(ipstr)), tcp_object->remote_port_, link);
        }
    }
    posix__pthread_mutex_unlock(&__tcp_server->locker_);

    return retval;
}

enum nsp__controlor_type_t nsp__get_link_type(HTCPLINK link) {
    struct avltree_node_t *found;
    nsp__tcp_object_t node;

    enum nsp__controlor_type_t type = kControlorType_Unknown;

    node.link_ = link;

    posix__pthread_mutex_lock(&__tcp_server->locker_);
    found = avlsearch(__tcp_server->index_client_bylink_, &node.index_, &nspi__compare_bylink);
    if (found) {
        nsp__tcp_object_t *tcp_object = (nsp__tcp_object_t *) found;
        type = tcp_object->link_type_;
    } else {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "link 0x%08X no found.", link);
    }
    posix__pthread_mutex_unlock(&__tcp_server->locker_);

    return type;
}

int nsp__check_link_access_tcp(HTCPLINK link) {
    struct avltree_node_t *found;
    nsp__tcp_object_t node;
    int retval = -1;

    node.link_ = link;

    posix__pthread_mutex_lock(&__tcp_server->locker_);
    found = avlsearch(__tcp_server->index_client_bylink_, &node.index_, &nspi__compare_bylink);
    if (found) {
        nsp__tcp_object_t *tcp_object = (nsp__tcp_object_t *) found;
        retval = tcp_object->access_;
        if (retval >= 0) {
            tcp_object->keepalive_chkcnt_ = 0;
        }
    } else {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "link 0x%08X no found.", link);
    }
    posix__pthread_mutex_unlock(&__tcp_server->locker_);

    return retval;
}

int nsp__report_status(uint64_t task_id, int vartype, enum status_describe_t status, const void *usrdat, int usrdatcb) {
    struct list_head *pos, *cursor;
    nsp__tcp_object_t *object;
    nsp__common_report_t *pkt_common_report;
    int retval = 0;

    if (!__tcp_server) {
        return -1;
    }

    pkt_common_report = (nsp__common_report_t *) malloc(sizeof ( nsp__common_report_t) + usrdatcb);
    if (!pkt_common_report) {
        return -1;
    }
    memset(&pkt_common_report->head_, 0, sizeof ( pkt_common_report->head_));
    pkt_common_report->head_.id_ = (uint32_t) posix__atomic_inc((volatile long *) &__atomic_pktid);
    pkt_common_report->head_.type_ = PKTTYPE_COMMON_STATUS_REPORT;
    pkt_common_report->head_.size_ = sizeof ( nsp__common_report_t) + usrdatcb;

    pkt_common_report->task_id_ = task_id;
    pkt_common_report->vartype_ = vartype;
    pkt_common_report->status_ = (int) status;
    pkt_common_report->usrcb_ = usrdatcb;
    memcpy(&pkt_common_report->usrdat_, usrdat, usrdatcb);

    // 向所有符合指定身份的连接发送报告
    posix__pthread_mutex_lock(&__tcp_server->locker_);

    list_for_each_safe(pos, cursor, &__tcp_server->lst_connected_) {
        object = containing_record(pos, nsp__tcp_object_t, lst_connected_);
        do {
            if (INVALID_HTCPLINK != object->link_) {
                if (tcp_write(object->link_, pkt_common_report->head_.size_, &nsp__packet_maker, pkt_common_report) >= 0) {
                    ++retval;
                    break;
                }
            }

            // 无效(且还未来得及删除)的节点
            list_del(pos);
            INIT_LIST_HEAD(pos);
            tcp_destroy(object->link_);
        } while (0);
    }
    posix__pthread_mutex_unlock(&__tcp_server->locker_);

    free(pkt_common_report);
    return retval;
}

int nsp__write_log(enum nsp__controlor_type_t identity, const char *logstr, uint32_t length) {
    struct list_head *pos, *cursor;
    nsp__tcp_object_t *object;
    nsp__report_dbglog_t *pkt_report_dbglog;
    int retval = 0;

    if (!__tcp_server || !logstr || 0 == length || length >= 2048) {
        return -1;
    }

    pkt_report_dbglog = (nsp__report_dbglog_t *) malloc(sizeof ( nsp__report_dbglog_t) + length);
    if (!pkt_report_dbglog) {
        return -1;
    }
    memset(&pkt_report_dbglog->head_, 0, sizeof ( pkt_report_dbglog->head_));
    pkt_report_dbglog->head_.id_ = (uint32_t) posix__atomic_inc((volatile long *) &__atomic_pktid);
    pkt_report_dbglog->head_.type_ = PKTTYPE_DBG_LOG_REPORT;
    pkt_report_dbglog->length_ = length;
    memcpy(pkt_report_dbglog->logstr_, logstr, length);

    // 向所有符合指定身份的连接发送报告
    posix__pthread_mutex_lock(&__tcp_server->locker_);

    list_for_each_safe(pos, cursor, &__tcp_server->lst_connected_) {
        object = containing_record(pos, nsp__tcp_object_t, lst_connected_);

        do {
            if (!(object->link_type_ & identity)) {
                break;
            }

            if (INVALID_HTCPLINK != object->link_) {
                if (tcp_write(object->link_, pkt_report_dbglog->head_.size_, &nsp__packet_maker, pkt_report_dbglog) >= 0) {
                    ++retval;
                    break;
                }
            }

            // 无效(且还未来得及删除)的节点
            list_del(pos);
            INIT_LIST_HEAD(pos);
            tcp_destroy(object->link_);
        } while (0);
    }
    posix__pthread_mutex_unlock(&__tcp_server->locker_);
    free(pkt_report_dbglog);
    return retval;
}

static
int canio_elmo_msg(HTCPLINK link, var__functional_object_t *obj) {
    int retval;
    if (INVALID_HTCPLINK == link) {
        return -1;
    }

    var__elmo_t *ptr_elmo = var__object_body_ptr(var__elmo_t, obj);

    struct nsp__canio_msg *pkt_msg = (struct nsp__canio_msg*) malloc(sizeof (struct nsp__canio_msg));
    if (!pkt_msg) {
        return -1;
    }
    memset(&pkt_msg->head_, 0, sizeof (struct nsp__canio_msg));
    pkt_msg->head_.size_ = sizeof (struct nsp__canio_msg);
    pkt_msg->head_.id_ = (uint32_t) posix__atomic_inc((volatile long *) &__atomic_pktid);
    pkt_msg->head_.type_ = PKTTYPE_CANIO_MSG;

    pkt_msg->id = obj->object_id_;
    pkt_msg->var_type_ = obj->type_;
    pkt_msg->command_ = ptr_elmo->status_.command_;
    pkt_msg->middle_ = ptr_elmo->status_.middle_;
    pkt_msg->response_ = ptr_elmo->status_.response_;

    retval = tcp_write(link, pkt_msg->head_.size_, &nsp__packet_maker, pkt_msg);
    free(pkt_msg);
    return retval;
}

static
int canio_moons_msg(HTCPLINK link, var__functional_object_t *obj) {
    int retval;

    if (INVALID_HTCPLINK == link) {
        return -1;
    }
    var__moos_t *ptr_canbus = var__object_body_ptr(var__moos_t, obj);

    struct nsp__canio_msg *pkt_msg = (struct nsp__canio_msg*) malloc(sizeof (struct nsp__canio_msg));
    if (!pkt_msg) {
        return -1;
    }
    memset(&pkt_msg->head_, 0, sizeof (struct nsp__canio_msg));
    pkt_msg->head_.size_ = sizeof (struct nsp__canio_msg);
    pkt_msg->head_.id_ = (uint32_t) posix__atomic_inc((volatile long *) &__atomic_pktid);
    pkt_msg->head_.type_ = PKTTYPE_CANIO_MSG;

    pkt_msg->id = obj->object_id_;
    pkt_msg->var_type_ = obj->type_;
    pkt_msg->command_ = ptr_canbus->status_.command_;
    pkt_msg->middle_ = ptr_canbus->status_.middle_;
    pkt_msg->response_ = ptr_canbus->status_.response_;

    retval = tcp_write(link, pkt_msg->head_.size_, &nsp__packet_maker, pkt_msg);
    free(pkt_msg);
    return retval;
}

void nsp__canio_msg(void *functional_object) {
    struct list_head *pos, *cursor;
    nsp__tcp_object_t *tcp_object;
    var__functional_object_t *obj;

    if (NULL == functional_object) {
        return;
    }
    obj = (var__functional_object_t *) functional_object;

    posix__pthread_mutex_lock(&__tcp_server->locker_);

    list_for_each_safe(pos, cursor, &__tcp_server->lst_connected_) {
        tcp_object = containing_record(pos, nsp__tcp_object_t, lst_connected_);
        do {
            if (kControlorType_Customer != tcp_object->link_type_) {
                continue;
            }

            switch (obj->type_) {
                case kVarType_Elmo:
                    canio_elmo_msg(tcp_object->link_, obj);
                    break;
                case kVarType_Moons:
                    canio_moons_msg(tcp_object->link_, obj);
                    break;
                default:
                    break;
            }
        } while (0);
    }
    posix__pthread_mutex_unlock(&__tcp_server->locker_);
}
