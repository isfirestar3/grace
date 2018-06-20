#include "map.h"
#include "xml_loaded.h"
#include "var.h"
#include "logger.h"

#include "posix_atomic.h"
#include "posix_ifos.h"
#include "posix_string.h"


int var__object_init(void *udata, void *ctx, int ctxcb) {
    return 0;
}

void var__object_uninit(int hld, void *udata) {
}

static
objhld_t __layout = -1;

typedef struct {
    int id_;
    position_t pos_;
    struct list_head link_;
} vari__map_node_t;

static
void vari__load_nodes(var__vector_t *nodes, xmlNodePtr element) {

    vari__map_node_t *i_node;
    var__map_node_t *e_node;
    struct list_head head;
    xmlAttrPtr attr;
    int count = 0;
    int i;

    if (!nodes || !element) return;

    nodes->data_ = NULL;
    nodes->count_ = 0;

    INIT_LIST_HEAD(&head);

    while (element) {
        if (0 == xmlStrcmp(element->name, BAD_CAST"node")) {
            i_node = (vari__map_node_t *) malloc(sizeof ( vari__map_node_t));
            attr = element->properties;
            while (attr) {
                if (attr->type == XML_ATTRIBUTE_NODE) {
                    if (0 == xmlStrcmp(attr->name, BAD_CAST"id")) {
                        i_node->id_ = covert_first_text_value_i(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"x")) {
                        i_node->pos_.x_ = covert_first_text_value_d(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"y")) {
                        i_node->pos_.y_ = covert_first_text_value_d(attr->xmlChildrenNode);
                    } else {
                        ;
                    }
                }
                attr = attr->next;
            }
            list_add_tail(&i_node->link_, &head);
            ++count;
        }

        element = element->next;
    }

    if (!list_empty(&head) && (count > 0)) {
        nodes->data_ = malloc(count * sizeof ( var__map_node_t));
        e_node = (var__map_node_t *) nodes->data_;
        nodes->count_ = count;
        i = 0;

        while (!list_empty(&head)) {
			i_node = list_first_entry( &head, vari__map_node_t, link_ );
			if ( i_node ) {
				list_del( &i_node->link_ );
				memcpy( &e_node[i], i_node, sizeof( var__map_node_t ) );
				++i;
				free( i_node );
			}
        }
    }
}

typedef struct {
    struct avltree_node_t index_;
    int id_;
    int start_node_id_;
    int end_node_id_;
    position_t control_point_1_;
    position_t control_point_2_;
    position_t start_point_;
    position_t end_point_;
    double length_; // 边的长度单位 m
    var__vector_t speed_limit_; // var__speed_limit_t[] 分段限速相关
    var__vector_t disperse_points_; // map__pos_t [] 离散点定义， 实施软件计算完成后， 填入文件， 下发到标准平台
    var__vector_t wops_; // int [] wop 集合
    struct list_head link_;
} vari__map_edge_t;

typedef struct {
    double x_;
    double y_;
    double w_;
    struct list_head link_;
} vari__position_t;

static
void vari__load_control_point(position_t *pos, xmlNodePtr seg_edge) {
    xmlAttrPtr attr = seg_edge->properties;
    while (attr) {
        if (XML_ATTRIBUTE_NODE == attr->type) {
            if (0 == xmlStrcmp(attr->name, BAD_CAST"x")) {
                pos->x_ = covert_first_text_value_d(attr->xmlChildrenNode);
            } else if (0 == xmlStrcmp(attr->name, BAD_CAST"y")) {
                pos->y_ = covert_first_text_value_d(attr->xmlChildrenNode);
            }
        }
        attr = attr->next;
    }
}

static
void vari__load_speed_limit(var__vector_t *lims, xmlNodePtr element) {
    vari__position_t *i_lims;
    var__speed_limit_t *e_lims;
    struct list_head head;
    xmlAttrPtr attr;
    int count = 0;
    int i;

    if (!lims || !element) return;

    lims->data_ = NULL;
    lims->count_ = 0;

    INIT_LIST_HEAD(&head);

    while (element) {
        if (0 == xmlStrcmp(element->name, BAD_CAST"segment")) {
            i_lims = (vari__position_t *) malloc(sizeof ( vari__position_t));
            attr = element->properties;
            while (attr) {
                if (attr->type == XML_ATTRIBUTE_NODE) {
                    if (0 == xmlStrcmp(attr->name, BAD_CAST"percent")) {
                        i_lims->x_ = covert_first_text_value_d(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"speed")) {
                        i_lims->y_ = covert_first_text_value_d(attr->xmlChildrenNode);
                    } else {
                        ;
                    }
                }
                attr = attr->next;
            }
            list_add_tail(&i_lims->link_, &head);
            ++count;
        }

        element = element->next;
    }

    if (!list_empty(&head) && (count > 0)) {
        lims->data_ = malloc(count * sizeof ( var__speed_limit_t));
        e_lims = (var__speed_limit_t *) lims->data_;
        lims->count_ = count;
        i = 0;

        while (!list_empty(&head)) {
			i_lims = list_first_entry(&head, vari__position_t, link_);
			if ( i_lims ) {
				list_del( &i_lims->link_ );
				e_lims[i].percent_ = i_lims->x_;
				e_lims[i].lim_speed_ = i_lims->y_;
				i++;
				free(i_lims);
			}
        }
    }
}

static
void vari__load_disperse_point(var__vector_t *pts, xmlNodePtr element) {
    vari__position_t *i_point;
    position_t *e_pts;
    struct list_head head;
    xmlAttrPtr attr;
    int count = 0;
    int i;

    if (!pts || !element) return;

    pts->data_ = NULL;
    pts->count_ = 0;

    INIT_LIST_HEAD(&head);

    while (element) {
        if (0 == xmlStrcmp(element->name, BAD_CAST"point")) {
            i_point = (vari__position_t *) malloc(sizeof ( vari__position_t));
            attr = element->properties;
            while (attr) {
                if (attr->type == XML_ATTRIBUTE_NODE) {
                    if (0 == xmlStrcmp(attr->name, BAD_CAST"x")) {
                        i_point->x_ = covert_first_text_value_d(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"y")) {
                        i_point->y_ = covert_first_text_value_d(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"distance_to_start")) {
                        i_point->w_ = covert_first_text_value_d(attr->xmlChildrenNode);
                    } else {
                        ;
                    }
                }
                attr = attr->next;
            }
            list_add_tail(&i_point->link_, &head);
            ++count;
        }

        element = element->next;
    }

    if (!list_empty(&head) && (count > 0)) {
        pts->data_ = malloc(count * sizeof ( position_t));
        e_pts = (position_t *) pts->data_;
        pts->count_ = count;
        i = 0;

        while (!list_empty(&head)) {
			i_point = list_first_entry(&head, vari__position_t, link_);
			list_del( &i_point->link_ );
			memcpy( &e_pts[i], i_point, sizeof( position_t ) );
			i++;
			free( i_point );
        }
    }
}

typedef struct {
    int wop_id_;
    posix__boolean_t enabled_;
    int wop_properties_[9];
    struct list_head link_;
} var__edge_wop_properties_link_t;

static
void vari__load_edge_wop_properties(xmlNode *node, var__edge_wop_properties_link_t *wop_properties) {
    xmlAttr *attr;
    const char *sz;
    static const char *EDGE_WOP_BANK_VALUE[9] = {"val0", "val1", "val2", "val3", "val4", "val5", "val6", "val7", "val8"};
    int i;

    if (!node) return;

    attr = node->properties;
    if (!attr) return;

    while (attr) {
        if (0 == xmlStrcmp(BAD_CAST"enable", attr->name)) {
            sz = get_first_text_properties(attr);
            if (sz) {
                wop_properties->enabled_ = atoi(sz);
            }
        } else {
            for (i = 0; i < (sizeof ( EDGE_WOP_BANK_VALUE) / sizeof ( EDGE_WOP_BANK_VALUE[0])); i++) {
                if (0 == xmlStrcmp(attr->name, BAD_CAST EDGE_WOP_BANK_VALUE[i])) {
                    sz = get_first_text_properties(attr);
                    if (sz) {
                        wop_properties->wop_properties_[i] = atoi(sz);
                    }
                }
            }
        }
        
        attr = attr->next;
    }
}

static
void vari__load_edge_wops(var__vector_t *wops, xmlNodePtr element) {

    var__edge_wop_properties_link_t *wop_properties_link;
    var__edge_wop_properties_t *wop_properties;
    struct list_head head;
    int count = 0, i;

    INIT_LIST_HEAD(&head);
    while (element) {
        wop_properties_link = (var__edge_wop_properties_link_t *) malloc(sizeof ( var__edge_wop_properties_link_t));
        if (!wop_properties_link) {
            return;
        }
        memset(wop_properties_link, 0, sizeof ( var__edge_wop_properties_link_t));

        if (0 == xmlStrcmp(element->name, BAD_CAST"wop")) {
            /* 读取边WOP属性， 包括BANK配置 */
            vari__load_edge_wop_properties(element, wop_properties_link);
            wop_properties_link->wop_id_ = covert_first_text_value_i(element->xmlChildrenNode);
            list_add_tail(&wop_properties_link->link_, &head);
            ++count;
        }
        element = element->next;
    }

    wops->count_ = count;
    wops->data_ = NULL;
    if (count > 0) {
        wops->data_ = (var__edge_wop_properties_t *) malloc(count * sizeof ( var__edge_wop_properties_t));
        if (wops->data_) {
            i = 0;
            while (!list_empty(&head)) {
                wop_properties = &((var__edge_wop_properties_t *) wops->data_)[i];
                wop_properties_link = list_first_entry(&head, var__edge_wop_properties_link_t, link_);
				list_del( &wop_properties_link->link_ );
                memcpy(wop_properties, wop_properties_link, sizeof ( var__edge_wop_properties_t));
                i++;
                free(wop_properties_link);
            }
        }
    }
}

static
void vari__load_edges(var__vector_t *edges, xmlNodePtr element) {

    xmlNodePtr seg_edge;
    struct list_head head;
    int count = 0;
    vari__map_edge_t *i_edge;
    var__edge_t *e_edge;
    int i;

    INIT_LIST_HEAD(&head);

    while (element) {
        if (0 == xmlStrcmp(element->name, BAD_CAST"edge")) {
            i_edge = (vari__map_edge_t *) malloc(sizeof ( vari__map_edge_t));
            if (!i_edge) {
                return;
            }
            memset(i_edge, 0, sizeof ( vari__map_edge_t));

            seg_edge = element->xmlChildrenNode;
            while (seg_edge) {
                if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"id")) {
                    i_edge->id_ = covert_first_text_value_i(seg_edge->xmlChildrenNode);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"start_node_id")) {
                    i_edge->start_node_id_ = covert_first_text_value_i(seg_edge->xmlChildrenNode);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"end_node_id")) {
                    i_edge->end_node_id_ = covert_first_text_value_i(seg_edge->xmlChildrenNode);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"ctrl_point1")) {
                    vari__load_control_point(&i_edge->control_point_1_, seg_edge);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"ctrl_point2")) {
                    vari__load_control_point(&i_edge->control_point_2_, seg_edge);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"start_node")) {
                    vari__load_control_point(&i_edge->start_point_, seg_edge);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"end_node")) {
                    vari__load_control_point(&i_edge->end_point_, seg_edge);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"distance")) {
                    i_edge->length_ = covert_first_text_value_d(seg_edge);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"speed_limit")) {
                    vari__load_speed_limit(&i_edge->speed_limit_, seg_edge->xmlChildrenNode);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"disperse_point")) {
                    vari__load_disperse_point(&i_edge->disperse_points_, seg_edge->xmlChildrenNode);
                } else if (0 == xmlStrcmp(seg_edge->name, BAD_CAST"wops")) {
                    vari__load_edge_wops(&i_edge->wops_, seg_edge->xmlChildrenNode);
                }

                seg_edge = seg_edge->next;
            }

            list_add_tail(&i_edge->link_, &head);
            ++count;
        }

        element = element->next;
    }

    edges->count_ = count;
    edges->data_ = NULL;
    if (count > 0) {
        edges->data_ = (int *) malloc(count * sizeof ( var__edge_t));
        if (edges->data_) {
            e_edge = (var__edge_t *) edges->data_;
            i = 0;
            while (!list_empty(&head)) {
                i_edge = list_first_entry(&head, vari__map_edge_t, link_);
				list_del( &i_edge->link_ );
                memcpy(&e_edge[i], i_edge, sizeof ( var__edge_t));
                i++;
                free(i_edge);
            }
        }
    }
}

typedef struct {
    struct avltree_node_t index_;
    int id_;
    int direction_; // 0:正向 1: 反向
    int angle_type_; // 0:相對角，1:全局角
    double angle_; // 弧度
    struct list_head link_;
} vari__way_of_pass_t;

static
void vari__load_wops(var__vector_t *wops, xmlNodePtr element) {
    vari__way_of_pass_t *i_wop;
    var__way_of_pass_t *e_wop;
    struct list_head head;
    xmlAttrPtr attr;
    int count = 0;
    int i;

    if (!wops || !element) return;

    wops->data_ = NULL;
    wops->count_ = 0;

    INIT_LIST_HEAD(&head);

    while (element) {
        if (0 == xmlStrcmp(element->name, BAD_CAST"wop")) {
            i_wop = (vari__way_of_pass_t *) malloc(sizeof ( vari__way_of_pass_t));
            if (!i_wop) {
                return;
            }
            memset(i_wop, 0, sizeof ( vari__way_of_pass_t));

            attr = element->properties;
            while (attr) {
                if (attr->type == XML_ATTRIBUTE_NODE) {
                    if (0 == xmlStrcmp(attr->name, BAD_CAST"id")) {
                        i_wop->id_ = covert_first_text_value_i(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"direction")) {
                        i_wop->direction_ = covert_first_text_value_i(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"angle_type")) {
                        i_wop->angle_type_ = covert_first_text_value_i(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"angle")) {
                        i_wop->angle_ = covert_first_text_value_d(attr->xmlChildrenNode);
                    } else {
                        ;
                    }
                }
                attr = attr->next;
            }
            list_add_tail(&i_wop->link_, &head);
            ++count;
        }

        element = element->next;
    }

    wops->count_ = count;
    wops->data_ = NULL;
    if (!list_empty(&head) && (count > 0)) {
        wops->data_ = malloc(count * sizeof ( var__way_of_pass_t));
        if (wops->data_) {
            e_wop = (var__way_of_pass_t *) wops->data_;
            i = 0;

            while (!list_empty(&head)) {
                i_wop = list_first_entry(&head, vari__way_of_pass_t, link_);
				list_del( &i_wop->link_ );
                memcpy(&e_wop[i], i_wop, sizeof ( var__way_of_pass_t));
                ++i;
                free(i_wop);
            }
        }
    }
}

typedef struct {
    int id_;
    int from_edge_;
    int from_wop_;
    int to_edge_;
    int to_wop_;
    struct list_head link_;
} vari__map_edge_link_t;

static
void vari__load_edge_link(var__vector_t *edgelnks, xmlNodePtr element) {
    vari__map_edge_link_t *i_edge_lnk;
    var__edge_link_t *e_edge_lnk;
    struct list_head head;
    xmlAttrPtr attr;
    int count = 0;
    int i;

    if (!edgelnks || !element) return;

    edgelnks->data_ = NULL;
    edgelnks->count_ = 0;

    INIT_LIST_HEAD(&head);

    while (element) {
        if (0 == xmlStrcmp(element->name, BAD_CAST"link")) {
            i_edge_lnk = (vari__map_edge_link_t *) malloc(sizeof ( vari__map_edge_link_t));
            if (!i_edge_lnk) {
                return;
            }
            attr = element->properties;
            while (attr) {
                if (attr->type == XML_ATTRIBUTE_NODE) {
                    if (0 == xmlStrcmp(attr->name, BAD_CAST"id")) {
                        i_edge_lnk->id_ = covert_first_text_value_i(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"from_edge")) {
                        i_edge_lnk->from_edge_ = covert_first_text_value_i(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"from_wop")) {
                        i_edge_lnk->from_wop_ = covert_first_text_value_i(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"to_edge")) {
                        i_edge_lnk->to_edge_ = covert_first_text_value_i(attr->xmlChildrenNode);
                    } else if (0 == xmlStrcmp(attr->name, BAD_CAST"to_wop")) {
                        i_edge_lnk->to_wop_ = covert_first_text_value_l(attr->xmlChildrenNode);
                    } else {
                        ;
                    }
                }
                attr = attr->next;
            }
            list_add_tail(&i_edge_lnk->link_, &head);
            ++count;
        }

        element = element->next;
    }

    edgelnks->count_ = count;
    edgelnks->data_ = NULL;
    if (!list_empty(&head) && (count > 0)) {
        edgelnks->data_ = malloc(count * sizeof ( var__edge_link_t));
        if (edgelnks->data_) {
            e_edge_lnk = (var__edge_link_t *) edgelnks->data_;
            i = 0;
            while (!list_empty(&head)) {
                i_edge_lnk = list_first_entry(&head, vari__map_edge_link_t, link_);
				list_del( &i_edge_lnk->link_ );
                memcpy(&e_edge_lnk[i], i_edge_lnk, sizeof ( var__edge_link_t));
                ++i;
                free(i_edge_lnk);
            }
        }
    }
}

static
int compare_edges(const void *left, const void *right) {
    var__way_of_pass_t *pleft = (var__way_of_pass_t *) left;
    var__way_of_pass_t *pright = (var__way_of_pass_t *) right;

    if (pleft->id_ < pright->id_) {
        return 1;
    } else {
        if (pleft->id_ > pright->id_) {
            return -1;
        }
    }

    return 0;
}

static
int compare_wops(const void *left, const void *right) {
    var__edge_t *pleft = (var__edge_t *) left;
    var__edge_t *pright = (var__edge_t *) right;

    if (pleft->id_ < pright->id_) {
        return 1;
    } else {
        if (pleft->id_ > pright->id_) {
            return -1;
        }
    }

    return 0;
}

int var__load_layout_ffile() {
    int fd = -1, retval = -1;
	uint64_t cur_pos = 0;
    char filepath[512];
    memset(filepath, 0, 512);
    const char filename[] = "layout.bin";
#ifdef _WIN32
    const char* cur_path = posix__getpedir();
	if( !cur_path ) {
		log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"cur_path return null!");
		return retval;
	}
	posix__sprintf(filepath, 512, "%s\\etc\\%s", cur_path, filename);
#else
    posix__sprintf(filepath, 512, "/etc/agv/%s", filename);
#endif
    log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"bin file full path:%s", filepath);

    //load data to data stream
    char *data_stream;
    uint64_t file_size = posix__get_filesize(filepath);
    if(file_size == (uint64_t) (-1)){
        log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"read file size failure!");
        return retval;
    }

    if ( !(data_stream = (char*) malloc (file_size)) ) {
        log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"malloc data stream[%ld] failure!", file_size);
        return retval;
    }
    
    if( posix__file_open(filepath, &fd) < 0 ){
        log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"open file %s failure!", filepath);
        free( data_stream );
        return retval;
    }

    if( posix__read_file(fd, data_stream, file_size) < 0 ){
        log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"open file %s failure!", filepath);
        goto FUN_OUT;
    }

    objhld_t previous;
    var__map_layout_t *layout;
    var__functional_object_t *object;
    int i = 0;

    if (var__allocate_functional_object(sizeof ( var__map_layout_t), kVarType_Map, &object) < 0) {
        log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"malloc layout failure!");
        goto FUN_OUT;
    }
    
    object->object_id_ = kVarFixedObject_Map;
    posix__strcpy(object->name_, cchof(object->name_), "layout");
    layout = var__object_body_ptr(var__map_layout_t, object);
    
	
	char *p_pos = data_stream;
	//layout->nodes_
	layout->nodes_.count_ = *(int*)p_pos;
	cur_pos += sizeof(int);
	if( cur_pos > file_size || layout->nodes_.count_ < 0) goto FUN_OUT;
	p_pos += sizeof(int);
	layout->nodes_.data_ = malloc( layout->nodes_.count_ * sizeof ( var__map_node_t) );
	if ( layout->nodes_.data_ ) {
		memcpy(layout->nodes_.data_, p_pos, layout->nodes_.count_ * sizeof ( var__map_node_t) );
		cur_pos += layout->nodes_.count_ * sizeof ( var__map_node_t);
		if( cur_pos > file_size) goto FUN_OUT;
		p_pos += layout->nodes_.count_ * sizeof ( var__map_node_t);
		log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"  layout->nodes_ size[%d]", layout->nodes_.count_);
	} else {
		log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"  malloc layout->nodes_ failure!! size[%ld]", 
                layout->nodes_.count_ * sizeof ( var__map_node_t) );
		cur_pos += layout->nodes_.count_ * sizeof ( var__map_node_t);
		if( cur_pos > file_size) goto FUN_OUT;
		p_pos += layout->nodes_.count_ * sizeof ( var__map_node_t);
		layout->nodes_.count_ = 0;
	}
	
	
	//layout->edges_
	layout->edges_.count_ = *(int*)p_pos;
	cur_pos += sizeof(int);
	if( cur_pos > file_size || layout->edges_.count_ < 0) goto FUN_OUT;
	p_pos += sizeof(int);
	log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"  layout->edges_ size[%d]\n", layout->edges_.count_);
	layout->edges_.data_ = malloc( layout->edges_.count_ * sizeof ( var__edge_t) );
	var__edge_t *p_edge;
	
	for( ; i < layout->edges_.count_; ++i) {
		p_edge = (var__edge_t *) &((var__edge_t*) layout->edges_.data_)[i];
		cur_pos += ( sizeof(int) * 3 + sizeof(position_t) * 4 + sizeof(double) );
		if( cur_pos > file_size) goto FUN_OUT;
		memcpy( (char*)p_edge + sizeof(struct avltree_node_t), p_pos, sizeof(int) * 3 + sizeof(position_t) * 4 + sizeof(double) );
		p_pos += ( sizeof(int) * 3 + sizeof(position_t) * 4 + sizeof(double) );
		
		//var__edge_t->speed_limit_
		p_edge->speed_limit_.count_ = *(int*)p_pos;
		cur_pos += sizeof(int);
		if( cur_pos > file_size || p_edge->speed_limit_.count_ < 0) goto FUN_OUT;
		p_pos += sizeof(int);
		log__save("map", kLogLevel_Info, kLogTarget_Filesystem, "    edges_->speed_limit_ size[%d]", p_edge->speed_limit_.count_);
		p_edge->speed_limit_.data_ = malloc( p_edge->speed_limit_.count_ * sizeof ( var__speed_limit_t) );
		memcpy(p_edge->speed_limit_.data_, p_pos, p_edge->speed_limit_.count_ * sizeof ( var__speed_limit_t) );
		cur_pos += p_edge->speed_limit_.count_ * sizeof ( var__speed_limit_t);
		if( cur_pos > file_size) goto FUN_OUT;
		p_pos += p_edge->speed_limit_.count_ * sizeof ( var__speed_limit_t);
		
		//var__edge_t->disperse_points_
		p_edge->disperse_points_.count_ = *(int*)p_pos;
		cur_pos += sizeof(int);
		if( cur_pos > file_size || p_edge->disperse_points_.count_ < 0 ) goto FUN_OUT;
		p_pos += sizeof(int);
		log__save("map", kLogLevel_Info, kLogTarget_Filesystem, "    edges_->disperse_points_ size[%d]", p_edge->disperse_points_.count_);
		p_edge->disperse_points_.data_ = malloc( p_edge->disperse_points_.count_ * sizeof ( position_t) );
		memcpy(p_edge->disperse_points_.data_, p_pos, p_edge->disperse_points_.count_ * sizeof ( position_t) );
		cur_pos += p_edge->disperse_points_.count_ * sizeof ( position_t);
		if( cur_pos > file_size) goto FUN_OUT;
		p_pos += p_edge->disperse_points_.count_ * sizeof ( position_t);
		
		//var__edge_t->wops_
		p_edge->wops_.count_ = *(int*)p_pos;
		cur_pos += sizeof(int);
		if( cur_pos > file_size || p_edge->wops_.count_ < 0 ) goto FUN_OUT;
		p_pos += sizeof(int);
		log__save("map", kLogLevel_Info, kLogTarget_Filesystem, "    edges_->wops_ size[%d]\n", p_edge->wops_.count_);
		p_edge->wops_.data_ = malloc( p_edge->wops_.count_ * sizeof ( var__edge_wop_properties_t) );
		memcpy(p_edge->wops_.data_, p_pos, p_edge->wops_.count_ * sizeof ( var__edge_wop_properties_t) );
		cur_pos += p_edge->wops_.count_ * sizeof ( var__edge_wop_properties_t);
		if( cur_pos > file_size) goto FUN_OUT;
		p_pos += p_edge->wops_.count_ * sizeof ( var__edge_wop_properties_t);
	}
	
	//layout->map_type_
	layout->map_type_ = *(int*)p_pos;
	cur_pos += sizeof(int);
	if( cur_pos > file_size) goto FUN_OUT;
	p_pos += sizeof(int);
	log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"  layout->map_type_ size[%d]", layout->map_type_);
	
	//layout->wops_
	layout->wops_.count_ = *(int*)p_pos;
	cur_pos += sizeof(int);
	if( cur_pos > file_size || layout->wops_.count_ < 0 ) goto FUN_OUT;
	p_pos += sizeof(int);
	log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"  layout->wops_ size[%d]", layout->wops_.count_);
	if( layout->wops_.count_ > 0 ) {
		layout->wops_.data_ = malloc( layout->wops_.count_ * sizeof ( var__way_of_pass_t) );
		var__way_of_pass_t *p_pass;
		for( i = 0; i < layout->wops_.count_; ++i) {
			p_pass = (var__way_of_pass_t *) &((var__way_of_pass_t*) layout->wops_.data_)[i];
			cur_pos += sizeof(int) * 3 + sizeof(double);
			if( cur_pos > file_size) goto FUN_OUT;
			memcpy( (char*)p_pass + sizeof(struct avltree_node_t), p_pos, sizeof(int) * 3 + sizeof(double) );
			p_pos += sizeof(int) * 3 + sizeof(double);
		}
	}
	
	//layout->links_
	layout->links_.count_ = *(int*)p_pos;
	cur_pos += sizeof(int);
	if( cur_pos > file_size || layout->links_.count_ < 0 ) goto FUN_OUT;
	p_pos += sizeof(int);
	log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"  layout->links_ size[%d]", layout->links_.count_);
	layout->links_.data_ = malloc( layout->links_.count_ * sizeof ( var__edge_link_t) );
	memcpy(layout->links_.data_, p_pos, layout->links_.count_ * sizeof ( var__edge_link_t) );
	cur_pos += layout->links_.count_ * sizeof ( var__edge_link_t);
	if( cur_pos > file_size) goto FUN_OUT;
	p_pos += layout->links_.count_ * sizeof ( var__edge_link_t);
	
    // 插入本地管理列表
    previous = posix__atomic_xchange((volatile objhld_t *) &__layout, (objhld_t) object->handle_);
    if (previous > 0) {
        objclos(previous);
    }

    // 插入全局的对象管理列表
    //var__insert_object(object);
    
	// 建立索引
    layout->wop_searcher_root_ = NULL;
    layout->edge_searcher_root_ = NULL;
    for (i = 0; i < layout->edges_.count_; ++i) {
        layout->edge_searcher_root_ = avlinsert(
                layout->edge_searcher_root_, (struct avltree_node_t *) &((var__edge_t *) layout->edges_.data_)[i], compare_edges);
    }
    for (i = 0; i < layout->wops_.count_; ++i) {
        layout->wop_searcher_root_ = avlinsert(
                layout->wop_searcher_root_, (struct avltree_node_t *) &((var__way_of_pass_t *) layout->wops_.data_)[i], compare_wops);
    }
    retval = 0;
	
FUN_OUT:
	if( fd != -1 ) posix__close(fd);
	if( !data_stream ) free(data_stream);
    if( 0 == retval ) {
		log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"var__load_layout_ffile successful!\n");
	} else {
		log__save("map", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,"var__load_layout_ffile failure!\n");
	}
	return retval;
}

int var__allocate_functional_object(int size, enum var__types type, var__functional_object_t **output) {
    objhld_t hld;
    var__functional_object_t *object;

    if (!output) return -1;
    *output = NULL;

    hld = objallo(size + sizeof ( var__functional_object_t), &var__object_init, &var__object_uninit, NULL, 0);
    if (hld < 0) {
        return -1;
    }

    // 创建阶段的引用不会被外部反引用， 只有触发 var__delete_functional_object 才会被真正反引用并释放
    object = objrefr(hld);
    if (!object) {
        return -1;
    }

    memset(object, 0, size + sizeof ( var__functional_object_t));
    object->type_ = type;
    object->length_ = size + sizeof ( var__functional_object_t);
    object->body_length_ = size;
    object->handle_ = hld;
    object->dup_ = -1;
    INIT_LIST_HEAD(&object->pagefault_);
    INIT_LIST_HEAD(&object->global_);
    INIT_LIST_HEAD(&object->local_);
    
    posix__pthread_mutex_init(&object->locker_);

    *output = object;
    return 0;
}

static
int vari__map_loaded_handler(xmlNodePtr element) {
    objhld_t previous;
    var__map_layout_t *layout;
    var__functional_object_t *object;
    int i;

    if (var__allocate_functional_object(sizeof ( var__map_layout_t), kVarType_Map, &object) < 0) {
        return -1;
    }
    object->object_id_ = kVarFixedObject_Map;
    posix__strcpy(object->name_, cchof(object->name_), "layout");

    layout = var__object_body_ptr(var__map_layout_t, object);

    while (element) {
        if (0 == xmlStrcmp(element->name, BAD_CAST"nodes")) {
            vari__load_nodes(&layout->nodes_, element->xmlChildrenNode);
        } else if (0 == xmlStrcmp(element->name, BAD_CAST"edges")) {
            vari__load_edges(&layout->edges_, element->xmlChildrenNode);
        } else if (0 == xmlStrcmp(element->name, BAD_CAST"wops")) {
            vari__load_wops(&layout->wops_, element->xmlChildrenNode);
        } else if (0 == xmlStrcmp(element->name, BAD_CAST"links")) {
            vari__load_edge_link(&layout->links_, element->xmlChildrenNode);
        } else if (0 == xmlStrcmp(element->name, BAD_CAST"type")) {
            layout->map_type_ = covert_first_text_value_i(element->xmlChildrenNode);
        } else {
            ;
        }

        element = element->next;
    }

    // 插入本地管理列表
    previous = posix__atomic_xchange((volatile objhld_t *) &__layout, (objhld_t) object->handle_);
    if (previous > 0) {
        objclos(previous);
    }

    // 插入全局的对象管理列表
    //var__insert_object(object);

    // 建立索引
    layout->wop_searcher_root_ = NULL;
    layout->edge_searcher_root_ = NULL;
    for (i = 0; i < layout->edges_.count_; i++) {
        layout->edge_searcher_root_ = avlinsert(
                layout->edge_searcher_root_, (struct avltree_node_t *) &((var__edge_t *) layout->edges_.data_)[i], compare_edges);
    }
    for (i = 0; i < layout->wops_.count_; i++) {
        layout->wop_searcher_root_ = avlinsert(
                layout->wop_searcher_root_, (struct avltree_node_t *) &((var__way_of_pass_t *) layout->wops_.data_)[i], compare_wops);
    }
    return 0;
}

int var__load_layout() {
    if ( var__load_layout_ffile() < 0) {
        return xml__read_data(NULL, "layout.xml", "root", vari__map_loaded_handler);
    }
    
    return 0;
}

const var__edge_t * const var__search_edge(int id) {
    var__map_layout_t *layout;
    var__edge_t target, *edge;
    var__functional_object_t *object;

    object = (var__functional_object_t *) objrefr(__layout);
    if (!object) {
        return NULL;
    }
    layout = var__object_body_ptr(var__map_layout_t, object);

    var__acquire_object_lock(layout);
    do {
        edge = NULL;
        if (!layout->edge_searcher_root_) {
            break;
        }

        target.id_ = id;
        edge = (var__edge_t *) avlsearch(layout->edge_searcher_root_, (struct avltree_node_t *) &target, compare_edges);
        if (!edge) {
            break;
        }

        var__release_object_lock(layout);
        return edge;
    } while (0);

    var__release_object_lock(layout);
    objdefr(__layout);
    return NULL;
}

const var__way_of_pass_t * const var__search_wop(int id) {
    var__map_layout_t *layout;
    var__way_of_pass_t target, *wop;
    var__functional_object_t *object;

    object = (var__functional_object_t *) objrefr(__layout);
    if (!object) {
        return NULL;
    }
    layout = var__object_body_ptr(var__map_layout_t, object);

    var__acquire_object_lock(layout);
    do {
        wop = NULL;
        if (!layout->wop_searcher_root_) {
            break;
        }

        target.id_ = id;
        wop = (var__way_of_pass_t *) avlsearch(layout->wop_searcher_root_, (struct avltree_node_t *) &target, compare_wops);
        if (!wop) {
            break;
        }

        var__release_object_lock(layout);
        return wop;
    } while (0);

    var__release_object_lock(layout);
    objdefr(__layout);
    return NULL;
}

void var__end_search() {
    objdefr(__layout);
}

var__map_layout_t *var__get_layout() {
    var__functional_object_t *obj;
    var__map_layout_t *layot;

    if (__layout > 0) {
        obj = objrefr(__layout);
        if (obj) {
            layot = var__object_body_ptr(var__map_layout_t, obj);
            var__acquire_lock(obj);
            return layot;
        }
    }

    return NULL;
}
