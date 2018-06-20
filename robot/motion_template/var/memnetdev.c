#include "memnetdev.h"
#include "object.h"
#include "var.h"
#include "xml_loaded.h"
#include "logger.h"
#include "netio.h"

#include "posix_string.h"

static
struct list_head __local;

static
void fill_endpoint_properties(xmlNode *node, char *ipv4, uint16_t *port) {

    xmlAttr *attr;
    const char *sz;

    if (!node) return;

    attr = node->properties;
    if (!attr) return;

    while (attr) {
        if (0 == xmlStrcmp(attr->name, BAD_CAST"ipv4")) {
            sz = get_first_text_properties(attr);
            if (sz && ipv4) {
                posix__strcpy(ipv4, MAXIMUM_IPV4STR, sz);
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"port")) {
            sz = get_first_text_properties(attr);
            if (sz && port) {
                *port = atoi(sz);
            }
        }
        attr = attr->next;
    }
}

static
var__memory_data_block *vari__get_block_by_properties(xmlNode *node, var__memory_netdev_t *device, int *current_block_index_r, int *current_block_index_w) {
    xmlAttr *attr;
    const char *sz;
    var__memory_data_block *block;

    block = NULL;

    if (!node || !current_block_index_r || !current_block_index_w) {
        return NULL;
    }

    attr = node->properties;
    if (!attr) {
        return NULL;
    }

    /* 必须读到 type 属性， 用于标明该片区是只读片区还是可写片区 */
    while (attr) {
        if (0 == xmlStrcmp(attr->name, BAD_CAST"type")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                if (0 == posix__strcasecmp("r", sz)) {
                    if (*current_block_index_r < MEM_MAXIMUM_BLOCK) {
                        block = &device->readonly_blocks[*current_block_index_r];
                        (*current_block_index_r)++;
                    }
                } else if (0 == posix__strcasecmp("rw", sz)) {
                    if (*current_block_index_w < MEM_MAXIMUM_BLOCK) {
                        block = &device->writeable_blocks_[*current_block_index_w];
                        (*current_block_index_w)++;
                    }
                }
            }
        }
        attr = attr->next;
    }

    return block;
}

static
int vari__memory_netdev_loaded_blocks(xmlNodePtr element, var__memory_netdev_t *device) {

    xmlNodePtr node;
    var__memory_data_block *block;
    char target[16];
    int current_block_index_r;
    int current_block_index_w;

    if (!device || !element) {
        return -1;
    }

    current_block_index_r = 0;
    current_block_index_w = 0;

    while (element) {
        block = NULL;

        if (0 == xmlStrcmp(BAD_CAST"block", element->name)) {
            block = vari__get_block_by_properties(element, device, &current_block_index_r, &current_block_index_w);
            if (block) {
                node = element->xmlChildrenNode;
                while (node) {
                    if (0 == xmlStrcmp(BAD_CAST"start_address", node->name)) {
                        covert_first_text_value_s(node, target, cchof(target));
                        block->start_address_ = strtoul(target, NULL, 16);
                    } else if (0 == xmlStrcmp(BAD_CAST"effective", node->name)) {
                        block->effective_count_in_word_ = covert_first_text_value_i(node);
                        if ((block->effective_count_in_word_ > 0) && (block->effective_count_in_word_ <= 128)) {
                            block->actived_ = posix__true;
                        }
                    } else if (0 == xmlStrcmp(BAD_CAST"period", node->name)) {
                        block->period_ = covert_first_text_value_i(node);
                    }

                    node = node->next;
                }
            }

        }

        element = element->next;
    }
    return 0;
}

static
int vari__memnetdev_loaded_handler(xmlNodePtr element) {
    var__memory_netdev_t *device;
    var__functional_object_t *object;
    xmlNodePtr node;
    enum var__types var_type;
    struct _xmlAttr *properties;
    const char *properties_context;

    while (element) {
        if ((0 != xmlStrcmp(BAD_CAST"device", element->name)) ||
                (!element->properties)) {
            goto TRYNEXT;
        }

        properties = element->properties;
        if (0 != xmlStrcmp(BAD_CAST"devtype", properties->name)) {
            goto TRYNEXT;
        }
        properties_context = get_first_text_properties(properties);

        // 根据设备类型文本，安排对象类型
        if (0 == posix__strcasecmp("kVarType_OmronPLC", properties_context)) {
            var_type = kVarType_OmronPLC;
        } else if (0 == posix__strcasecmp("kVarType_ModBus_TCP", properties_context)) {
            var_type = kVarType_ModBus_TCP;
        } else {
            goto TRYNEXT;
        }

        if (var__allocate_functional_object(sizeof ( var__memory_netdev_t), var_type, &object) < 0) {
            break;
        }
        device = var__object_body_ptr(var__memory_netdev_t, object);

        node = element->xmlChildrenNode;
        while (node) {
            if (0 == xmlStrcmp(BAD_CAST"id", node->name)) {
                object->object_id_ = covert_first_text_value_l(node);
            } else if (0 == xmlStrcmp(BAD_CAST"name", node->name)) {
                covert_first_text_value_s(node, object->name_, cchof(object->name_));
            } else if (0 == xmlStrcmp(BAD_CAST"local", node->name)) { /* 读取本地IP 配置 */
                fill_endpoint_properties(node, device->netdev_head_.local_.ipv4_, &device->netdev_head_.local_.port_);
            } else if (0 == xmlStrcmp(BAD_CAST"remote", node->name)) { /* 读取欧姆龙设备 IP 配置 */
                fill_endpoint_properties(node, device->netdev_head_.remote_.ipv4_, &device->netdev_head_.remote_.port_);
            } else if (0 == xmlStrcmp(BAD_CAST"proto", node->name)) { /* 协议类型 */
                device->netdev_head_.protocol_ = (enum var__network_proto_types_t)covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(BAD_CAST"byte_order", node->name)) { /* 解析字节序 */
                device->netdev_head_.byte_order_ = (enum var__byte_orders_t)covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(BAD_CAST"interval_r", node->name)) { /* 读线程的间隔 */
                device->interval_for_read_thread_ = covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(BAD_CAST"interval_w", node->name)) { /* 写线程的间隔 */
                device->interval_for_write_thread_ = covert_first_text_value_i(node);
            } else if (0 == xmlStrcmp(BAD_CAST"blocks", node->name)) {
                vari__memory_netdev_loaded_blocks(node->xmlChildrenNode, device);
            }

            node = node->next;
        }

        if (netdev__create(object) >= 0) {
            /* 插入本地管理列表 */
            list_add_tail(&object->local_, &__local);
            /* 插入全局的对象管理列表 */
            var__insert_object(object);
        } else {
            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "failed to init omron plc device for object:%d", object->object_id_);
            var__delete_functional_object(object);
        }

TRYNEXT:
        element = element->next;
    }

    return 0;
}

// 从 ./etc/dev/memnetdev.xml 读取所有的 内存块+网络 设备对象，并加入到对象管理区

int var__load_memory_netdev() {

	INIT_LIST_HEAD( &__local );

    if (xml__read_data("dev", "memnetdev.xml", "memnetdev", &vari__memnetdev_loaded_handler)) {
        return -1;
    }

    return 0;
}

var__memory_netdev_t *var__get_memory_netdev(objhld_t hld) {
    var__functional_object_t *obj;
    var__memory_netdev_t *device;

    if (hld > 0) {
        obj = objrefr(hld);
        if (obj) {
            device = var__object_body_ptr(var__memory_netdev_t, obj);
            var__acquire_lock(obj);
            return device;
        }
    }

    return NULL;
}

var__memory_netdev_t *var__get_memory_netdev_byid(int id) {
    objhld_t hld;

    hld = var__getobj_handle_byid(id);
    if (hld < 0) {
        return NULL;
    }
    return var__get_memory_netdev(hld);
}
