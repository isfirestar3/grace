
#include "driver_elmo.h"
#include "elmo.h"
#include "var.h"
#include "log.h"
#include <iostream>
#include <math.h>
#include <cstdlib>

driver_elmo::driver_elmo() : m_emergency_stop(false) {
}

driver_elmo::~driver_elmo() {
}

int driver_elmo::read(var__functional_object_t* var) {
    var__elmo_t* elmo = var__object_body_ptr(var__elmo_t, var);
    elmo->i.time_stamp_ = driver_base::get_timestamp() - elmo->candev_head_.latency_;
    elmo->i.error_code_ = get_di(elmo->i.di_);
    if (elmo->i.error_code_) {
        return -1;
    }

    switch (get_ctrl_mode(elmo->control_mode_)) {
        case CIA402MODE_CAN_PP:
        {
            int p_get = 0;
            {
                if (elmo->command_position_ > elmo->i.actual_position_) {
                    if (elmo->command_position_ - elmo->i.actual_position_ > elmo->profile_speed_) {
                        p_get += elmo->profile_speed_;
                    } else {
                        p_get = elmo->command_position_ - elmo->i.actual_position_;
                    }
                } else {
                    if (elmo->command_position_ - elmo->i.actual_position_ < -1 * elmo->profile_speed_) {
                        p_get -= elmo->profile_speed_;
                    } else {
                        p_get = elmo->command_position_ - elmo->i.actual_position_;
                    }
                }

            }
            //elmo->error_code_ = get_position( p_get );
            //if ( 0 != elmo->error_code_ ) {
            //	return -1;
            //}

            elmo->i.actual_position_ += p_get;

            if (elmo->status_.middle_ == kStatusDescribe_Running &&
                    elmo->status_.response_ == kStatusDescribe_Running) {
                bool b_reach = false;
                if (std::abs(last_postion_ - (int64_t) elmo->i.actual_position_) < 3) {
                    error_cnt_++;
                }
                if (std::abs((long long) (elmo->i.actual_position_ - elmo->command_position_)) < 10) {
                    ++postion_cnt_;
                }
                if (postion_cnt_ > 5 && error_cnt_ > 5) {
                    b_reach = true;
                    postion_cnt_ = 10;
                } else if (error_cnt_ > 200) {
                    //m_amp.ClearFaults();
                    //m_amp.Disable();
                    //m_amp.Enable();
                    elmo->i.error_code_ = set_position((int) elmo->command_position_);
                    if (0 != elmo->i.error_code_) {
                        return -1;
                    }
                    postion_cnt_ = 0;
                    nspinfo << "elmo " << node_id << " retry set_position:" << (int) elmo->command_position_;

                    error_cnt_ = 0;
                    retry_cnt_++;

                    if (retry_cnt_++ > 3 && elmo->status_.middle_ != kStatusDescribe_Error && elmo->status_.response_ != kStatusDescribe_Error) {
                        elmo->status_.middle_ = kStatusDescribe_Error;
                        elmo->status_.response_ = kStatusDescribe_Error;
                        nspinfo << "elmo " << node_id << " postion error! p = " << (int64_t) elmo->i.actual_position_;
                        driver_base::callback_data(var); //??
                    }
                }

                if (b_reach && elmo->status_.middle_ != kStatusDescribe_Completed && elmo->status_.response_ != kStatusDescribe_Completed) {
                    elmo->status_.middle_ = kStatusDescribe_Completed;
                    elmo->status_.response_ = kStatusDescribe_Completed;
                    nspinfo << "elmo " << node_id << " postion finsihed! p = " << (int64_t) elmo->i.actual_position_;
					uint64_t dt = driver_base::get_timestamp() - time_position_begin;
					nspinfo << "CK TEST elmo " << node_id << " deta p=" << deta_position << " deta t=" << dt
						<< " average speed=" << deta_position / dt;

                    driver_base::callback_data(var);
                }
                last_postion_ = (int64_t) elmo->i.actual_position_;
            }
        }
            break;
        case CIA402MODE_CAN_PV:
        {
            int v_get = 0;
            elmo->i.error_code_ = get_veloctiy(v_get);
            if (0 != elmo->i.error_code_) {
                return -1;
            }
            elmo->i.actual_velocity_ = v_get;
        }
            break;
        default:
            break;
    }


    int current_get = 0;
    elmo->i.error_code_ = get_current(current_get);
    if (0 != elmo->i.error_code_) {
        return -1;
    }
    elmo->i.actual_current_ = current_get;

    return 0;
}

int driver_elmo::write(var__functional_object_t* var) {
    var__elmo_t* elmo = var__object_body_ptr(var__elmo_t, var);

    if (enabled_ != elmo->enable_) {
        elmo->i.error_code_ = set_enable(elmo->enable_ == 1);
        if (0 != elmo->i.error_code_) {
            return -1;
        }
        nspinfo << "elmo" << node_id << " set enable " << elmo->enable_;
    }
    enabled_ = elmo->enable_;
    elmo->i.enabled_ = enabled_;
    if (last_control_mode_ != elmo->control_mode_) {
        if (get_ctrl_mode(elmo->control_mode_) > 0) {
            //const Error* err = m_amp.SetOperationMode(get_ctrl_mode(elmo->control_mode_));
            //if (err) {
            //	loerr << "elmo " << node_id << "SetOperationMode:" << (int)elmo->control_mode_ << " failed!" << err->toString();
            //	return err->GetID();
            //}
            last_control_mode_ = elmo->control_mode_;
            nspinfo << "elmo " << node_id << " SetOperationMode:" << (int) elmo->control_mode_;
        }
    }

    switch (get_ctrl_mode(elmo->control_mode_)) {
        case CIA402MODE_CAN_PP:

            if ((elmo->status_.command_ == kStatusDescribe_Startup)
                    && (elmo->status_.middle_ == kStatusDescribe_Idle)) {

                elmo->i.error_code_ = set_position((int) elmo->command_position_);
                if (0 != elmo->i.error_code_) {
                    return -1;
                }
                elmo->status_.middle_ = kStatusDescribe_Running;
                elmo->status_.response_ = kStatusDescribe_Running;
                postion_cnt_ = 0;
                error_cnt_ = 0;
                retry_cnt_ = 0;
                nspinfo << "elmo" << node_id << " set_position:" << (int) elmo->command_position_;
				time_position_begin = driver_base::get_timestamp();
				deta_position = elmo->command_position_ - elmo->i.actual_position_;
            }
            break;
        case CIA402MODE_CAN_PV:
            elmo->i.error_code_ = set_velocity((int) elmo->command_velocity_);
            if (0 != elmo->i.error_code_) {
                return -1;
            }
            break;
        default:
            break;
    }
    return 0;
}

int driver_elmo::set_velocity(int v) {
    last_v_ = v;
    return 0;
}

int driver_elmo::get_veloctiy(int& v) {
    v = last_v_;
    return 0;
}

int driver_elmo::set_enable(bool enable) {
    return 0;
}

int driver_elmo::get_current(int& c) {

    //return 0;
    //int16 cur;
    //const Error* e = m_amp.GetCurrentActual( cur );
    //if ( e ) {
    //	loerr << "elmo " << node_id << " GetCurrentActual:" << e->toString();
    //	return e->GetID();
    //}
    //c = cur;
    return 0;
}

bool driver_elmo::get_stop_state(int di, bool &stop_state) {
    stop_state = false;
    return 0;
}

int driver_elmo::read_and_write(var__functional_object_t* var) {
    read(var);
    write(var);
    return 0;
}

int driver_elmo::set_position(int pos) {
    return 0;
}

int driver_elmo::get_position(int& pos) {
    return 0;
}

int driver_elmo::get_di(int& di) {
    return 0;
}

int driver_elmo::pos_is_reached(bool& b_reached) {
    return 0;
}

int driver_elmo::get_error_code(int &error) {
    return 0;
}

CIA402_6060_CODE driver_elmo::get_ctrl_mode(control_mode_t mode) {
    switch (mode) {
        case kDriverControlMode_SpeedMode:
            return CIA402MODE_CAN_PV;
        case kDriverControlMode_PositionMode:
            return CIA402MODE_CAN_PP;
        case kDriverControlMode_CurrentMode:
        default:
            break;
    }

    return CIA402MODE_DISABLED;
}