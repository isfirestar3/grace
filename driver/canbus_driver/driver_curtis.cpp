#include "driver_curtis.h"
#include "curtis.h"
#include "var.h"
#include "log.h"
#include <iostream>
#include <math.h>
#include <cstdlib>


driver_curtis::driver_curtis() :driver_base(kVarType_Curtis) {
}

driver_curtis::~driver_curtis() {
}

int driver_curtis::add_node(CML::CanOpen *canOpen, const var__functional_object_t *d) {
	var__curtis_t* curtis = var__object_body_ptr(var__curtis_t, d);

	_canOpen = canOpen;
	temp_recv_id = curtis->recv_id_;
	_canOpen->EnableReceiver(curtis->recv_id_, this);//or this.Get()//完成了使能和加入结点node两个功能

	return 0;
}

int driver_curtis::NewFrame(CanFrame &recv_frame){	
	std::lock_guard<decltype(__mtx_curtis)> lock(__mtx_curtis);
	//tempFrame.data= recv_frame;
	if (temp_recv_id == recv_frame.id)		
		memcpy(temp_recv_data, recv_frame.data, recv_frame.length);
	nsptrace << "curtis " << " read data[8]: " << (int)recv_frame.data[0] << " "
		<< (int)recv_frame.data[1] << " "
		<< (int)recv_frame.data[2] << " "
		<< (int)recv_frame.data[3] << " "
		<< (int)recv_frame.data[4] << " "
		<< (int)recv_frame.data[5] << " "
		<< (int)recv_frame.data[6] << " "
		<< (int)recv_frame.data[7];
	return 0;
}

void driver_curtis::analysis_1(var__curtis_t* curtis){

	if (temp_recv_data[7] & 0x01){

		curtis->i.actual_velocity_ = (temp_recv_data[1] * 256 + temp_recv_data[0]);
	}
	if (temp_recv_data[7] & 0x02){
	
		curtis->i.actual_velocity_ = ((-1)*(temp_recv_data[1] * 256 + temp_recv_data[0]));
	}
	curtis->i.error_code_ = (int)temp_recv_data[2];
	curtis->i.actual_angle_ = (temp_recv_data[5] * 256 + temp_recv_data[4]);
	curtis->i.voltage_ = (int)temp_recv_data[6];
	curtis->i.control_feedback_ = (int)temp_recv_data[7];

}

int driver_curtis::read(var__functional_object_t* var){
	var__curtis_t* curtis = var__object_body_ptr(var__curtis_t, var);
	curtis->i.timestamp_ = driver_base::get_timestamp() - curtis->candev_head_.latency_;
	
	std::lock_guard<decltype(__mtx_curtis)> lock(__mtx_curtis);

    switch (curtis->internel_type_)
    {
    case 1:
        analysis_1(curtis);
    	break;
    case 2:
        analysis_2(curtis);
        break;
    default:
        break;
    }
	
	
	return 0;
}

int driver_curtis::write(var__functional_object_t* var) {
	var__curtis_t* curtis = var__object_body_ptr(var__curtis_t, var);
	const Error *err = 0;

	CanFrame send_frame;
	send_frame.id = curtis->send_id_;//???
	send_frame.type = (CML::CAN_FRAME_TYPE) CAN_FRAME_DATA;
	send_frame.length = 8;
	memset(send_frame.data, 0, sizeof(send_frame.data));

    switch (curtis->internel_type_)
    {
    case 1:
        package_1(curtis, send_frame);
        break;
    case 2:
        package_2(curtis, send_frame);
        break;
    default:
        break;
    }	 
	err=_canOpen->Xmit(send_frame, 1000);//发送 ******************88
	if (err)
	{
		curtis->i.error_code_ = err->GetID();
		nsperror << " curtis " << " error:  " << err->toString();
		return -1;
	}

	nsptrace << "curtis " << " send_data[8]: " << (int)send_frame.data[0] << " " << (int)send_frame.data[1] << " " << (int)send_frame.data[2]
		<< " " << (int)send_frame.data[3] << " " 
		<< (int)send_frame.data[4] << " " 
		<< (int)send_frame.data[5] << " " 
		<< (int)send_frame.data[6] << " " 
		<< (int)send_frame.data[7];
	return 0;
}

void driver_curtis::package_1(var__curtis_t* curtis, CanFrame &send_frame){
	send_frame.data[0] = 0;
	send_frame.data[1] |= 0x01;
	
    if (0.1 > abs(curtis->command_velocity_))
	{
		send_frame.data[0] = 1;
	}
	else
	{
		send_frame.data[2] = (unsigned char)((int)(curtis->command_velocity_) & 0xFF);
		send_frame.data[3] = (unsigned char)(((int)(curtis->command_velocity_) >> 8) & 0xff);
	}

	if (0 > curtis->command_lift_speed1_)
	{
		send_frame.data[0] &= 0xEF;
		send_frame.data[0] |= 0x20;
		send_frame.data[4] = (unsigned char)((int)(curtis->command_lift_speed1_) & 0xFF);
		send_frame.data[5] = (unsigned char)(((int)(curtis->command_lift_speed1_) >> 8) & 0xff);
	}
	else if (0 < curtis->command_lift_speed1_)
	{
		send_frame.data[0] &= 0xDF;
		send_frame.data[0] |= 0x10;
		send_frame.data[4] = (unsigned char)((int)(curtis->command_lift_speed1_) & 0xFF);
		send_frame.data[5] = (unsigned char)(((int)(curtis->command_lift_speed1_) >> 8) & 0xff);
	}
	else
	{
		send_frame.data[0] &= 0xCF;
	}

	if (0 > curtis->command_lift_speed2_)
	{
		send_frame.data[0] &= 0xBF;
		send_frame.data[0] |= 0x80;
		send_frame.data[6] = (unsigned char)((int)(curtis->command_lift_speed2_) & 0xFF);
		send_frame.data[7] = (unsigned char)(((int)(curtis->command_lift_speed2_) >> 8) & 0xff);
	}
	else if (0 < curtis->command_lift_speed2_)
	{
		send_frame.data[0] &= 0x7F;
		send_frame.data[0] |= 0x40;
		send_frame.data[6] = (unsigned char)((int)(curtis->command_lift_speed2_) & 0xFF);
		send_frame.data[7] = (unsigned char)(((int)(curtis->command_lift_speed2_) >> 8) & 0xff);
	}
	else
	{
		send_frame.data[0] &= 0x3F;
	}
}

int driver_curtis::read_and_write(var__functional_object_t* var) {
	read(var);
	write(var);
	return 0;
}

void driver_curtis::analysis_2(var__curtis_t* curtis)
{
    if (temp_recv_data[7] & 0x01){

        curtis->i.actual_velocity_ = (temp_recv_data[1] * 256 + temp_recv_data[0]);
    }
    if (temp_recv_data[7] & 0x02){

        curtis->i.actual_velocity_ = ((-1)*(temp_recv_data[1] * 256 + temp_recv_data[0]));
    }
    curtis->i.error_code_ = (int)temp_recv_data[2];
    curtis->i.actual_angle_ = (temp_recv_data[5] * 256 + temp_recv_data[4]);
    curtis->i.voltage_ = (int)temp_recv_data[6];
    curtis->i.control_feedback_ = (int)temp_recv_data[7];
}

void driver_curtis::package_2(var__curtis_t* curtis, CanFrame &send_frame)
{
    send_frame.data[0] = 0;
    send_frame.data[1] |= 0x01;

    if (0.1 > abs(curtis->command_velocity_))
    {
        send_frame.data[0] = 1;
    }
    else
    {
        send_frame.data[2] = (unsigned char)((int)(curtis->command_velocity_) & 0xFF);
        send_frame.data[3] = (unsigned char)(((int)(curtis->command_velocity_) >> 8) & 0xff);
    }

    if (0 > curtis->command_lift_speed1_)
    {
        send_frame.data[0] &= 0xEF;
        send_frame.data[0] |= 0x20;
		send_frame.data[4] = (unsigned char)((int)(curtis->command_lift_speed1_) & 0xFF);
		send_frame.data[5] = (unsigned char)(((int)(curtis->command_lift_speed1_) >> 8) & 0xff);
    }
    else if (0 < curtis->command_lift_speed1_)
    {
        send_frame.data[0] &= 0xDF;
        send_frame.data[0] |= 0x10;
		send_frame.data[4] = (unsigned char)((int)(curtis->command_lift_speed1_) & 0xFF);
		send_frame.data[5] = (unsigned char)(((int)(curtis->command_lift_speed1_) >> 8) & 0xff);
    }
    else
    {
        send_frame.data[0] &= 0xCF;
    }

    if (0 > curtis->command_lift_speed2_)
    {
        send_frame.data[0] &= 0xBF;
        send_frame.data[0] |= 0x80;
		send_frame.data[6] = (unsigned char)((int)(curtis->command_lift_speed2_) & 0xFF);
		send_frame.data[7] = (unsigned char)(((int)(curtis->command_lift_speed2_) >> 8) & 0xff);
    }
    else if (0 < curtis->command_lift_speed2_)
    {
        send_frame.data[0] &= 0x7F;
        send_frame.data[0] |= 0x40;
		send_frame.data[6] = (unsigned char)((int)(curtis->command_lift_speed2_) & 0xFF);
		send_frame.data[7] = (unsigned char)(((int)(curtis->command_lift_speed2_) >> 8) & 0xff);
    }
    else
    {
        send_frame.data[0] &= 0x3F;
    }


    {   //角度控制
        CanFrame send_frame;
        send_frame.id = 0x2e0;//???
        send_frame.type = (CML::CAN_FRAME_TYPE) CAN_FRAME_DATA;
        send_frame.length = 8;
        memset(send_frame.data, 0, sizeof(send_frame.data));
        send_frame.data[0] = ((short)curtis->command_angle_) & 0xFF;
        send_frame.data[1] = ((short)(curtis->command_angle_ >> 8)) & 0xFF;

        const Error *err = 0;
        err = _canOpen->Xmit(send_frame, 1000);//发送 ******************88
        if (err)
        {
            curtis->i.error_code_ = err->GetID();
            nsperror << " curtis " << "Xmit angle "<<curtis->command_angle_<<" error:  " << err->toString();
            return;
        }
    }
}

