#include "driver_dio.h"
#include "dio.h"
#include "var.h"
#include "log.h"

driver_dio::driver_dio()
:driver_base(kVarType_DIO)
{
}


driver_dio::~driver_dio()
{
}

int driver_dio::add_node(CML::CanOpen *canOpen, const var__functional_object_t *d)
{
	var__dio_t* dio = var__object_body_ptr(var__dio_t, d);

	node_id = dio->candev_head_.cannode_;
	const Error *err;
	NodeSettings node_settings;
	node_settings.synchUseFirstNode = true;
	node_settings.synchPeriod = 10000;
	node_settings.synchUseFirstNode = true;
	node_settings.heartbeatPeriod = 200;
	node_settings.guardTime = 200;
	node_settings.pdoCnt = 0;
	err = m_node.Init(*canOpen, node_id, node_settings);

	if (err){
		nsperror << "dio " << node_id << " init dio! " << err->toString();
		return -1;
	}
	else
		nspinfo << "dio  " << node_id << " init success!";
    m_node.sdo.SetTimeout(20);

	int8 value = 0;
	int do_value = 0;
	for (int i = 0; i < dio->do_channel_num_; i++)
	{
		err = m_node.sdo.Upld8(0x6200, i + 1, value);
		if (err)
		{
			dio->i.error_code_ = err->GetID();
			nsperror << " dio " << node_id << " read do[" << i << "] error = " << err->toString();
			return -1;
		}
		do_value += (value << (i * 8));
	}
	dio->i.do2_ = do_value;

	dio->do_ = dio->i.do2_;
	return 0;
}

int driver_dio::read(var__functional_object_t* var)
{
	var__dio_t* dio = var__object_body_ptr(var__dio_t, var);
	dio->i.time_stamp_ = driver_base::get_timestamp() - dio->candev_head_.latency_;
	int8 value;
	const Error *err = 0;
	int di_value = 0;
	for (int n_di = 0; n_di < dio->di_channel_num_;n_di++)
	{
		err = m_node.sdo.Upld8(0x6000, n_di+1, value);
		if (err)
		{
			dio->i.error_code_ = err->GetID();
			nsperror << " dio " << node_id << " read di[" << n_di << "]  error = " << err->toString();
			continue;
		}
		di_value += value<<(n_di*8);
	}
	dio->i.di_ = di_value;


	int do_value = 0;
	for (int n_do = 0; n_do < dio->do_channel_num_; n_do++)
	{
		err = m_node.sdo.Upld8(0x6200, n_do + 1, value);
		if (err)
		{
			dio->i.error_code_ = err->GetID();
			nsperror << " dio " << node_id << " read do[" << n_do << "] error = " << err->toString();
			continue;
		}
		do_value += (value << (n_do * 8));
	}
	dio->i.do2_ = do_value;


    for (int i = 0; i < MAXIMUM_DIO_BLOCK_COUNT; ++i)//read ai
	{
        if (dio->i.ai_[i].start_address_ <= 0)
        {
            continue;
        }
        if (dio->i.ai_[i].effective_count_of_index_ == 0)
        {
            switch (dio->i.ai_[i].internel_type_)
            {
            case 8:
            {
                      int8 ai = 0;
                      err = m_node.sdo.Upld8(dio->i.ai_[i].start_address_, 0, ai);
                      if (err)
                      {
                          dio->i.error_code_ = err->GetID();
                          nsperror << " dio " << node_id << " read addr=" << dio->i.ai_[i].start_address_ << " [" << i << "] type=" << dio->i.ai_[i].internel_type_ << " error = " << err->toString();
                          continue;
                      }
                      dio->i.ai_[i].data_[0] = ai;
            }
                break;
            case 16:
            {
                       int16 ai = 0;
                       err = m_node.sdo.Upld16(dio->i.ai_[i].start_address_, 0, ai);
                       if (err)
                       {
                           dio->i.error_code_ = err->GetID();
                           nsperror << " dio " << node_id << " read addr=" << dio->i.ai_[i].start_address_ << " [" << i << "] type=" << dio->i.ai_[i].internel_type_ << " error = " << err->toString();
                           continue;
                       }
                       dio->i.ai_[i].data_[0] = ai;
            }
                break;
            case 32:
            {
                       int32 ai = 0;
                       err = m_node.sdo.Upld32(dio->i.ai_[i].start_address_, 0, ai);
                       if (err)
                       {
                           dio->i.error_code_ = err->GetID();
                           nsperror << " dio " << node_id << " read addr=" << dio->i.ai_[i].start_address_ << " [" << i << "] type=" << dio->i.ai_[i].internel_type_ << " error = " << err->toString();
                           continue;
                       }
                       dio->i.ai_[i].data_[0] = ai;
            }
                break;
            default:
                break;
            }
        }
        else
        {
            for (int j = 0; j < dio->i.ai_[i].effective_count_of_index_; ++j)
            {
                switch (dio->i.ai_[i].internel_type_)
                {
                case 8:
                {
                          int8 ai = 0;
                          err = m_node.sdo.Upld8(dio->i.ai_[i].start_address_, j + 1, ai);
                          if (err)
                          {
                              dio->i.error_code_ = err->GetID();
                              nsperror << " dio " << node_id << " read addr=" << dio->i.ai_[i].start_address_ << " [" << i << "] type=" << dio->i.ai_[i].internel_type_ << " error = " << err->toString();
                              continue;
                          }
                          dio->i.ai_[i].data_[j] = ai;
                }
                    break;
                case 16:
                {
                           int16 ai = 0;
                           err = m_node.sdo.Upld16(dio->i.ai_[i].start_address_, j + 1, ai);
                           if (err)
                           {
                               dio->i.error_code_ = err->GetID();
                               nsperror << " dio " << node_id << " read addr=" << dio->i.ai_[i].start_address_ << " [" << j << "] type=" << dio->i.ai_[i].internel_type_ << " error = " << err->toString();
                               continue;
                           }
                           dio->i.ai_[i].data_[j] = ai;
                }
                    break;
                case 32:
                {
                           int32 ai = 0;
                           err = m_node.sdo.Upld32(dio->i.ai_[i].start_address_, j + 1, ai);
                           if (err)
                           {
                               dio->i.error_code_ = err->GetID();
                               nsperror << " dio " << node_id << " read addr=" << dio->i.ai_[i].start_address_ << " [" << j << "] type=" << dio->i.ai_[i].internel_type_ << " error = " << err->toString();
                               continue;
                           }
                           dio->i.ai_[i].data_[j] = ai;
                }
                    break;
                default:
                    break;
                }
            }
        }

	}
	
    for (int i = 0; i < MAXIMUM_DIO_BLOCK_COUNT; ++i)//read ai
    {
        if (dio->i.ao2_[i].start_address_ <= 0)
        {
            continue;
        }
        if (dio->i.ao2_[i].effective_count_of_index_ == 0)
        {
            switch (dio->i.ao2_[i].internel_type_)
            {
            case 8:
            {
                      int8 ai = 0;
                      err = m_node.sdo.Upld8(dio->i.ao2_[i].start_address_, 0, ai);
                      if (err)
                      {
                          dio->i.error_code_ = err->GetID();
                          nsperror << " dio " << node_id << " read addr=" << dio->i.ao2_[i].start_address_ << " [" << i << "] type=" << dio->i.ao2_[i].internel_type_ << " error = " << err->toString();
                          continue;
                      }
                      dio->i.ao2_[i].data_[0] = ai;
            }
                break;
            case 16:
            {
                       int16 ai = 0;
                       err = m_node.sdo.Upld16(dio->i.ao2_[i].start_address_, 0, ai);
                       if (err)
                       {
                           dio->i.error_code_ = err->GetID();
                           nsperror << " dio " << node_id << " read addr=" << dio->i.ao2_[i].start_address_ << " [" << i << "] type=" << dio->i.ao2_[i].internel_type_ << " error = " << err->toString();
                           continue;
                       }
                       dio->i.ao2_[i].data_[0] = ai;
            }
                break;
            case 32:
            {
                       int32 ai = 0;
                       err = m_node.sdo.Upld32(dio->i.ao2_[i].start_address_, 0, ai);
                       if (err)
                       {
                           dio->i.error_code_ = err->GetID();
                           nsperror << " dio " << node_id << " read addr=" << dio->i.ao2_[i].start_address_ << " [" << i << "] type=" << dio->i.ao2_[i].internel_type_ << " error = " << err->toString();
                           continue;
                       }
                       dio->i.ao2_[i].data_[0] = ai;
            }
                break;
            default:
                break;
            }
        }
        else
        {
            for (int j = 0; j < dio->i.ao2_[i].effective_count_of_index_; ++j)
            {
                switch (dio->i.ao2_[i].internel_type_)
                {
                case 8:
                {
                          int8 ai = 0;
                          err = m_node.sdo.Upld8(dio->i.ao2_[i].start_address_, j + 1, ai);
                          if (err)
                          {
                              dio->i.error_code_ = err->GetID();
                              nsperror << " dio " << node_id << " read addr=" << dio->i.ao2_[i].start_address_ << " [" << i << "] type=" << dio->i.ao2_[i].internel_type_ << " error = " << err->toString();
                              continue;
                          }
                          dio->i.ao2_[i].data_[j] = ai;
                }
                    break;
                case 16:
                {
                           int16 ai = 0;
                           err = m_node.sdo.Upld16(dio->i.ao2_[i].start_address_, j + 1, ai);
                           if (err)
                           {
                               dio->i.error_code_ = err->GetID();
                               nsperror << " dio " << node_id << " read addr=" << dio->i.ao2_[i].start_address_ << " [" << i << "] type=" << dio->i.ao2_[i].internel_type_ << " error = " << err->toString();
                               continue;
                           }
                           dio->i.ao2_[i].data_[j] = ai;
                }
                    break;
                case 32:
                {
                           int32 ai = 0;
                           err = m_node.sdo.Upld32(dio->i.ao2_[i].start_address_, j + 1, ai);
                           if (err)
                           {
                               dio->i.error_code_ = err->GetID();
                               nsperror << " dio " << node_id << " read addr=" << dio->i.ao2_[i].start_address_ << " [" << i << "] type=" << dio->i.ao2_[i].internel_type_ << " error = " << err->toString();
                               continue;
                           }
                           dio->i.ao2_[i].data_[j] = ai;
                }
                    break;
                default:
                    break;
                }
            }
        }

 
    }
	return 0;
}

int driver_dio::write(var__functional_object_t* var)
{
	var__dio_t* dio = var__object_body_ptr(var__dio_t, var);
	const Error *err = 0;

	for (int n_do = 0; n_do < dio->do_channel_num_;n_do++)
	{
		int8 do_value = (dio->do_ >> (n_do*8)) & 0xFF;
		err = m_node.sdo.Dnld8(0x6200, n_do + 1, do_value);
		if (err)
		{
			dio->i.error_code_ = err->GetID();
			nsperror << " dio " << node_id << " write do dio->do_[" << n_do << "]:" << dio->do_ << " error:  " << err->toString();
			return -1;
		}
	}

    for (int i = 0; i < MAXIMUM_DIO_BLOCK_COUNT; ++i)//write ao
    {
        if (dio->ao_[i].start_address_ <= 0)
        {
            continue;
        }
        if (dio->ao_[i].effective_count_of_index_ == 0)
        {
            switch (dio->ao_[i].internel_type_)
            {
            case 8:
            {
                      int8 ai = dio->ao_[i].data_[0];
                      err = m_node.sdo.Dnld8(dio->ao_[i].start_address_, 0, ai);
                      if (err)
                      {
                          dio->i.error_code_ = err->GetID();
                          nsperror << " dio " << node_id << " write addr=" << dio->ao_[i].start_address_ << " [" << i << "] type=" << dio->ao_[i].internel_type_ << " value=" << ai << " error = " << err->toString();
                          continue;
                      }
            }
                break;
            case 16:
            {
                       int16 ai = dio->ao_[i].data_[0];
                       err = m_node.sdo.Dnld16(dio->ao_[i].start_address_, 0, ai);
                       if (err)
                       {
                           dio->i.error_code_ = err->GetID();
                           nsperror << " dio " << node_id << " write addr=" << dio->ao_[i].start_address_ << " [" << i << "] type=" << dio->ao_[i].internel_type_ << " value=" << ai << " error = " << err->toString();
                           continue;
                       }
            }
                break;
            case 32:
            {
                       int32 ai = dio->ao_[i].data_[0];
                       err = m_node.sdo.Dnld32(dio->ao_[i].start_address_, 0, ai);
                       if (err)
                       {
                           dio->i.error_code_ = err->GetID();
                           nsperror << " dio " << node_id << " write addr=" << dio->ao_[i].start_address_ << " [" << i << "] type=" << dio->ao_[i].internel_type_ << " value=" << ai << " error = " << err->toString();
                           continue;
                       }
            }
                break;
            default:
                break;
            }
        }
        else
        {
            for (int j = 0; j < dio->ao_[i].effective_count_of_index_; ++j)
            {
                switch (dio->ao_[i].internel_type_)
                {
                case 8:
                {
                          int8 ai = dio->ao_[i].data_[j];
                          err = m_node.sdo.Dnld8(dio->ao_[i].start_address_, j + 1, ai);
                          if (err)
                          {
                              dio->i.error_code_ = err->GetID();
                              nsperror << " dio " << node_id << " write addr=" << dio->ao_[i].start_address_ << " [" << i << "] type=" << dio->ao_[i].internel_type_ << " value=" << ai << " error = " << err->toString();
                              continue;
                          }
                }
                    break;
                case 16:
                {
                           int16 ai = dio->ao_[i].data_[j];
                           err = m_node.sdo.Dnld16(dio->ao_[i].start_address_, j + 1, ai);
                           if (err)
                           {
                               dio->i.error_code_ = err->GetID();
                               nsperror << " dio " << node_id << " write addr=" << dio->ao_[i].start_address_ << " [" << i << "] type=" << dio->ao_[i].internel_type_ << " value=" << ai << " error = " << err->toString();
                               continue;
                           }
                }
                    break;
                case 32:
                {
                           int32 ai = dio->ao_[i].data_[j];
                           err = m_node.sdo.Dnld32(dio->ao_[i].start_address_, j + 1, ai);
                           if (err)
                           {
                               dio->i.error_code_ = err->GetID();
                               nsperror << " dio " << node_id << " write addr=" << dio->ao_[i].start_address_ << " [" << i << "] type=" << dio->ao_[i].internel_type_ << " value=" << ai << " error = " << err->toString();
                               continue;
                           }
                }
                    break;
                default:
                    break;
                }
            }
        }

    }
	return 0;
}

int driver_dio::read_and_write(var__functional_object_t* var)
{
	read(var);
	write(var);
	return 0;
}
