#include "view_net.h"
#include "net_task.h"
#include "angle_encoder.h"
#include "dio.h"
#include "elmo.h"
#include "moons.h"
#include "navigation.h"
#include "operation.h"
#include "optpar.h"
#include "safety.h"
#include "usrdef.h"
#include "vehicle.h"
#include "wheel.h"
#include "common_impls.hpp"
#include "proto_typedef.h"
#include <cassert>
#include "mntypes.h"

extern agv_api agv_interface_single_;

template <typename T>
void CopyDbgVarEntire(char *ack_data, std::function<void(void *)> &data_cb)
{
	std::shared_ptr<T> t = std::make_shared<T>();
	memcpy(t.get(), ack_data, sizeof(T));

	if (nullptr != data_cb)
	{
		data_cb(t.get());
	}
}

template <typename T>
void CopyDbgVarPos(const std::vector<mn::common_title_item> &common_vec, char *ack_data, std::function<void(void *)> &data_cb)
{
	char *dst = nullptr;
	char *src = nullptr;
	int len = 0;
	int pos = 0;
	std::shared_ptr<T> t = std::make_shared<T>();
	memset(t.get(), 0, sizeof(T));

	for (auto common : common_vec)
	{
		dst = (char *)t.get() + common.offset_;
		src = ack_data + pos;
		len = common.length_;
		memcpy(dst, src, len);
		pos += len;
	}

	if (nullptr != data_cb)
	{
		data_cb(t.get());
	}
}

bool net_task::disconnect_to_host(uint32_t robot_id)
{
	if (agv_interface_single_.logout() < 0)
	{
		nsperror << __FUNCTION__ << "Call disconnect_host func failed!";
		return false;
	}
	return true;
}

bool net_task::connect_to_host(const QString &ip, int port, uint32_t &robot_id, std::function<void(uint32_t, void *, int)> to_notify)
{
	agv_interface_single_.set_agv_id(1);
	if (agv_interface_single_.login(qPrintable(ip), port) < 0)
	{
		nsperror << "failed to connect to target endpoint:" << qPrintable(ip);
		return false;
	}
	return true;
}

bool net_task::post_dbg_varls_request_(uint32_t robot_id, std::vector<mn::var_item> &items)
{
	bool success = false;
	agv_interface_single_.get_agv_obj_list_syn(items) >= 0 ? success = true : success = false;
	return success;
}

bool net_task::post_common_read_dbg_var(uint32_t robot_id, const mn::common_title &common_vec, var__types type, bool entire, std::function<void(void *)> &data_cb)
{
	if (common_vec.items.size() == 0) return false;
	int id = common_vec.items[0].varid;
	int result = 0;
	switch (type)
	{
		case kVarType_MotionTemplateFramwork:
		{

		}
		break;
		case kVarType_Vehicle:
		{
			var__vehicle_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__vehicle_t>(id, var_data);
			CopyDbgVarEntire<var__vehicle_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_Navigation:
		{
			var__navigation_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__navigation_t>(id, var_data);
			CopyDbgVarEntire<var__navigation_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_Operation:
		{
			var__operation_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__operation_t>(id, var_data);
			CopyDbgVarEntire<var__operation_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_UserDefined:
		{
			var__usrdef_buffer_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__usrdef_buffer_t>(id, var_data);
			CopyDbgVarEntire<var__usrdef_buffer_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_SWheel:
		{
			var__swheel_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__swheel_t>(id, var_data);
			CopyDbgVarEntire<var__swheel_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_DWheel:
		{
			var__dwheel_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__dwheel_t>(id, var_data);
			CopyDbgVarEntire<var__dwheel_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_SDDExtra:
		{
			var__sdd_extra_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__sdd_extra_t>(id, var_data);
			CopyDbgVarEntire<var__sdd_extra_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_DriveUnit:
		{

		}
		break;
		case kVarType_Map:
		{

		}
		break;
		case kVarType_OperationTarget:
		{
			var__operation_parameter_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__operation_parameter_t>(id, var_data);
			CopyDbgVarEntire<var__operation_parameter_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_CanBus:
		{
			//CopyDbgVarEntire<var__canbus_t>(ack_data, data_cb);
		}
		break;
		case kVarType_Copley:
		{

		}
		break;
		case kVarType_Elmo:
		{
			var__elmo_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__elmo_t>(id, var_data);
			CopyDbgVarEntire<var__elmo_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_DIO:
		{
			var__dio_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__dio_t>(id, var_data);
			CopyDbgVarEntire<var__dio_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_Moons:
		{
			var__moos_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__moos_t>(id, var_data);
			CopyDbgVarEntire<var__moos_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_AngleEncoder:
		{
			var__angle_encoder_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__angle_encoder_t>(id, var_data);
			CopyDbgVarEntire<var__angle_encoder_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_Curtis:
		{

		}
		break;
		case kVarType_VoiceDevice:
		{

		}
		break;
		case kVarType_OmronPLC:
		{

		}
		break;
		case kVarType_ModBus_TCP:
		{

		}
		break;
		case kVarType_ErrorHandler:
		{

		}
		break;
		case kVarType_SafetyProtec:
		{
			var__safety_t var_data;
			result = agv_interface_single_.get_var_info_by_id_syn<var__safety_t>(id, var_data);
			CopyDbgVarEntire<var__safety_t>((char*)&var_data, data_cb);
		}
		break;
		case kVarType_MaximumFunction:
		{

		}
		break;
		default:
		{
			assert(false);
		}
		break;
	}
	

	//int ret = post_common_read_request_by_id(robot_id, common_vec, std::make_shared<motion::asio_partnet>([&](uint32_t robot_id_, const void *data){
	//	waitable_sig ws{ &wh };

	//	if (robot_id != robot_id_)
	//	{
	//		nsperror << __FUNCTION__ << "Robot id is not matched.[robot_id:" << robot_id << ",robot_id_:" << robot_id_ << "].";

	//		return;
	//	}

	//	if (nullptr == data)
	//	{
	//		nsperror << __FUNCTION__ << "The data is null.";

	//		return;
	//	}

	//	motion::asio_data *asio_data = (motion::asio_data *)data;
	//	int asio_err = asio_data->get_err();
	//	nspinfo << __FUNCTION__ << "The asio err is:" << asio_err;

	//	if (asio_err < 0)
	//	{
	//		nsperror << __FUNCTION__ << "The asio data get err is less than 0.";

	//		return;
	//	}

	//	common_ack ack = *(common_ack*)(data);
	//	char *ack_data = (char *)(ack.data.c_str());

	//	if (entire)
	//	{
	//		switch (type)
	//		{
	//			case kVarType_MotionTemplateFramwork:
	//			{

	//			}
	//				break;
	//			case kVarType_Vehicle:
	//			{
	//				CopyDbgVarEntire<var__vehicle_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Navigation:
	//			{
	//				CopyDbgVarEntire<var__navigation_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Operation:
	//			{
	//				CopyDbgVarEntire<var__operation_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_UserDefined:
	//			{
	//				CopyDbgVarEntire<var__usrdef_buffer_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_SWheel:
	//			{
	//				CopyDbgVarEntire<var__swheel_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_DWheel:
	//			{
	//				CopyDbgVarEntire<var__dwheel_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_SDDExtra:
	//			{
	//				CopyDbgVarEntire<var__sdd_extra_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_DriveUnit:
	//			{

	//			}
	//				break;
	//			case kVarType_Map:
	//			{

	//			}
	//				break;
	//			case kVarType_OperationTarget:
	//			{
	//				CopyDbgVarEntire<var__operation_parameter_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_CanBus:
	//			{
	//				//CopyDbgVarEntire<var__canbus_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Copley:
	//			{

	//			}
	//				break;
	//			case kVarType_Elmo:
	//			{
	//				CopyDbgVarEntire<var__elmo_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_DIO:
	//			{
	//				CopyDbgVarEntire<var__dio_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Moons:
	//			{
	//				CopyDbgVarEntire<var__moos_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_AngleEncoder:
	//			{
	//				CopyDbgVarEntire<var__angle_encoder_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Curtis:
	//			{

	//			}
	//				break;
	//			case kVarType_VoiceDevice:
	//			{

	//			}
	//				break;
	//			case kVarType_OmronPLC:
	//			{

	//			}
	//				break;
	//			case kVarType_ModBus_TCP:
	//			{

	//			}
	//				break;
	//			case kVarType_ErrorHandler:
	//			{

	//			}
	//				break;
	//			case kVarType_SafetyProtec:
	//			{
	//				CopyDbgVarEntire<var__safety_t>(ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_MaximumFunction:
	//			{

	//			}
	//				break;
	//			default:
	//			{
	//				assert(false);
	//			}
	//				break;
	//		}
	//	}
	//	else
	//	{
	//		char *dst = nullptr;
	//		char *src = nullptr;
	//		int len = 0;
	//		int pos = 0;

	//		switch (type)
	//		{
	//			case kVarType_MotionTemplateFramwork:
	//			{

	//			}
	//				break;
	//			case kVarType_Vehicle:
	//			{
	//				CopyDbgVarPos<var__vehicle_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Navigation:
	//			{
	//				CopyDbgVarPos<var__navigation_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Operation:
	//			{
	//				CopyDbgVarPos<var__operation_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_UserDefined:
	//			{
	//				CopyDbgVarPos<var__usrdef_buffer_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_SWheel:
	//			{
	//				CopyDbgVarPos<var__swheel_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_DWheel:
	//			{
	//				CopyDbgVarPos<var__dwheel_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_SDDExtra:
	//			{
	//				CopyDbgVarPos<var__sdd_extra_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_DriveUnit:
	//			{

	//			}
	//				break;
	//			case kVarType_Map:
	//			{

	//			}
	//				break;
	//			case kVarType_OperationTarget:
	//			{
	//				CopyDbgVarPos<var__operation_parameter_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_CanBus:
	//			{
	//				//CopyDbgVarPos<var__canbus_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Copley:
	//			{

	//			}
	//				break;
	//			case kVarType_Elmo:
	//			{
	//				CopyDbgVarPos<var__elmo_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_DIO:
	//			{
	//				CopyDbgVarPos<var__dio_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Moons:
	//			{
	//				CopyDbgVarPos<var__moos_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_AngleEncoder:
	//			{
	//				CopyDbgVarPos<var__angle_encoder_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_Curtis:
	//			{

	//			}
	//				break;
	//			case kVarType_VoiceDevice:
	//			{

	//			}
	//				break;
	//			case kVarType_OmronPLC:
	//			{

	//			}
	//				break;
	//			case kVarType_ModBus_TCP:
	//			{

	//			}
	//				break;
	//			case kVarType_ErrorHandler:
	//			{

	//			}
	//				break;
	//			case kVarType_SafetyProtec:
	//			{
	//				CopyDbgVarPos<var__safety_t>(common_vec, ack_data, data_cb);
	//			}
	//				break;
	//			case kVarType_MaximumFunction:
	//			{

	//			}
	//				break;
	//			default:
	//			{
	//				assert(false);
	//			}
	//				break;
	//		}
	//	}

	//	success = true;
	//}));

	//nspinfo << __FUNCTION__ << "post_common_read_dbg_var ret is:" << ret;

	//if (RET_FAILED == ret)
	//{
	//	nsperror << __FUNCTION__ << "Call post_common_read_dbg_var func failed.";

	//	return false;
	//}

	//waitable_wait ww{ &wh };

	//if (!ww.wait())
	//{
	//	return false;
	//}

	//if (!success)
	//{
	//	nsperror << __FUNCTION__ << "Post common read dbg var failed!";
	//}
	if (result < 0)return false;
	return true;
}
