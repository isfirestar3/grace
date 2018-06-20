#include "driver_moons.h"
#include "moons.h"
#include "var.h"
#include "log.h"
#include "driver_elmo.h"

driver_moons::driver_moons() : m_emergency_stop(false) {
}

driver_moons::~driver_moons() {
}

int driver_moons::read(var__functional_object_t* var) {
    var__moos_t* moons = var__object_body_ptr(var__moos_t, var);

    moons->i.time_stamp_ = driver_base::get_timestamp() - moons->candev_head_.latency_;

    moons->i.error_code_ = get_di(moons->i.di_);
    if (moons->i.error_code_) {
        return -1;
    }
    //bool emergency_stop = false;
    //get_stop_state(moons->di_, emergency_stop);
    //if (emergency_stop)
    //{
    //	if (m_emergency_stop != emergency_stop)
    //	{
    //		lowarn << "moons " << node_id << " EmergencyStopButton is On ,Please Release!";

    //	}
    //	m_emergency_stop = emergency_stop;

    //	return -1;
    //}
    //if (emergency_stop != m_emergency_stop) {
    //	clear_fault();

    //	m_emergency_stop = emergency_stop;
    //}

    switch (moons->control_mode_) {
        case kDriverControlMode_PositionMode:
        {
            int p_get = 0;
            {
                if (moons->command_position_ > moons->i.actual_position_) {
                    if (moons->command_position_ - moons->i.actual_position_ > moons->profile_speed_) {
                        p_get += moons->profile_speed_;
                    } else {
                        p_get = moons->command_position_ - moons->i.actual_position_;
                    }
                } else {
                    if (moons->command_position_ - moons->i.actual_position_ < -1 * moons->profile_speed_) {
                        p_get -= moons->profile_speed_;
                    } else {
                        p_get = moons->command_position_ - moons->i.actual_position_;
                    }
                }

            }
            //elmo->error_code_ = get_position( p_get );
            //if ( 0 != elmo->error_code_ ) {
            //	return -1;
            //}

            moons->i.actual_position_ += p_get;

            bool b_reach = false;

            if (moons->status_.middle_ == kStatusDescribe_Running
                    && moons->status_.response_ == kStatusDescribe_Running) {
                if (abs(last_postion_ - (int64_t) moons->i.actual_position_) < 30
                        && abs(moons->i.actual_position_ - moons->command_position_) < 10) {
                    ++postion_cnt_;
                }
            }

            if (postion_cnt_ > 5) {
                b_reach = true;
                postion_cnt_ = 10;
            }
            if (b_reach && moons->status_.middle_ != kStatusDescribe_Completed && moons->status_.response_ != kStatusDescribe_Completed) {
                moons->status_.middle_ = kStatusDescribe_Completed;
                moons->status_.response_ = kStatusDescribe_Completed;
                nspinfo << "moons " << node_id << " postion finsihed! p = " << (int64_t) moons->i.actual_position_;
                driver_base::callback_data(var); //??
            }
            last_postion_ = (int64_t) moons->i.actual_position_;

        }
            break;
        default:
            break;
    }

    return 0;
}

int driver_moons::write(var__functional_object_t* var) {
    //if (m_emergency_stop)
    //{
    //	return 0;
    //}
    var__moos_t* moons = var__object_body_ptr(var__moos_t, var);
    switch (moons->control_mode_) {
        case kDriverControlMode_PositionMode:
            //?????
            if (moons->status_.middle_ == kStatusDescribe_Running &&
                    moons->status_.response_ == kStatusDescribe_Running) {
                if (abs(last_postion_ - moons->i.actual_position_) < 3) {
                    position_err_cnt_++;
                }

                if (position_err_cnt_ > 200) {
                    //???
                    //clear_fault();
                    moons->i.error_code_ = set_position((int) moons->command_position_);
                    if (0 != moons->i.error_code_) {
                        return -1;
                    }
                    postion_cnt_ = 0;
                    nspinfo << "moons" << node_id << "retry set_position:" << (int) moons->command_position_;

                    position_err_cnt_ = 0;
                    retry_cnt_++;

                    if (retry_cnt_ > 4 && moons->status_.middle_ != kStatusDescribe_Error && moons->status_.response_ != kStatusDescribe_Error)//error
                    {
                        moons->status_.middle_ = kStatusDescribe_Error;
                        moons->status_.response_ = kStatusDescribe_Error;
                        nspinfo << "moons " << node_id << " postion error!retryed 3 times p = " << (int64_t) moons->i.actual_position_;
                        driver_base::callback_data(var); //??
                    }
                }
            }

            if (moons->status_.command_ == kStatusDescribe_Startup && moons->status_.middle_ == kStatusDescribe_Idle) {
                moons->i.error_code_ = set_position((int) moons->command_position_);
                if (0 != moons->i.error_code_) {
                    return -1;
                }
                moons->status_.middle_ = kStatusDescribe_Running;
                moons->status_.response_ = kStatusDescribe_Running;
                postion_cnt_ = 0;
                position_err_cnt_ = 0;
                retry_cnt_ = 0;
                nspinfo << "moons" << node_id << " set_position:" << (int) moons->command_position_;

            }

            break;
            //	case CIA402MODE_CAN_PV:
            //// 		moons->error_code_ = set_velocity((int)moons->command_velocity_);
            //// 		if (0 != moons->error_code_)
            //// 		{
            //// 			return -1;
            //// 		}
            //		break;
        default:
            break;
    }


    return 0;
}

bool driver_moons::get_stop_state(int di, bool &stop_state) {
    return 0;
}

int driver_moons::read_and_write(var__functional_object_t* var) {
    read(var);
    write(var);
    return 0;
}

int driver_moons::pos_is_reached(bool& b_reached) {
    return 0;

}

int driver_moons::set_position(int pos) {
    return 0;
}

int driver_moons::get_position(int& pos) {
    return 0;
}

int driver_moons::get_di(int& di) {
    return 0;
}

int driver_moons::clear_fault() {
    return 0;

}