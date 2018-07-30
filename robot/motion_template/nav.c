#include "version.h"

#include "nav.h"
#include "operation.h"
#include "v_decomp.h"
#include "math.h"
#include "map.h"
#include "PosandTrack.h"
#include "logger.h"
#include "nsp.h"
#include "proto_typedef.h"
#include "error.h"
#include "posix_time.h"

#define PRINT_NAV_DEBUG_LOG 1
#define PRINT_CALIB_LOG 1
#define IF_NAV_DEBUG 0

enum NAV_PROC_STATE {
    INIT = 0,
    SEG_SMOOTH,
    SEG_PARTITION_LAST,
    SEG_GLOBAL_LAST,
    SEG_FINAL_ROTATE,
    SEG_WAIT_TRAFFIC,
    FINISH,
};
static const double g_thres_is_same_piont = 1.0;

static
int g_proc_state = FINISH;
static
int g_rotation_mode = 0;
static int nav__mapping_upl(var__navigation_t *nav);
static int nav__traj_control_proc(var__navigation_t *nav, var__vehicle_t *veh, const struct list_head *drive_unit_entry, double* v_x, double *v_y, double* w, double deta_time);
static int nav__is_forward(const var__way_of_pass_t* wop);
static void nav__check_proc_state(var__navigation_t *nav);
static int nav__cal_nav_poionts(var__navigation_t *nav);
static int nav__cal_nav_poionts_ext(var__navigation_t *nav);
static void nav__cal_dis_left(var__navigation_t *nav);
static int nav__check_path(var__navigation_t *nav);
#if PRINT_NAV_DEBUG_LOG
static void nav__log(var__navigation_t *nav, var__vehicle_t *veh);
#endif

#if PRINT_CALIB_LOG
static void nav__log_calib(var__navigation_t *nav, var__vehicle_t *veh);
#endif

static int nav__get_global_wop_angle(const var__way_of_pass_t* wop, const var__edge_t* edge, const double t, double* global_wop_angle);
static int nav__is_partition(trail_t* cur_trail_edge, trail_t* nxt_trail_edge);
static int nav__is_upl_same_pos(upl_t upl, position_t pos);
static void nav_pos_interpolated(var__navigation_t *nav, var__vehicle_t *veh);
static int nav__update_upl(var__navigation_t *nav);
static int nav__update_bank(var__navigation_t *nav);

static double v_last = 0;
static double v_line_limit = 0;
static double nav_angle_last = 0;
static double aim_goal_angle = 0;
static int first_nav_angle = 0;
static position_t nav_pos;
static uint64_t time_stamp_nav_pos = 0;
static uint64_t time_stamp_c = 0;
static int firsttime = 0;
static uint64_t time_stamp_last = 0;
static double v_sita = 0;
static double ey = 0;
static double e_angle = 0;
static double w_temp = 0;
static int g_loc_rcv_cnt = 0;
static int g_loc_init_cnt = 0;
static int g_last_ctrl_mode = -1;
static int g_simflag = -1;
static int g_is_track_err = 0;
static double g_average_loc_delay = 0;

#if _WIN32
static int firstfream = 0;
static int num_fream = 0;
static double g_average_odo_delay = 0;
#endif

static int g_confidence_err = 0;

static void nav_pos_interpolated(var__navigation_t *nav, var__vehicle_t *veh) {
    if (g_simflag > 0) {
        nav->pos_.x_ = veh->i.odo_meter_.x_;
        nav->pos_.y_ = veh->i.odo_meter_.y_;
        nav->pos_.angle_ = veh->i.odo_meter_.angle_;
        nav->pos_time_stamp_ = veh->i.time_stamp_;
        nav->pos_confidence_ = 1;

        nav_pos.x_ = nav->pos_.x_;
        nav_pos.y_ = nav->pos_.y_;
        nav_pos.angle_ = nav->pos_.angle_;
        return;
    }
#if IF_NAV_DEBUG
    nav->pos_.x_ = veh->i.odo_meter_.x_;
    nav->pos_.y_ = veh->i.odo_meter_.y_;
    nav->pos_.angle_ = veh->i.odo_meter_.angle_;
    nav->pos_time_stamp_ = veh->i.time_stamp_;
    nav->pos_confidence_ = 1;

    nav_pos.x_ = nav->pos_.x_;
    nav_pos.y_ = nav->pos_.y_;
    nav_pos.angle_ = nav->pos_.angle_;
    return;
#endif
    if (time_stamp_nav_pos != nav->pos_time_stamp_) {
        nav_pos.x_ = nav->pos_.x_;
        nav_pos.y_ = nav->pos_.y_;
        nav_pos.angle_ = nav->pos_.angle_;
        time_stamp_nav_pos = nav->pos_time_stamp_;
        time_stamp_c = nav->pos_time_stamp_;
    }

#if !_WIN32
    uint64_t current_time = (uint64_t) (posix__clock_epoch() / 10000.0);
#else
    uint64_t current_time = (uint64_t) (posix__clock_gettime() / 10000.0);
#endif

	double deta_cur_nav = 0;
	if (current_time < time_stamp_c)
	{
		deta_cur_nav = 0;
		log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "current time < nav_time,current: "UINT64_STRFMT" nav_time: "UINT64_STRFMT, current_time, time_stamp_c);
	}
	else
	{
		deta_cur_nav = (double)((current_time - time_stamp_c) / 1000.0);
	}
    var__error_handler_t * err = var__get_error_handler();
    if (deta_cur_nav > err->localization_timeout_) {
        err->error_[kVarFixedObject_Navigation].status = -1;
        err->error_[kVarFixedObject_Navigation].framwork = var__make_error_code(kVarType_Navigation, kNav_ErrorLocTimeOut);
    }
    if (g_average_loc_delay != 0) {
        g_average_loc_delay = (g_average_loc_delay + deta_cur_nav) / 2;
    } else {
        g_average_loc_delay = deta_cur_nav;
    }
    if (g_average_loc_delay > err->average_localization_delay_threshold_) {
        err->error_[kVarFixedObject_Navigation].status = -1;
        err->error_[kVarFixedObject_Navigation].framwork = var__make_error_code(kVarType_Navigation, kNav_ErrorLocAverageTimeOut);
    }
    var__release_object_reference(err);


    nav_pos.angle_ = nav_pos.angle_/* + deta_cur_nav*veh->i.actual_velocity_.w_*/;
    nav_pos.x_ = nav_pos.x_ + deta_cur_nav * veh->i.actual_velocity_.x_ * cos(nav_pos.angle_) - deta_cur_nav * veh->i.actual_velocity_.y_ * sin(nav_pos.angle_);
    nav_pos.y_ = nav_pos.y_ + deta_cur_nav * veh->i.actual_velocity_.x_ * sin(nav_pos.angle_) + deta_cur_nav * veh->i.actual_velocity_.y_ * cos(nav_pos.angle_);
    time_stamp_c = current_time;
}

static void nav__cal_dis_left(var__navigation_t *nav) {

    if (nav->traj_ref_.count_ <= 0) {
        nav->i.dist_to_dest_ = 0;
        nav->i.dist_to_partition_ = 0;
        return;
    }
    if (nav->i.traj_ref_index_curr_ >= nav->traj_ref_.count_) {
        nav->i.dist_to_dest_ = 0;
        nav->i.dist_to_partition_ = 0;
        return;
    }
    int i;
    if (nav->i.traj_ref_index_curr_ == (nav->traj_ref_.count_ - 1)
            && nav->is_traj_whole_ == 1) {
        trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
        double t_close_p_online = 0;
        if (GetTClosestOnLine(nav_pos, cur_trail_edge->edge_id_, &t_close_p_online) < 0) {
            nav->i.dist_to_dest_ = 0;
            nav->i.dist_to_partition_ = 0;
            return;
        }
        double t_dest_p_online = 0;
        if (GetTClosestOnLine(nav->dest_pos_, cur_trail_edge->edge_id_, &t_dest_p_online) < 0) {
            nav->i.dist_to_dest_ = 0;
            nav->i.dist_to_partition_ = 0;
            return;
        }

        const var__edge_t* cur_edge = var__search_edge(cur_trail_edge->edge_id_);
        const var__way_of_pass_t* cur_wop = var__search_wop(cur_trail_edge->wop_id_);
        if (!cur_edge || !cur_wop) {
            nav->i.dist_to_dest_ = 0;
            nav->i.dist_to_partition_ = 0;
            return;
        }
        double per = 0;
        if (GetpercentbyT(t_close_p_online, cur_edge, &per) < 0) {
            nav->i.dist_to_dest_ = 0;
            nav->i.dist_to_partition_ = 0;
            return;
        }

        double dest_per = 0;
        if (GetpercentbyT(t_dest_p_online, cur_edge, &dest_per) < 0) {
            nav->i.dist_to_dest_ = 0;
            nav->i.dist_to_partition_ = 0;
            return;
        }
        if (nav__is_forward(cur_wop) == 1) {
            nav->i.dist_to_dest_ = cur_edge->length_ * (dest_per - per) / 100;
            nav->i.dist_to_partition_ = cur_edge->length_ * (dest_per - per) / 100;
        } else {
            nav->i.dist_to_dest_ = cur_edge->length_ * (per - dest_per) / 100;
            nav->i.dist_to_partition_ = cur_edge->length_ * (per - dest_per) / 100;
        }

        if (nav->i.dist_to_dest_ < 0) {
            nav->i.dist_to_dest_ = 0;
            nav->i.dist_to_partition_ = 0;
        }
        var__end_search();
        var__end_search();
        return;
    }

    trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
    double t_close_p_online = 0;
    if (GetTClosestOnLine(nav_pos, cur_trail_edge->edge_id_, &t_close_p_online) < 0) {
        nav->i.dist_to_dest_ = 0;
        nav->i.dist_to_partition_ = 0;
        return;
    }
    // double aim_dis = nav->aim_dist_;
    // double predict_dis = fabs(nav->predict_time_ / 1000.0 * v_last);
    //log__save("pridict", kLogLevel_Info, kLogTarget_Filesystem, "%.5f %.5f", predict_dis, v_last);
    const var__edge_t* cur_edge = var__search_edge(cur_trail_edge->edge_id_);
    const var__way_of_pass_t* cur_wop = var__search_wop(cur_trail_edge->wop_id_);
    if (!cur_edge || !cur_wop) {
        nav->i.dist_to_dest_ = 0;
        nav->i.dist_to_partition_ = 0;
        return;
    }

    double per = 0;
    if (GetpercentbyT(t_close_p_online, cur_edge, &per) < 0) {
        nav->i.dist_to_dest_ = 0;
        nav->i.dist_to_partition_ = 0;
        return;
    }
    if (nav__is_forward(cur_wop)) {
        nav->i.dist_to_dest_ = cur_edge->length_ * (100 - per) / 100;
        nav->i.dist_to_partition_ = cur_edge->length_ * (100 - per) / 100;
    } else {
        nav->i.dist_to_dest_ = cur_edge->length_ * per / 100;
        nav->i.dist_to_partition_ = cur_edge->length_ * per / 100;
    }

    var__end_search();
    var__end_search();

    int is_partition = 0;
    for (i = nav->i.traj_ref_index_curr_ + 1; i < nav->traj_ref_.count_; ++i) {
        trail_t* nxt_trail_edge = &((trail_t*) nav->traj_ref_.data_)[i];
        const var__edge_t* nxt_edge = var__search_edge(nxt_trail_edge->edge_id_);
        const var__way_of_pass_t* nxt_wop = var__search_wop(nxt_trail_edge->wop_id_);
        if (!nxt_edge || !nxt_wop) {
            continue;
        }

        if (is_partition == 0) {
            if (nav__is_partition(cur_trail_edge, nxt_trail_edge) == 1) {
                is_partition = 1;
            }

            if (nav__check_path(nav) != 1)
            {
                is_partition = 1;
            }
        }
        if (i == nav->traj_ref_.count_ - 1 && nav->is_traj_whole_ == 1) {
            double t_dest_p_online = 0;
            if (GetTClosestOnLine(nav->dest_pos_, nxt_trail_edge->edge_id_, &t_dest_p_online) < 0) {
                nav->i.dist_to_dest_ = 0;
                nav->i.dist_to_partition_ = 0;
                return;
            }

            double dest_per = 0;
            if (GetpercentbyT(t_dest_p_online, nxt_edge, &dest_per) < 0) {
                nav->i.dist_to_dest_ = 0;
                nav->i.dist_to_partition_ = 0;
                return;
            }
            if (nav__is_forward(nxt_wop) == 1) {
                nav->i.dist_to_dest_ += nxt_edge->length_ * (dest_per) / 100;
                if (is_partition == 0) {
                    nav->i.dist_to_partition_ += nxt_edge->length_ * (dest_per) / 100;
                }
            } else {
                nav->i.dist_to_dest_ += nxt_edge->length_ * (100 - dest_per) / 100;
                if (is_partition == 0) {
                    nav->i.dist_to_partition_ += nxt_edge->length_ * (100 - dest_per) / 100;
                }
            }

            if (nav->i.dist_to_dest_ < 0) {
                nav->i.dist_to_dest_ = 0;
            }
            if (nav->i.dist_to_partition_ < 0) {
                nav->i.dist_to_partition_ = 0;
            }

        } else {
            nav->i.dist_to_dest_ += nxt_edge->length_;
            if (is_partition == 0) {
                nav->i.dist_to_partition_ += nxt_edge->length_;
            }
        }

        cur_trail_edge = nxt_trail_edge;

        var__end_search();
        var__end_search();
    }
}

static void print_cur_traj(var__navigation_t *nav, char* traj, int l) {
    //char traj[1000];
    memset(traj, 0, l);
    int i = 0;
    int len = 0;
    for (i = 0; i < nav->traj_ref_.count_; ++i) {
        trail_t* t = &((trail_t*) nav->traj_ref_.data_)[i];
#if _WIN32
		len += sprintf_s(traj + len, l - len, "[%d %d]", t->edge_id_, t->wop_id_);
#else
        len += sprintf(traj + len, "[%d %d]", t->edge_id_, t->wop_id_);
#endif
        if (len > l - 20) {
            log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "traj buf not enough,print maybe imcomplete!");
            break;
        }
    }

    log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
        "cur traj[%d in %d]: %s", nav->i.traj_ref_index_curr_,nav->traj_ref_.count_, traj);
}

extern
int nav__traj_control(var__navigation_t *nav, var__vehicle_t *veh, const struct list_head *drive_unit_entry, posix__boolean_t simflag) {
    g_simflag = simflag;
    if (g_simflag > 0) {
        nav->pos_.x_ = veh->i.odo_meter_.x_;
        nav->pos_.y_ = veh->i.odo_meter_.y_;
        nav->pos_.angle_ = veh->i.odo_meter_.angle_;
        nav->pos_time_stamp_ = veh->i.time_stamp_;
        nav->pos_confidence_ = 1;
    }
#if IF_NAV_DEBUG
    nav->pos_.x_ = veh->i.odo_meter_.x_;
    nav->pos_.y_ = veh->i.odo_meter_.y_;
    nav->pos_.angle_ = veh->i.odo_meter_.angle_;
    nav->pos_time_stamp_ = veh->i.time_stamp_;
    nav->pos_confidence_ = 1;
#endif

    double v_x = 0;
    double v_y = 0;
    double w = 0;

    uint64_t time_current = (uint64_t) (posix__clock_gettime() / 10000.0);
    if (firsttime == 0) {
        time_stamp_last = time_current;
        firsttime = 1;
    }
	double deta_time = (double)((int64_t)time_current - (int64_t)time_stamp_last) / 1000.0;

    velocity_t v_temp;

    if (!nav || !veh || !drive_unit_entry) { 
        return -1;
    }
    
    v_temp.x_ = 0;
    v_temp.y_ = 0;
    v_temp.w_ = 0;
    v_last = sqrt(veh->i.command_velocity_.x_ * veh->i.command_velocity_.x_ + veh->i.command_velocity_.y_ * veh->i.command_velocity_.y_);

    if (veh->control_mode_ == kVehicleControlMode_Manual) {
        v_x = veh->manual_velocity_.x_;
        v_y = veh->manual_velocity_.y_;
        w = veh->manual_velocity_.w_;

        if (nav->track_status_.response_ != kStatusDescribe_Hang) {
            if (nav->track_status_.response_ < kStatusDescribe_FinalFunction
                    && nav->track_status_.response_ > kStatusDescribe_PendingFunction) {
                g_proc_state = FINISH;
                //非暂停状态，取消导航
                var__xchange_middle_status(&nav->track_status_, kStatusDescribe_Idle, NULL);
                var__xchange_response_status(&nav->track_status_, kStatusDescribe_Terminated, NULL);
                nsp__report_status(nav->user_task_id_, VAR_TYPE_NAVIGATION, kStatusDescribe_Terminated, 0, 0);
                log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "nav task is force canceled by manual control!");
            }

            if (nav->pos_confidence_ > 0.5) {
                int e = nav__mapping_upl(nav);
                if (e < 0) {
                    log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "nav__mapping_upl failed!");
                }

            } else {
                nav->i.upl_.edge_id_ = -1;

            }

            if (nav->i.upl_.edge_id_ <= 0 && nav->pos_confidence_ < 0.5) {
                g_loc_init_cnt++;
            } else {
                g_loc_init_cnt = 0;
            }

            if (g_loc_init_cnt > 1000) {
                var__mark_software_error(kVarFixedObject_Navigation, var__make_error_code(kVarType_Navigation, kNav_ErrorLocNotInit));
                log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "loc timeout on startup");
            }

        } else {
            ;
        }

        if (g_last_ctrl_mode != veh->control_mode_) {
            log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, 
					"veh->control_mode_ = kVehicleControlMode_Manual!");
        }
        g_last_ctrl_mode = veh->control_mode_;
        
    } else if (kVehicleControlMode_Calibrated == veh->control_mode_) {
        v_x = veh->manual_velocity_.x_;
        v_y = veh->manual_velocity_.y_;
        w = veh->manual_velocity_.w_;

        if (g_last_ctrl_mode != veh->control_mode_) {
            log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "veh->control_mode_ = kVehicleControlMode_Calibrated!");
        }
        g_last_ctrl_mode = veh->control_mode_;
    } else if (veh->control_mode_ == kVehicleControlMode_Navigation) {

        if (g_last_ctrl_mode != veh->control_mode_) {
            log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "veh->control_mode_ = kVehicleControlMode_Navigation!");
        }
        g_last_ctrl_mode = veh->control_mode_;
        if (nav->i.upl_.edge_id_ <= 0 && nav->pos_confidence_ > 0.5) {
            g_loc_rcv_cnt++;
        }

        if (g_loc_rcv_cnt > 20) {
            int e = nav__mapping_upl(nav);
            if (e < 0) {
                log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "nav__mapping_upl failed!");
            }

            log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "nav__mapping_upl:"
                    "pos (%.5f,%.5f,%.5f)"
                    "upl (%d,%.5f,%.5f)"
                    , nav->pos_.x_, nav->pos_.y_, nav->pos_.angle_
                    , nav->i.upl_.edge_id_, nav->i.upl_.percentage_, nav->i.upl_.angle_
                    );

            g_loc_rcv_cnt = 0;
        }

        if (nav->i.upl_.edge_id_ <= 0 && nav->pos_confidence_ < 0.5) {
            g_loc_init_cnt++;
        } else {
            g_loc_init_cnt = 0;
        }

        if (g_loc_init_cnt > 10000) {
            var__mark_software_error(kVarFixedObject_Navigation, var__make_error_code(kVarType_Navigation, kNav_ErrorLocNotInit));
            log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "loc timeout on startup");
        }

        if (nav->track_status_.command_ == kStatusDescribe_Startup && nav->track_status_.middle_ == kStatusDescribe_Idle) {

            var__xchange_middle_status(&nav->track_status_, kStatusDescribe_Running, NULL);
            var__xchange_response_status(&nav->track_status_, kStatusDescribe_Running, NULL);
            nav->i.current_task_id_ = nav->user_task_id_;
            nsp__report_status(nav->user_task_id_, VAR_TYPE_NAVIGATION, kStatusDescribe_Running, 0, 0);
            nav->i.traj_ref_index_curr_ = 0;
            g_proc_state = INIT;
            nav->i.tracking_error_ = 0;
            nav->i.on_last_segment_ = 0;
            log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "get new task:"
                    "dest_pos (%.5f,%.5f,%.5f)"
                    "dest_upl (%d,%.5f,%.5f)"
                    "task_id="UINT64_STRFMT
                    , nav->dest_pos_.x_, nav->dest_pos_.y_, nav->dest_pos_.angle_
                    , nav->dest_upl_.edge_id_, nav->dest_upl_.percentage_, nav->dest_upl_.angle_
                    ,nav->user_task_id_);

            //切换一周期，保证状态转移生效
            return 0;
        }

        if (nav->track_status_.middle_ == kStatusDescribe_Running) {
            switch (nav->track_status_.command_) {
                case kStatusDescribe_Pause:
                    v_x = 0;
                    v_y = 0;
                    w = 0;
                    trim(v_x, v_y, w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
                    v_x = v_temp.x_;
                    v_y = v_temp.y_;
                    w = v_temp.w_;
                    if (fabs(veh->i.command_velocity_.x_) < 0.001
                            && fabs(veh->i.command_velocity_.y_) < 0.001) {

                        if (nav->track_status_.response_ != kStatusDescribe_Hang) {
                            log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                                    "nav task paused!");
                            var__xchange_response_status(&nav->track_status_, kStatusDescribe_Hang, NULL);
                            nsp__report_status(nav->user_task_id_, VAR_TYPE_NAVIGATION, kStatusDescribe_Hang, 0, 0);
                        }

                    } else {
                        log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "nav task pause!");
                    }
                    //var__xchange_middle_status( &nav->track_status_, kStatusDescribe_Hang, NULL );
                    //var__xchange_response_status(&nav->track_status_, kStatusDescribe_Hang, NULL);

                    break;
                case kStatusDescribe_Cancel:
                    g_proc_state = FINISH;
                    nav->i.tracking_error_ = 0;
                    v_x = 0;
                    v_y = 0;
                    w = 0;
                    trim(v_x, v_y, w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
                    v_x = v_temp.x_;
                    v_y = v_temp.y_;
                    w = v_temp.w_;
                    if (nav->track_status_.response_ < kStatusDescribe_FinalFunction
                            && nav->track_status_.response_ > kStatusDescribe_PendingFunction) {

                        if (fabs(veh->i.command_velocity_.x_) < 0.001
                                && fabs(veh->i.command_velocity_.y_) < 0.001) {
                            if (nav->track_status_.response_ != kStatusDescribe_Terminated) {

                                log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "nav task canceled!");
                                var__xchange_middle_status(&nav->track_status_, kStatusDescribe_Idle, NULL);
                                var__xchange_response_status(&nav->track_status_, kStatusDescribe_Terminated, NULL);

                                nsp__report_status(nav->user_task_id_, VAR_TYPE_NAVIGATION, kStatusDescribe_Terminated, 0, 0);
                            }
                        } else {
                            log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "nav task cancel!");
                        }
                    }
                    break;
                case kStatusDescribe_Resume:
                    //if (nav->pos_confidence_ > 0.5) {
                    //    int e = nav__mapping_upl(nav);
                    //    if (e < 0) {
                    //        log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout,
                    //            "nav__mapping_upl failed!");
                    //        //return e;
                    //    }
                    //}
                    if (nav->track_status_.response_ == kStatusDescribe_Hang) {
                        log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                                "nav task resume!");
                        nsp__report_status(nav->user_task_id_, VAR_TYPE_NAVIGATION, kStatusDescribe_Running, 0, 0);
                    }

                    var__xchange_middle_status(&nav->track_status_, kStatusDescribe_Running, NULL);
                    var__xchange_response_status(&nav->track_status_, kStatusDescribe_Running, NULL);
                default:
                    break;
            }
            if (nav->traj_ref_.count_ <= 0 || (!nav->traj_ref_.data_)) {
                return 0;
            }
            if (nav->pos_confidence_ > 0.5) {
                if (g_confidence_err != 0)
                {
                    log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "pos_confidence_ error resume");
                }
                g_confidence_err = 0;    
                nav_pos_interpolated(nav, veh);
                nav__cal_dis_left(nav);

                nav__update_bank(nav);
                int e = nav__traj_control_proc(nav, veh, drive_unit_entry, &v_x, &v_y, &w, deta_time);
                if (e < 0) {
                    char traj[1000];
                    print_cur_traj(nav, traj, 999);
                    var__mark_software_error(kVarFixedObject_Navigation, var__make_error_code(kVarType_Navigation, kNav_ErrorTraj));
                    log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "nav__traj_control_proc error,%s", traj);
                    v_x = 0;
                    v_y = 0;
                    w = 0;
                    trim(v_x, v_y, w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
                    v_x = v_temp.x_;
                    v_y = v_temp.y_;
                    w = v_temp.w_;
                }

                if (nav->i.tracking_error_) {
                    if (g_is_track_err != nav->i.tracking_error_) {
                        char traj[1000];
                        print_cur_traj(nav, traj, 999);
                        log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "tracking_error,%s", traj);
                    }

                    v_x = 0;
                    v_y = 0;
                    w = 0;
                    trim(v_x, v_y, w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
                    v_x = v_temp.x_;
                    v_y = v_temp.y_;
                    w = v_temp.w_;

                    var__mark_software_error(kVarFixedObject_Navigation, var__make_error_code(kVarType_Navigation, kNav_ErrorTracking));
                } else {
                    if (g_is_track_err != nav->i.tracking_error_) {
                        log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "tracking resume!");
                    }
                }

                g_is_track_err = nav->i.tracking_error_;
            } else {
                if (g_confidence_err != 1)
                {
                    log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "pos_confidence_ < 0.5!");
                }
                g_confidence_err = 1;
                v_x = 0;
                v_y = 0;
                w = 0;
                trim(v_x, v_y, w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
                v_x = v_temp.x_;
                v_y = v_temp.y_;
                w = v_temp.w_;

                var__mark_software_error(kVarFixedObject_Navigation, var__make_error_code(kVarType_Navigation, kNav_ErrorLocConfidence));
            }
        } else {
            v_x = 0;
            v_y = 0;
            w = 0;
            trim(v_x, v_y, w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
            v_x = v_temp.x_;
            v_y = v_temp.y_;
            w = v_temp.w_;
        }

        if (veh->stop_normal_) {
            //log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            //	"veh->stop_normal!");
            v_x = 0;
            v_y = 0;
            w = 0;
            trim(v_x, v_y, w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
            v_x = v_temp.x_;
            v_y = v_temp.y_;
            w = v_temp.w_;
        }

        if (veh->stop_emergency_) {
            //log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            //	"veh->stop_emergency!");
            v_x = 0;
            v_y = 0;
            w = 0;
            trim(v_x, v_y, w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
            v_x = v_temp.x_;
            v_y = v_temp.y_;
            w = v_temp.w_;
        }
        if (VEH_HEALTHY != veh->fault_stop_) {
            //log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
            //	"veh->stop_emergency!");
            v_x = 0;
            v_y = 0;
            w = 0;
            trim(v_x, v_y, w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
            v_x = v_temp.x_;
            v_y = v_temp.y_;
            w = v_temp.w_;
        }

        veh->i.ref_velocity_.x_ = v_x;
        veh->i.ref_velocity_.y_ = v_y;
        veh->i.ref_velocity_.w_ = w;

    }
    if (veh->control_mode_ == kVehicleControlMode_Navigation && (veh->chassis_type_ & 0x03) == 3) {
        double v_com = 0;
        v_com = sqrt(v_x * v_x + v_y * v_y);
        double v_com_temp = 0;
        trim2(v_com, nav, veh, deta_time, NON, &v_com_temp);
        v_x = v_com_temp * cos(v_sita);
        v_y = v_com_temp * sin(v_sita);
    } else {
        trim(v_x, v_y, w, nav, veh, deta_time, VEHICLE_LIMIT, &v_temp);
        v_x = v_temp.x_;
        v_y = v_temp.y_;
    }
    trim(v_x, v_y, w, nav, veh, deta_time, VEHICLE_LIMIT, &v_temp);
    w = v_temp.w_;

    veh->i.command_velocity_.x_ = v_x;
    veh->i.command_velocity_.y_ = v_y;
    veh->i.command_velocity_.w_ = w;

#if _WIN32
    v_chassis *V_command_com = (v_chassis*) malloc(sizeof (v_chassis)* 2);
    if (VwResolution(V_command_com, veh, drive_unit_entry) == 0) {
        V_command_com[1].V_x = veh->i.actual_velocity_.x_;
        V_command_com[1].V_y = veh->i.actual_velocity_.y_;
        V_command_com[1].W = veh->i.actual_velocity_.w_;
    }
    if (!((veh->chassis_type_ & 0x03) == 3)) {
        veh->i.command_velocity_.x_ = V_command_com[0].V_x;
        veh->i.command_velocity_.y_ = V_command_com[0].V_y;
        veh->i.command_velocity_.w_ = V_command_com[0].W;
    }
    if (firstfream == 0) {
        if (g_simflag <= 0) {
            veh->i.odo_meter_.x_ = 0;
            veh->i.odo_meter_.y_ = 0;
        }
        num_fream++;
        if (num_fream == 10) {
            firstfream = 1;
        }
    } else {
        double deta_t = 0;

        if (g_simflag > 0) {
            deta_t = 0.02;
            veh->i.odo_meter_.theta_ += V_command_com[0].W*deta_t;
            veh->i.odo_meter_.theta_ = NormalAngle(veh->i.odo_meter_.theta_);
            veh->i.odo_meter_.x_ += V_command_com[0].V_x * cos(veh->i.odo_meter_.theta_) * deta_t - V_command_com[0].V_y * sin(veh->i.odo_meter_.theta_) * deta_t;
            veh->i.odo_meter_.y_ += V_command_com[0].V_x * sin(veh->i.odo_meter_.theta_) * deta_t + V_command_com[0].V_y * cos(veh->i.odo_meter_.theta_) * deta_t;
        } else {
            uint64_t current_time = (uint64_t) (posix__clock_gettime() / 10000.0);
			double deta_odo_nav = (double)((int64_t)current_time - (int64_t)V_command_com[1].time_stamp) / 1000.0;
            var__error_handler_t * err = var__get_error_handler();
            if (deta_odo_nav > err->odo_timeout_) {
                err->error_[kVarFixedObject_Navigation].status = -1;
                err->error_[kVarFixedObject_Navigation].software = var__make_error_code(kVarType_Navigation, kNav_ErrorOdoTimeOut);
            }
            if (g_average_odo_delay != 0) {
                g_average_odo_delay = (g_average_loc_delay + deta_odo_nav) / 2;
            } else {
                g_average_odo_delay = deta_odo_nav;
            }
            if (g_average_odo_delay > err->average_odo_delay_threshold_) {
                err->error_[kVarFixedObject_Navigation].status = -1;
                err->error_[kVarFixedObject_Navigation].software = var__make_error_code(kVarType_Navigation, kNav_ErrorOdoAverageTimeOut);
            }
            var__release_object_reference(err);

			deta_t = fabs((double)((int64_t)V_command_com[1].time_stamp - (int64_t)veh->i.time_stamp_)) / 1000.0;
            veh->i.odo_meter_.theta_ += V_command_com[1].W*deta_t;
            veh->i.odo_meter_.theta_ = NormalAngle(veh->i.odo_meter_.theta_);
            veh->i.odo_meter_.x_ += V_command_com[1].V_x * cos(veh->i.odo_meter_.theta_) * deta_t - V_command_com[1].V_y * sin(veh->i.odo_meter_.theta_) * deta_t;
            veh->i.odo_meter_.y_ += V_command_com[1].V_x * sin(veh->i.odo_meter_.theta_) * deta_t + V_command_com[1].V_y * cos(veh->i.odo_meter_.theta_) * deta_t;
        }
    }
    veh->i.actual_velocity_.x_ = V_command_com[1].V_x;
    veh->i.actual_velocity_.y_ = V_command_com[1].V_y;
    veh->i.actual_velocity_.w_ = V_command_com[1].W;
    veh->i.time_stamp_ = V_command_com[1].time_stamp;
    free(V_command_com);

    if (veh->enable_ == 0) {
        EnableDrive_unit(drive_unit_entry, 0);
        veh->i.enabled_ = 0;
    } else {
        EnableDrive_unit(drive_unit_entry, 1);
        veh->i.enabled_ = 1;
    }
#endif

    if (fabs(veh->i.actual_velocity_.x_) > 0.001 || fabs(veh->i.actual_velocity_.y_) > 0.001 || fabs(veh->i.actual_velocity_.w_) > 0.001) {
        veh->i.is_moving_ = 1;
    } else {
        veh->i.is_moving_ = 0;
    }

    nav_angle_last = nav_pos.angle_;
    time_stamp_last = time_current;
#if PRINT_NAV_DEBUG_LOG
    nav__log(nav, veh);
#endif
#if PRINT_CALIB_LOG
    nav__log_calib(nav, veh);
#endif

    return 0;
}

static int nav__update_upl(var__navigation_t *nav) {
    if ((nav->traj_ref_.count_ - 1) < nav->i.traj_ref_index_curr_) {
        return -1;
    }
    trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
    double t_close_p_online = 0;
    if (GetTClosestOnLine(nav_pos, cur_trail_edge->edge_id_, &t_close_p_online) < 0) {
        return -1;
    }
    const var__edge_t* cur_edge = var__search_edge(cur_trail_edge->edge_id_);
    if (!cur_edge) {
        return -1;
    }
    position_t p;
    if (GetposbyT(t_close_p_online, cur_edge, &p) < 0) {
        return -1;
    }
    double per = 0;
    if (GetpercentbyT(t_close_p_online, cur_edge, &per)) {
        return -1;
    }
    nav->i.upl_.edge_id_ = cur_trail_edge->edge_id_;
    nav->i.upl_.percentage_ = per;
    nav->i.upl_.angle_ = NormalAngle(nav->pos_.angle_ - p.angle_);
    return 0;
}

static
int nav__mapping_upl(var__navigation_t *nav) {
    int i;
    int i_wop;
    double min_weight = 9999999;
    upl_t upl;
    var__map_layout_t * map = var__get_layout();
    if (!map) {
        var__mark_software_error(kVarFixedObject_Navigation, var__make_error_code(kVarType_Navigation, kNav_ErrorNoMap));
        return -1;
    }
    for (i = 0; i < map->edges_.count_; ++i) {
        double edge_weight = 9999999;
        var__edge_t* e = &((var__edge_t*) map->edges_.data_)[i];
        if (!e) {
            return -1;
        }
        double t = 0;
        if (GetTClosestOnLine(nav->pos_, e->id_, &t) < 0) {
            return -1;
        }
        position_t p_on_line;
        if (GetposbyT(t, e, &p_on_line) < 0) {
            return -1;
        }

        double per = 0;
        if (GetpercentbyT(t, e, &per) < 0) {
            return -1;
        }
        double dis_weight = sqrt(pow(p_on_line.x_ - nav->pos_.x_, 2) + pow(p_on_line.y_ - nav->pos_.y_, 2));
        double angle_weight = 9999999;
        // int suitable_wop_id = -1;
        for (i_wop = 0; i_wop < e->wops_.count_; ++i_wop) {
            int wop_id = ((var__edge_wop_properties_t*) e->wops_.data_)[i_wop].wop_id_;
            const var__way_of_pass_t* wop = var__search_wop(wop_id);
            if (!wop) {
                continue;
            }
            double wop_global_angle = 0;
            if (nav__get_global_wop_angle(wop, e, t, &wop_global_angle) < 0) {
                continue;
            }
            double angle_err = fabs(NormalAngle(nav->pos_.angle_ - wop_global_angle));
            if (angle_err < angle_weight) {
                angle_weight = angle_err;
                // suitable_wop_id = wop->id_;
            }

            var__end_search();
        }

        edge_weight = dis_weight * 0.4 + angle_weight;
        if (edge_weight < min_weight) {
            min_weight = edge_weight;
            upl.edge_id_ = e->id_;
            upl.percentage_ = per;
            //upl.wop_id_ = suitable_wop_id;
            upl.angle_ = NormalAngle(nav->pos_.angle_ - p_on_line.angle_);
        }
    }
    nav->i.upl_ = upl;
    var__release_object_reference(map);
    return 0;
}

static int nav__get_global_wop_angle(const var__way_of_pass_t* wop, const var__edge_t* edge, const double t, double* global_wop_angle) {
    if (!wop || !edge || !global_wop_angle) {
        return -1;
    }
    position_t pos;
    if (GetposbyT(t, edge, &pos) < 0) {
        return -1;
    }
    switch (wop->angle_type_) {
        case 0:
            *global_wop_angle = pos.angle_ + wop->angle_;
            break;
        case 1:
            *global_wop_angle = wop->angle_;
            break;
        default:
            return -1;
    }

    return 0;
}

static int nav__is_partition(trail_t* cur_trail_edge, trail_t* nxt_trail_edge) {
    const var__edge_t* cur_edge = var__search_edge(cur_trail_edge->edge_id_);
    const var__edge_t* nxt_edge = var__search_edge(nxt_trail_edge->edge_id_);
    if (!cur_edge || !nxt_edge) {
        return 0;
    }
    const var__way_of_pass_t* cur_wop = var__search_wop(cur_trail_edge->wop_id_);
    const var__way_of_pass_t* nxt_wop = var__search_wop(nxt_trail_edge->wop_id_);
    if (!cur_wop || !nxt_wop) {
        return 0;
    }

    int connect_node_cur = -1;
    if (nav__is_forward(cur_wop) == 1)//正向
    {
        connect_node_cur = cur_edge->end_node_id_;
    }
    else
    {
        connect_node_cur = cur_edge->start_node_id_;
    }

    int connect_node_nxt = -1;
    if (nav__is_forward(nxt_wop) == 1)//正向
    {
        connect_node_nxt = nxt_edge->start_node_id_;
    }
    else
    {
        connect_node_nxt = nxt_edge->end_node_id_;
    }

    if (connect_node_cur != connect_node_nxt)  //节点不相连，截断
    {
        return 1;
    }

    double cur_g_wop_angle = 0;
    double nxt_g_wop_angle = 0;

    if (nav__get_global_wop_angle(cur_wop, cur_edge, (nav__is_forward(cur_wop) == 1 ? 1 : 0), &cur_g_wop_angle) < 0
            || nav__get_global_wop_angle(nxt_wop, nxt_edge, (nav__is_forward(nxt_wop) == 1 ? 0.0 : 1.0), &nxt_g_wop_angle) < 0) {
        return 0;
    }

    if (fabs(NormalAngle(cur_g_wop_angle - nxt_g_wop_angle)) > 30.0 / 180 * Pi) {
        return 1;
    } else {
        double cur_t = cur_wop->direction_ == 0 ? 1 : 0;
        position_t cur_end_pos;
        if (GetposbyT(cur_t, cur_edge, &cur_end_pos) < 0) {
            return 0;
        }

        double nxt_t = nxt_wop->direction_ == 0 ? 0 : 1;
        position_t nxt_start_pos;
        if (GetposbyT(nxt_t, nxt_edge, &nxt_start_pos) < 0) {
            return 0;
        }

        if (fabs(NormalAngle((cur_end_pos.angle_ + cur_wop->direction_ * Pi) - (nxt_start_pos.angle_ + nxt_wop->direction_ * Pi))) > 30.0 / 180 * Pi) {
            return 1;
        }
    }

    var__end_search();
    var__end_search();
    var__end_search();
    var__end_search();
    return 0;
}

static
void nav__check_proc_state(var__navigation_t *nav) {
    int is_global_last_segment = ((nav->traj_ref_.count_ - 1) == nav->i.traj_ref_index_curr_) ? 1 : 0;
    int last_segment_in_partition = is_global_last_segment;
    if (last_segment_in_partition == 0) {
        trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
        trail_t* nxt_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_ + 1];
        last_segment_in_partition = nav__is_partition(cur_trail_edge, nxt_trail_edge);
    }

    if (last_segment_in_partition == 1) {
        g_proc_state = ((is_global_last_segment == 1) ? SEG_GLOBAL_LAST : SEG_PARTITION_LAST);
    } else {
        g_proc_state = SEG_SMOOTH;
    }

    if (g_proc_state == SEG_GLOBAL_LAST
            && (nav->is_traj_whole_ == 0)) {
        g_proc_state = SEG_PARTITION_LAST;
    }

}

static
int nav__is_forward(const var__way_of_pass_t* wop) {
    return (wop->direction_ == 0) ? 1 : 0;

}

static int nav__cal_nav_poionts(var__navigation_t *nav) {
    int i;

    trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
    double t_close_p_online = 0;
    if (GetTClosestOnLine(nav_pos, cur_trail_edge->edge_id_, &t_close_p_online) < 0) {
        return -1;
    }
    double aim_dis = nav->aim_dist_;
    double predict_dis = fabs(nav->predict_time_ * v_last);
    const var__edge_t *cur_edge = var__search_edge(cur_trail_edge->edge_id_);
    const var__way_of_pass_t* cur_wop = var__search_wop(cur_trail_edge->wop_id_);
    trail_t* nxt_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_ + 1];
    const var__edge_t* nxt_edge = var__search_edge(nxt_trail_edge->edge_id_);
    const var__way_of_pass_t* nxt_wop = var__search_wop(nxt_trail_edge->wop_id_);
    if (!cur_edge || !nxt_edge || !cur_wop || !nxt_wop) {
        return -1;
    }

    //nav->i.base_point_;
    if (GetposbyT(t_close_p_online, cur_edge, &nav->i.base_point_) < 0) {
        return -1;
    }

    double per = 0;
    if (GetpercentbyT(t_close_p_online, cur_edge, &per) < 0) {
        return -1;
    }
    if (cur_edge->speed_limit_.count_ < 1) {
        v_line_limit = nav->max_speed_;
    } else if (cur_edge->speed_limit_.count_ == 1) {
        var__speed_limit_t* s_l = &(((var__speed_limit_t*) cur_edge->speed_limit_.data_)[0]);
        v_line_limit = s_l->lim_speed_;
    } else {
        for (i = 1; i < cur_edge->speed_limit_.count_; ++i) {
            var__speed_limit_t* s_l_lst = &(((var__speed_limit_t*) cur_edge->speed_limit_.data_)[i - 1]);
            var__speed_limit_t* s_l_cur = &(((var__speed_limit_t*) cur_edge->speed_limit_.data_)[i]);
            if (per > s_l_lst->percent_ && per < s_l_cur->percent_) {
                v_line_limit = s_l_cur->lim_speed_;
                break;
            }

        }
    }


    if (nav__is_forward(cur_wop) == 1) {
        double nt_aim = 0;
        if (GetTnewbyDistandTcur(t_close_p_online, aim_dis, cur_edge, &nt_aim) < 0) {
            return -1;
        }
		
        if (nt_aim < 1)
        {
            if (GetposbyT(nt_aim, cur_edge, &nav->i.aim_point_) < 0) {
                return -1;
            }
        } else {
            double ami_dis_left = 0;
            double dist_new = 0;
            if (GetdistbyT(t_close_p_online, cur_edge, &ami_dis_left) < 0) {
                return -1;
            }
            ami_dis_left += aim_dis - cur_edge->length_;
            if (nav__is_forward(nxt_wop) == 1) {
                dist_new = ami_dis_left;
            } else {
                dist_new = nxt_edge->length_ - ami_dis_left;
            }
            nt_aim = 0;
            /*  if (GetTnewbyDistandTcur(0, ami_dis_left, nxt_edge, &nt_aim) < 0) {
                  return -1;
                  }*/

            if (GetTnewbyDistandTcur(0, dist_new, nxt_edge, &nt_aim) < 0) {
                return -1;
            }
            if (GetposbyT(nt_aim, nxt_edge, &nav->i.aim_point_) < 0) {
                return -1;
            }

        }

        double nt_predict = 0;
        if (GetTnewbyDistandTcur(t_close_p_online, predict_dis, cur_edge, &nt_predict) < 0) {
            return -1;
        }
        if (nt_predict < 1)
        {
            track__pos_info_t pre;
            if (GetPosAllInfobyT(nt_predict, cur_edge, &pre) < 0) {
                return -1;
            }
            nav->i.predict_point_ = pre.pos_ang_;
            nav->i.predict_point_curvature_ = (cur_wop->direction_ == 0 ? 1 : -1) * pre.curve_;
        } else {
            if (nav__is_forward(nxt_wop)) {
                double pre_dis_left = 0;
                if (GetdistbyT(t_close_p_online, cur_edge, &pre_dis_left) < 0) {
                    return -1;
                }
                pre_dis_left += predict_dis - cur_edge->length_;
                nt_predict = 0;
                if (GetTnewbyDistandTcur(0, pre_dis_left, nxt_edge, &nt_predict) < 0) {
                    return -1;
                }

                track__pos_info_t pre;
                if (GetPosAllInfobyT(nt_predict, nxt_edge, &pre) < 0) {
                    return -1;
                }
                nav->i.predict_point_ = pre.pos_ang_;
                nav->i.predict_point_curvature_ = (nxt_wop->direction_ == 0 ? 1 : -1) * pre.curve_;
            } else {
                double pre_dis_left = 0;
                if (GetdistbyT(t_close_p_online, cur_edge, &pre_dis_left) < 0) {
                    return -1;
                }
                pre_dis_left = nxt_edge->length_ - (predict_dis - cur_edge->length_) - pre_dis_left;
                nt_predict = 0;
                if (GetTnewbyDistandTcur(0, pre_dis_left, nxt_edge, &nt_predict) < 0) {
                    return -1;
                }

                track__pos_info_t pre;
                if (GetPosAllInfobyT(nt_predict, nxt_edge, &pre) < 0) {
                    return -1;
                }
                nav->i.predict_point_ = pre.pos_ang_;
                nav->i.predict_point_curvature_ = (nxt_wop->direction_ == 0 ? 1 : -1) * pre.curve_;
            }

        }
    } else
    {
        aim_dis *= -1;
        predict_dis *= -1;

        double nt_aim = 0;
        if (GetTnewbyDistandTcur(t_close_p_online, aim_dis, cur_edge, &nt_aim) < 0) {
            return -1;
        }
        if (nt_aim > 0)
        {
            if (GetposbyT(nt_aim, cur_edge, &nav->i.aim_point_) < 0) {
                return -1;
            }
        } else {
            double ami_dis_left = 0;
            double dist_new = 0;
            if (GetdistbyT(t_close_p_online, cur_edge, &ami_dis_left) < 0) {
                return -1;
            }
            ami_dis_left = +aim_dis;
            if (nav__is_forward(nxt_wop) == 1) // forward
            {
                dist_new = 0 - ami_dis_left;
            } else {
                dist_new = nxt_edge->length_ + ami_dis_left;
            }

            if (GetTnewbyDistandTcur(0, dist_new, nxt_edge, &nt_aim) < 0) {
                return -1;
            }
            if (GetposbyT(nt_aim, nxt_edge, &nav->i.aim_point_) < 0) {
                return -1;
            }
        }

        double nt_predict = 0;
        if (GetTnewbyDistandTcur(t_close_p_online, predict_dis, cur_edge, &nt_predict) < 0) {
            return -1;
        }
        if (nt_predict > 0) {
            track__pos_info_t pre;
            if (GetPosAllInfobyT(nt_predict, cur_edge, &pre) < 0) {
                return -1;
            }
            nav->i.predict_point_ = pre.pos_ang_;
            nav->i.predict_point_curvature_ = (cur_wop->direction_ == 0 ? 1 : -1) * pre.curve_;
        } else {
            double dist_new = 0;

            if (nav__is_forward(nxt_wop)) {
                track__pos_info_t pre;
                double pre_dis_left = 0;

                if (GetdistbyT(t_close_p_online, cur_edge, &pre_dis_left) < 0) {
                    return -1;
                }
                pre_dis_left += predict_dis;
                dist_new = 0 - pre_dis_left;
                nt_predict = 0;
                if (GetTnewbyDistandTcur(0, dist_new, nxt_edge, &nt_predict) < 0) {
                    return -1;
                }

                if (GetPosAllInfobyT(nt_predict, nxt_edge, &pre) < 0) {
                    return -1;
                }
                nav->i.predict_point_ = pre.pos_ang_;
                nav->i.predict_point_curvature_ = (nxt_wop->direction_ == 0 ? 1 : -1) * pre.curve_;
            } else {
                track__pos_info_t pre;
                double pre_dis_left = 0;
                if (GetdistbyT(t_close_p_online, cur_edge, &pre_dis_left) < 0) {
                    return -1;
                }
                pre_dis_left += predict_dis;
                dist_new = nxt_edge->length_ + pre_dis_left;
                nt_predict = 0;

                if (GetTnewbyDistandTcur(0, dist_new, nxt_edge, &nt_predict) < 0) {
                    return -1;
                }

                if (GetPosAllInfobyT(nt_predict, nxt_edge, &pre) < 0) {
                    return -1;
                }
                nav->i.predict_point_ = pre.pos_ang_;
                nav->i.predict_point_curvature_ = (nxt_wop->direction_ == 0 ? 1 : -1) * pre.curve_;
            }


        }

    }

    {
        nav->i.aim_point_.angle_ = nav->i.base_point_.angle_;
        nav->i.aim_point_.x_ = nav->i.base_point_.x_ + aim_dis * cos(nav->i.base_point_.angle_);
        nav->i.aim_point_.y_ = nav->i.base_point_.y_ + aim_dis * sin(nav->i.base_point_.angle_);
    }

    double goal_angle = 0;
    switch (cur_wop->angle_type_) {
        case 0:
            goal_angle = nav->i.aim_point_.angle_ + cur_wop->angle_;
            break;
        case 1:
            goal_angle = cur_wop->angle_;
            break;
        default:
            break;
    }
    aim_goal_angle = goal_angle;
    nav->i.aim_heading_error_ = NormalAngle(goal_angle - nav_pos.angle_);
    var__end_search();
    var__end_search();
    var__end_search();
    var__end_search();
    return 0;
}

static int nav__cal_nav_poionts_ext(var__navigation_t *nav) {
    int i;

    trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
    double t_close_p_online = 0;
    if (GetTClosestOnLine(nav_pos, cur_trail_edge->edge_id_, &t_close_p_online) < 0) {
        return -1;
    }
    double aim_dis = nav->aim_dist_;
    double predict_dis = fabs(nav->predict_time_ * v_last);
    const var__edge_t* cur_edge = var__search_edge(cur_trail_edge->edge_id_);
    const var__way_of_pass_t* cur_wop = var__search_wop(cur_trail_edge->wop_id_);
    if (!cur_edge || !cur_wop) {
        return -1;
    }

    if (GetposbyT(t_close_p_online, cur_edge, &nav->i.base_point_) < 0) {
        return -1;
    }

    double per = 0;
    if (GetpercentbyT(t_close_p_online, cur_edge, &per) < 0) {
        return -1;
    }
    if (cur_edge->speed_limit_.count_ < 1) {
        v_line_limit = nav->max_speed_;
    } else if (cur_edge->speed_limit_.count_ == 1) {
        var__speed_limit_t* s_l = &(((var__speed_limit_t*) cur_edge->speed_limit_.data_)[0]);
        v_line_limit = s_l->lim_speed_;
    } else {
        for (i = 1; i < cur_edge->speed_limit_.count_; ++i) {
            var__speed_limit_t* s_l_lst = &(((var__speed_limit_t*) cur_edge->speed_limit_.data_)[i - 1]);
            var__speed_limit_t* s_l_cur = &(((var__speed_limit_t*) cur_edge->speed_limit_.data_)[i]);
            if (per > s_l_lst->percent_ && per < s_l_cur->percent_) {
                v_line_limit = s_l_cur->lim_speed_;
                break;
            }

        }
    }
    if (nav__is_forward(cur_wop) == 0)
    {
        aim_dis *= -1;
        predict_dis *= -1;
    }

    track__pos_info_t aim;
    if (GetPosInfobyTdistExt(t_close_p_online, aim_dis, cur_edge, &aim) < 0) {
        return -1;
    }
    nav->i.aim_point_ = aim.pos_ang_;

    track__pos_info_t pre;
    if (GetPosInfobyTdistExt(t_close_p_online, predict_dis, cur_edge, &pre) < 0) {
        return -1;
    }
    nav->i.predict_point_ = pre.pos_ang_;
    nav->i.predict_point_curvature_ = (cur_wop->direction_ == 0 ? 1 : -1) * pre.curve_;

    {
        nav->i.aim_point_.angle_ = nav->i.base_point_.angle_;
        nav->i.aim_point_.x_ = nav->i.base_point_.x_ + aim_dis * cos(nav->i.base_point_.angle_);
        nav->i.aim_point_.y_ = nav->i.base_point_.y_ + aim_dis * sin(nav->i.base_point_.angle_);
    }
    double goal_angle = 0;
    switch (cur_wop->angle_type_) {
        case 0:
            goal_angle = nav->i.aim_point_.angle_ + cur_wop->angle_;
            break;
        case 1:
            goal_angle = cur_wop->angle_;
            break;
        default:
            break;
    }
    aim_goal_angle = goal_angle;
    nav->i.aim_heading_error_ = NormalAngle(goal_angle - nav_pos.angle_);

    var__end_search();
    var__end_search();
    return 0;
}

static int nav__is_upl_same_pos(upl_t upl, position_t pos) {
    const var__edge_t *dest_cur_edge = var__search_edge(upl.edge_id_);
    if (!dest_cur_edge) {
        return -1;
    }

    double t_online = 0;
    if (GetTClosestOnLine(pos, dest_cur_edge->id_, &t_online)) {
        return -1;
    }
    position_t dest_upl_pos;
    if (GetposbyT(t_online, dest_cur_edge, &dest_upl_pos) < 0) {
        return -1;
    }

    double dist_dest_upl_pos = sqrt((dest_upl_pos.x_ - pos.x_)*(dest_upl_pos.x_ - pos.x_) + (dest_upl_pos.y_ - pos.y_)*(dest_upl_pos.y_ - pos.y_));
    if (dist_dest_upl_pos < g_thres_is_same_piont) {
        return 1;
    }

    return 0;
}

static int nav__check_path(var__navigation_t *nav)
{
    if (nav->i.traj_ref_index_curr_ >= nav->traj_ref_.count_ - 1) {
        return 1;
    }

    trail_t* cur_trail_edge = &((trail_t*)nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
    trail_t* nxt_trail_edge = &((trail_t*)nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_ + 1];

    const var__edge_t* cur_edge = var__search_edge(cur_trail_edge->edge_id_);
    const var__edge_t* nxt_edge = var__search_edge(nxt_trail_edge->edge_id_);
    if (!cur_edge || !nxt_edge) {
        return -1;
    }

    int ok = 0;
    if (cur_edge->id_ != nxt_edge->id_)
    {
        if (cur_edge->start_node_id_ == nxt_edge->start_node_id_
            || cur_edge->start_node_id_ == nxt_edge->end_node_id_
            || cur_edge->end_node_id_ == nxt_edge->start_node_id_
            || cur_edge->end_node_id_ == nxt_edge->end_node_id_)
        {
            ok = 1;
        }
    }
    else
    {
        if (cur_trail_edge->wop_id_ != nxt_trail_edge->wop_id_)
        {
            ok = 1;
        }
    }

    var__end_search();
    var__end_search();        

    if (ok != 1){
        log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout, "nav__check_path failed,[%d,%d][%d,%d] not connected"
            , cur_trail_edge->edge_id_, cur_trail_edge->wop_id_, nxt_trail_edge->edge_id_, nxt_trail_edge->wop_id_);
    }

    return  ok;
}

static
int nav__traj_control_proc(var__navigation_t *nav, var__vehicle_t *veh, const struct list_head *drive_unit_entry, double* v_x, double *v_y, double* w, double deta_time) {
    velocity_t v_temp;
    double v = 0;
    int vx_dir;
    if (first_nav_angle == 0) {
        nav_angle_last = nav_pos.angle_;
        first_nav_angle = 1;
    }                   
    


    if (g_proc_state == INIT) {
        nav__check_proc_state(nav);
    }

    if (g_proc_state == SEG_SMOOTH) {
        int is_global_last_segment = ((nav->traj_ref_.count_ - 1) == nav->i.traj_ref_index_curr_) ? 1 : 0;
        if (is_global_last_segment) {
            if (nav->is_traj_whole_ == 1) {
                g_proc_state = SEG_GLOBAL_LAST;
            } else {
                g_proc_state = SEG_PARTITION_LAST;
            }

        } else {           

            trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
            trail_t* nxt_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_ + 1];

            double t_cur = 0;
            if (GetTClosestOnLine(nav_pos, cur_trail_edge->edge_id_, &t_cur) < 0) {
                return -1;
            }
            const var__edge_t* cur_edge = var__search_edge(cur_trail_edge->edge_id_);
            const var__edge_t* nxt_edge = var__search_edge(nxt_trail_edge->edge_id_);
            if (!cur_edge || !nxt_edge) {
                return -1;
            }

            position_t p_on_cur_line;
            if (GetposbyT(t_cur, cur_edge, &p_on_cur_line)) {
                return -1;
            }

            double dis_2_cur_edge = sqrt(pow(p_on_cur_line.x_ - nav_pos.x_, 2) + pow(p_on_cur_line.y_ - nav_pos.y_, 2));
            double t_nxt = 0;
            if (GetTClosestOnLine(nav_pos, nxt_trail_edge->edge_id_, &t_nxt) < 0) {
                return -1;
            }
            position_t p_on_nxt_line;
            if (GetposbyT(t_nxt, nxt_edge, &p_on_nxt_line) < 0) {
                return -1;
            }
            double dis_2_nxt_edge = sqrt(pow(p_on_nxt_line.x_ - nav_pos.x_, 2) + pow(p_on_nxt_line.y_ - nav_pos.y_, 2));

            if (dis_2_cur_edge > dis_2_nxt_edge) {
                if (nav__check_path(nav) != 1)
                {
                    return -1;
                }
                ++nav->i.traj_ref_index_curr_;
                // 				nav->i.upl_.edge_id_ = nxt_trail_edge->edge_id_;
                // 				nav->i.upl_.wop_id_ = nxt_trail_edge->wop_id_;
                // 				nav->i.upl_.percentage_ = 0;
                nav__cal_dis_left(nav);
                nav__update_upl(nav);
                nav__check_proc_state(nav);
            }
            var__end_search();
            var__end_search();
        }
    }

    if (g_proc_state == SEG_PARTITION_LAST) {
        if (nav->i.traj_ref_index_curr_ >= nav->traj_ref_.count_) {
            return -1;
        }
        int b_reached = 0;
        trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
        double t_cur = 0;
        if (GetTClosestOnLine(nav_pos, cur_trail_edge->edge_id_, &t_cur) < 0) {
            return -1;
        }
        const var__edge_t* cur_edge = var__search_edge(cur_trail_edge->edge_id_);
        const var__way_of_pass_t* cur_wop = var__search_wop(cur_trail_edge->wop_id_);

        if (!cur_edge || !cur_wop) {
            return -1;
        }
        double dis_2_start = 0;
        if (GetdistbyT(t_cur, cur_edge, &dis_2_start) < 0) {
            return -1;
        }
        if (nav__is_forward(cur_wop))
        {
            if (dis_2_start + nav->stop_tolerance_ > cur_edge->length_) {
                b_reached = 1;
            }
        } else {
            if (dis_2_start - nav->stop_tolerance_ < 0) {
                b_reached = 1;
            }
        }
        var__end_search();
        var__end_search();
        if (b_reached) {
            if (nav->i.traj_ref_index_curr_ < nav->traj_ref_.count_ - 1) {
                if (nav__check_path(nav) != 1)
                {
                    return -1;
                }
                ++nav->i.traj_ref_index_curr_;
                nav__check_proc_state(nav);
                nav__update_upl(nav);
            } else {
                g_proc_state = SEG_WAIT_TRAFFIC;
            }

            nav__cal_dis_left(nav);
        }
    }

    if (g_proc_state == SEG_GLOBAL_LAST) {
        nav__cal_dis_left(nav);
        nav__check_proc_state(nav);
        nav__update_upl(nav);
        int b_reached = 0;
        if (nav->i.dist_to_partition_ < 1) {

            if (nav__is_upl_same_pos(nav->dest_upl_, nav->dest_pos_) != 1) {
                /*if ((veh->chassis_type_ & 0x01) == 0x01)
                {
                trail_t* cur_trail_edge = &((trail_t*)nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
                double t_cur = GetTClosestOnLine(nav_pos, cur_trail_edge->edge_id_);
                position_t pos_cur = GetposbyT(t_cur,)
                }*/
            } else {
                trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
                double t_cur = 0;
                if (GetTClosestOnLine(nav_pos, cur_trail_edge->edge_id_, &t_cur) < 0) {
                    return -1;
                }
                const var__edge_t *cur_edge = var__search_edge(cur_trail_edge->edge_id_);

                const var__way_of_pass_t *cur_wop = var__search_wop(cur_trail_edge->wop_id_);
                if (!cur_edge || !cur_wop) {
                    return -1;
                }
                double dis_2_start = 0;
                if (GetdistbyT(t_cur, cur_edge, &dis_2_start) < 0) {
                    return -1;
                }

                double t_dest_p_online = 0;
                if (GetTClosestOnLine(nav->dest_pos_, cur_trail_edge->edge_id_, &t_dest_p_online) < 0) {
                    return -1;
                }
                double dest_per = 0;
                if (GetpercentbyT(t_dest_p_online, cur_edge, &dest_per) < 0) {
                    return -1;
                }
                double dist_to_dest = dest_per / 100.0 * cur_edge->length_;

                if (nav__is_forward(cur_wop)) {
                    if (dis_2_start + nav->stop_tolerance_ > dist_to_dest) {
                        b_reached = 1;
                    }
                } else {
                    if (dis_2_start - nav->stop_tolerance_ < dist_to_dest) {
                        b_reached = 1;
                    }
                }
                var__end_search();
                var__end_search();
            }

        }
        if (b_reached == 1) {
            if (nav->is_traj_whole_ == 1) {
                if ((veh->chassis_type_ & 0x01) == 0x01) {
                    g_proc_state = SEG_FINAL_ROTATE;
                } else {
                    var__xchange_middle_status(&nav->track_status_, kStatusDescribe_Completed, NULL);
                    var__xchange_response_status(&nav->track_status_, kStatusDescribe_Completed, NULL);
                    g_proc_state = FINISH;
                    nsp__report_status(nav->user_task_id_, VAR_TYPE_NAVIGATION, kStatusDescribe_Completed, 0, 0);

                    log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                            "Finish :dest_pos (%.5f,%.5f,%.5f)"
                            "nav_pos (%.5f,%.5f,%.5f)"
                            "err (%.5f,%.5f,%.5f)"
                            , nav->dest_pos_.x_, nav->dest_pos_.y_, nav->dest_pos_.angle_
                            , nav_pos.x_, nav_pos.y_, nav_pos.angle_
                            , nav->dest_pos_.x_ - nav_pos.x_, nav->dest_pos_.y_ - nav_pos.y_, nav->dest_pos_.angle_ - nav_pos.angle_);
                }
            } 
			else {
                g_proc_state = SEG_WAIT_TRAFFIC;
            }
        }

        if (nav->is_traj_whole_ == 1) {
            nav->i.on_last_segment_ = 1;
        } else {
            nav->i.on_last_segment_ = 0;
        }
    }

    if (g_proc_state == SEG_FINAL_ROTATE) {
        int e = nav__cal_nav_poionts_ext(nav);
        if (e < 0) {
            return e;
        }
        int zag_reached = 0;

        if (fabs(nav->i.aim_heading_error_) > 20.0 / 180.0 * Pi) {
            zag_reached = 0;
        } else {
            if ((NormalAngle((nav_angle_last + (veh->i.command_velocity_.w_ > 0 ? 1 : -1) * nav->stop_tolerance_angle_ - aim_goal_angle)) * NormalAngle((nav_pos.angle_ + (nav->i.aim_heading_error_ > 0 ? 1 : -1) * nav->stop_tolerance_angle_ - aim_goal_angle)) < 0) || (fabs(NormalAngle(nav_pos.angle_ - aim_goal_angle)) < nav->stop_tolerance_angle_)) {
                zag_reached = 1;
            } else {
                zag_reached = 0;
            }
        }
        if (zag_reached == 1) {
            nav__update_upl(nav);
            var__xchange_middle_status(&nav->track_status_, kStatusDescribe_Completed, NULL);
            var__xchange_response_status(&nav->track_status_, kStatusDescribe_Completed, NULL);

            g_proc_state = FINISH;
            nsp__report_status(nav->user_task_id_, VAR_TYPE_NAVIGATION, kStatusDescribe_Completed, 0, 0);

            log__save("nav", kLogLevel_Info, kLogTarget_Filesystem | kLogTarget_Stdout,
                    "Finish :dest_pos (%.5f,%.5f,%.5f)"
                    "nav_pos (%.5f,%.5f,%.5f)"
                    "err (%.5f,%.5f,%.5f)"
                    , nav->dest_pos_.x_, nav->dest_pos_.y_, nav->dest_pos_.angle_
                    , nav_pos.x_, nav_pos.y_, nav_pos.angle_
                    , nav->dest_pos_.x_ - nav_pos.x_, nav->dest_pos_.y_ - nav_pos.y_, nav->dest_pos_.angle_ - nav_pos.angle_);
        }
    } else if (g_proc_state == SEG_WAIT_TRAFFIC) {
        if ((nav->traj_ref_.count_ - 1) <= nav->i.traj_ref_index_curr_) {
            g_proc_state = SEG_WAIT_TRAFFIC;
        } else {
            nav__check_proc_state(nav);
        }
    }

    if (g_proc_state == SEG_SMOOTH
            || g_proc_state == SEG_PARTITION_LAST
            || g_proc_state == SEG_GLOBAL_LAST
            || g_proc_state == SEG_WAIT_TRAFFIC) {
        int e = nav__update_upl(nav);
        if (e < 0) {
            log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "nav__update_upl failed!");
            return e;
        }

        if (g_proc_state == SEG_SMOOTH) {
            int e = nav__cal_nav_poionts(nav);
            if (e < 0) {
                log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "nav__cal_nav_poionts failed!");
                //return e;
            }
        } else if (g_proc_state == SEG_PARTITION_LAST
                || g_proc_state == SEG_WAIT_TRAFFIC) {
            int e = nav__cal_nav_poionts_ext(nav);
            if (e < 0) {
                log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "nav__cal_nav_poionts_ext failed!");
                //return e;
            }
        } else if (g_proc_state == SEG_GLOBAL_LAST) {

            if (nav->i.dist_to_partition_ > 1) {
                int e = nav__cal_nav_poionts_ext(nav);
                if (e < 0) {
                    log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "nav__cal_nav_poionts_ext failed!");
                    //return e;
                }
            } else {
                if (nav__is_upl_same_pos(nav->dest_upl_, nav->dest_pos_) == 1) {
                    int e = nav__cal_nav_poionts_ext(nav);
                    if (e < 0) {
                        log__save("nav", kLogLevel_Error, kLogTarget_Filesystem | kLogTarget_Stdout, "nav__cal_nav_poionts_ext failed!");
                        //return e;
                    }
                } else {
                    if ((veh->chassis_type_ & 0x01) == 0x01)
                    {
                        //todo
                    } else {
                        //todo
                    }
                }
            }
        }

        double err_dis = sqrt(pow(nav->i.base_point_.x_ - nav_pos.x_, 2) + pow(nav->i.base_point_.y_ - nav_pos.y_, 2));
        if (err_dis > nav->tracking_error_tolerance_dist_
                || ((fabs(nav->i.aim_heading_error_) > nav->tracking_error_tolerance_angle_) && (!((veh->chassis_type_ & 0x01) == 0x01)))) {
            nav->i.tracking_error_ = 1;
            *v_x = 0;
            *v_y = 0;
            *w = 0;
            trim(*v_x, *v_y, *w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
            *v_x = v_temp.x_;
            *v_y = v_temp.y_;
            *w = v_temp.w_;
        }
        //else {
        //    nav->i.tracking_error_ = 0;
        //}

        if (g_rotation_mode == 0
                && (fabs(nav->i.aim_heading_error_) > 20.0 * Pi / 180)
                && ((veh->chassis_type_ & 0x01) == 0x01)) {
            g_rotation_mode = 1;
        }

        int zag_reached = 0;

        if (fabs(nav->i.aim_heading_error_) > 20.0 / 180 * Pi) {
            zag_reached = 0;
        } else {
            if ((NormalAngle((nav_angle_last + (veh->i.command_velocity_.w_ > 0 ? 1 : -1) * nav->stop_tolerance_angle_ - aim_goal_angle)) * NormalAngle((nav_pos.angle_ + (nav->i.aim_heading_error_ > 0 ? 1 : -1) * nav->stop_tolerance_angle_ - aim_goal_angle)) < 0) || (fabs(NormalAngle(nav_pos.angle_ - aim_goal_angle)) < nav->stop_tolerance_angle_)) {
                zag_reached = 1;
            } else {
                zag_reached = 0;
            }
        }


        if (g_rotation_mode == 1
                && zag_reached == 1) {
            g_rotation_mode = 0;
        }

        if (g_rotation_mode == 1) {

            *v_x = 0;
            *v_y = 0;

            *w = sqrt(2 * nav->dec_w_ * max((fabs(nav->i.aim_heading_error_) - nav->creep_theta_), 0));
            *w = (nav->i.aim_heading_error_ > 0 ? 1 : -1)*(*w);
            if (fabs(*w) < nav->creep_w_) {
                *w = (double) (nav->i.aim_heading_error_ > 0 ? 1 : -1) * nav->creep_w_;
            }
            trim(*v_x, *v_y, *w, nav, veh, deta_time, ONLY_ACC, &v_temp);
            *w = v_temp.w_;
            trim(*v_x, *v_y, *w, nav, veh, deta_time, NON, &v_temp);
            *v_x = v_temp.x_;
            *v_y = v_temp.y_;
        } else {
            trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
            const var__way_of_pass_t *cur_wop = var__search_wop(cur_trail_edge->wop_id_);
            if (cur_wop == NULL) {
                return -1;
            }
            double goal_angle, L_ref_aim, sita_ref_aim, angle_ex, ex;
            switch (cur_wop->angle_type_) {
                case 0:
                    goal_angle = nav->i.aim_point_.angle_ + cur_wop->angle_;
                    break;
                case 1:
                    goal_angle = cur_wop->angle_;
                    break;
                default:
                    goal_angle = 0;
                    break;
            }
            L_ref_aim = sqrt(pow(nav->i.aim_point_.x_ - nav_pos.x_, 2) + pow(nav->i.aim_point_.y_ - nav_pos.y_, 2));
            sita_ref_aim = atan2(nav->i.aim_point_.y_ - nav_pos.y_, nav->i.aim_point_.x_ - nav_pos.x_);
            angle_ex = sita_ref_aim - goal_angle;
            ex = L_ref_aim * cos(angle_ex);
            vx_dir = (ex >= 0 ? 1 : -1);
            if ((veh->chassis_type_ & 0x03) == 3) {
                v_sita = NormalAngle(sita_ref_aim - nav_pos.angle_);
            } else {
                v_sita = atan2(0, 1.0 * vx_dir);
            }
            if (g_proc_state == SEG_SMOOTH) {
                v = min(nav->max_speed_, v_line_limit);
                if (veh->slow_down_) {
                    v = min(fabs(v), nav->slow_down_speed_);
                    trim2(v, nav, veh, deta_time, NON, &v);
                }
                double v_sq = sqrt(2 * nav->dec_ * max((nav->i.dist_to_partition_ - nav->creep_distance_), 0));
                v_sq = max(fabs(v_sq), nav->creep_speed_);
                v = min(fabs(v_sq), fabs(v));
                if ((veh->chassis_type_ & 0x03) == 3) {
                    trim2(v, nav, veh, deta_time, ONLY_ACC, &v);
                    *v_x = v * cos(v_sita);
                    *v_y = v * sin(v_sita);
                } else {
                    *v_x = v * cos(v_sita);
                    *v_y = v * sin(v_sita);
                    trim(*v_x, *v_y, *w, nav, veh, deta_time, ONLY_ACC, &v_temp);
                    *v_x = v_temp.x_;
                    *v_y = v_temp.y_;
                }
            } else if (g_proc_state == SEG_WAIT_TRAFFIC) {
                *v_x = 0;
                *v_y = 0;
                *w = 0;
                trim(*v_x, *v_y, *w, nav, veh, deta_time, NON, &v_temp);
                *v_x = v_temp.x_;
                *v_y = v_temp.y_;
                *w = v_temp.w_;
            } else {
                v = sqrt(2 * nav->dec_ * max((nav->i.dist_to_partition_ - nav->creep_distance_), 0));
                v = max(fabs(v), nav->creep_speed_);
                v = min(fabs(v), v_line_limit);
                if ((veh->chassis_type_ & 0x03) == 3) {
                    trim2(v, nav, veh, deta_time, ONLY_ACC, &v);
                    *v_x = v * cos(v_sita);
                    *v_y = v * sin(v_sita);
                } else {
                    *v_x = v * cos(v_sita);
                    *v_y = v * sin(v_sita);
                    trim(*v_x, *v_y, *w, nav, veh, deta_time, ONLY_ACC, &v_temp);
                    *v_x = v_temp.x_;
                    *v_y = v_temp.y_;
                }
            }
            switch (nav->track_status_.command_) {
                case kStatusDescribe_Pause:
                case kStatusDescribe_Cancel:
                    *v_x = 0;
                    *v_y = 0;
					*w = 0;
                    trim(*v_x, *v_y, *w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
                    *v_x = v_temp.x_;
                    *v_y = v_temp.y_;
					*w = v_temp.w_;
                    break;
                default:
                    break;
            }

            if ((veh->chassis_type_ & 0x03) == 3) {
                double curve_r, curve_ea;
                e_angle = NormalAngle(goal_angle - nav_pos.angle_);
                curve_r = nav->i.predict_point_curvature_;
                curve_ea = nav->aim_angle_p_*e_angle;
                w_temp = sqrt(pow(*v_x, 2) + pow(*v_y, 2))*(curve_ea + curve_r);
                *w = w_temp;
                log__save("w_temp", kLogLevel_Info, kLogTarget_Filesystem, "%.5f %.5f %.5f %.5f", e_angle, curve_r, curve_ea, w_temp);
            } else { // 不可平移的车辆，w与ey，ea，curve
                int forward;
                forward = (vx_dir == 1 ? 1 : 0);
                double angle_ey;
                double curve_ey, curve_ea, curve_r;
                double aim_ey_p, aim_angle_p;
                double vx_rev = sqrt(pow(*v_x, 2) + pow(*v_y, 2));
                aim_ey_p = nav->aim_ey_p_ * 1.0 / (1.0 + vx_rev);
                aim_angle_p = nav->aim_angle_p_ * 1.0 / (1.0 + vx_rev);
                angle_ey = sita_ref_aim - (nav_pos.angle_ + (1 - forward) * Pi);
                ey = L_ref_aim * sin(angle_ey);
                e_angle = NormalAngle(goal_angle - nav_pos.angle_);
                curve_ey = aim_ey_p*ey;
                curve_ea = aim_angle_p*e_angle;
                curve_r = nav->i.predict_point_curvature_;
                w_temp = sqrt(pow(*v_x, 2) + pow(*v_y, 2))*(curve_ey + curve_ea + curve_r);
                *w = w_temp;
            }
            trim(*v_x, *v_y, *w, nav, veh, deta_time, NON, &v_temp);
            *w = v_temp.w_;
            var__end_search();
        }
    } else if (g_proc_state == SEG_FINAL_ROTATE) {

        nav__cal_nav_poionts(nav);

        double dist_cur_dest = sqrt(pow((nav_pos.x_ - nav->dest_pos_.x_), 2) + pow((nav_pos.y_ - nav->dest_pos_.y_), 2));
        if (dist_cur_dest > nav->tracking_error_tolerance_dist_) {
            nav->i.tracking_error_ = 1;
            *v_x = 0;
            *v_y = 0;
            *w = 0;
            trim(*v_x, *v_y, *w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
            *v_x = v_temp.x_;
            *v_y = v_temp.y_;
            *w = v_temp.w_;
        }

        *v_x = 0;
        *v_y = 0;


        *w = sqrt(2 * nav->dec_w_ * max(fabs(nav->i.aim_heading_error_) - nav->creep_theta_, 0));
        *w = (nav->i.aim_heading_error_ > 0 ? 1 : -1)*(*w);
        if (fabs(*w) < nav->creep_w_) {
            *w = (nav->i.aim_heading_error_ > 0 ? 1 : -1) * nav->creep_w_;
        }
        trim(*v_x, *v_y, *w, nav, veh, deta_time, ONLY_ACC, &v_temp);
        *w = v_temp.w_;
        trim(*v_x, *v_y, *w, nav, veh, deta_time, NON, &v_temp);
        *v_x = v_temp.x_;
        *v_y = v_temp.y_;
    } else {
        *v_x = 0;
        *v_y = 0;
        *w = 0;
        trim(*v_x, *v_y, *w, nav, veh, deta_time, DEC_DSTOP, &v_temp);
        *v_x = v_temp.x_;
        *v_y = v_temp.y_;
        *w = v_temp.w_;
    }

    return 0;
}

#if PRINT_NAV_DEBUG_LOG

static void nav__log(var__navigation_t *nav, var__vehicle_t *veh) {
    if (nav->traj_ref_.count_ <= 0 || (nav->track_status_.middle_ != kStatusDescribe_Running)) {
        return;
    }
    trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
    const var__edge_t *cur_edge = var__search_edge(cur_trail_edge->edge_id_);
    const var__way_of_pass_t *cur_wop = var__search_wop(cur_trail_edge->wop_id_);
    if (!cur_edge || !cur_wop) {
        return;
    }
    double t_online = 0;
    if (GetTClosestOnLine(nav_pos, cur_edge->id_, &t_online)) {
        return;
    }
    position_t online_pos;
    if (GetposbyT(t_online, cur_edge, &online_pos) < 0) {
        return;
    }

    double dist_err = sqrt((online_pos.x_ - nav_pos.x_)*(online_pos.x_ - nav_pos.x_) + (online_pos.y_ - nav_pos.y_)*(online_pos.y_ - nav_pos.y_));

    log__save("nav_debug", kLogLevel_Info, kLogTarget_Filesystem,
            "%.5f %.5f %.5f "
            "%.5f %.5f %.5f "
            "%.5f %.5f %.5f %.5f "
            "%.5f %.5f "
            "%.5f %.5f %.5f "
            "%.5f %.5f %.5f "
            "%d %.5f %.5f "
            "%d "
            "%d %d %d %.5f "
            "%.5f "
            "%d "
            "%.5f",
            nav_pos.x_, nav_pos.y_, nav_pos.angle_,
            nav->i.aim_point_.x_, nav->i.aim_point_.y_, nav->i.aim_point_.angle_,
            nav->i.predict_point_.x_, nav->i.predict_point_.y_, nav->i.predict_point_.angle_, nav->i.predict_point_curvature_,
            nav->aim_angle_p_, nav->aim_ey_p_,
            ey, e_angle, w_temp,
            veh->i.command_velocity_.x_, veh->i.command_velocity_.y_, veh->i.command_velocity_.w_
            , nav->i.upl_.edge_id_, nav->i.upl_.percentage_, nav->i.upl_.angle_
            , cur_edge->id_
            , cur_wop->id_, cur_wop->direction_, cur_wop->angle_type_, cur_wop->angle_
            , nav->i.dist_to_partition_
            , g_proc_state
            , dist_err);
    var__end_search();
    var__end_search();
}

#endif

#if PRINT_CALIB_LOG

static void nav__log_calib(var__navigation_t *nav, var__vehicle_t *veh) {
    log__save("data_odo", kLogLevel_Info, kLogTarget_Filesystem, 
            "%.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f "UINT64_STRFMT, 
            veh->i.odo_meter_.x_, veh->i.odo_meter_.y_, veh->i.odo_meter_.theta_, veh->i.command_velocity_.x_, veh->i.command_velocity_.y_, veh->i.command_velocity_.w_, veh->i.actual_velocity_.x_, veh->i.actual_velocity_.y_, veh->i.actual_velocity_.w_, veh->i.time_stamp_);
}

#endif

int nav__update_bank(var__navigation_t *nav) {
    if (nav->i.traj_ref_index_curr_ >= nav->traj_ref_.count_) {
        return -1;
    }
    trail_t* cur_trail_edge = &((trail_t*) nav->traj_ref_.data_)[nav->i.traj_ref_index_curr_];
    int i = 0;
    if (cur_trail_edge) {
        const var__edge_t* cur_edge = var__search_edge(cur_trail_edge->edge_id_);
        if (cur_edge) {
            for (i = 0; i < cur_edge->wops_.count_; ++i) {
                var__edge_wop_properties_t* ewp = &((var__edge_wop_properties_t*) cur_edge->wops_.data_)[i];
                if (ewp->wop_id_ == cur_trail_edge->wop_id_) {
                    memcpy(&nav->i.current_edge_wop_properties_, ewp, sizeof (var__edge_wop_properties_t));
                    return 0;
                }
            }
        }
    }
    return -1;
}
