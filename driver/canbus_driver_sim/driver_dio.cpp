

#include "dio.h"
#include "var.h"
#include "log.h"
#include "driver_dio.h"
static int flag_Count_Up = 0;
static int flag_Count_Down = 0;

driver_dio::driver_dio()
{
}


driver_dio::~driver_dio()
{
}


int driver_dio::read(var__functional_object_t* var)
{
	var__dio_t* dio = var__object_body_ptr(var__dio_t, var);
	dio->i.time_stamp_ = driver_base::get_timestamp() - dio->candev_head_.latency_;
	//上升
	if (flag_Count_Up >= 150)
	{
		dio->i.di_ &= 0xDD;
		flag_Count_Up = 150;

		//loinfo << "flag_Count_Up :" << flag_Count_Up << " dio->i.di_:" << dio->i.di_;
	}
	//下降
	if (flag_Count_Down >= 150)
	{
		dio->i.di_ |= 0x22;
		flag_Count_Down = 150;

	//	loinfo << "flag_Count_Down :" << flag_Count_Down << " dio->i.di_:" << dio->i.di_;
			
	}

	/*var__dio_t* dio = var__object_body_ptr(var__dio_t, var);
	dio->i.time_stamp_ = driver_base::get_timestamp() - dio->candev_head_.latency_;
	int8 value;
	const Error *err = 0;
	int di_value = 0;
	for (int n_di = 0; n_di < dio->di_channel_num_;n_di++)
	{
		err = m_node.sdo.Upld8(0x6000, 0x01, value);
		if (err)
		{
			dio->i.error_code_ = err->GetID();
			loerr << " dio " << node_id << " read di[" << n_di << "]  error = " << err->toString();
			continue;
		}
		di_value += value<<(n_di*8);
	}
	dio->i.di_ = di_value;


	int do_value = 0;
	for (int n_do = 0; n_do < dio->do_channel_num_; n_do++)
	{
		err = m_node.sdo.Upld8(0x6200, n_do , value);
		if (err)
		{
			dio->i.error_code_ = err->GetID();
			loerr << " dio " << node_id << " read do[" << n_do << "] error = " << err->toString();
			continue;
		}
		do_value += (value << (n_do * 8));
	}
	dio->i.do2_ = do_value;


	for (int i = 0; i < dio->ai_channel_num_; ++i)//read ai
	{
		int16 ai = 0;
		err = m_node.sdo.Upld16(0x2170, i , ai);
		if (err)
		{
			dio->i.error_code_ = err->GetID();
			loerr << " dio " << node_id << " read ai[" << i << "] error = " << err->toString();
			continue;
		}
		dio->i.ai_[i] = ai;
	}*/
	
	return 0;
}


int driver_dio::write(var__functional_object_t* var)
{
	var__dio_t* dio = var__object_body_ptr(var__dio_t, var);
	
	//上升
	if ((dio->do_ & 0x01) == 0x01)
	{
		dio->i.do2_ = dio->do_;
		flag_Count_Up += 1;
		flag_Count_Down = 0;
	}

	//下降
	if ((dio->do_ & 0x04) == 0x04)
	{
		dio->i.do2_= dio->do_ ;
		flag_Count_Down += 1;	
		flag_Count_Up = 0;
	}

		return 0;
}

int driver_dio::read_and_write(var__functional_object_t* var)
{
	read(var);
	write(var);
	return 0;
}