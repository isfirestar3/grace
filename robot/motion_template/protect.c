#include "vartypes.h"
#include "logger.h"
#include "clist.h"
#include "xml_loaded.h"
#include "navigation.h"
#include "var.h"
#include "elmo.h"
#include "dio.h"
#include "vehicle.h"
#include <math.h>
#include "memnetdev.h"
#include "safety.h"
#include "protect.h"
#include "posix_time.h"

//#define SAFETY_DELAY_RESUME_TIME  (10000*1000*3) //延时启动时间 单位100ns
#define SAFETY_LINE_BANK_LEVEL  (10)

#pragma pack(push, 1)

typedef enum {
    kSafetyProtectSignal_StopArea = 0,
    kSafetyProtectSignal_SlowdownArea,

    kSafetyProtectSignal_MaxNum,
} em_safety_protect_signal_t;

typedef struct {
    struct list_head link_;
    st_safety_dev_bank_src_info sensor_;
} st_dev_bank_src_list_info;

typedef struct {
    int __bank_id;
    int __resume_time;
    struct list_head __dev_banks_list;
} st_safety_bank_info;

typedef struct {
    struct list_head link_;
    st_safety_bank_info __dev_banks;
} st_safety_bank_list_info;

typedef enum {
    kVtype_Vx = 1,
    kVtype_Vy,
    kVtype_W,
} st_v_types;

typedef struct {
    struct list_head link_;
    int v_type;
    double v_start;
    double v_end;
} st_speed_bank_info;

typedef struct {
    struct list_head link_;
    int bank;
    int level;
    struct list_head v_info;
} st_speed_bank_list_info;

//set bank

typedef struct {
    struct list_head link_;
    enum safety_dev_data_types __dev_data_type;
    enum em_safety_protect_output_t __reslut;
    int __do_id[kSafetyProtectIdex_MaxNum];
    int __do_value;
} st_safety_set_do_info;

typedef struct {
    struct list_head link_;
    int __dst_dev_id;
    enum var__types __dst_dev_type;
    struct list_head __set_do_list;
} st_safety_dev_bank_dst_info;

typedef struct {
    struct list_head link_;

    int __bank_id;
    struct list_head __set_bank_list;
} st_safety_set_bank_list_info;

#pragma pack(pop)

static struct list_head g_safety_banks;
static struct list_head g_speed_banks;
static struct list_head g_set_banks;
static int g_cfg_enable = 1;
static int g_sfw_enable = 1;
static st_safety_dev_bank_src_info g_sensor_triggered;
static uint64_t g_last_stop_time = 0;
static enum em_safety_protect_output_t g_last_out_rslt = kSafetyProtectOutput_Normal;
static int g_sensor_log = 0;
static int g_last_bank = 0;
static int g_last_level = 0;
static int g_bank_log = 0;
static int g_resume_time = 3000;

static int g_manual_bank = 0;
static int g_manual_bank_lv = 0;

static int safety__get_cur_line_bank(int* cur_bank);
static int safety__get_cur_speed_bank(int* cur_bank, int* level);
static int safety__bank_proc(int cur_bank, enum em_safety_protect_output_t * out_rslt);
static int safety__check_sensor(st_safety_dev_bank_src_info* sensor_bank, enum em_safety_protect_output_t * out_rslt);
static int safety__set_bank(int bank, enum em_safety_protect_output_t o);
static int safety__set_do(st_safety_dev_bank_dst_info* dev, enum em_safety_protect_output_t o);
static int safety__check_dev_ctrl_con(st_safety_set_do_info * do_set, enum em_safety_protect_output_t o);

static int safety_etc_loaded_handler(xmlNodePtr element) {

    xmlNodePtr dev_dio_param;
    xmlNodePtr v_param;
    xmlAttrPtr attr;
    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"enable", element->name)) {
            g_cfg_enable = covert_first_text_value_i(element);
            log__save("safety", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "g_cfg_enable = %d", g_cfg_enable);
        }
        if (0 == xmlStrcmp(BAD_CAST"sensor_log", element->name)) {
            g_sensor_log = covert_first_text_value_i(element);
        }
        if (0 == xmlStrcmp(BAD_CAST"bank_log", element->name)) {
            g_bank_log = covert_first_text_value_i(element);
        }

        if (0 == xmlStrcmp(BAD_CAST"v_banks", element->name)) {
            dev_dio_param = element->xmlChildrenNode;
            INIT_LIST_HEAD(&g_speed_banks);
            while (dev_dio_param) {
                if (0 == xmlStrcmp(BAD_CAST"v_bank", dev_dio_param->name)) {

                    st_speed_bank_list_info *node;

                    if (NULL == (node = (st_speed_bank_list_info *) malloc(sizeof (st_speed_bank_list_info)))) {
                        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
							"failed to allocate vitual memory for safety_etc_loaded_handler::node");
                        return -1;
                    }

                    attr = dev_dio_param->properties;
                    while (attr) {
                        if (attr->type == XML_ATTRIBUTE_NODE) {
                            if (0 == xmlStrcmp(BAD_CAST"bank", attr->name)) {
                                node->bank = covert_first_text_value_i(attr->xmlChildrenNode);
                            } else if (0 == xmlStrcmp(BAD_CAST"level", attr->name)) {
                                node->level = covert_first_text_value_i(attr->xmlChildrenNode);
                            }

                        }
                        attr = attr->next;
                    }

                    INIT_LIST_HEAD(&node->v_info);

                    v_param = dev_dio_param->xmlChildrenNode;
                    while (v_param) {
                        if (0 == xmlStrcmp(BAD_CAST"v", v_param->name)) {
                            st_speed_bank_info* v_node;
                            if (NULL == (v_node = (st_speed_bank_info *) malloc(sizeof (st_speed_bank_info)))) {
                                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
									"failed to allocate vitual memory for safety_etc_loaded_handler::v_node");
                                return -1;
                            }

                            attr = v_param->properties;
                            while (attr) {
                                if (attr->type == XML_ATTRIBUTE_NODE) {
                                    if (0 == xmlStrcmp(BAD_CAST"v_s", attr->name)) {
                                        v_node->v_start = covert_first_text_value_d(attr->xmlChildrenNode);
                                    } else if (0 == xmlStrcmp(BAD_CAST"v_e", attr->name)) {
                                        v_node->v_end = covert_first_text_value_d(attr->xmlChildrenNode);
                                    } else if (0 == xmlStrcmp(BAD_CAST"v_type", attr->name)) {
                                        v_node->v_type = covert_first_text_value_i(attr->xmlChildrenNode);
                                    }

                                }
                                attr = attr->next;
                            }

                            list_add_tail(&v_node->link_, &node->v_info);
                        }

                        v_param = v_param->next;
                    }

                    list_add_tail(&node->link_, &g_speed_banks);
                }
                dev_dio_param = dev_dio_param->next;
            }

        }

        element = element->next;
    }

    return 0;
}

static int safety_etc_loaded_bank_handler(xmlNodePtr element) {

    INIT_LIST_HEAD(&g_safety_banks);
    INIT_LIST_HEAD(&g_set_banks);

    xmlNodePtr dev_dio_param;
    xmlNodePtr set_bank_param;
    xmlNodePtr do_param;
    xmlAttrPtr attr;
    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"bank", element->name)) {

            st_safety_bank_list_info *node = 0;
            st_safety_set_bank_list_info* set_bank_node = 0;

            attr = element->properties;
            while (attr) {
                if (attr->type == XML_ATTRIBUTE_NODE) {
                    if (0 == xmlStrcmp(BAD_CAST"id", attr->name)) {
                        //bank src
                        if (NULL == (node = (st_safety_bank_list_info *) malloc(sizeof (st_safety_bank_list_info)))) {
                            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
								"failed to allocate vitual memory for safety_etc_loaded_bank_handler::v_node");
                            return -1;
                        }
                        memset(node, 0, sizeof (st_safety_bank_list_info));
                        node->__dev_banks.__bank_id = covert_first_text_value_i(attr->xmlChildrenNode);
                        INIT_LIST_HEAD(&node->__dev_banks.__dev_banks_list);
                        //set bank

                        if (NULL == (set_bank_node = (st_safety_set_bank_list_info *) malloc(sizeof (st_safety_set_bank_list_info)))) {
                            log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
								"failed to allocate vitual memory for safety_etc_loaded_bank_handler::set_bank_node");
                            return -1;
                        }

                        set_bank_node->__bank_id = node->__dev_banks.__bank_id;
                    } else if (0 == xmlStrcmp(BAD_CAST"resume_time", attr->name)) {
                        node->__dev_banks.__resume_time = covert_first_text_value_i(attr->xmlChildrenNode);
                    }
                }
                attr = attr->next;
            }
            dev_dio_param = element->xmlChildrenNode;
            while (dev_dio_param) {
                if (0 == xmlStrcmp(BAD_CAST"dev", dev_dio_param->name)) {
                    st_dev_bank_src_list_info* dev_node;
                    if (NULL == (dev_node = (st_dev_bank_src_list_info *) malloc(sizeof (st_dev_bank_src_list_info)))) {
                        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
							"failed to allocate vitual memory for safety_etc_loaded_bank_handler::dev_node");
                        return -1;
                    }
                    memset(dev_node->sensor_.sensor_name, 0, sizeof (dev_node->sensor_.sensor_name));
                    attr = dev_dio_param->properties;
                    while (attr) {
                        if (attr->type == XML_ATTRIBUTE_NODE) {
                            if (0 == xmlStrcmp(BAD_CAST"id", attr->name)) {
                                dev_node->sensor_.__src_dev_id = covert_first_text_value_i(attr->xmlChildrenNode);
                            } else if (0 == xmlStrcmp(BAD_CAST"dev_type", attr->name)) {
                                dev_node->sensor_.__src_dev_type = covert_first_text_value_i(attr->xmlChildrenNode);
                            } else if (0 == xmlStrcmp(BAD_CAST"index", attr->name)) {
                                dev_node->sensor_.__data_src_channel[kSafetyProtectIdex_MainIndex] = covert_first_text_value_i(attr->xmlChildrenNode);
                            } else if (0 == xmlStrcmp(BAD_CAST"sub_index", attr->name)) {
                                dev_node->sensor_.__data_src_channel[kSafetyProtectIdex_SubIndex] = covert_first_text_value_i(attr->xmlChildrenNode);
                            } else if (0 == xmlStrcmp(BAD_CAST"ssub_index", attr->name)) {
                                dev_node->sensor_.__data_src_channel[kSafetyProtectIdex_SsubIndex] = covert_first_text_value_i(attr->xmlChildrenNode);
                            } else if (0 == xmlStrcmp(BAD_CAST"data_src_type", attr->name)) {
                                dev_node->sensor_.__dev_data_type = covert_first_text_value_i(attr->xmlChildrenNode);
                            } else if (0 == xmlStrcmp(BAD_CAST"thres", attr->name)) {
                                switch (dev_node->sensor_.__dev_data_type) {
                                    case safety_data_type_digital:
                                        dev_node->sensor_.__di_mask = covert_first_text_value_i(attr->xmlChildrenNode);
                                        break;
                                    case safety_data_type_analog:
                                    case safety_data_type_analogword:
                                        dev_node->sensor_.__ai_thres = covert_first_text_value_d(attr->xmlChildrenNode);
                                        break;
                                    default:
                                        break;
                                }

                            } else if (0 == xmlStrcmp(BAD_CAST"reslut", attr->name)) {
                                dev_node->sensor_.__reslut = covert_first_text_value_i(attr->xmlChildrenNode);
                            } else if (0 == xmlStrcmp(BAD_CAST"name", attr->name)) {
                                covert_first_text_value_s(attr->xmlChildrenNode, dev_node->sensor_.sensor_name, sizeof (dev_node->sensor_.sensor_name) - 1);
                            }

                        }
                        attr = attr->next;
                    }
                    list_add_tail(&dev_node->link_, &node->__dev_banks.__dev_banks_list);
                } else if (0 == xmlStrcmp(BAD_CAST"ctrl", dev_dio_param->name)) {
                    INIT_LIST_HEAD(&set_bank_node->__set_bank_list);


                    set_bank_param = dev_dio_param->xmlChildrenNode;
                    while (set_bank_param) {
                        //
                        if (0 == xmlStrcmp(BAD_CAST"dev", set_bank_param->name)) {
                            st_safety_dev_bank_dst_info* bk_node;
                            if (NULL == (bk_node = (st_safety_dev_bank_dst_info *) malloc(sizeof (st_safety_dev_bank_dst_info)))) {
                                log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
									"failed to allocate vitual memory for safety_etc_loaded_bank_handler::bk_node");
                                return -1;
                            }
                            // INIT_LIST_HEAD(&bk_node->link_);
                            INIT_LIST_HEAD(&bk_node->__set_do_list);


                            attr = set_bank_param->properties;
                            while (attr) {
                                if (attr->type == XML_ATTRIBUTE_NODE) {
                                    if (0 == xmlStrcmp(BAD_CAST"id", attr->name)) {
                                        bk_node->__dst_dev_id = covert_first_text_value_i(attr->xmlChildrenNode);
                                    } else if (0 == xmlStrcmp(BAD_CAST"dev_type", attr->name)) {
                                        bk_node->__dst_dev_type = covert_first_text_value_i(attr->xmlChildrenNode);
                                    }
                                }
                                attr = attr->next;
                            }

                            do_param = set_bank_param->xmlChildrenNode;
                            while (do_param) {
                                if (0 == xmlStrcmp(BAD_CAST"do", do_param->name)) {
                                    st_safety_set_do_info* do_node;
                                    if (NULL == (do_node = (st_safety_set_do_info *) malloc(sizeof (st_safety_set_do_info)))) {
                                        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
											"failed to allocate vitual memory for safety_etc_loaded_bank_handler::do_node");
                                        return -1;
                                    }
                                    INIT_LIST_HEAD(&do_node->link_);

                                    attr = do_param->properties;
                                    while (attr) {
                                        if (attr->type == XML_ATTRIBUTE_NODE) {
                                            if (0 == xmlStrcmp(BAD_CAST"index", attr->name)) {
                                                do_node->__do_id[kSafetyProtectIdex_MainIndex] = covert_first_text_value_i(attr->xmlChildrenNode);
                                            }
                                            if (0 == xmlStrcmp(BAD_CAST"sub_index", attr->name)) {
                                                do_node->__do_id[kSafetyProtectIdex_SubIndex] = covert_first_text_value_i(attr->xmlChildrenNode);
                                            }
                                            if (0 == xmlStrcmp(BAD_CAST"ssub_index", attr->name)) {
                                                do_node->__do_id[kSafetyProtectIdex_SsubIndex] = covert_first_text_value_i(attr->xmlChildrenNode);
                                            } else if (0 == xmlStrcmp(BAD_CAST"data_src_type", attr->name)) {
                                                do_node->__dev_data_type = covert_first_text_value_i(attr->xmlChildrenNode);
                                            } else if (0 == xmlStrcmp(BAD_CAST"con_reslut", attr->name)) {
                                                do_node->__reslut = covert_first_text_value_i(attr->xmlChildrenNode);
                                            } else if (0 == xmlStrcmp(BAD_CAST"value", attr->name)) {
                                                do_node->__do_value = covert_first_text_value_i(attr->xmlChildrenNode);
                                            }
                                        }

                                        attr = attr->next;

                                    }
                                    list_add_tail(&do_node->link_, &bk_node->__set_do_list);
                                }
                                do_param = do_param->next;
                            }


                            list_add_tail(&bk_node->link_, &set_bank_node->__set_bank_list);
                        }
                        //


                        set_bank_param = set_bank_param->next;
                    }

                    list_add_tail(&set_bank_node->link_, &g_set_banks);

                }

                dev_dio_param = dev_dio_param->next;
            }
            list_add_tail(&node->link_, &g_safety_banks);
        }

        element = element->next;
    }

    return 0;
}

extern int safety__init() {
    memset(&g_sensor_triggered, 0, sizeof (g_sensor_triggered));
    if (xml__read_data(NULL, "safety.xml", "safety", safety_etc_loaded_handler)) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "xml__read_data safety.xml failed!");
        return -1;
    }
    if (xml__read_data(NULL, "banks.xml", "banks", safety_etc_loaded_bank_handler)) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "xml__read_data banks.xml failed!");
        return -1;
    }

    log__save("motion_template", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "safety__init success!");

    var__safety_t *sf = var__get_safety();
    if (sf) {
        sf->enable_ = g_cfg_enable;
        sf->enabled_ = sf->enable_;
    }
    var__release_object_reference(sf);

    return 0;
}

extern int safety__proc() {

    int cur_bank = 0;
    int level = 0;
    enum em_safety_protect_output_t out_slt = kSafetyProtectOutput_Normal;
    

    do {
        if (g_cfg_enable == 0) {
            break;
        }
        var__safety_t *sf = var__get_safety();
        if (sf) {
            if (g_sfw_enable != sf->enable_) {
                g_sfw_enable = sf->enable_;
                log__save("safety", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "g_sfw_enable = %d", g_sfw_enable);
            }
        }
        var__release_object_reference(sf);

        if (g_sfw_enable == 0) {
            break;
        }

        int v_bank = 0;
        int l_bank = 0;
        if (safety__get_cur_speed_bank(&v_bank, &level) < 0) {
            level = 0;
        } else {
            cur_bank = v_bank;
        }


        if (level <= SAFETY_LINE_BANK_LEVEL) {
            if (safety__get_cur_line_bank(&l_bank) >= 0) {
                cur_bank = l_bank;
                level = SAFETY_LINE_BANK_LEVEL;
            }
        }

        if (sf->manual_bank_id_ > 0) {
            if (sf->manual_bank_level_ >= level) {
                cur_bank = sf->manual_bank_id_;
                level = sf->manual_bank_level_;
            }

        }

        if ((g_last_bank != cur_bank || g_last_level != level) && g_bank_log) {
            log__save("safety", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "cur_bank=%d level=%d", cur_bank, level);
        }

        safety__bank_proc(cur_bank, &out_slt);
        safety__set_bank(cur_bank, out_slt);

        g_last_bank = cur_bank;
        g_last_level = level;

    } while (0);

    var__safety_t *sf = var__get_safety();
    if (sf) {
        sf->cur_bank_id_ = g_last_bank;
        sf->cur_bank_level = g_last_level;
        sf->safety_reslut_ = out_slt;
        if (g_cfg_enable == 0) {
            sf->enabled_ = 0;
        }
        else
        {
            sf->enabled_ = g_sfw_enable;
        }
        
        memcpy(&sf->sensor_trrigered_, &g_sensor_triggered, sizeof (st_safety_dev_bank_src_info));
    }
    var__release_object_reference(sf);

    var__vehicle_t*veh = var__get_vehicle();
    if (!veh) {
        log__save("motion_template", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "var__get_vehicle failed");
        return -1;
    }
    switch (out_slt) {
        case kSafetyProtectOutput_Stop:
            veh->stop_emergency_ = 1;
            break;
        case kSafetyProtectOutput_Slowdown:
            veh->slow_down_ = 1;
            veh->stop_emergency_ = 0;
            break;
        case kSafetyProtectOutput_Normal:
        default:
            veh->slow_down_ = 0;
            veh->stop_emergency_ = 0;
            break;
    }
    var__release_object_reference(veh);


    return 0;
}

extern
int safety__enable(posix__boolean_t en) {
    var__safety_t *sf = var__get_safety();
    if (sf) {
        sf->enable_ = en;
    }
    var__release_object_reference(sf);
    return 0;
}

static int safety__get_cur_line_bank(int* cur_bank) {
    int ret = 0;
    if (!cur_bank) {
        return -1;
    }
    var__navigation_t *nav = var__get_navigation();
    if (!nav) {
        return -1;
    }
    if (nav->i.current_edge_wop_properties_.enabled_ == 1) {
        *cur_bank = nav->i.current_edge_wop_properties_.wop_properties_[0];
    } else {
        ret = -2;
    }
    var__release_object_reference(nav);
    return ret;
}

static int safety__get_cur_speed_bank(int* cur_bank, int* level) {
    if (!cur_bank
            || !level) {
        return -1;
    }
    *level = 0;
    velocity_t cmd_velocity;
    var__vehicle_t *veh = var__get_vehicle();
    cmd_velocity = veh->i.command_velocity_;
    var__release_object_reference(veh);

    if (g_last_bank > 0) {
        if (fabs(cmd_velocity.x_) < 0.005
                && fabs(cmd_velocity.y_) < 0.005
                && fabs(cmd_velocity.w_) < 0.01) {
            *cur_bank = g_last_bank;
            *level = g_last_level;
            return 0;
        }
    }


    st_speed_bank_list_info* node;
    struct list_head *loop;

    st_speed_bank_info* v_node;
    struct list_head *v_loop;

    list_for_each(loop, &g_speed_banks) {
        node = containing_record(loop, st_speed_bank_list_info, link_);

        int con = 1;

        list_for_each(v_loop, &node->v_info) {
            v_node = containing_record(v_loop, st_speed_bank_info, link_);

            switch (v_node->v_type) {
                case kVtype_Vx:
                {
                    if (cmd_velocity.x_ > v_node->v_start
                            && cmd_velocity.x_ <= v_node->v_end) {
                        //*cur_bank = node->bank;                                    
                    } else {
                        con = 0;
                        break;
                    }
                }
                    break;
                case kVtype_Vy:
                {
                    if (cmd_velocity.y_ > v_node->v_start
                            && cmd_velocity.y_ <= v_node->v_end) {
                        //*cur_bank = node->bank;
                    } else {
                        con = 0;
                        break;
                    }
                }
                    break;
                case kVtype_W:
                {
                    if (cmd_velocity.w_ > v_node->v_start
                            && cmd_velocity.w_ <= v_node->v_end) {

                    } else {
                        con = 0;
                        break;
                    }
                }
                    break;
                default:
                    break;
            }

            if (con == 0) {
                break;
            }

        }

        if (con == 1) //满足条件
        {
            *cur_bank = node->bank;
            *level = node->level;
            return 0;
        }


    }
    return -1;
}

static int safety__bank_proc(int cur_bank, enum em_safety_protect_output_t * protect_result) {
    if (!protect_result) {
        return -1;
    }
    *protect_result = kSafetyProtectOutput_Normal;
    st_safety_bank_list_info *node;
    struct list_head *loop;

    st_dev_bank_src_list_info *senser;
    struct list_head *loop_sensor;

    list_for_each(loop, &g_safety_banks) {
        node = containing_record(loop, st_safety_bank_list_info, link_);
        if (node->__dev_banks.__bank_id == cur_bank) {

            list_for_each(loop_sensor, &node->__dev_banks.__dev_banks_list) {
                senser = containing_record(loop_sensor, st_dev_bank_src_list_info, link_);

                //对比
                enum em_safety_protect_output_t o;
                safety__check_sensor(&senser->sensor_, &o);
                if (o == kSafetyProtectOutput_Stop) {
                    *protect_result = o;
                    g_sensor_triggered = senser->sensor_;
                    break;
                }
                if (o == kSafetyProtectOutput_Slowdown) {
                    *protect_result = o;
                    g_sensor_triggered = senser->sensor_;
                }
            }

            if (node->__dev_banks.__resume_time > 0) {
                g_resume_time = node->__dev_banks.__resume_time;
            }
            break;
        }
    }

    if (*protect_result == kSafetyProtectOutput_Stop) {
        g_last_stop_time = 0;
    } else {
        if (g_last_out_rslt == kSafetyProtectOutput_Stop) {
            uint64_t cur_time = posix__clock_gettime();
            if (g_last_stop_time == 0)//第一次触发
            {
                g_last_stop_time = cur_time;
                log__save("safety", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout
                        , "safety protect ready to resume %d,cur_bank=%d, result = %d,last = %d,sensor:dev=%d channel=%d"
                        , g_resume_time, cur_bank, *protect_result, g_last_out_rslt, g_sensor_triggered.__src_dev_id, g_sensor_triggered.__data_src_channel[kSafetyProtectSignal_StopArea]);
            }

            if (cur_time - g_last_stop_time > g_resume_time * 10000) {
                g_last_out_rslt = *protect_result;
                g_last_stop_time = 0; //恢复
                log__save("safety", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout
                        , "safety protect resumed %d,cur_bank=%d, result = %d,last = %d", g_resume_time, cur_bank, *protect_result, g_last_out_rslt);
            } else//延时恢复
            {
                *protect_result = kSafetyProtectOutput_Stop;
                g_last_out_rslt = *protect_result;
                return 0;
            }
        }
    }

    if (g_last_bank > 0
            && cur_bank <= 0) {
        *protect_result = g_last_out_rslt;
    }

    if (g_last_out_rslt != *protect_result) {
        switch (*protect_result) {
            case kSafetyProtectOutput_Stop:
                log__save("safety", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout
                        , "safety protect stop,cur_bank=%d, result = %d,last = %d,sensor:dev=%d channel=%d"
                        , cur_bank, *protect_result, g_last_out_rslt, g_sensor_triggered.__src_dev_id, g_sensor_triggered.__data_src_channel[kSafetyProtectSignal_StopArea]);
                break;
            case kSafetyProtectOutput_Slowdown:
                log__save("safety", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout
                        , "safety protect slowdown,cur_bank=%d, result = %d,last = %d,sensor:dev=%d channel=%d"
                        , cur_bank, *protect_result, g_last_out_rslt, g_sensor_triggered.__src_dev_id, g_sensor_triggered.__data_src_channel[kSafetyProtectSignal_SlowdownArea]);
                break;
            case kSafetyProtectOutput_Normal:
                log__save("safety", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout
                        , "safety protect normal,cur_bank=%d, result = %d,last = %d"
                        , cur_bank, *protect_result, g_last_out_rslt);
                break;
            default:
                break;
        }
    }

    g_last_out_rslt = *protect_result;
    g_last_stop_time = 0;
    return 0;
}

static int safety__check_sensor(st_safety_dev_bank_src_info* sensor_bank, enum em_safety_protect_output_t * out_rslt) {
    if (!out_rslt) {
        return -1;
    }
    *out_rslt = kSafetyProtectOutput_Normal;
    switch (sensor_bank->__src_dev_type) {
        case kVarType_DIO:
        {
            var__dio_t* dio = var__get_dio_byid(sensor_bank->__src_dev_id);
            if (!dio) {
                break;
            }
            switch (sensor_bank->__dev_data_type) {
                case safety_data_type_digital:
                {
                    if (g_sensor_log) {
                        log__save("safety_debug", kLogLevel_Info, kLogTarget_Filesystem
                                , "dio id=%d,index=%d,di=%d",
                                sensor_bank->__src_dev_id,
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex],
                                ((dio->i.di_ >> sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex]) & 0x01));
                    }
                    sensor_bank->__di_cur = ((dio->i.di_ >> sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex]) & 0x01);
                    if ((sensor_bank->__di_mask & 0x01) ==
                            (sensor_bank->__di_cur & 0x01)) {
                        *out_rslt = sensor_bank->__reslut;
                        break;
                    }

                }
                    break;
                case safety_data_type_analog:
                {
                    sensor_bank->__ai_cur = dio->i.ai_[sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex]].data_[sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex]];
                    if (g_sensor_log) {
                        log__save("safety_debug", kLogLevel_Info, kLogTarget_Filesystem
                                , "dio id=%d,index=%d %d,ai=%.2f",
                                sensor_bank->__src_dev_id,
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex],
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex],
                                sensor_bank->__ai_cur);
                    }

                    if (sensor_bank->__ai_thres >= sensor_bank->__ai_cur) {
                        *out_rslt = sensor_bank->__reslut;
                        break;
                    }
                }
                    break;
                default:
                    break;
            }

            var__release_object_reference(dio);
        }
            break;
        case kVarType_OmronPLC:
        case kVarType_ModBus_TCP:
        {
            var__memory_netdev_t* mem = var__get_memory_netdev_byid(sensor_bank->__src_dev_id);
            if (!mem) {
                break;
            }
            switch (sensor_bank->__dev_data_type) {
                case safety_data_type_digital:
                {
                    if (g_sensor_log) {
                        log__save("safety_debug", kLogLevel_Info, kLogTarget_Filesystem
                                , "memory_netdev id=%d,index=%d %d %d,di=%d",
                                sensor_bank->__src_dev_id,
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex],
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex],
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_SsubIndex],
                                ((mem->readonly_blocks[sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex]].data_[sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex]] >> sensor_bank->__data_src_channel[kSafetyProtectIdex_SsubIndex]) & 0x01));
                    }
                    sensor_bank->__di_cur = ((mem->readonly_blocks[sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex]].data_[sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex]] >> sensor_bank->__data_src_channel[kSafetyProtectIdex_SsubIndex]) & 0x01);

                    if ((sensor_bank->__di_mask & 0x01) ==
                            (sensor_bank->__di_cur & 0x01)
                            ) {
                        *out_rslt = sensor_bank->__reslut;
                        break;
                    }

                }
                    break;
                case safety_data_type_analog:
                {
                    if (g_sensor_log) {
                        log__save("safety_debug", kLogLevel_Info, kLogTarget_Filesystem
                                , "memory_netdev id=%d,index=%d %d %d,ai=%d",
                                sensor_bank->__src_dev_id,
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex],
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex],
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_SsubIndex],
                                mem->readonly_blocks[sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex]].data_[sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex]]);
                    }
                    sensor_bank->__ai_cur = mem->readonly_blocks[sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex]].data_[sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex]];

                    if (sensor_bank->__ai_thres >= sensor_bank->__ai_cur) {
                        *out_rslt = sensor_bank->__reslut;
                        break;
                    }
                }
                    break;
                case safety_data_type_analogword:
                {
                    int word_val = mem->readonly_blocks[sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex]].data_[sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex]] << 8
                            | mem->readonly_blocks[sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex]].data_[sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex] + 1];
                    if (g_sensor_log) {
                        log__save("safety_debug", kLogLevel_Info, kLogTarget_Filesystem
                                , "memory_netdev id=%d,index=%d %d %d,word=%d",
                                sensor_bank->__src_dev_id,
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_MainIndex],
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_SubIndex],
                                sensor_bank->__data_src_channel[kSafetyProtectIdex_SsubIndex],
                                word_val
                                );
                    }
                    sensor_bank->__ai_cur = word_val;
                    if (sensor_bank->__ai_thres >= word_val) {
                        *out_rslt = sensor_bank->__reslut;


                        break;
                    }
                }
                default:
                    break;
            }
            var__release_object_reference(mem);
        }
            break;
        default:
            break;
    }

    return 0;
}

static int safety__set_bank(int bank, enum em_safety_protect_output_t o) {
    st_safety_set_bank_list_info *node_banks;
    struct list_head *loop_banks;

    st_safety_dev_bank_dst_info *node_dev;
    struct list_head *loop_dev;

    list_for_each(loop_banks, &g_set_banks) {
        node_banks = containing_record(loop_banks, st_safety_set_bank_list_info, link_);
        if (node_banks->__bank_id == bank) {

            list_for_each(loop_dev, &node_banks->__set_bank_list) {
                node_dev = containing_record(loop_dev, st_safety_dev_bank_dst_info, link_);
                safety__set_do(node_dev, o);
            }
        }
    }

    return 0;
}

static int safety__set_do(st_safety_dev_bank_dst_info* dev, enum em_safety_protect_output_t o) {

    switch (dev->__dst_dev_type) {
        case kVarType_DIO:
        {
            var__dio_t* dio = var__get_dio_byid(dev->__dst_dev_id);
            if (!dio) {
                break;
            }

            st_safety_set_do_info *node_do;
            struct list_head *loop_do;
            int do_t = dio->i.do2_;

            list_for_each(loop_do, &dev->__set_do_list) {
                node_do = containing_record(loop_do, st_safety_set_do_info, link_);
                if (safety__check_dev_ctrl_con(node_do, o) < 0)
                    continue;
                switch (node_do->__dev_data_type) {
                    case safety_data_type_digital:
                    {
                        if (node_do->__do_value == 1) {
                            do_t = do_t | (0x01 << node_do->__do_id[kSafetyProtectIdex_MainIndex]);
                        } else {
                            do_t = do_t & (~((int) 0x01 << node_do->__do_id[kSafetyProtectIdex_MainIndex]));
                        }
                    }
                        break;
                    case safety_data_type_analog:
                    {
                        dio->ao_[node_do->__do_id[kSafetyProtectIdex_MainIndex]].data_[node_do->__do_id[kSafetyProtectIdex_SubIndex]] = node_do->__do_value;
                    }
                        break;
                    default:
                        break;
                }
            }

            dio->do_ = do_t;
            var__release_object_reference(dio);
        }
            break;
        case kVarType_OmronPLC:
        case kVarType_ModBus_TCP:
        {
            var__memory_netdev_t* mem = var__get_memory_netdev_byid(dev->__dst_dev_id);
            if (!mem) {
                break;
            }
            st_safety_set_do_info *node_do;
            struct list_head *loop_do;

            list_for_each(loop_do, &dev->__set_do_list) {
                node_do = containing_record(loop_do, st_safety_set_do_info, link_);
                if (safety__check_dev_ctrl_con(node_do, o) < 0)
                    continue;
                switch (node_do->__dev_data_type) {
                    case safety_data_type_digital:
                    {
                        if (node_do->__do_value == 1) {
                            mem->writeable_blocks_[node_do->__do_id[kSafetyProtectIdex_MainIndex]].data_[node_do->__do_id[kSafetyProtectIdex_SubIndex]] |= (0x01 << node_do->__do_id[kSafetyProtectIdex_SsubIndex]);
                        } else {
                            mem->writeable_blocks_[node_do->__do_id[kSafetyProtectIdex_MainIndex]].data_[node_do->__do_id[kSafetyProtectIdex_SubIndex]] &= (~(0x01 << node_do->__do_id[kSafetyProtectIdex_SsubIndex]));
                        }
                        mem->writeable_blocks_[node_do->__do_id[kSafetyProtectIdex_MainIndex]].flush_ = 1;
                    }
                        break;
                    case safety_data_type_analog:
                    {
                        mem->writeable_blocks_[node_do->__do_id[kSafetyProtectIdex_MainIndex]].data_[node_do->__do_id[kSafetyProtectIdex_SubIndex]] = node_do->__do_value;
                        mem->writeable_blocks_[node_do->__do_id[kSafetyProtectIdex_MainIndex]].flush_ = 1;

                    }
                        break;
                    default:
                        break;
                }
            }
            var__release_object_reference(mem);
        }
            break;
        default:
            break;
    }
    return 0;
}

static int safety__check_dev_ctrl_con(st_safety_set_do_info * do_set, enum em_safety_protect_output_t o) {
    if (do_set->__reslut != kSafetyProtectOutput_None) {
        if (o != do_set->__reslut) {
            return -1;
        }
    }

    return 0;
}

int safety__set_manual_bank(int bank_id, int level) {
    g_manual_bank = bank_id;
    g_manual_bank_lv = level;
    return 0;
}