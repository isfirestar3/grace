#include "driver_elmo.h"
#include "elmo.h"
#include "var.h"
#include "log.h"
#include <iostream>
#include "os_util.hpp"

driver_elmo::driver_elmo() :
driver_base(kVarType_Elmo) ,
m_emergency_stop( false ) {
}


driver_elmo::~driver_elmo() {
}

int driver_elmo::add_node( CML::CanOpen *canOpen, const var__functional_object_t *d ) {
	var__elmo_t* elmo = var__object_body_ptr( var__elmo_t, d );

	node_id = elmo->candev_head_.cannode_;
	const CML::Error *err;
	CML::NodeSettings node_settings;
	node_settings.synchUseFirstNode = true;
	node_settings.synchPeriod = 10000;
	node_settings.synchUseFirstNode = true;
	node_settings.heartbeatPeriod = 200;
	node_settings.guardTime = 200;
	err = m_node.Init( *canOpen, node_id, node_settings );
	if ( err ) {
		nsperror << "Init ElmoChassis Node Fail! " << node_id << " Error: " << err->toString();
		return err->GetID();
	} else {
		err = m_amp.Init( m_node, 1 );
		if ( err ) {
			nsperror << "Init ElmoChassis Amp Fail! " << node_id << " Error:" << err->toString();
			return err->GetID();
		}
	}

	nspinfo << "Init ElmoChassis Amp success!" << node_id;

	m_node.sdo.SetTimeout(20);

	//{
	//	int8 mode = 0;
	//	err = m_node.sdo.Upld8(CIA402_OP_MODE_DISP + 0, 0, mode);
	//	if (err)
	//	{
	//		nsperror << "elmo " << node_id << "get mode Error:" << err->toString();
	//		return -1;
	//	}
	//	else
	//	{
	//		nspinfo << "elmo " << node_id << " get mode:" << mode;
	//		if (mode == CIA402MODE_CAN_HOMING
	//			|| mode == CIA402MODE_CAN_PP)
	//		{
	//			nsp::os::waitable_handle wt;

	//			while (true) {

	//				wt.wait(1000);
	//				{
	//					int32 reach;
	//					err = m_node.sdo.Upld32(0x6041 + 0, 0, reach);
	//					if (err)
	//					{
	//						nsperror << "elmo " << node_id << "get 0x6041 Error:" << err->toString();
	//					}
	//					else
	//					{
	//						//if ((reach & 0x400))
	//						//{
	//						//	break;
	//						//}

	//						//if (reach & 0x2000)
	//						//{
	//						//	nsperror << "elmo " << node_id << " home Error";
	//						//	var__error_item_t e = { 0 };
	//						//	e.framwork_ = var__make_error_code(kVarType_Elmo, kCanbusDriver_ErrorHoming);
	//						//	driver_base::set_error_code(d->object_id_, e);
	//						//	return -1;
	//						//}
	//					}
	//					nsperror << "elmo " << node_id << " 0x6041=" << reach << " Target Reached=" << ((reach & 0x400) ? true : false);


	//				}

	//			}

	//		}

	//	}


	//}

	//while (true)
	//{
	//	int32  zero = 0;
	//	const Error* err = m_node.sdo.Upld32(0x2f00, 1, zero);
	//	if (err)
	//	{
	//		nsperror << "elmo " << node_id << "get UI[1] Error:" << err->toString();
	//		return -1;
	//	}

	//	if (zero != 2)
	//	{

	//	}
	//}


	
	
	m_amp.ClearFaults();
	m_amp.Disable();
	m_amp.Enable();

	err = m_amp.SetProfileAcc((int32)elmo->profile_acc_);
	if (err)
	{
		nsperror << "elmo " << node_id << " SetProfileAcc:" << (int32)elmo->profile_acc_ << " failed," << err->toString();
		return -1;
	}
	nspinfo << "elmo " << node_id << " SetProfileAcc:" << (int32)elmo->profile_acc_;

	err = m_amp.SetProfileDec((int32)elmo->profile_dec_);
	if (err)
	{
		nsperror << "elmo " << node_id << " SetProfileDec:" << (int32)elmo->profile_dec_ << " failed," << err->toString();
		return -1;
	}
	nspinfo << "elmo " << node_id << " SetProfileDec:" << (int32)elmo->profile_dec_;

	err = m_amp.SetProfileVel((int32)elmo->profile_speed_);
	if (err)
	{
		nsperror << "elmo " << node_id << " SetProfileVel:" << (int32)elmo->profile_speed_ << " failed," << err->toString();
		return -1;
	}
	nspinfo << "elmo " << node_id << " SetProfileVel:" << (int32)elmo->profile_speed_;


	int32 rate;
	const Error* e1 = m_amp.GetCurrentRate(rate);
	if (e1) {
		nsperror << "elmo " << node_id << " GetCurrentRate:" << e1->toString();
		return -1;
	}
	nspinfo << "elmo " << node_id << " GetCurrentRate:" << rate;

	current_rate = rate;

	if (last_control_mode_ != elmo->control_mode_)
	{
		if (get_ctrl_mode(elmo->control_mode_)>0)
		{
			const Error* err = m_amp.SetOperationMode(get_ctrl_mode(elmo->control_mode_));
			if (err) {
				nsperror << "elmo " << node_id << " SetOperationMode " << (int)elmo->control_mode_ << " failed!" << err->toString();
				return err->GetID();
			}
			nspinfo << "elmo " << node_id << " SetOperationMode " << (int)elmo->control_mode_;
			last_control_mode_ = elmo->control_mode_;
		}
	}

    {
        const CML::Error*err = 0;
        err = m_node.sdo.Dnld16(0x2f00, 0x01, (int16)0x02);
        if (err) {
            nsperror << "elmo " << node_id << " set heartbeat  failed.FR:" << err->toString();
        }
    }
	return 0;
}

int driver_elmo::read( var__functional_object_t* var ) {
	var__elmo_t* elmo = var__object_body_ptr( var__elmo_t, var );
	elmo->i.time_stamp_ = driver_base::get_timestamp() - elmo->candev_head_.latency_;

    {
        int err = 0;
        if (get_error_code(err) == 0)
        {
            var__error_item_t e = { 0 };
			driver_base::get_error_obj(var->object_id_, e);
            if (err)
            {
				if (err == 21569)
				{
					//e.condition_code_ = 1;
				}
				else
				{
					e.status = -1;
				}
                
                e.software = var__make_error_code(kVarType_Elmo, kCanbusDriver_ErrorHardware);
                e.hardware = err;
				driver_base::set_error_code(var->object_id_, e);
            }
            
        }
        
    }

	elmo->i.error_code_ = get_di(elmo->i.di_);
	if (elmo->i.error_code_) {
        driver_base::set_comm_err(elmo->i.error_code_);
		return -1;
	}
	bool emergency_stop = false;
	get_stop_state(elmo->i.di_, emergency_stop);
	if (emergency_stop) {
		if (m_emergency_stop != emergency_stop)
		{
			nspwarn << "elmo " << node_id << " EmergencyStopButton is On ,Please Release!"; 
			m_amp.Disable();
		}
        
		m_emergency_stop = emergency_stop;
		elmo->i.actual_velocity_ = 0;
        var__error_item_t e = { 0 };
        e.status = -1;
        e.software = var__make_warning_code(kVarType_Elmo, kCanbusDriver_WarnEmergencyStop);
        driver_base::set_error_code(var->object_id_, e);
		//return -1;
	}
	else if (emergency_stop != m_emergency_stop) {
		m_amp.ClearFaults();
        m_amp.Disable();
        m_amp.Enable();
		const CML::Error *err = m_amp.SetOperationMode(get_ctrl_mode(elmo->control_mode_));
		if (err) {
            driver_base::set_comm_err(err->GetID());
			std::cout << "elmo " << node_id << " SetOperationMode " << (int)get_ctrl_mode(elmo->control_mode_) << "  failed.FR:" << err->toString();
			return err->GetID();
		}
		m_amp.Disable();
		err = m_amp.Enable();
		if (err) {
            driver_base::set_comm_err(err->GetID());
			nsperror << "elmo " << node_id << " Enable failed" << err->toString();
			return err->GetID();
		}

		m_emergency_stop = emergency_stop;
		nspwarn << "elmo " << node_id << " EmergencyStopButton is Off !";
	}

	switch ( get_ctrl_mode( elmo->control_mode_ ) ) {
		case CIA402MODE_CAN_PP:
			{
				int p_get = 0;
				elmo->i.error_code_ = get_position(p_get);
				if (0 != elmo->i.error_code_) {
                    driver_base::set_comm_err(elmo->i.error_code_);
					return -1;
				}
				elmo->i.actual_position_ = p_get;

				if (elmo->status_.middle_ == kStatusDescribe_Running &&
					elmo->status_.response_ == kStatusDescribe_Running)
				{
					bool b_reach = false;
					if (abs(last_postion_ - (int64_t)elmo->i.actual_position_) < 3)
					{
						error_cnt_++;
					}
					if (abs(elmo->i.actual_position_ - elmo->command_position_) < 100) {
						++postion_cnt_;
					}
					if (postion_cnt_ > 5 && error_cnt_ > 5) {
						b_reach = true;
						postion_cnt_ = 10;
					}
					else if (error_cnt_ > 200)
					{
						//???
						m_amp.ClearFaults();
						m_amp.Disable();
						m_amp.Enable();
						elmo->i.error_code_ = set_position((int)elmo->command_position_);
						if (0 != elmo->i.error_code_)
						{
                            driver_base::set_comm_err(elmo->i.error_code_);
							return -1;
						}
						postion_cnt_ = 0;
						nspinfo << "elmo " << node_id << " retry set_position:" << (int)elmo->command_position_;

						error_cnt_ = 0;
						retry_cnt_++;

						if (retry_cnt_++ > 3 && elmo->status_.middle_ != kStatusDescribe_Error && elmo->status_.response_ != kStatusDescribe_Error)
						{
							elmo->status_.middle_ = kStatusDescribe_Error;
							elmo->status_.response_ = kStatusDescribe_Error;
							nspinfo << "elmo " << node_id << " postion error! p = " << (int64_t)elmo->i.actual_position_;
							driver_base::callback_data(var);//??
						}
					}

					if (b_reach && elmo->status_.middle_ != kStatusDescribe_Completed && elmo->status_.response_ != kStatusDescribe_Completed) {
						elmo->status_.middle_ = kStatusDescribe_Completed;
						elmo->status_.response_ = kStatusDescribe_Completed;
						nspinfo << "elmo " << node_id << " postion finsihed! p = " << (int64_t)elmo->i.actual_position_;
						driver_base::callback_data(var);//??
						uint64_t dt = driver_base::get_timestamp() - time_position_begin;
						nspinfo << "CK TEST elmo " << node_id << " deta p=" << deta_position << " deta t=" << dt
							<< " average speed=" << deta_position / dt;
					}
					last_postion_ = (int64_t)elmo->i.actual_position_;
				}
			}
			break;
		case CIA402MODE_CAN_PV:
		case 2:
			{
				int v_get = 0;
				elmo->i.error_code_ = get_veloctiy(v_get);
				if (0 != elmo->i.error_code_) {
                    driver_base::set_comm_err(elmo->i.error_code_);
					return -1;
				}
				elmo->i.actual_velocity_ = v_get;
                driver_base::check_velocity_follow_err(var, (double)elmo->command_velocity_, (double)elmo->i.actual_velocity_,(double)elmo->profile_acc_);
			}
			break;
		default:
			break;
	}


	int current_get = 0;
	elmo->i.error_code_ = get_current(current_get);
	if (0 != elmo->i.error_code_) {
        driver_base::set_comm_err(elmo->i.error_code_);
		return -1;
	}
	elmo->i.actual_current_ = current_get;

	return 0;
}

int driver_elmo::write( var__functional_object_t* var ) {
   
    {
        const CML::Error*err = 0;
        err = m_node.sdo.Dnld16(0x2f00, 0x01, (int16)0x02);
        if (err) {
            nsperror << "elmo " << node_id << " set heartbeat  failed.FR:" << err->toString();
        }
    }

	var__elmo_t* elmo = var__object_body_ptr( var__elmo_t, var );


	if ( enabled_ != elmo->enable_ ) {
		elmo->i.error_code_ = set_enable(elmo->enable_ == 1);
		if (0 != elmo->i.error_code_) {
            driver_base::set_comm_err(elmo->i.error_code_);
			return -1;
		}
		nspinfo << "elmo" << node_id << " set enable " << elmo->enable_;
	}
	enabled_ = elmo->enable_;
	elmo->i.enabled_ = enabled_;
	if (last_control_mode_ != elmo->control_mode_)
	{
		if (get_ctrl_mode(elmo->control_mode_)>0)
		{
			const Error* err = m_amp.SetOperationMode(get_ctrl_mode(elmo->control_mode_));
			if (err) {
                driver_base::set_comm_err(err->GetID());
				nsperror << "elmo " << node_id << "SetOperationMode:" << (int)elmo->control_mode_ << "  failed.FR:" << err->toString();
				return err->GetID();
			}
			last_control_mode_ = elmo->control_mode_;
			nspinfo << "elmo " << node_id << " SetOperationMode to:" << (int)elmo->control_mode_;
		}
	}

	switch ( get_ctrl_mode( elmo->control_mode_ ) ) {
		case CIA402MODE_CAN_PP:
            if (m_emergency_stop)
            {
                return 0;
            }
			if ( (elmo->status_.command_ == kStatusDescribe_Startup )
				&& (elmo->status_.middle_ == kStatusDescribe_Idle) ) {

				elmo->i.error_code_ = set_position((int)elmo->command_position_);
				if (0 != elmo->i.error_code_) {
                    driver_base::set_comm_err(elmo->i.error_code_);
					return -1;
				}
				elmo->status_.middle_ = kStatusDescribe_Running;
				elmo->status_.response_ = kStatusDescribe_Running;
				postion_cnt_ = 0;
				error_cnt_ = 0;
				retry_cnt_ = 0;
				nspinfo << "elmo" << node_id << " set_position:" << (int)elmo->command_position_;
				time_position_begin = driver_base::get_timestamp();
				deta_position = elmo->command_position_ - elmo->i.actual_position_;
			}
			break;
		case CIA402MODE_CAN_PV:
		case 2:
			elmo->i.error_code_ = set_velocity((int)elmo->command_velocity_);
			if (0 != elmo->i.error_code_) {
                driver_base::set_comm_err(elmo->i.error_code_);
				return -1;
			}
			break;
		default:
			break;
	}


	return 0;
}


int driver_elmo::set_velocity( int v ) {
	CML::int32 v_send = v;
	const CML::Error* e = m_amp.SetTargetVel( v_send );
	if ( e ) {
		nsperror << "elmo " << node_id << " SetTargetVel:" << v_send << " failed,FR: " << e->toString();
		return e->GetID();
	}

	return 0;
}

int driver_elmo::get_veloctiy( int& v ) {
	CML::int32 v_get = 0;
	const CML::Error* e = m_amp.GetVelocityActual( v_get );
	if ( e ) {
		nsperror << "elmo " << node_id << " GetVelocityActual failed,FR:" << e->toString();
		return e->GetID();
	}

	
	v = v_get;
	if (abs(v) > 1500000)
		v = 0;

	return 0;
}

int driver_elmo::set_enable( bool enable ) {
	const CML::Error*err = 0;
	if ( enable ) {
		err = m_amp.Disable();
		err = m_amp.Enable();
	} else {
		err = m_amp.Disable();
	}

	if ( err ) {
		nsperror << "elmo " << node_id << " set_enable " << enable << "  failed.FR:" << err->toString();
		m_amp.ClearFaults();
		return err->GetID();
	}


	return 0;
}

int driver_elmo::get_current( int& c ) {
	int16 cur;
	const Error* e = m_amp.GetCurrentActual( cur );
	if ( e ) {
		nsperror << "elmo " << node_id << " GetCurrentActual failed.FR:" << e->toString();
		return e->GetID();
	}
	c = cur * current_rate / 1000;

	return 0;

}

bool driver_elmo::get_stop_state( int di, bool &stop_state ) {

	stop_state = false;
	//return true;
	uint16 value = di;
	stop_state = true;

	int m_switch_id = 0;//2; DIO e-stop
	bool m_effect_io = false;
	value >>= m_switch_id ;
	value &= 0x001;
        
	if ( m_effect_io ) {
		if ( value ) {
			stop_state = false;
		} else {
			stop_state = true;
		}
	} else {
		if ( value ) {
			stop_state = true;
		} else {
			stop_state = false;
		}
	}
	return true;
}

int driver_elmo::read_and_write( var__functional_object_t* var ) {
	if (driver_base::need_clearfault(var->object_id_))
	{
		m_amp.ClearFaults();
		m_amp.Disable();
		m_amp.Enable();
	}
	read( var );
	write( var );
	return 0;
}

int8 driver_elmo::get_ctrl_mode( control_mode_t mode ) {
	switch ( mode ) {
		case kDriverControlMode_SpeedMode:
			return 3;
		case kDriverControlMode_PositionMode:
			return CML::CIA402MODE_CAN_PP;
            default:
                break;
	}

	return CML::CIA402MODE_DISABLED;
}

int driver_elmo::set_position( int pos ) {
	CML::int32 p_send = pos;
	const CML::Error* e = m_amp.SetTargetPos( p_send );
	if ( e ) {
		nsperror << "elmo " << node_id << " SetTargetPos:" << p_send << " failed.FR: " << e->toString();
		return e->GetID();
	}

	return 0;

}

int driver_elmo::get_position( int& pos ) {
	CML::int32 p;
	const Error* e = m_amp.GetPositionActual( p );
	if ( e ) {
		nsperror << "elmo " << node_id << " GetPositionActual  failed.FR:" << e->toString();
		return e->GetID();
	}
	pos = p;
	return 0;
}

int driver_elmo::get_di( int& di ) {
	uint16 value;
	const Error *e1 = m_amp.GetInputs( value );
	if ( e1 ) {
		nsperror << "elmo " << node_id << " GetInputs FR:" << e1->toString();
		return e1->GetID();
	}

	di = value;
	return 0;
}

int driver_elmo::pos_is_reached( bool& b_reached ) {
	b_reached = false;
	const Error *e1 = m_amp.PosIsReached( b_reached );
	if ( e1 ) {
		nsperror << "elmo " << node_id << " PosIsReached  failed.FR:" << e1->toString();
		return -1;
	}

	return 0;
}

int driver_elmo::get_error_code( int &error ) {
	
	CML::int32 value;
	const Error* e = m_amp.GetErrorCode( value );
	if ( e ) {
		nsperror << "elmo " << node_id << "elmo GetErrorCode  failed.FR:" << e->toString();
		return -1;
	}
	error = value;
	if (error != last_err)
    {
		nsperror << "elmo " << node_id << "elmo GetErrorCode=" << error;
    }
	last_err = error;
	return 0;
}
