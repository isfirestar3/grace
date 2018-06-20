#include "driver_moons.h"
#include "moons.h"
#include "var.h"
#include "log.h"
#include "os_util.hpp"

driver_moons::driver_moons() :driver_base(kVarType_Moons), m_emergency_stop(false)
{
}


driver_moons::~driver_moons()
{
}

int driver_moons::add_node(CML::CanOpen *canOpen, const var__functional_object_t *d)
{
    var__moos_t* moons = var__object_body_ptr(var__moos_t, d);

    node_id = moons->candev_head_.cannode_;
    const Error *err;
    NodeSettings node_settings;
    node_settings.synchUseFirstNode = true;
    node_settings.synchPeriod = 10000;
    node_settings.synchUseFirstNode = true;
    node_settings.heartbeatPeriod = 200;
    node_settings.guardTime = 200;
    err = m_node.Init(*canOpen, node_id, node_settings);

    nsperror << "moons " << node_id << " CK  new version " ;    
    if (err){
        nsperror << "moons " << node_id << " init failed! " << err->toString();
        return -1;
    }
    else
        nspinfo << "moons  " << node_id << " init success!";
    m_node.sdo.SetTimeout(20);

	//{
	//	//int8 mode = 0;
	//	//err = m_node.sdo.Upld8(CIA402_OP_MODE_DISP + 0, 0, mode);
	//	//if (err)
	//	//{
	//	//	nsperror << "moons " << node_id << "get mode Error:" << err->toString();
	//	//	return -1;
	//	//}
	//	//else
	//	{
	//		//nspinfo << "moons " << node_id << " get mode:" << mode;
	//		//if (mode == CIA402MODE_CAN_HOMING
	//		//	|| mode == CIA402MODE_CAN_PP)
	//		{
	//			nsp::os::waitable_handle wt;

	//			while (true) {

	//				wt.wait(1000);
	//				{
	//					int32 reach;
	//					err = m_node.sdo.Upld32(0x6041 + 0, 0, reach);
	//					if (err)
	//					{
	//						nsperror << "moons " << node_id << "get 0x6041 Error:" << err->toString();
	//					}
	//					else
	//					{
	//						nsperror << "moons " << node_id << " 0x6041=" << reach << " Target Reached=1:" << ((reach & 0x400) ? true : false);

	//						//if ((reach & 0x400))
	//						//{
	//						//	break;
	//						//}

	//						//if (reach & 0x2000)
	//						//{
	//						//	nsperror << "moons " << node_id << "home Error";
	//						//	var__error_item_t e = { 0 };
	//						//	e.framwork_ = var__make_error_code(kVarType_Moons, kCanbusDriver_ErrorHoming);
	//						//	driver_base::set_error_code(d->object_id_, e);
	//						//	return -1;
	//						//}
	//					}


	//				}

	//			}

	//		}

	//	}
	//}

    err = 0;

    if (!err)
        err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x06);
    if (!err)
        err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x07);
    if (!err)
        err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x010f);
    if (!err)
        err = m_node.sdo.Dnld8(OBJID_OP_MODE, 0, (uint8)0x01);//Profile Position Mode
    if (!err)
        err = m_node.sdo.Dnld32(OBJID_PROFILE_VEL, 0, (uint32)moons->profile_speed_);
    if (!err)
        err = m_node.sdo.Dnld32(OBJID_PROFILE_ACC, 0, (uint32)moons->profile_acc_);
    if (!err)
        err = m_node.sdo.Dnld32(OBJID_PROFILE_DEC, 0, (uint32)moons->profile_dec_);
    //if (!err)
    //	err = m_node.sdo.Dnld32(OBJID_PROFILE_POS, 0, (uint32)0x0);
    if (!err)
        err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x3f);
    if (!err)
        err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x2f);

    if (err)
    {
        nsperror << " moons " << node_id << "init error = " << err->toString();
        return -1;
    }
    return 0;
}

int driver_moons::read(var__functional_object_t* var)
{
    var__moos_t* moons = var__object_body_ptr(var__moos_t, var);

    moons->i.time_stamp_ = driver_base::get_timestamp() - moons->candev_head_.latency_;

    {
        int16 err = 0;
        m_node.sdo.Upld16(0x603f, 0, err);

        if (err)
        {
			if (last_errcode != err)
			{
				nspwarn << "moons " << node_id << "get errorcode=" << err;
				last_errcode = err;
			}
			
			var__error_item_t e;
            e.status = -1;
            e.software = var__make_error_code(kVarType_Moons, kCanbusDriver_ErrorHardware);
            e.hardware = err;
			driver_base::set_error_code(var->object_id_, e);
        }
        
    }
    moons->i.error_code_ = get_di(moons->i.di_);
    if (moons->i.error_code_)
    {
        driver_base::set_comm_err(moons->i.error_code_);
        return -1;
    }
    bool emergency_stop = false;
    get_stop_state(moons->i.di_, emergency_stop);
    if (emergency_stop)
    {
        if (m_emergency_stop != emergency_stop)
        {
            nspwarn << "moons " << node_id << " EmergencyStopButton is On ,Please Release!";
        }
        m_emergency_stop = emergency_stop;
        var__error_item_t e = { 0 };
        e.status = -1;
        e.software = var__make_warning_code(kVarType_Moons, kCanbusDriver_WarnEmergencyStop);
        driver_base::set_error_code(var->object_id_, e);
        // return -1;
    }
    else if (emergency_stop != m_emergency_stop) {
        clear_fault();

        m_emergency_stop = emergency_stop;
        nspinfo << "moons " << node_id << " EmergencyStopButton is Off!";
    }

    switch (get_ctrl_mode(moons->control_mode_))
    {
    case CIA402MODE_CAN_PP:
    {
                              if (m_emergency_stop)
                              {
                                  return 0;
                              }
                              int p_get = 0;
                              moons->i.error_code_ = get_position(p_get);
                              if (0 != moons->i.error_code_) {
                                  driver_base::set_comm_err(moons->i.error_code_);
                                  return -1;
                              }
                              moons->i.actual_position_ = p_get;

                              bool b_reach = false;

                              if (moons->status_.middle_ == kStatusDescribe_Running
                                  && moons->status_.response_ == kStatusDescribe_Running)
                              {
                                  if (abs(last_postion_ - (int64_t)moons->i.actual_position_) < 5
                                      && abs(moons->i.actual_position_ - moons->command_position_) < 100) {
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
                                  nspinfo << "moons " << node_id << " postion finsihed! p = " << (int64_t)moons->i.actual_position_;
                                  uint64_t dt = driver_base::get_timestamp() - time_position_begin;
                                  nspinfo << "CK TEST moons " << node_id << " deta p=" << deta_position << " deta t=" << dt
                                      << " average speed=" << deta_position / dt;
                                  driver_base::callback_data(var);
                              }

							  if (moons->status_.middle_ == kStatusDescribe_Running &&
								  moons->status_.response_ == kStatusDescribe_Running)
							  {
								  if (abs(last_postion_ - moons->i.actual_position_) < 3)
								  {
									  position_err_cnt_++;
                                                                          nspinfo << "moons" << node_id << " position_err_cnt_:" << position_err_cnt_
                                                                                  <<" lp="<<last_postion_<<" ap="<<moons->i.actual_position_;
								  }
								  else
								  {
									  position_err_cnt_ = 0;
								  }
							  }
                              last_postion_ = (int64_t)moons->i.actual_position_;

    }
        break;
    case CIA402MODE_CAN_PV:
    {

    }
        break;
    default:
        break;
    }

    return 0;
}

int driver_moons::write(var__functional_object_t* var)
{

    var__moos_t* moons = var__object_body_ptr(var__moos_t, var);
    switch (get_ctrl_mode(moons->control_mode_))
    {
    case CIA402MODE_CAN_PP:
        if (m_emergency_stop)
        {
            return 0;
        }
        if (moons->status_.middle_ == kStatusDescribe_Running &&
            moons->status_.response_ == kStatusDescribe_Running)
        {
   //         if (abs(last_postion_ - moons->i.actual_position_) < 3)
   //         {
   //             position_err_cnt_++;
			//}
			//else
			//{
			//	position_err_cnt_ = 0;
			//}

            if (position_err_cnt_ > 50)
            {
                clear_fault();
                moons->i.error_code_ = set_position((int)moons->command_position_);
                if (0 != moons->i.error_code_)
                {
                    driver_base::set_comm_err(moons->i.error_code_);
                    return -1;
                }
                postion_cnt_ = 0;
                nspinfo << "moons" << node_id << "retry set_position:" << (int)moons->command_position_;

                position_err_cnt_ = 0;
                retry_cnt_++;

                if (retry_cnt_ > 4 && moons->status_.middle_ != kStatusDescribe_Error && moons->status_.response_ != kStatusDescribe_Error)//error
                {
                    moons->status_.middle_ = kStatusDescribe_Error;
                    moons->status_.response_ = kStatusDescribe_Error;
                    nspinfo << "moons " << node_id << " postion error!retryed 3 times p = " << (int64_t)moons->i.actual_position_;
                    driver_base::callback_data(var);
                }
            }
        }

        if (moons->status_.command_ == kStatusDescribe_Startup && moons->status_.middle_ == kStatusDescribe_Idle)
        {
            moons->i.error_code_ = set_position((int)moons->command_position_);
            if (0 != moons->i.error_code_)
            {
                driver_base::set_comm_err(moons->i.error_code_);
                return -1;
            }
            moons->status_.middle_ = kStatusDescribe_Running;
            moons->status_.response_ = kStatusDescribe_Running;
            postion_cnt_ = 0;
            position_err_cnt_ = 0;
            retry_cnt_ = 0;
            nspinfo << "moons" << node_id << " set_position:" << (int)moons->command_position_;
            time_position_begin = driver_base::get_timestamp();
            deta_position = moons->command_position_ - moons->i.actual_position_;
        }

        break;
    case CIA402MODE_CAN_PV:
        // 		moons->error_code_ = set_velocity((int)moons->command_velocity_);
        // 		if (0 != moons->error_code_)
        // 		{
        // 			return -1;
        // 		}
        break;
    default:
        break;
    }


    return 0;
}




bool driver_moons::get_stop_state(int di, bool &stop_state)
{
    uint16 value = di;
    stop_state = true;

    int m_switch_id = 4;
    bool m_effect_io = false;
    value >>= m_switch_id;
    value &= 0x001;
    if (m_effect_io)
    {
        if (value)
        {
            stop_state = false;
        }
        else
        {
            stop_state = true;
        }
    }
    else
    {
        if (value)
        {
            stop_state = true;
        }
        else
        {
            stop_state = false;
        }
    }
    return true;
}

int driver_moons::read_and_write(var__functional_object_t* var)
{
	if (driver_base::need_clearfault(var->object_id_))
	{
		clear_fault();
	}
    read(var);
    write(var);
    return 0;
}


int driver_moons::pos_is_reached(bool& b_reached)
{
    int32 reach;
    const Error *err = m_node.sdo.Upld32(OBJID_STATUS, 0, reach);
    if (err)
    {
        nsperror << " moons " << node_id << "pos_is_reached error = " << err->toString();
        return -1;
    }
    b_reached = (reach & 0x0400) == 0x0400;
    return 0;
}

int driver_moons::set_position(int pos)
{

    const Error *err = 0;
    int32 counts;
    err = m_node.sdo.Upld32(OBJID_POS_ACT, 0, counts);
    if (err)
    {
        nsperror << " moons" << node_id << " get position counts error = " << err->toString();
        return -1;
    }
    nspinfo << " moons" << node_id << " get position counts = " << counts;
    int32 position;
    err = m_node.sdo.Upld32(0x700A, 0, position);
    if (err)
    {
        nsperror << " moons" << node_id << " get encoder position error = " << err->toString();
        return -1;
    }
    nspinfo << " moons" << node_id << " get encoder position = " << position;

    int32 position_motor = pos - position + counts;
    nspinfo << " moons" << node_id << " set position counts= " << position_motor;
    /*
    60B 08 00 2B 40 60 00 06 00 00 00
    60B 08 00 2B 40 60 00 07 00 00 00
    60B 08 00 2B 40 60 00 0F 01 00 00
    60B 08 00 2F 60 60 00 01 00 00 00
    60B 08 00 23 81 60 00 60 09 00 00
    60B 08 00 23 83 60 00 58 02 00 00
    60B 08 00 23 84 60 00 58 02 00 00
    60B 08 00 23 7A 60 00 40 0D 03 00
    60B 08 00 2B 40 60 00 1F 00 00 00
    60B 08 00 2B 40 60 00 0F 00 00 00
    */
    if (!err)
        err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x06);
    if (!err)
        err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x07);
    if (!err)
        err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x010f);

    if (err)
    {
        nsperror << " moons " << node_id << "aa set postion error = " << err->toString();
        return -1;
    }
    err = m_node.sdo.Dnld32(OBJID_PROFILE_POS, 0, (int32)position_motor);
    if (err)
    {
        nsperror << " moons " << node_id << " set postion error = " << err->toString();
        return -1;
    }
    err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x1f);
    if (err)
    {
        nsperror << " moons " << node_id << " error = " << err->toString();
        return -1;
    }
    err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x0f);
    if (err)
    {
        nsperror << " moons " << node_id << " error = " << err->toString();
        return -1;
    }

    return 0;
}

int driver_moons::get_position(int& pos)
{
    const Error *err = 0;
    int32 position;
    if (!err)
        err = m_node.sdo.Upld32(0x700A, 0, position);

    if (err)
    {
        nsperror << " moons " << node_id << "get_position error = " << err->toString();
        return -1;
    }

    pos = position;
    return 0;
}

int driver_moons::get_di(int& di)
{
    const Error *err = 0;
    uint16 value;
    if (!err)
        err = m_node.sdo.Upld16(0x7003, 0, value);

    if (err)
    {
        nsperror << " moons " << node_id << "get_position error = " << err->toString();
        di = 0;
        return -1;
    }

    di = value;
    return 0;

}

int driver_moons::clear_fault()
{
    const Error *err = m_node.sdo.Dnld16(CIA402_CONTROL, 0, (uint16)0x0080);
    if (err)
    {
        nsperror << " moons " << node_id << "clear_fault : error = " << err->toString();
        return err->GetID();
    }
    err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x0005);
    if (err)
    {
        nsperror << " moons " << node_id << " error = " << err->toString();
        return -1;
    }
    err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x0006);
    if (err)
    {
        nsperror << " moons " << node_id << " error = " << err->toString();
        return -1;
    }
    err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x0007);
    if (err)
    {
        nsperror << " moons " << node_id << " error = " << err->toString();
        return -1;
    }
    err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x000F);
    if (err)
    {
        nsperror << " moons " << node_id << " error = " << err->toString();
        return -1;
    }

	err = m_node.sdo.Dnld32(0x7006, 0, (int32)0x0001);
	if (err)
	{
		nsperror << " moons " << node_id << " error = " << err->toString();
		return -1;
	}
	err = m_node.sdo.Dnld16(OBJID_CONTROL, 0, (uint16)0x018F);
	if (err)
	{
		nsperror << " moons " << node_id << " error = " << err->toString();
		return -1;
	}
    return 0;
}

CML::CIA402_6060_CODE driver_moons::get_ctrl_mode(control_mode_t mode) {
    switch (mode) {
        case kDriverControlMode_SpeedMode:
            return CML::CIA402MODE_CAN_PV;
        case kDriverControlMode_PositionMode:
            return CML::CIA402MODE_CAN_PP;
        default:
            break;
    }

    return CML::CIA402MODE_DISABLED;
}