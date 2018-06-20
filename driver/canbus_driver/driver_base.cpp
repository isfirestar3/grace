
#include "driver_base.h"
#include "log.h"
#include <numeric>
#include <cmath>

driver_base::driver_base(var__types type) :__drv_type(type)
{
}


driver_base::~driver_base()
{
}

uint64_t driver_base::get_timestamp()
{
#if _WIN32
    LARGE_INTEGER counter;
    static LARGE_INTEGER frequency = { 0 };
    if (0 == frequency.QuadPart) {
        if (!QueryPerformanceFrequency(&frequency)) {
            return 0;
        }
    }

    if (QueryPerformanceCounter(&counter)) {
        return (1000 * counter.QuadPart / frequency.QuadPart);
    }
    return 0;
#else
    // gcc -lrt
    struct timespec tsc;
    if (clock_gettime(CLOCK_MONOTONIC, &tsc) >= 0) { // CLOCK_REALTIME CLOCK_MONOTONIC
        return tsc.tv_sec * 1000 + tsc.tv_nsec / 1000 / 1000; // 
    }
    return 0;
#endif
}

int driver_base::set_driver_callback(void(*driver_call_back)(void *functional_object))
{
    driver_call_back_ = driver_call_back;
    return 0;
}

void driver_base::callback_data(void* v)
{
    if (driver_call_back_)
    {
        driver_call_back_(v);
    }
}

int driver_base::canio__set_error_ptr(var__functional_object_t *functional_object)
{
    if (functional_object)
    {
        error_var_ = var__object_body_ptr(var__error_handler_t, functional_object);
    }
    return 0;
}

void driver_base::set_error_code(int obj_id, const var__error_item_t& err)
{
    if (error_var_)
    {
        error_var_->error_[obj_id] = err;
    }
}

int driver_base::get_error_obj(int obj_id, var__error_item_t& err)
{
	if (error_var_)
	{
		err = error_var_->error_[obj_id];
		return 0;
	}
	return -1;
}

int driver_base::rw(var__functional_object_t* var)
{
    read_and_write(var);
    if (error_var_)
    {
        if (__cur_window_count > 10 &&
            ((double)__err_cout_in_window / __cur_window_count >= error_var_->driver_err_thres_framelost_))
        {
            var__error_item_t& e = error_var_->error_[var->object_id_];
            e.status |= 0x02;
			if (e.status > 0)
				e.status *= -1;
            e.software = var__make_error_code(__drv_type, kCanbusDriver_ErrorFrameWindowExtend);
            nsperror << "driver_base lost error obj_id= " << var->object_id_ << " lost per=" << double((double)__err_cout_in_window / __cur_window_count);
        }

        if (__err_cons > error_var_->driver_err_cons_lost_cnt_)
        {
            var__error_item_t& e = error_var_->error_[var->object_id_];
			e.status =-1;
            e.software = var__make_error_code(__drv_type, kCanbusDriver_ErrorFrameCountExtend);
            nsperror << "driver_base lost_cons error obj_id= " << var->object_id_ << " lost count=" << __err_cons;

        }
    }

    return 0;
}

void driver_base::set_comm_err(int error_code)
{
    if (error_var_)
    {
        __q_err_window.push_back(error_code);
        if (error_code != 0)
        {
            __err_cout_in_window++;
        }
        if ((int)__q_err_window.size() > error_var_->driver_err_frame_window_)
        {
            int e = __q_err_window.front();
            if (e != 0)
            {
                __err_cout_in_window--;
            }
            __q_err_window.pop_front();
        }
        __cur_window_count = (int)__q_err_window.size();
    }
}

int driver_base::check_velocity_follow_err(var__functional_object_t* var, double cmd_velocity, double act_velocity,double acc)
{

    if (error_var_)
    {
        acc_min_ = error_var_->driver_err_speed_min_acc_;
        double diff = 0.0;
        if (cmd_velocity >= act_velocity){
            limit_low_ = pre_actual_ + acc_min_;
            limit_high_ = cmd_velocity;
            if (limit_low_ > limit_high_){
                limit_low_ = limit_high_;
            }
        }
        else{
            limit_low_ = cmd_velocity;
            limit_high_ = pre_actual_ - acc_min_;
            if (limit_high_ < limit_low_){
                limit_high_ = limit_low_;
            }
        }

        if (act_velocity > limit_high_){
            //diff  = fabs(actual - limit_high_);
            diff = act_velocity - limit_high_;
        }
        else if (act_velocity < limit_low_){
            //diff  = fabs(actual - limit_low_);
            diff = act_velocity - limit_low_;
        }
        else{
            diff = 0;
        }

        pre_actual_ = act_velocity;

        if ((int)track_error_speed_list_.size() < window_size_){
            track_error_speed_list_.push_back(diff);
            return true;
        }
        track_error_speed_list_.pop_front();
        track_error_speed_list_.push_back(diff);

        double err = std::accumulate(track_error_speed_list_.begin(), track_error_speed_list_.end(), 0.0);

        if (fabs(err) > error_var_->driver_err_speed_follow_error_)
        {
            var__error_item_t& e = error_var_->error_[var->object_id_];
            e.status |= 0x04;
			if (e.status > 0)
				e.status *= -1;
            e.software = var__make_error_code(__drv_type, kCanbusDriver_ErrorFrameSpeedFollowErr);
			nsperror << "driver_base obj=" << var->object_id_ << " kCanbusDriver_ErrorFrameSpeedFollowErr err=" << err << " thres=" << error_var_->driver_err_speed_follow_error_;
        }

    }
    return  0;
}

bool driver_base::need_clearfault(int obj_id)
{
	if (__last_err_it.status <0
		&& error_var_->error_[obj_id].status>=0)
	{
        __last_err_it = error_var_->error_[obj_id];
		return true;
	}
    __last_err_it = error_var_->error_[obj_id];
	return false;
}
