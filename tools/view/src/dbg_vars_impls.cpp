#include "dbg_vars_impls.h"
#include "operation.h"
#include "usrdef.h"
#include "optpar.h"
#include "safety.h"
#include "data_type.h"
#include "common_impls.hpp"

QString &dbg_vars_impls::var_type_to_string(var__types vt)
{
	switch (vt)
	{
		case kVarType_MotionTemplateFramwork:
		{
			m_vtString = QObject::tr("MotionTemplateFramwork");
		}
			break;
		case kVarType_Vehicle:
		{
			m_vtString = QObject::tr("Vehicle");
		}
			break;
		case kVarType_Navigation:
		{
			m_vtString = QObject::tr("Navigation");
		}
			break;
		case kVarType_Operation:
		{
			m_vtString = QObject::tr("Operation");
		}
			break;
		case kVarType_UserDefined:
		{
			m_vtString = QObject::tr("UserDefined");
		}
			break;
		case kVarType_SWheel:
		{
			m_vtString = QObject::tr("SWheel");
		}
			break;
		case kVarType_DWheel:
		{
			m_vtString = QObject::tr("DWheel");
		}
			break;
		case kVarType_SDDExtra:
		{
			m_vtString = QObject::tr("SDDExtra");
		}
			break;
		case kVarType_DriveUnit:
		{
			m_vtString = QObject::tr("DriveUnit");
		}
			break;
		case kVarType_Map:
		{
			m_vtString = QObject::tr("Map");
		}
			break;
		case kVarType_OperationTarget:
		{
			m_vtString = QObject::tr("OperationTarget");
		}
			break;
		case kVarType_CanBus:
		{
			m_vtString = QObject::tr("CanBus");
		}
			break;
		case kVarType_Copley:
		{
			m_vtString = QObject::tr("Copley");
		}
			break;
		case kVarType_Elmo:
		{
			m_vtString = QObject::tr("Elmo");
		}
			break;
		case kVarType_DIO:
		{
			m_vtString = QObject::tr("DIO");
		}
			break;
		case kVarType_Moons:
		{
			m_vtString = QObject::tr("Moons");
		}
			break;
		case kVarType_AngleEncoder:
		{
			m_vtString = QObject::tr("AngleEncoder");
		}
			break;
		case kVarType_Curtis:
		{
			m_vtString = QObject::tr("Curtis");
		}
			break;
		case kVarType_VoiceDevice:
		{
			m_vtString = QObject::tr("VoiceDevice");
		}
			break;
		case kVarType_OmronPLC:
		{
			m_vtString = QObject::tr("OmronPLC");
		}
			break;
		case kVarType_ModBus_TCP:
		{
			m_vtString = QObject::tr("ModBus_TCP");
		}
			break;
		case kVarType_ErrorHandler:
		{
			m_vtString = QObject::tr("ErrorHandler");
		}
			break;
		case kVarType_SafetyProtec:
		{
			m_vtString = QObject::tr("SafetyProtec");
		}
			break;
		case kVarType_MaximumFunction:
		{
			m_vtString = QObject::tr("MaximumFunction");
		}
			break;
		default:
		{
			Q_ASSERT(false);
		}
			break;
	}

	return m_vtString;
}

QMap<QString, mn::common_title_item> &dbg_vars_impls::properties_from_type(int id, var__types vt)
{
	qMapFree<QString, mn::common_title_item>(m_propertiesMap);
	mn::common_title_item ct{ id, 0 };

	switch (vt)
	{
		case kVarType_MotionTemplateFramwork:
		{

		}
			break;
		case kVarType_Vehicle:
		{
			ct.offset = offsetof_data(_var__vehicle_t, vehicle_id_);
			ct.length = msizeof(_var__vehicle_t, vehicle_id_);
			m_propertiesMap.insert("vehicle_id_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, vehicle_name_);
			ct.length = msizeof(_var__vehicle_t, vehicle_name_);
			m_propertiesMap.insert("vehicle_name_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, vehicle_type_);
			ct.length = msizeof(_var__vehicle_t, vehicle_type_);
			m_propertiesMap.insert("vehicle_type_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, chassis_type_);
			ct.length = msizeof(_var__vehicle_t, chassis_type_);
			m_propertiesMap.insert("chassis_type_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, max_speed_);
			ct.length = msizeof(_var__vehicle_t, max_speed_);
			m_propertiesMap.insert("max_speed_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, creep_speed_);
			ct.length = msizeof(_var__vehicle_t, creep_speed_);
			m_propertiesMap.insert("creep_speed_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, max_acc_);
			ct.length = msizeof(_var__vehicle_t, max_acc_);
			m_propertiesMap.insert("max_acc_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, max_dec_);
			ct.length = msizeof(_var__vehicle_t, max_dec_);
			m_propertiesMap.insert("max_dec_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, max_w_);
			ct.length = msizeof(_var__vehicle_t, max_w_);
			m_propertiesMap.insert("max_w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, creep_w_);
			ct.length = msizeof(_var__vehicle_t, creep_w_);
			m_propertiesMap.insert("creep_w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, max_acc_w_);
			ct.length = msizeof(_var__vehicle_t, max_acc_w_);
			m_propertiesMap.insert("max_acc_w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, max_dec_w_);
			ct.length = msizeof(_var__vehicle_t, max_dec_w_);
			m_propertiesMap.insert("max_dec_w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, steer_angle_error_tolerance_);
			ct.length = msizeof(_var__vehicle_t, steer_angle_error_tolerance_);
			m_propertiesMap.insert("steer_angle_error_tolerance_", ct);

			ct.offset = offsetof_data(_var__vehicle_t, manual_velocity_.x_);
			ct.length = msizeof(_var__vehicle_t, manual_velocity_.x_);
			m_propertiesMap.insert("manual_velocity_.x_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, manual_velocity_.y_);
			ct.length = msizeof(_var__vehicle_t, manual_velocity_.y_);
			m_propertiesMap.insert("manual_velocity_.y_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, manual_velocity_.theta_);
			ct.length = msizeof(_var__vehicle_t, manual_velocity_.theta_);
			m_propertiesMap.insert("manual_velocity_.theta_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, manual_velocity_.angle_);
			ct.length = msizeof(_var__vehicle_t, manual_velocity_.angle_);
			m_propertiesMap.insert("manual_velocity_.angle_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, manual_velocity_.w_);
			ct.length = msizeof(_var__vehicle_t, manual_velocity_.w_);
			m_propertiesMap.insert("manual_velocity_.w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, manual_velocity_.distance_);
			ct.length = msizeof(_var__vehicle_t, manual_velocity_.distance_);
			m_propertiesMap.insert("manual_velocity_.distance_", ct);

			ct.offset = offsetof_data(_var__vehicle_t, stop_normal_);
			ct.length = msizeof(_var__vehicle_t, stop_normal_);
			m_propertiesMap.insert("stop_normal_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, stop_emergency_);
			ct.length = msizeof(_var__vehicle_t, stop_emergency_);
			m_propertiesMap.insert("stop_emergency_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, fault_stop_);
			ct.length = msizeof(_var__vehicle_t, fault_stop_);
			m_propertiesMap.insert("fault_stop_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, slow_down_);
			ct.length = msizeof(_var__vehicle_t, slow_down_);
			m_propertiesMap.insert("slow_down_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, enable_);
			ct.length = msizeof(_var__vehicle_t, enable_);
			m_propertiesMap.insert("enable_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, control_mode_);
			ct.length = msizeof(_var__vehicle_t, control_mode_);
			m_propertiesMap.insert("control_mode_", ct);

			ct.offset = offsetof_data(_var__vehicle_t, i.command_velocity_.x_);
			ct.length = msizeof(_var__vehicle_t, i.command_velocity_.x_);
			m_propertiesMap.insert("i.command_velocity_.x_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.command_velocity_.y_);
			ct.length = msizeof(_var__vehicle_t, i.command_velocity_.y_);
			m_propertiesMap.insert("i.command_velocity_.y_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.command_velocity_.theta_);
			ct.length = msizeof(_var__vehicle_t, i.command_velocity_.theta_);
			m_propertiesMap.insert("i.command_velocity_.theta_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.command_velocity_.angle_);
			ct.length = msizeof(_var__vehicle_t, i.command_velocity_.angle_);
			m_propertiesMap.insert("i.command_velocity_.angle_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.command_velocity_.w_);
			ct.length = msizeof(_var__vehicle_t, i.command_velocity_.w_);
			m_propertiesMap.insert("i.command_velocity_.w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.command_velocity_.distance_);
			ct.length = msizeof(_var__vehicle_t, i.command_velocity_.distance_);
			m_propertiesMap.insert("i.command_velocity_.distance_", ct);

			ct.offset = offsetof_data(_var__vehicle_t, i.ref_velocity_.x_);
			ct.length = msizeof(_var__vehicle_t, i.ref_velocity_.x_);
			m_propertiesMap.insert("i.ref_velocity_.x_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.ref_velocity_.y_);
			ct.length = msizeof(_var__vehicle_t, i.ref_velocity_.y_);
			m_propertiesMap.insert("i.ref_velocity_.y_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.ref_velocity_.theta_);
			ct.length = msizeof(_var__vehicle_t, i.ref_velocity_.theta_);
			m_propertiesMap.insert("i.ref_velocity_.theta_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.ref_velocity_.angle_);
			ct.length = msizeof(_var__vehicle_t, i.ref_velocity_.angle_);
			m_propertiesMap.insert("i.ref_velocity_.angle_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.ref_velocity_.w_);
			ct.length = msizeof(_var__vehicle_t, i.ref_velocity_.w_);
			m_propertiesMap.insert("i.ref_velocity_.w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.ref_velocity_.distance_);
			ct.length = msizeof(_var__vehicle_t, i.ref_velocity_.distance_);
			m_propertiesMap.insert("i.ref_velocity_.distance_", ct);

			ct.offset = offsetof_data(_var__vehicle_t, i.actual_command_velocity_.x_);
			ct.length = msizeof(_var__vehicle_t, i.actual_command_velocity_.x_);
			m_propertiesMap.insert("i.actual_command_velocity_.x_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_command_velocity_.y_);
			ct.length = msizeof(_var__vehicle_t, i.actual_command_velocity_.y_);
			m_propertiesMap.insert("i.actual_command_velocity_.y_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_command_velocity_.theta_);
			ct.length = msizeof(_var__vehicle_t, i.actual_command_velocity_.theta_);
			m_propertiesMap.insert("i.actual_command_velocity_.theta_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_command_velocity_.angle_);
			ct.length = msizeof(_var__vehicle_t, i.actual_command_velocity_.angle_);
			m_propertiesMap.insert("i.actual_command_velocity_.angle_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_command_velocity_.w_);
			ct.length = msizeof(_var__vehicle_t, i.actual_command_velocity_.w_);
			m_propertiesMap.insert("i.actual_command_velocity_.w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_command_velocity_.distance_);
			ct.length = msizeof(_var__vehicle_t, i.actual_command_velocity_.distance_);
			m_propertiesMap.insert("i.actual_command_velocity_.distance_", ct);

			ct.offset = offsetof_data(_var__vehicle_t, i.actual_velocity_.x_);
			ct.length = msizeof(_var__vehicle_t, i.actual_velocity_.x_);
			m_propertiesMap.insert("i.actual_velocity_.x_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_velocity_.y_);
			ct.length = msizeof(_var__vehicle_t, i.actual_velocity_.y_);
			m_propertiesMap.insert("i.actual_velocity_.y_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_velocity_.theta_);
			ct.length = msizeof(_var__vehicle_t, i.actual_velocity_.theta_);
			m_propertiesMap.insert("i.actual_velocity_.theta_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_velocity_.angle_);
			ct.length = msizeof(_var__vehicle_t, i.actual_velocity_.angle_);
			m_propertiesMap.insert("i.actual_velocity_.angle_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_velocity_.w_);
			ct.length = msizeof(_var__vehicle_t, i.actual_velocity_.w_);
			m_propertiesMap.insert("i.actual_velocity_.w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.actual_velocity_.distance_);
			ct.length = msizeof(_var__vehicle_t, i.actual_velocity_.distance_);
			m_propertiesMap.insert("i.actual_velocity_.distance_", ct);

			ct.offset = offsetof_data(_var__vehicle_t, i.odo_meter_.x_);
			ct.length = msizeof(_var__vehicle_t, i.odo_meter_.x_);
			m_propertiesMap.insert("i.odo_meter_.x_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.odo_meter_.y_);
			ct.length = msizeof(_var__vehicle_t, i.odo_meter_.y_);
			m_propertiesMap.insert("i.odo_meter_.y_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.odo_meter_.theta_);
			ct.length = msizeof(_var__vehicle_t, i.odo_meter_.theta_);
			m_propertiesMap.insert("i.odo_meter_.theta_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.odo_meter_.angle_);
			ct.length = msizeof(_var__vehicle_t, i.odo_meter_.angle_);
			m_propertiesMap.insert("i.odo_meter_.angle_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.odo_meter_.w_);
			ct.length = msizeof(_var__vehicle_t, i.odo_meter_.w_);
			m_propertiesMap.insert("i.odo_meter_.w_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.odo_meter_.distance_);
			ct.length = msizeof(_var__vehicle_t, i.odo_meter_.distance_);
			m_propertiesMap.insert("i.odo_meter_.distance_", ct);

			ct.offset = offsetof_data(_var__vehicle_t, i.time_stamp_);
			ct.length = msizeof(_var__vehicle_t, i.time_stamp_);
			m_propertiesMap.insert("i.time_stamp_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.is_moving_);
			ct.length = msizeof(_var__vehicle_t, i.is_moving_);
			m_propertiesMap.insert("i.is_moving_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.normal_stopped_);
			ct.length = msizeof(_var__vehicle_t, i.normal_stopped_);
			m_propertiesMap.insert("i.normal_stopped_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.emergency_stopped_);
			ct.length = msizeof(_var__vehicle_t, i.emergency_stopped_);
			m_propertiesMap.insert("i.emergency_stopped_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.slow_done_);
			ct.length = msizeof(_var__vehicle_t, i.slow_done_);
			m_propertiesMap.insert("i.slow_done_", ct);
			ct.offset = offsetof_data(_var__vehicle_t, i.total_odo_meter_);
			ct.length = msizeof(_var__vehicle_t, i.total_odo_meter_);
			m_propertiesMap.insert("i.total_odo_meter_", ct);

			ct.offset = offsetof_data(_var__vehicle_t, u_);
			ct.length = msizeof(_var__vehicle_t, u_);
			m_propertiesMap.insert("u_", ct);
		}
			break;
		case kVarType_Navigation:
		{
			ct.offset = offsetof_data(var__navigation_t, max_speed_);
			ct.length = msizeof(var__navigation_t, max_speed_);
			m_propertiesMap.insert("max_speed_", ct);
			ct.offset = offsetof_data(var__navigation_t, creep_speed_);
			ct.length = msizeof(var__navigation_t, creep_speed_);
			m_propertiesMap.insert("creep_speed_", ct);
			ct.offset = offsetof_data(var__navigation_t, max_w_);
			ct.length = msizeof(var__navigation_t, max_w_);
			m_propertiesMap.insert("max_w_", ct);
			ct.offset = offsetof_data(var__navigation_t, creep_w_);
			ct.length = msizeof(var__navigation_t, creep_w_);
			m_propertiesMap.insert("creep_w_", ct);
			ct.offset = offsetof_data(var__navigation_t, slow_down_speed_);
			ct.length = msizeof(var__navigation_t, slow_down_speed_);
			m_propertiesMap.insert("slow_down_speed_", ct);
			ct.offset = offsetof_data(var__navigation_t, acc_);
			ct.length = msizeof(var__navigation_t, acc_);
			m_propertiesMap.insert("acc_", ct);
			ct.offset = offsetof_data(var__navigation_t, dec_);
			ct.length = msizeof(var__navigation_t, dec_);
			m_propertiesMap.insert("dec_", ct);
			ct.offset = offsetof_data(var__navigation_t, dec_estop_);
			ct.length = msizeof(var__navigation_t, dec_estop_);
			m_propertiesMap.insert("dec_estop_", ct);
			ct.offset = offsetof_data(var__navigation_t, acc_w_);
			ct.length = msizeof(var__navigation_t, acc_w_);
			m_propertiesMap.insert("acc_w_", ct);
			ct.offset = offsetof_data(var__navigation_t, dec_w_);
			ct.length = msizeof(var__navigation_t, dec_w_);
			m_propertiesMap.insert("dec_w_", ct);
			ct.offset = offsetof_data(var__navigation_t, creep_distance_);
			ct.length = msizeof(var__navigation_t, creep_distance_);
			m_propertiesMap.insert("creep_distance_", ct);
			ct.offset = offsetof_data(var__navigation_t, creep_theta_);
			ct.length = msizeof(var__navigation_t, creep_theta_);
			m_propertiesMap.insert("creep_theta_", ct);
			ct.offset = offsetof_data(var__navigation_t, upl_mapping_angle_tolerance_);
			ct.length = msizeof(var__navigation_t, upl_mapping_angle_tolerance_);
			m_propertiesMap.insert("upl_mapping_angle_tolerance_", ct);
			ct.offset = offsetof_data(var__navigation_t, upl_mapping_dist_tolerance_);
			ct.length = msizeof(var__navigation_t, upl_mapping_dist_tolerance_);
			m_propertiesMap.insert("upl_mapping_dist_tolerance_", ct);
			ct.offset = offsetof_data(var__navigation_t, upl_mapping_angle_weight_);
			ct.length = msizeof(var__navigation_t, upl_mapping_angle_weight_);
			m_propertiesMap.insert("upl_mapping_angle_weight_", ct);
			ct.offset = offsetof_data(var__navigation_t, upl_mapping_dist_weight_);
			ct.length = msizeof(var__navigation_t, upl_mapping_dist_weight_);
			m_propertiesMap.insert("upl_mapping_dist_weight_", ct);
			ct.offset = offsetof_data(var__navigation_t, tracking_error_tolerance_dist_);
			ct.length = msizeof(var__navigation_t, tracking_error_tolerance_dist_);
			m_propertiesMap.insert("tracking_error_tolerance_dist_", ct);
			ct.offset = offsetof_data(var__navigation_t, tracking_error_tolerance_angle_);
			ct.length = msizeof(var__navigation_t, tracking_error_tolerance_angle_);
			m_propertiesMap.insert("tracking_error_tolerance_angle_", ct);
			ct.offset = offsetof_data(var__navigation_t, aim_dist_);
			ct.length = msizeof(var__navigation_t, aim_dist_);
			m_propertiesMap.insert("aim_dist_", ct);
			ct.offset = offsetof_data(var__navigation_t, predict_time_);
			ct.length = msizeof(var__navigation_t, predict_time_);
			m_propertiesMap.insert("predict_time_", ct);
			ct.offset = offsetof_data(var__navigation_t, is_traj_whole_);
			ct.length = msizeof(var__navigation_t, is_traj_whole_);
			m_propertiesMap.insert("is_traj_whole_", ct);
			ct.offset = offsetof_data(var__navigation_t, aim_angle_p_);
			ct.length = msizeof(var__navigation_t, aim_angle_p_);
			m_propertiesMap.insert("aim_angle_p_", ct);
			ct.offset = offsetof_data(var__navigation_t, aim_angle_i_);
			ct.length = msizeof(var__navigation_t, aim_angle_i_);
			m_propertiesMap.insert("aim_angle_i_", ct);
			ct.offset = offsetof_data(var__navigation_t, aim_angle_d_);
			ct.length = msizeof(var__navigation_t, aim_angle_d_);
			m_propertiesMap.insert("aim_angle_d_", ct);
			ct.offset = offsetof_data(var__navigation_t, stop_tolerance_);
			ct.length = msizeof(var__navigation_t, stop_tolerance_);
			m_propertiesMap.insert("stop_tolerance_", ct);
			ct.offset = offsetof_data(var__navigation_t, stop_tolerance_angle_);
			ct.length = msizeof(var__navigation_t, stop_tolerance_angle_);
			m_propertiesMap.insert("stop_tolerance_angle_", ct);
			ct.offset = offsetof_data(var__navigation_t, stop_point_trim_);
			ct.length = msizeof(var__navigation_t, stop_point_trim_);
			m_propertiesMap.insert("stop_point_trim_", ct);
			ct.offset = offsetof_data(var__navigation_t, aim_ey_p_);
			ct.length = msizeof(var__navigation_t, aim_ey_p_);
			m_propertiesMap.insert("aim_ey_p_", ct);
			ct.offset = offsetof_data(var__navigation_t, aim_ey_i_);
			ct.length = msizeof(var__navigation_t, aim_ey_i_);
			m_propertiesMap.insert("aim_ey_i_", ct);
			ct.offset = offsetof_data(var__navigation_t, aim_ey_d_);
			ct.length = msizeof(var__navigation_t, aim_ey_d_);
			m_propertiesMap.insert("aim_ey_d_", ct);

			ct.offset = offsetof_data(var__navigation_t, track_status_.command_);
			ct.length = msizeof(var__navigation_t, track_status_.command_);
			m_propertiesMap.insert("track_status_.command_", ct);
			ct.offset = offsetof_data(var__navigation_t, track_status_.middle_);
			ct.length = msizeof(var__navigation_t, track_status_.middle_);
			m_propertiesMap.insert("track_status_.middle_", ct);
			ct.offset = offsetof_data(var__navigation_t, track_status_.response_);
			ct.length = msizeof(var__navigation_t, track_status_.response_);
			m_propertiesMap.insert("track_status_.response_", ct);

			ct.offset = offsetof_data(var__navigation_t, user_task_id_);
			ct.length = msizeof(var__navigation_t, user_task_id_);
			m_propertiesMap.insert("user_task_id_", ct);
			ct.offset = offsetof_data(var__navigation_t, ato_task_id_);
			ct.length = msizeof(var__navigation_t, ato_task_id_);
			m_propertiesMap.insert("ato_task_id_", ct);

			ct.offset = offsetof_data(var__navigation_t, dest_upl_.edge_id_);
			ct.length = msizeof(var__navigation_t, dest_upl_.edge_id_);
			m_propertiesMap.insert("dest_upl_.edge_id_", ct);
			ct.offset = offsetof_data(var__navigation_t, dest_upl_.percentage_);
			ct.length = msizeof(var__navigation_t, dest_upl_.percentage_);
			m_propertiesMap.insert("dest_upl_.percentage_", ct);
			ct.offset = offsetof_data(var__navigation_t, dest_upl_.wop_id_);
			ct.length = msizeof(var__navigation_t, dest_upl_.wop_id_);
			m_propertiesMap.insert("dest_upl_.wop_id_", ct);
			ct.offset = offsetof_data(var__navigation_t, dest_upl_.angle_);
			ct.length = msizeof(var__navigation_t, dest_upl_.angle_);
			m_propertiesMap.insert("dest_upl_.angle_", ct);

			ct.offset = offsetof_data(var__navigation_t, dest_pos_.x_);
			ct.length = msizeof(var__navigation_t, dest_pos_.x_);
			m_propertiesMap.insert("dest_pos_.x_", ct);
			ct.offset = offsetof_data(var__navigation_t, dest_pos_.y_);
			ct.length = msizeof(var__navigation_t, dest_pos_.y_);
			m_propertiesMap.insert("dest_pos_.y_", ct);
			ct.offset = offsetof_data(var__navigation_t, dest_pos_.theta_);
			ct.length = msizeof(var__navigation_t, dest_pos_.theta_);
			m_propertiesMap.insert("dest_pos_.theta_", ct);
			ct.offset = offsetof_data(var__navigation_t, dest_pos_.angle_);
			ct.length = msizeof(var__navigation_t, dest_pos_.angle_);
			m_propertiesMap.insert("dest_pos_.angle_", ct);
			ct.offset = offsetof_data(var__navigation_t, dest_pos_.w_);
			ct.length = msizeof(var__navigation_t, dest_pos_.w_);
			m_propertiesMap.insert("dest_pos_.w_", ct);
			ct.offset = offsetof_data(var__navigation_t, dest_pos_.distance_);
			ct.length = msizeof(var__navigation_t, dest_pos_.distance_);
			m_propertiesMap.insert("dest_pos_.distance_", ct);

			ct.offset = offsetof_data(var__navigation_t, traj_ref_.count_);
			ct.length = msizeof(var__navigation_t, traj_ref_.count_);
			m_propertiesMap.insert("traj_ref_.count_", ct);
			ct.offset = offsetof_data(var__navigation_t, traj_ref_.data64_);
			ct.length = msizeof(var__navigation_t, traj_ref_.data64_);
			m_propertiesMap.insert("traj_ref_.data64_", ct);

			ct.offset = offsetof_data(var__navigation_t, pos_.x_);
			ct.length = msizeof(var__navigation_t, pos_.x_);
			m_propertiesMap.insert("pos_.x_", ct);
			ct.offset = offsetof_data(var__navigation_t, pos_.y_);
			ct.length = msizeof(var__navigation_t, pos_.y_);
			m_propertiesMap.insert("pos_.y_", ct);
			ct.offset = offsetof_data(var__navigation_t, pos_.theta_);
			ct.length = msizeof(var__navigation_t, pos_.theta_);
			m_propertiesMap.insert("pos_.theta_", ct);
			ct.offset = offsetof_data(var__navigation_t, pos_.angle_);
			ct.length = msizeof(var__navigation_t, pos_.angle_);
			m_propertiesMap.insert("pos_.angle_", ct);
			ct.offset = offsetof_data(var__navigation_t, pos_.w_);
			ct.length = msizeof(var__navigation_t, pos_.w_);
			m_propertiesMap.insert("pos_.w_", ct);
			ct.offset = offsetof_data(var__navigation_t, pos_.distance_);
			ct.length = msizeof(var__navigation_t, pos_.distance_);
			m_propertiesMap.insert("pos_.distance_", ct);

			ct.offset = offsetof_data(var__navigation_t, pos_time_stamp_);
			ct.length = msizeof(var__navigation_t, pos_time_stamp_);
			m_propertiesMap.insert("pos_time_stamp_", ct);
			ct.offset = offsetof_data(var__navigation_t, pos_status);
			ct.length = msizeof(var__navigation_t, pos_status);
			m_propertiesMap.insert("pos_status", ct);
			ct.offset = offsetof_data(var__navigation_t, pos_confidence_);
			ct.length = msizeof(var__navigation_t, pos_confidence_);
			m_propertiesMap.insert("pos_confidence_", ct);

			ct.offset = offsetof_data(var__navigation_t, i.traj_ref_index_curr_);
			ct.length = msizeof(var__navigation_t, i.traj_ref_index_curr_);
			m_propertiesMap.insert("i.traj_ref_index_curr_", ct);

			ct.offset = offsetof_data(var__navigation_t, i.upl_.edge_id_);
			ct.length = msizeof(var__navigation_t, i.upl_.edge_id_);
			m_propertiesMap.insert("i.upl_.edge_id_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.upl_.percentage_);
			ct.length = msizeof(var__navigation_t, i.upl_.percentage_);
			m_propertiesMap.insert("i.upl_.percentage_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.upl_.wop_id_);
			ct.length = msizeof(var__navigation_t, i.upl_.wop_id_);
			m_propertiesMap.insert("i.upl_.wop_id_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.upl_.angle_);
			ct.length = msizeof(var__navigation_t, i.upl_.angle_);
			m_propertiesMap.insert("i.upl_.angle_", ct);

			ct.offset = offsetof_data(var__navigation_t, i.tracking_error_);
			ct.length = msizeof(var__navigation_t, i.tracking_error_);
			m_propertiesMap.insert("i.tracking_error_", ct);

			ct.offset = offsetof_data(var__navigation_t, i.base_point_.x_);
			ct.length = msizeof(var__navigation_t, i.base_point_.x_);
			m_propertiesMap.insert("i.base_point_.x_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.base_point_.y_);
			ct.length = msizeof(var__navigation_t, i.base_point_.y_);
			m_propertiesMap.insert("i.base_point_.y_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.base_point_.theta_);
			ct.length = msizeof(var__navigation_t, i.base_point_.theta_);
			m_propertiesMap.insert("i.base_point_.theta_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.base_point_.angle_);
			ct.length = msizeof(var__navigation_t, i.base_point_.angle_);
			m_propertiesMap.insert("i.base_point_.angle_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.base_point_.w_);
			ct.length = msizeof(var__navigation_t, i.base_point_.w_);
			m_propertiesMap.insert("i.base_point_.w_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.base_point_.distance_);
			ct.length = msizeof(var__navigation_t, i.base_point_.distance_);
			m_propertiesMap.insert("i.base_point_.distance_", ct);

			ct.offset = offsetof_data(var__navigation_t, i.aim_point_.x_);
			ct.length = msizeof(var__navigation_t, i.aim_point_.x_);
			m_propertiesMap.insert("i.aim_point_.x_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.aim_point_.y_);
			ct.length = msizeof(var__navigation_t, i.aim_point_.y_);
			m_propertiesMap.insert("i.aim_point_.y_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.aim_point_.theta_);
			ct.length = msizeof(var__navigation_t, i.aim_point_.theta_);
			m_propertiesMap.insert("i.aim_point_.theta_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.aim_point_.angle_);
			ct.length = msizeof(var__navigation_t, i.aim_point_.angle_);
			m_propertiesMap.insert("i.aim_point_.angle_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.aim_point_.w_);
			ct.length = msizeof(var__navigation_t, i.aim_point_.w_);
			m_propertiesMap.insert("i.aim_point_.w_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.aim_point_.distance_);
			ct.length = msizeof(var__navigation_t, i.aim_point_.distance_);
			m_propertiesMap.insert("i.aim_point_.distance_", ct);

			ct.offset = offsetof_data(var__navigation_t, i.aim_heading_error_);
			ct.length = msizeof(var__navigation_t, i.aim_heading_error_);
			m_propertiesMap.insert("i.aim_heading_error_", ct);

			ct.offset = offsetof_data(var__navigation_t, i.predict_point_.x_);
			ct.length = msizeof(var__navigation_t, i.predict_point_.x_);
			m_propertiesMap.insert("i.predict_point_.x_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.predict_point_.y_);
			ct.length = msizeof(var__navigation_t, i.predict_point_.y_);
			m_propertiesMap.insert("i.predict_point_.y_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.predict_point_.theta_);
			ct.length = msizeof(var__navigation_t, i.predict_point_.theta_);
			m_propertiesMap.insert("i.predict_point_.theta_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.predict_point_.angle_);
			ct.length = msizeof(var__navigation_t, i.predict_point_.angle_);
			m_propertiesMap.insert("i.predict_point_.angle_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.predict_point_.w_);
			ct.length = msizeof(var__navigation_t, i.predict_point_.w_);
			m_propertiesMap.insert("i.predict_point_.w_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.predict_point_.distance_);
			ct.length = msizeof(var__navigation_t, i.predict_point_.distance_);
			m_propertiesMap.insert("i.predict_point_.distance_", ct);

			ct.offset = offsetof_data(var__navigation_t, i.predict_point_curvature_);
			ct.length = msizeof(var__navigation_t, i.predict_point_curvature_);
			m_propertiesMap.insert("i.predict_point_curvature_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.on_last_segment_);
			ct.length = msizeof(var__navigation_t, i.on_last_segment_);
			m_propertiesMap.insert("i.on_last_segment_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.dist_to_partition_);
			ct.length = msizeof(var__navigation_t, i.dist_to_partition_);
			m_propertiesMap.insert("i.dist_to_partition_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.dist_to_dest_);
			ct.length = msizeof(var__navigation_t, i.dist_to_dest_);
			m_propertiesMap.insert("i.dist_to_dest_", ct);

			ct.offset = offsetof_data(var__navigation_t, i.current_edge_wop_properties_.wop_id_);
			ct.length = msizeof(var__navigation_t, i.current_edge_wop_properties_.wop_id_);
			m_propertiesMap.insert("i.current_edge_wop_properties_.wop_id_", ct);
			ct.offset = offsetof_data(var__navigation_t, i.current_edge_wop_properties_.enabled_);
			ct.length = msizeof(var__navigation_t, i.current_edge_wop_properties_.enabled_);
			m_propertiesMap.insert("i.current_edge_wop_properties_.enabled_", ct);

			QString wop_properties_str;

			for (int i = 0; i < 9; ++i)
			{
				ct.offset = offsetof_data(var__navigation_t, i.current_edge_wop_properties_.wop_properties_[i]);
				ct.length = msizeof(var__navigation_t, i.current_edge_wop_properties_.wop_properties_[i]);
				wop_properties_str = "i.current_edge_wop_properties_.wop_properties_[" + QString::number(i) + "]";
				m_propertiesMap.insert(wop_properties_str, ct);
			}

			ct.offset = offsetof_data(var__navigation_t, u_);
			ct.length = msizeof(var__navigation_t, u_);
			m_propertiesMap.insert("u_", ct);
			ct.offset = offsetof_data(var__navigation_t, runtime_limiting_velocity_);
			ct.length = msizeof(var__navigation_t, runtime_limiting_velocity_);
			m_propertiesMap.insert("runtime_limiting_velocity_", ct);
		}
			break;
		case kVarType_Operation:
		{
			ct.offset = offsetof_data(var__operation_t, status_.command_);
			ct.length = msizeof(var__operation_t, status_.command_);
			m_propertiesMap.insert("status_.command_", ct);
			ct.offset = offsetof_data(var__operation_t, status_.middle_);
			ct.length = msizeof(var__operation_t, status_.middle_);
			m_propertiesMap.insert("status_.middle_", ct);
			ct.offset = offsetof_data(var__operation_t, status_.response_);
			ct.length = msizeof(var__operation_t, status_.response_);
			m_propertiesMap.insert("status_.response_", ct);

			ct.offset = offsetof_data(var__operation_t, user_task_id_);
			ct.length = msizeof(var__operation_t, user_task_id_);
			m_propertiesMap.insert("user_task_id_", ct);
			ct.offset = offsetof_data(var__operation_t, ato_task_id_);
			ct.length = msizeof(var__operation_t, ato_task_id_);
			m_propertiesMap.insert("ato_task_id_", ct);
			ct.offset = offsetof_data(var__operation_t, code_);
			ct.length = msizeof(var__operation_t, code_);
			m_propertiesMap.insert("code_", ct);
			ct.offset = offsetof_data(var__operation_t, param0_);
			ct.length = msizeof(var__operation_t, param0_);
			m_propertiesMap.insert("param0_", ct);
			ct.offset = offsetof_data(var__operation_t, param1_);
			ct.length = msizeof(var__operation_t, param1_);
			m_propertiesMap.insert("param1_", ct);
			ct.offset = offsetof_data(var__operation_t, param2_);
			ct.length = msizeof(var__operation_t, param2_);
			m_propertiesMap.insert("param2_", ct);
			ct.offset = offsetof_data(var__operation_t, param3_);
			ct.length = msizeof(var__operation_t, param3_);
			m_propertiesMap.insert("param3_", ct);
			ct.offset = offsetof_data(var__operation_t, param4_);
			ct.length = msizeof(var__operation_t, param4_);
			m_propertiesMap.insert("param4_", ct);
			ct.offset = offsetof_data(var__operation_t, param5_);
			ct.length = msizeof(var__operation_t, param5_);
			m_propertiesMap.insert("param5_", ct);
			ct.offset = offsetof_data(var__operation_t, param6_);
			ct.length = msizeof(var__operation_t, param6_);
			m_propertiesMap.insert("param6_", ct);
			ct.offset = offsetof_data(var__operation_t, param7_);
			ct.length = msizeof(var__operation_t, param7_);
			m_propertiesMap.insert("param7_", ct);
			ct.offset = offsetof_data(var__operation_t, param8_);
			ct.length = msizeof(var__operation_t, param8_);
			m_propertiesMap.insert("param8_", ct);
			ct.offset = offsetof_data(var__operation_t, param9_);
			ct.length = msizeof(var__operation_t, param9_);
			m_propertiesMap.insert("param9_", ct);

			ct.offset = offsetof_data(var__operation_t, i.param10_);
			ct.length = msizeof(var__operation_t, i.param10_);
			m_propertiesMap.insert("i.param10_", ct);
			ct.offset = offsetof_data(var__operation_t, i.param11_);
			ct.length = msizeof(var__operation_t, i.param11_);
			m_propertiesMap.insert("i.param11_", ct);
			ct.offset = offsetof_data(var__operation_t, i.param12_);
			ct.length = msizeof(var__operation_t, i.param12_);
			m_propertiesMap.insert("i.param12_", ct);
			ct.offset = offsetof_data(var__operation_t, i.param13_);
			ct.length = msizeof(var__operation_t, i.param13_);
			m_propertiesMap.insert("i.param13_", ct);
			ct.offset = offsetof_data(var__operation_t, i.param14_);
			ct.length = msizeof(var__operation_t, i.param14_);
			m_propertiesMap.insert("i.param14_", ct);
			ct.offset = offsetof_data(var__operation_t, i.param15_);
			ct.length = msizeof(var__operation_t, i.param15_);
			m_propertiesMap.insert("i.param15_", ct);
			ct.offset = offsetof_data(var__operation_t, i.param16_);
			ct.length = msizeof(var__operation_t, i.param16_);
			m_propertiesMap.insert("i.param16_", ct);
			ct.offset = offsetof_data(var__operation_t, i.param17_);
			ct.length = msizeof(var__operation_t, i.param17_);
			m_propertiesMap.insert("i.param17_", ct);
			ct.offset = offsetof_data(var__operation_t, i.param18_);
			ct.length = msizeof(var__operation_t, i.param18_);
			m_propertiesMap.insert("i.param18_", ct);
			ct.offset = offsetof_data(var__operation_t, i.param19_);
			ct.length = msizeof(var__operation_t, i.param19_);
			m_propertiesMap.insert("i.param19_", ct);

			ct.offset = offsetof_data(var__operation_t, u_);
			ct.length = msizeof(var__operation_t, u_);
			m_propertiesMap.insert("u_", ct);
		}
			break;
		case kVarType_UserDefined:
		{
			ct.offset = offsetof_data(var__usrdef_buffer_t, usrbuf_);
			ct.length = msizeof(var__usrdef_buffer_t, usrbuf_);
			m_propertiesMap.insert("usrbuf_", ct);
		}
			break;
		case kVarType_SWheel:
		{
			ct.offset = offsetof_data(var__swheel_t, min_angle_);
			ct.length = msizeof(var__swheel_t, min_angle_);
			m_propertiesMap.insert("min_angle_", ct);
			ct.offset = offsetof_data(var__swheel_t, max_angle_);
			ct.length = msizeof(var__swheel_t, max_angle_);
			m_propertiesMap.insert("max_angle_", ct);
			ct.offset = offsetof_data(var__swheel_t, zero_angle_);
			ct.length = msizeof(var__swheel_t, zero_angle_);
			m_propertiesMap.insert("zero_angle_", ct);
			ct.offset = offsetof_data(var__swheel_t, zero_angle_enc_);
			ct.length = msizeof(var__swheel_t, zero_angle_enc_);
			m_propertiesMap.insert("zero_angle_enc_", ct);
			ct.offset = offsetof_data(var__swheel_t, max_w_);
			ct.length = msizeof(var__swheel_t, max_w_);
			m_propertiesMap.insert("max_w_", ct);
			ct.offset = offsetof_data(var__swheel_t, control_mode_);
			ct.length = msizeof(var__swheel_t, control_mode_);
			m_propertiesMap.insert("control_mode_", ct);
			ct.offset = offsetof_data(var__swheel_t, scale_control_);
			ct.length = msizeof(var__swheel_t, scale_control_);
			m_propertiesMap.insert("scale_control_", ct);
			ct.offset = offsetof_data(var__swheel_t, scale_feedback_);
			ct.length = msizeof(var__swheel_t, scale_feedback_);
			m_propertiesMap.insert("scale_feedback_", ct);
			ct.offset = offsetof_data(var__swheel_t, control_cp_);
			ct.length = msizeof(var__swheel_t, control_cp_);
			m_propertiesMap.insert("control_cp_", ct);
			ct.offset = offsetof_data(var__swheel_t, control_ci_);
			ct.length = msizeof(var__swheel_t, control_ci_);
			m_propertiesMap.insert("control_ci_", ct);
			ct.offset = offsetof_data(var__swheel_t, control_cd_);
			ct.length = msizeof(var__swheel_t, control_cd_);
			m_propertiesMap.insert("control_cd_", ct);
			ct.offset = offsetof_data(var__swheel_t, enabled_);
			ct.length = msizeof(var__swheel_t, enabled_);
			m_propertiesMap.insert("enabled_", ct);
			ct.offset = offsetof_data(var__swheel_t, actual_angle_);
			ct.length = msizeof(var__swheel_t, actual_angle_);
			m_propertiesMap.insert("actual_angle_", ct);
			ct.offset = offsetof_data(var__swheel_t, actual_angle_enc_);
			ct.length = msizeof(var__swheel_t, actual_angle_enc_);
			m_propertiesMap.insert("actual_angle_enc_", ct);
			ct.offset = offsetof_data(var__swheel_t, time_stamp_);
			ct.length = msizeof(var__swheel_t, time_stamp_);
			m_propertiesMap.insert("time_stamp_", ct);
			ct.offset = offsetof_data(var__swheel_t, error_code_);
			ct.length = msizeof(var__swheel_t, error_code_);
			m_propertiesMap.insert("error_code_", ct);

			ct.offset = offsetof_data(var__swheel_t, i.enable_);
			ct.length = msizeof(var__swheel_t, i.enable_);
			m_propertiesMap.insert("i.enable_", ct);
			ct.offset = offsetof_data(var__swheel_t, i.command_angle_);
			ct.length = msizeof(var__swheel_t, i.command_angle_);
			m_propertiesMap.insert("i.command_angle_", ct);
			ct.offset = offsetof_data(var__swheel_t, i.command_angle_enc_);
			ct.length = msizeof(var__swheel_t, i.command_angle_enc_);
			m_propertiesMap.insert("i.command_angle_enc_", ct);
			ct.offset = offsetof_data(var__swheel_t, i.command_rate_);
			ct.length = msizeof(var__swheel_t, i.command_rate_);
			m_propertiesMap.insert("i.command_rate_", ct);
			ct.offset = offsetof_data(var__swheel_t, i.command_rate_enc_);
			ct.length = msizeof(var__swheel_t, i.command_rate_enc_);
			m_propertiesMap.insert("i.command_rate_enc_", ct);
			
			ct.offset = offsetof_data(var__swheel_t, u_);
			ct.length = msizeof(var__swheel_t, u_);
			m_propertiesMap.insert("u_", ct);
		}
			break;
		case kVarType_DWheel:
		{
			ct.offset = offsetof_data(var__dwheel_t, max_speed_);
			ct.length = msizeof(var__dwheel_t, max_speed_);
			m_propertiesMap.insert("max_speed_", ct);
			ct.offset = offsetof_data(var__dwheel_t, max_acc_);
			ct.length = msizeof(var__dwheel_t, max_acc_);
			m_propertiesMap.insert("max_acc_", ct);
			ct.offset = offsetof_data(var__dwheel_t, max_dec_);
			ct.length = msizeof(var__dwheel_t, max_dec_);
			m_propertiesMap.insert("max_dec_", ct);
			ct.offset = offsetof_data(var__dwheel_t, control_mode_);
			ct.length = msizeof(var__dwheel_t, control_mode_);
			m_propertiesMap.insert("control_mode_", ct);
			ct.offset = offsetof_data(var__dwheel_t, scale_control_);
			ct.length = msizeof(var__dwheel_t, scale_control_);
			m_propertiesMap.insert("scale_control_", ct);
			ct.offset = offsetof_data(var__dwheel_t, scale_feedback_);
			ct.length = msizeof(var__dwheel_t, scale_feedback_);
			m_propertiesMap.insert("scale_feedback_", ct);
			ct.offset = offsetof_data(var__dwheel_t, roll_weight_);
			ct.length = msizeof(var__dwheel_t, roll_weight_);
			m_propertiesMap.insert("roll_weight_", ct);
			ct.offset = offsetof_data(var__dwheel_t, slide_weight_);
			ct.length = msizeof(var__dwheel_t, slide_weight_);
			m_propertiesMap.insert("slide_weight_", ct);
			ct.offset = offsetof_data(var__dwheel_t, enabled_);
			ct.length = msizeof(var__dwheel_t, enabled_);
			m_propertiesMap.insert("enabled_", ct);
			ct.offset = offsetof_data(var__dwheel_t, actual_velocity_);
			ct.length = msizeof(var__dwheel_t, actual_velocity_);
			m_propertiesMap.insert("actual_velocity_", ct);
			ct.offset = offsetof_data(var__dwheel_t, actual_velocity_enc_);
			ct.length = msizeof(var__dwheel_t, actual_velocity_enc_);
			m_propertiesMap.insert("actual_velocity_enc_", ct);
			ct.offset = offsetof_data(var__dwheel_t, actual_position_);
			ct.length = msizeof(var__dwheel_t, actual_position_);
			m_propertiesMap.insert("actual_position_", ct);
			ct.offset = offsetof_data(var__dwheel_t, actual_position_enc_);
			ct.length = msizeof(var__dwheel_t, actual_position_enc_);
			m_propertiesMap.insert("actual_position_enc_", ct);
			ct.offset = offsetof_data(var__dwheel_t, actual_current_);
			ct.length = msizeof(var__dwheel_t, actual_current_);
			m_propertiesMap.insert("actual_current_", ct);
			ct.offset = offsetof_data(var__dwheel_t, time_stamp_);
			ct.length = msizeof(var__dwheel_t, time_stamp_);
			m_propertiesMap.insert("time_stamp_", ct);
			ct.offset = offsetof_data(var__dwheel_t, error_code_);
			ct.length = msizeof(var__dwheel_t, error_code_);
			m_propertiesMap.insert("error_code_", ct);

			ct.offset = offsetof_data(var__dwheel_t, i.enable_);
			ct.length = msizeof(var__dwheel_t, i.enable_);
			m_propertiesMap.insert("i.enable_", ct);
			ct.offset = offsetof_data(var__dwheel_t, i.command_velocity_);
			ct.length = msizeof(var__dwheel_t, i.command_velocity_);
			m_propertiesMap.insert("i.command_velocity_", ct);
			ct.offset = offsetof_data(var__dwheel_t, i.command_velocity_enc_);
			ct.length = msizeof(var__dwheel_t, i.command_velocity_enc_);
			m_propertiesMap.insert("i.command_velocity_enc_", ct);
			ct.offset = offsetof_data(var__dwheel_t, i.command_position_);
			ct.length = msizeof(var__dwheel_t, i.command_position_);
			m_propertiesMap.insert("i.command_position_", ct);
			ct.offset = offsetof_data(var__dwheel_t, i.command_position_enc_);
			ct.length = msizeof(var__dwheel_t, i.command_position_enc_);
			m_propertiesMap.insert("i.command_position_enc_", ct);
			ct.offset = offsetof_data(var__dwheel_t, i.command_current_);
			ct.length = msizeof(var__dwheel_t, i.command_current_);
			m_propertiesMap.insert("i.command_current_", ct);

			ct.offset = offsetof_data(var__dwheel_t, u_);
			ct.length = msizeof(var__dwheel_t, u_);
			m_propertiesMap.insert("u_", ct);
		}
			break;
		case kVarType_SDDExtra:
		{
			ct.offset = offsetof_data(var__sdd_extra_t, gauge_);
			ct.length = msizeof(var__sdd_extra_t, gauge_);
			m_propertiesMap.insert("gauge_", ct);
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
			ct.offset = offsetof_data(var__operation_parameter_t, ull00_);
			ct.length = msizeof(var__operation_parameter_t, ull00_);
			m_propertiesMap.insert("ull00_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull01_);
			ct.length = msizeof(var__operation_parameter_t, ull01_);
			m_propertiesMap.insert("ull01_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull02_);
			ct.length = msizeof(var__operation_parameter_t, ull02_);
			m_propertiesMap.insert("ull02_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull03_);
			ct.length = msizeof(var__operation_parameter_t, ull03_);
			m_propertiesMap.insert("ull03_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull04_);
			ct.length = msizeof(var__operation_parameter_t, ull04_);
			m_propertiesMap.insert("ull04_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull05_);
			ct.length = msizeof(var__operation_parameter_t, ull05_);
			m_propertiesMap.insert("ull05_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull06_);
			ct.length = msizeof(var__operation_parameter_t, ull06_);
			m_propertiesMap.insert("ull06_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull07_);
			ct.length = msizeof(var__operation_parameter_t, ull07_);
			m_propertiesMap.insert("ull07_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull08_);
			ct.length = msizeof(var__operation_parameter_t, ull08_);
			m_propertiesMap.insert("ull08_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull09_);
			ct.length = msizeof(var__operation_parameter_t, ull09_);
			m_propertiesMap.insert("ull09_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull10_);
			ct.length = msizeof(var__operation_parameter_t, ull10_);
			m_propertiesMap.insert("ull10_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull11_);
			ct.length = msizeof(var__operation_parameter_t, ull11_);
			m_propertiesMap.insert("ull11_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull12_);
			ct.length = msizeof(var__operation_parameter_t, ull12_);
			m_propertiesMap.insert("ull12_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull13_);
			ct.length = msizeof(var__operation_parameter_t, ull13_);
			m_propertiesMap.insert("ull13_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull14_);
			ct.length = msizeof(var__operation_parameter_t, ull14_);
			m_propertiesMap.insert("ull14_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull15_);
			ct.length = msizeof(var__operation_parameter_t, ull15_);
			m_propertiesMap.insert("ull15_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull16_);
			ct.length = msizeof(var__operation_parameter_t, ull16_);
			m_propertiesMap.insert("ull16_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull17_);
			ct.length = msizeof(var__operation_parameter_t, ull17_);
			m_propertiesMap.insert("ull17_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull18_);
			ct.length = msizeof(var__operation_parameter_t, ull18_);
			m_propertiesMap.insert("ull18_", ct);
			ct.offset = offsetof_data(var__operation_parameter_t, ull19_);
			ct.length = msizeof(var__operation_parameter_t, ull19_);
			m_propertiesMap.insert("ull19_", ct);
		}
			break;
		case kVarType_CanBus:
		{

		}
			break;
		case kVarType_Copley:
		{

		}
			break;
		case kVarType_Elmo:
		{
			ct.offset = offsetof_data(var__elmo_t, candev_head_.canbus_);
			ct.length = msizeof(var__elmo_t, candev_head_.canbus_);
			m_propertiesMap.insert("candev_head_.canbus_", ct);
			ct.offset = offsetof_data(var__elmo_t, candev_head_.canport_);
			ct.length = msizeof(var__elmo_t, candev_head_.canport_);
			m_propertiesMap.insert("candev_head_.canport_", ct);
			ct.offset = offsetof_data(var__elmo_t, candev_head_.cannode_);
			ct.length = msizeof(var__elmo_t, candev_head_.cannode_);
			m_propertiesMap.insert("candev_head_.cannode_", ct);
			ct.offset = offsetof_data(var__elmo_t, candev_head_.latency_);
			ct.length = msizeof(var__elmo_t, candev_head_.latency_);
			m_propertiesMap.insert("candev_head_.latency_", ct);
			ct.offset = offsetof_data(var__elmo_t, candev_head_.merge_);
			ct.length = msizeof(var__elmo_t, candev_head_.merge_);
			m_propertiesMap.insert("candev_head_.merge_", ct);
			ct.offset = offsetof_data(var__elmo_t, candev_head_.self_rw64_);
			ct.length = msizeof(var__elmo_t, candev_head_.self_rw64_);
			m_propertiesMap.insert("candev_head_.self_rw64_", ct);
			ct.offset = offsetof_data(var__elmo_t, candev_head_.pdocnt_);
			ct.length = msizeof(var__elmo_t, candev_head_.pdocnt_);
			m_propertiesMap.insert("candev_head_.pdocnt_", ct);

			ct.offset = offsetof_data(var__elmo_t, profile_speed_);
			ct.length = msizeof(var__elmo_t, profile_speed_);
			m_propertiesMap.insert("profile_speed_", ct);
			ct.offset = offsetof_data(var__elmo_t, profile_acc_);
			ct.length = msizeof(var__elmo_t, profile_acc_);
			m_propertiesMap.insert("profile_acc_", ct);
			ct.offset = offsetof_data(var__elmo_t, profile_dec_);
			ct.length = msizeof(var__elmo_t, profile_dec_);
			m_propertiesMap.insert("profile_dec_", ct);

			ct.offset = offsetof_data(var__elmo_t, status_.command_);
			ct.length = msizeof(var__elmo_t, status_.command_);
			m_propertiesMap.insert("status_.command_", ct);
			ct.offset = offsetof_data(var__elmo_t, status_.middle_);
			ct.length = msizeof(var__elmo_t, status_.middle_);
			m_propertiesMap.insert("status_.middle_", ct);
			ct.offset = offsetof_data(var__elmo_t, status_.response_);
			ct.length = msizeof(var__elmo_t, status_.response_);
			m_propertiesMap.insert("status_.response_", ct);

			ct.offset = offsetof_data(var__elmo_t, control_mode_);
			ct.length = msizeof(var__elmo_t, control_mode_);
			m_propertiesMap.insert("control_mode_", ct);
			ct.offset = offsetof_data(var__elmo_t, command_velocity_);
			ct.length = msizeof(var__elmo_t, command_velocity_);
			m_propertiesMap.insert("command_velocity_", ct);
			ct.offset = offsetof_data(var__elmo_t, command_position_);
			ct.length = msizeof(var__elmo_t, command_position_);
			m_propertiesMap.insert("command_position_", ct);
			ct.offset = offsetof_data(var__elmo_t, command_current_);
			ct.length = msizeof(var__elmo_t, command_current_);
			m_propertiesMap.insert("command_current_", ct);
			ct.offset = offsetof_data(var__elmo_t, enable_);
			ct.length = msizeof(var__elmo_t, enable_);
			m_propertiesMap.insert("enable_", ct);

			ct.offset = offsetof_data(var__elmo_t, i.node_state_);
			ct.length = msizeof(var__elmo_t, i.node_state_);
			m_propertiesMap.insert("i.node_state_", ct);
			ct.offset = offsetof_data(var__elmo_t, i.error_code_);
			ct.length = msizeof(var__elmo_t, i.error_code_);
			m_propertiesMap.insert("i.error_code_", ct);
			ct.offset = offsetof_data(var__elmo_t, i.time_stamp_);
			ct.length = msizeof(var__elmo_t, i.time_stamp_);
			m_propertiesMap.insert("i.time_stamp_", ct);
			ct.offset = offsetof_data(var__elmo_t, i.actual_velocity_);
			ct.length = msizeof(var__elmo_t, i.actual_velocity_);
			m_propertiesMap.insert("i.actual_velocity_", ct);
			ct.offset = offsetof_data(var__elmo_t, i.actual_position_);
			ct.length = msizeof(var__elmo_t, i.actual_position_);
			m_propertiesMap.insert("i.actual_position_", ct);
			ct.offset = offsetof_data(var__elmo_t, i.actual_current_);
			ct.length = msizeof(var__elmo_t, i.actual_current_);
			m_propertiesMap.insert("i.actual_current_", ct);
			ct.offset = offsetof_data(var__elmo_t, i.enabled_);
			ct.length = msizeof(var__elmo_t, i.enabled_);
			m_propertiesMap.insert("i.enabled_", ct);
			ct.offset = offsetof_data(var__elmo_t, i.di_);
			ct.length = msizeof(var__elmo_t, i.di_);
			m_propertiesMap.insert("i.di_", ct);
			ct.offset = offsetof_data(var__elmo_t, i.do_);
			ct.length = msizeof(var__elmo_t, i.do_);
			m_propertiesMap.insert("i.do_", ct);

			ct.offset = offsetof_data(var__elmo_t, u_);
			ct.length = msizeof(var__elmo_t, u_);
			m_propertiesMap.insert("u_", ct);
		}
			break;
		case kVarType_DIO:
		{
			ct.offset = offsetof_data(var__dio_t, candev_head_.canbus_);
			ct.length = msizeof(var__dio_t, candev_head_.canbus_);
			m_propertiesMap.insert("candev_head_.canbus_", ct);
			ct.offset = offsetof_data(var__dio_t, candev_head_.canport_);
			ct.length = msizeof(var__dio_t, candev_head_.canport_);
			m_propertiesMap.insert("candev_head_.canport_", ct);
			ct.offset = offsetof_data(var__dio_t, candev_head_.cannode_);
			ct.length = msizeof(var__dio_t, candev_head_.cannode_);
			m_propertiesMap.insert("candev_head_.cannode_", ct);
			ct.offset = offsetof_data(var__dio_t, candev_head_.latency_);
			ct.length = msizeof(var__dio_t, candev_head_.latency_);
			m_propertiesMap.insert("candev_head_.latency_", ct);
			ct.offset = offsetof_data(var__dio_t, candev_head_.merge_);
			ct.length = msizeof(var__dio_t, candev_head_.merge_);
			m_propertiesMap.insert("candev_head_.merge_", ct);
			ct.offset = offsetof_data(var__dio_t, candev_head_.self_rw64_);
			ct.length = msizeof(var__dio_t, candev_head_.self_rw64_);
			m_propertiesMap.insert("candev_head_.self_rw64_", ct);
			ct.offset = offsetof_data(var__dio_t, candev_head_.pdocnt_);
			ct.length = msizeof(var__dio_t, candev_head_.pdocnt_);
			m_propertiesMap.insert("candev_head_.pdocnt_", ct);

			ct.offset = offsetof_data(var__dio_t, di_channel_num_);
			ct.length = msizeof(var__dio_t, di_channel_num_);
			m_propertiesMap.insert("di_channel_num_", ct);
			ct.offset = offsetof_data(var__dio_t, do_channel_num_);
			ct.length = msizeof(var__dio_t, do_channel_num_);
			m_propertiesMap.insert("do_channel_num_", ct);

			ct.offset = offsetof_data(var__dio_t, status_.command_);
			ct.length = msizeof(var__dio_t, status_.command_);
			m_propertiesMap.insert("status_.command_", ct);
			ct.offset = offsetof_data(var__dio_t, status_.middle_);
			ct.length = msizeof(var__dio_t, status_.middle_);
			m_propertiesMap.insert("status_.middle_", ct);
			ct.offset = offsetof_data(var__dio_t, status_.response_);
			ct.length = msizeof(var__dio_t, status_.response_);
			m_propertiesMap.insert("status_.response_", ct);

			ct.offset = offsetof_data(var__dio_t, do_);
			ct.length = msizeof(var__dio_t, do_);
			m_propertiesMap.insert("do_", ct);

			QString ao_key, ao_data_key;

			for (int i = 0; i < MAXIMUM_DIO_BLOCK_COUNT; ++i)
			{
				ct.offset = offsetof_data(var__dio_t, ao_[i].start_address_);
				ct.length = msizeof(var__dio_t, ao_[i].start_address_);
				ao_key = "ao_[" + QString::number(i) + "].start_address_";
				m_propertiesMap.insert(ao_key, ct);
				ct.offset = offsetof_data(var__dio_t, ao_[i].effective_count_of_index_);
				ct.length = msizeof(var__dio_t, ao_[i].effective_count_of_index_);
				ao_key = "ao_[" + QString::number(i) + "].effective_count_of_index_";
				m_propertiesMap.insert(ao_key, ct);
				ct.offset = offsetof_data(var__dio_t, ao_[i].internel_type_);
				ct.length = msizeof(var__dio_t, ao_[i].internel_type_);
				ao_key = "ao_[" + QString::number(i) + "].internel_type_";
				m_propertiesMap.insert(ao_key, ct);

				for (int j = 0; j < DIO_BLOCK_DATACB; ++j)
				{
					ct.offset = offsetof_data(var__dio_t, ao_[i].data_[j]);
					ct.length = msizeof(var__dio_t, ao_[i].data_[j]);
					ao_key = "ao_[" + QString::number(i) + "].data_[" + QString::number(j) + "]";
					m_propertiesMap.insert(ao_key, ct);
				}
			}

			ct.offset = offsetof_data(var__dio_t, i.bus_state_);
			ct.length = msizeof(var__dio_t, i.bus_state_);
			m_propertiesMap.insert("i.bus_state_", ct);
			ct.offset = offsetof_data(var__dio_t, i.error_code_);
			ct.length = msizeof(var__dio_t, i.error_code_);
			m_propertiesMap.insert("i.error_code_", ct);
			ct.offset = offsetof_data(var__dio_t, i.time_stamp_);
			ct.length = msizeof(var__dio_t, i.time_stamp_);
			m_propertiesMap.insert("i.time_stamp_", ct);
			ct.offset = offsetof_data(var__dio_t, i.enabled_);
			ct.length = msizeof(var__dio_t, i.enabled_);
			m_propertiesMap.insert("i.enabled_", ct);
			ct.offset = offsetof_data(var__dio_t, i.di_);
			ct.length = msizeof(var__dio_t, i.di_);
			m_propertiesMap.insert("i.di_", ct);

			QString ai_key, ai_data_key;

			for (int i = 0; i < MAXIMUM_DIO_BLOCK_COUNT; ++i)
			{
				ct.offset = offsetof_data(var__dio_t, i.ai_[i].start_address_);
				ct.length = msizeof(var__dio_t, i.ai_[i].start_address_);
				ai_key = "i.ai_[" + QString::number(i) + "].start_address_";
				m_propertiesMap.insert(ai_key, ct);
				ct.offset = offsetof_data(var__dio_t, i.ai_[i].effective_count_of_index_);
				ct.length = msizeof(var__dio_t, i.ai_[i].effective_count_of_index_);
				ai_key = "i.ai_[" + QString::number(i) + "].effective_count_of_index_";
				m_propertiesMap.insert(ai_key, ct);
				ct.offset = offsetof_data(var__dio_t, i.ai_[i].internel_type_);
				ct.length = msizeof(var__dio_t, i.ai_[i].internel_type_);
				ai_key = "i.ai_[" + QString::number(i) + "].internel_type_";
				m_propertiesMap.insert(ai_key, ct);

				for (int j = 0; j < DIO_BLOCK_DATACB; ++j)
				{
					ct.offset = offsetof_data(var__dio_t, i.ai_[i].data_[j]);
					ct.length = msizeof(var__dio_t, i.ai_[i].data_[j]);
					ai_key = "i.ai_[" + QString::number(i) + "].data_[" + QString::number(j) + "]";
					m_propertiesMap.insert(ai_key, ct);
				}
			}

			ct.offset = offsetof_data(var__dio_t, i.do2_);
			ct.length = msizeof(var__dio_t, i.do2_);
			m_propertiesMap.insert("i.do2_", ct);

			QString ao2_key, ao2_data_key;

			for (int i = 0; i < MAXIMUM_DIO_BLOCK_COUNT; ++i)
			{
				ct.offset = offsetof_data(var__dio_t, i.ao2_[i].start_address_);
				ct.length = msizeof(var__dio_t, i.ao2_[i].start_address_);
				ao2_key = "i.ao2_[" + QString::number(i) + "].start_address_";
				m_propertiesMap.insert(ao2_key, ct);
				ct.offset = offsetof_data(var__dio_t, i.ao2_[i].effective_count_of_index_);
				ct.length = msizeof(var__dio_t, i.ao2_[i].effective_count_of_index_);
				ao2_key = "i.ao2_[" + QString::number(i) + "].effective_count_of_index_";
				m_propertiesMap.insert(ao2_key, ct);
				ct.offset = offsetof_data(var__dio_t, i.ao2_[i].internel_type_);
				ct.length = msizeof(var__dio_t, i.ao2_[i].internel_type_);
				ao2_key = "i.ao2_[" + QString::number(i) + "].internel_type_";
				m_propertiesMap.insert(ao2_key, ct);

				for (int j = 0; j < DIO_BLOCK_DATACB; ++j)
				{
					ct.offset = offsetof_data(var__dio_t, i.ao2_[i].data_[j]);
					ct.length = msizeof(var__dio_t, i.ao2_[i].data_[j]);
					ao2_key = "i.ao2_[" + QString::number(i) + "].data_[" + QString::number(j) + "]";
					m_propertiesMap.insert(ao2_key, ct);
				}
			}

			ct.offset = offsetof_data(var__dio_t, u_);
			ct.length = msizeof(var__dio_t, u_);
			m_propertiesMap.insert("u_", ct);
		}
			break;
		case kVarType_Moons:
		{
			ct.offset = offsetof_data(var__moos_t, candev_head_.canbus_);
			ct.length = msizeof(var__moos_t, candev_head_.canbus_);
			m_propertiesMap.insert("candev_head_.canbus_", ct);
			ct.offset = offsetof_data(var__moos_t, candev_head_.canport_);
			ct.length = msizeof(var__moos_t, candev_head_.canport_);
			m_propertiesMap.insert("candev_head_.canport_", ct);
			ct.offset = offsetof_data(var__moos_t, candev_head_.cannode_);
			ct.length = msizeof(var__moos_t, candev_head_.cannode_);
			m_propertiesMap.insert("candev_head_.cannode_", ct);
			ct.offset = offsetof_data(var__moos_t, candev_head_.latency_);
			ct.length = msizeof(var__moos_t, candev_head_.latency_);
			m_propertiesMap.insert("candev_head_.latency_", ct);
			ct.offset = offsetof_data(var__moos_t, candev_head_.merge_);
			ct.length = msizeof(var__moos_t, candev_head_.merge_);
			m_propertiesMap.insert("candev_head_.merge_", ct);
			ct.offset = offsetof_data(var__moos_t, candev_head_.self_rw64_);
			ct.length = msizeof(var__moos_t, candev_head_.self_rw64_);
			m_propertiesMap.insert("candev_head_.self_rw64_", ct);
			ct.offset = offsetof_data(var__moos_t, candev_head_.pdocnt_);
			ct.length = msizeof(var__moos_t, candev_head_.pdocnt_);
			m_propertiesMap.insert("candev_head_.pdocnt_", ct);

			ct.offset = offsetof_data(var__moos_t, profile_speed_);
			ct.length = msizeof(var__moos_t, profile_speed_);
			m_propertiesMap.insert("profile_speed_", ct);
			ct.offset = offsetof_data(var__moos_t, profile_acc_);
			ct.length = msizeof(var__moos_t, profile_acc_);
			m_propertiesMap.insert("profile_acc_", ct);
			ct.offset = offsetof_data(var__moos_t, profile_dec_);
			ct.length = msizeof(var__moos_t, profile_dec_);
			m_propertiesMap.insert("profile_dec_", ct);

			ct.offset = offsetof_data(var__moos_t, status_.command_);
			ct.length = msizeof(var__moos_t, status_.command_);
			m_propertiesMap.insert("status_.command_", ct);
			ct.offset = offsetof_data(var__moos_t, status_.middle_);
			ct.length = msizeof(var__moos_t, status_.middle_);
			m_propertiesMap.insert("status_.middle_", ct);
			ct.offset = offsetof_data(var__moos_t, status_.response_);
			ct.length = msizeof(var__moos_t, status_.response_);
			m_propertiesMap.insert("status_.response_", ct);

			ct.offset = offsetof_data(var__moos_t, control_mode_);
			ct.length = msizeof(var__moos_t, control_mode_);
			m_propertiesMap.insert("control_mode_", ct);
			ct.offset = offsetof_data(var__moos_t, command_velocity_);
			ct.length = msizeof(var__moos_t, command_velocity_);
			m_propertiesMap.insert("command_velocity_", ct);
			ct.offset = offsetof_data(var__moos_t, command_position_);
			ct.length = msizeof(var__moos_t, command_position_);
			m_propertiesMap.insert("command_position_", ct);
			ct.offset = offsetof_data(var__moos_t, command_current_);
			ct.length = msizeof(var__moos_t, command_current_);
			m_propertiesMap.insert("command_current_", ct);
			ct.offset = offsetof_data(var__moos_t, enable_);
			ct.length = msizeof(var__moos_t, enable_);
			m_propertiesMap.insert("enable_", ct);

			ct.offset = offsetof_data(var__moos_t, i.state_);
			ct.length = msizeof(var__moos_t, i.state_);
			m_propertiesMap.insert("i.state_", ct);
			ct.offset = offsetof_data(var__moos_t, i.error_code_);
			ct.length = msizeof(var__moos_t, i.error_code_);
			m_propertiesMap.insert("i.error_code_", ct);
			ct.offset = offsetof_data(var__moos_t, i.time_stamp_);
			ct.length = msizeof(var__moos_t, i.time_stamp_);
			m_propertiesMap.insert("i.time_stamp_", ct);
			ct.offset = offsetof_data(var__moos_t, i.actual_velocity_);
			ct.length = msizeof(var__moos_t, i.actual_velocity_);
			m_propertiesMap.insert("i.actual_velocity_", ct);
			ct.offset = offsetof_data(var__moos_t, i.actual_position_);
			ct.length = msizeof(var__moos_t, i.actual_position_);
			m_propertiesMap.insert("i.actual_position_", ct);
			ct.offset = offsetof_data(var__moos_t, i.actual_current_);
			ct.length = msizeof(var__moos_t, i.actual_current_);
			m_propertiesMap.insert("i.actual_current_", ct);
			ct.offset = offsetof_data(var__moos_t, i.enabled_);
			ct.length = msizeof(var__moos_t, i.enabled_);
			m_propertiesMap.insert("i.enabled_", ct);
			ct.offset = offsetof_data(var__moos_t, i.di_);
			ct.length = msizeof(var__moos_t, i.di_);
			m_propertiesMap.insert("i.di_", ct);
			ct.offset = offsetof_data(var__moos_t, i.do_);
			ct.length = msizeof(var__moos_t, i.do_);
			m_propertiesMap.insert("i.do_", ct);

			ct.offset = offsetof_data(var__moos_t, u_);
			ct.length = msizeof(var__moos_t, u_);
			m_propertiesMap.insert("u_", ct);
		}
			break;
		case kVarType_AngleEncoder:
		{
			ct.offset = offsetof_data(var__angle_encoder_t, candev_head_.canbus_);
			ct.length = msizeof(var__angle_encoder_t, candev_head_.canbus_);
			m_propertiesMap.insert("candev_head_.canbus_", ct);
			ct.offset = offsetof_data(var__angle_encoder_t, candev_head_.canport_);
			ct.length = msizeof(var__angle_encoder_t, candev_head_.canport_);
			m_propertiesMap.insert("candev_head_.canport_", ct);
			ct.offset = offsetof_data(var__angle_encoder_t, candev_head_.cannode_);
			ct.length = msizeof(var__angle_encoder_t, candev_head_.cannode_);
			m_propertiesMap.insert("candev_head_.cannode_", ct);
			ct.offset = offsetof_data(var__angle_encoder_t, candev_head_.latency_);
			ct.length = msizeof(var__angle_encoder_t, candev_head_.latency_);
			m_propertiesMap.insert("candev_head_.latency_", ct);
			ct.offset = offsetof_data(var__angle_encoder_t, candev_head_.merge_);
			ct.length = msizeof(var__angle_encoder_t, candev_head_.merge_);
			m_propertiesMap.insert("candev_head_.merge_", ct);
			ct.offset = offsetof_data(var__angle_encoder_t, candev_head_.self_rw64_);
			ct.length = msizeof(var__angle_encoder_t, candev_head_.self_rw64_);
			m_propertiesMap.insert("candev_head_.self_rw64_", ct);
			ct.offset = offsetof_data(var__angle_encoder_t, candev_head_.pdocnt_);
			ct.length = msizeof(var__angle_encoder_t, candev_head_.pdocnt_);
			m_propertiesMap.insert("candev_head_.pdocnt_", ct);

			ct.offset = offsetof_data(var__angle_encoder_t, encoder_type_);
			ct.length = msizeof(var__angle_encoder_t, encoder_type_);
			m_propertiesMap.insert("encoder_type_", ct);

			ct.offset = offsetof_data(var__angle_encoder_t, i.state_);
			ct.length = msizeof(var__angle_encoder_t, i.state_);
			m_propertiesMap.insert("i.state_", ct);
			ct.offset = offsetof_data(var__angle_encoder_t, i.actual_angle_);
			ct.length = msizeof(var__angle_encoder_t, i.actual_angle_);
			m_propertiesMap.insert("i.actual_angle_", ct);
			ct.offset = offsetof_data(var__angle_encoder_t, i.error_code_);
			ct.length = msizeof(var__angle_encoder_t, i.error_code_);
			m_propertiesMap.insert("i.error_code_", ct);
			ct.offset = offsetof_data(var__angle_encoder_t, i.time_stamp_);
			ct.length = msizeof(var__angle_encoder_t, i.time_stamp_);
			m_propertiesMap.insert("i.time_stamp_", ct);

			ct.offset = offsetof_data(var__angle_encoder_t, u_);
			ct.length = msizeof(var__angle_encoder_t, u_);
			m_propertiesMap.insert("u_", ct);
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
			ct.offset = offsetof_data(var__safety_t, enable_);
			ct.length = msizeof(var__safety_t, enable_);
			m_propertiesMap.insert("enable_", ct);
			ct.offset = offsetof_data(var__safety_t, enabled_);
			ct.length = msizeof(var__safety_t, enabled_);
			m_propertiesMap.insert("enabled_", ct);
			ct.offset = offsetof_data(var__safety_t, cur_bank_id_);
			ct.length = msizeof(var__safety_t, cur_bank_id_);
			m_propertiesMap.insert("cur_bank_id_", ct);
			ct.offset = offsetof_data(var__safety_t, cur_bank_level);
			ct.length = msizeof(var__safety_t, cur_bank_level);
			m_propertiesMap.insert("cur_bank_level", ct);
			ct.offset = offsetof_data(var__safety_t, safety_reslut_);
			ct.length = msizeof(var__safety_t, safety_reslut_);
			m_propertiesMap.insert("safety_reslut_", ct);

			ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.__src_dev_id);
			ct.length = msizeof(var__safety_t, sensor_trrigered_.__src_dev_id);
			m_propertiesMap.insert("sensor_trrigered_.__src_dev_id", ct);
			ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.__src_dev_type);
			ct.length = msizeof(var__safety_t, sensor_trrigered_.__src_dev_type);
			m_propertiesMap.insert("sensor_trrigered_.__src_dev_type", ct);

			QString data_src_channel_key;

			for (int i = 0; i < kSafetyProtectIdex_MaxNum; ++i)
			{
				ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.__data_src_channel[i]);
				ct.length = msizeof(var__safety_t, sensor_trrigered_.__data_src_channel[i]);
				data_src_channel_key = "sensor_trrigered_.__data_src_channel[" + QString::number(i) + "]";
				m_propertiesMap.insert(data_src_channel_key, ct);
			}

			ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.__dev_data_type);
			ct.length = msizeof(var__safety_t, sensor_trrigered_.__dev_data_type);
			m_propertiesMap.insert("sensor_trrigered_.__dev_data_type", ct);
			ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.__reslut);
			ct.length = msizeof(var__safety_t, sensor_trrigered_.__reslut);
			m_propertiesMap.insert("sensor_trrigered_.__reslut", ct);
			ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.sensor_name);
			ct.length = msizeof(var__safety_t, sensor_trrigered_.sensor_name);
			m_propertiesMap.insert("sensor_trrigered_.sensor_name", ct);

			ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.__di_mask);
			ct.length = msizeof(var__safety_t, sensor_trrigered_.__di_mask);
			m_propertiesMap.insert("sensor_trrigered_.__di_mask", ct);
			ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.__ai_thres);
			ct.length = msizeof(var__safety_t, sensor_trrigered_.__ai_thres);
			m_propertiesMap.insert("sensor_trrigered_.__ai_thres", ct);
			ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.__di_cur);
			ct.length = msizeof(var__safety_t, sensor_trrigered_.__di_cur);
			m_propertiesMap.insert("sensor_trrigered_.__di_cur", ct);
			ct.offset = offsetof_data(var__safety_t, sensor_trrigered_.__ai_cur);
			ct.length = msizeof(var__safety_t, sensor_trrigered_.__ai_cur);
			m_propertiesMap.insert("sensor_trrigered_.__ai_cur", ct);
		}
			break;
		case kVarType_MaximumFunction:
		{

		}
			break;
		default:
		{
			Q_ASSERT(false);
		}
			break;
	}

	return m_propertiesMap;
}

void dbg_vars_impls::data_map_from_properties(var__types type, void *data, QMap<QString, QString> &data_map) const
{
	if (nullptr == data)
	{
		return;
	}

	qMapFree(data_map);

	switch (type)
	{
	case kVarType_MotionTemplateFramwork:
		break;
	case kVarType_Vehicle:
	{
		_var__vehicle_t *p_vehicle_t = (_var__vehicle_t *)data;

		data_map.insert("vehicle_id_", QString::number(p_vehicle_t->vehicle_id_));
		data_map.insert("vehicle_name_", QString(p_vehicle_t->vehicle_name_));
		data_map.insert("vehicle_type_", QString::number(p_vehicle_t->vehicle_type_));
		data_map.insert("chassis_type_", QString::number(p_vehicle_t->chassis_type_));
		data_map.insert("max_speed_", QString::number(p_vehicle_t->max_speed_));
		data_map.insert("creep_speed_", QString::number(p_vehicle_t->creep_speed_));
		data_map.insert("max_acc_", QString::number(p_vehicle_t->max_acc_));
		data_map.insert("max_dec_", QString::number(p_vehicle_t->max_dec_));
		data_map.insert("max_w_", QString::number(p_vehicle_t->max_w_));
		data_map.insert("creep_w_", QString::number(p_vehicle_t->creep_w_));
		data_map.insert("max_acc_w_", QString::number(p_vehicle_t->max_acc_w_));
		data_map.insert("max_dec_w_", QString::number(p_vehicle_t->max_dec_w_));
		data_map.insert("steer_angle_error_tolerance_", QString::number(p_vehicle_t->steer_angle_error_tolerance_));

		data_map.insert("manual_velocity_.x_", QString::number(p_vehicle_t->manual_velocity_.x_));
		data_map.insert("manual_velocity_.y_", QString::number(p_vehicle_t->manual_velocity_.y_));
		data_map.insert("manual_velocity_.theta_", QString::number(p_vehicle_t->manual_velocity_.theta_));
		data_map.insert("manual_velocity_.angle_", QString::number(p_vehicle_t->manual_velocity_.angle_));
		data_map.insert("manual_velocity_.w_", QString::number(p_vehicle_t->manual_velocity_.w_));
		data_map.insert("manual_velocity_.distance_", QString::number(p_vehicle_t->manual_velocity_.distance_));

		data_map.insert("stop_normal_", QString::number(p_vehicle_t->stop_normal_));
		data_map.insert("stop_emergency_", QString::number(p_vehicle_t->stop_emergency_));
		data_map.insert("fault_stop_", QString::number(p_vehicle_t->fault_stop_));
		data_map.insert("slow_down_", QString::number(p_vehicle_t->slow_down_));
		data_map.insert("enable_", QString::number(p_vehicle_t->enable_));
		data_map.insert("control_mode_", QString::number(p_vehicle_t->control_mode_));

		data_map.insert("i.command_velocity_.x_", QString::number(p_vehicle_t->i.command_velocity_.x_));
		data_map.insert("i.command_velocity_.y_", QString::number(p_vehicle_t->i.command_velocity_.y_));
		data_map.insert("i.command_velocity_.theta_", QString::number(p_vehicle_t->i.command_velocity_.theta_));
		data_map.insert("i.command_velocity_.angle_", QString::number(p_vehicle_t->i.command_velocity_.angle_));
		data_map.insert("i.command_velocity_.w_", QString::number(p_vehicle_t->i.command_velocity_.w_));
		data_map.insert("i.command_velocity_.distance_", QString::number(p_vehicle_t->i.command_velocity_.distance_));

		data_map.insert("i.ref_velocity_.x_", QString::number(p_vehicle_t->i.ref_velocity_.x_));
		data_map.insert("i.ref_velocity_.y_", QString::number(p_vehicle_t->i.ref_velocity_.y_));
		data_map.insert("i.ref_velocity_.theta_", QString::number(p_vehicle_t->i.ref_velocity_.theta_));
		data_map.insert("i.ref_velocity_.angle_", QString::number(p_vehicle_t->i.ref_velocity_.angle_));
		data_map.insert("i.ref_velocity_.w_", QString::number(p_vehicle_t->i.ref_velocity_.w_));
		data_map.insert("i.ref_velocity_.distance_", QString::number(p_vehicle_t->i.ref_velocity_.distance_));

		data_map.insert("i.actual_command_velocity_.x_", QString::number(p_vehicle_t->i.actual_command_velocity_.x_));
		data_map.insert("i.actual_command_velocity_.y_", QString::number(p_vehicle_t->i.actual_command_velocity_.y_));
		data_map.insert("i.actual_command_velocity_.theta_", QString::number(p_vehicle_t->i.actual_command_velocity_.theta_));
		data_map.insert("i.actual_command_velocity_.angle_", QString::number(p_vehicle_t->i.actual_command_velocity_.angle_));
		data_map.insert("i.actual_command_velocity_.w_", QString::number(p_vehicle_t->i.actual_command_velocity_.w_));
		data_map.insert("i.actual_command_velocity_.distance_", QString::number(p_vehicle_t->i.actual_command_velocity_.distance_));

		data_map.insert("i.actual_velocity_.x_", QString::number(p_vehicle_t->i.actual_velocity_.x_));
		data_map.insert("i.actual_velocity_.y_", QString::number(p_vehicle_t->i.actual_velocity_.y_));
		data_map.insert("i.actual_velocity_.theta_", QString::number(p_vehicle_t->i.actual_velocity_.theta_));
		data_map.insert("i.actual_velocity_.angle_", QString::number(p_vehicle_t->i.actual_velocity_.angle_));
		data_map.insert("i.actual_velocity_.w_", QString::number(p_vehicle_t->i.actual_velocity_.w_));
		data_map.insert("i.actual_velocity_.distance_", QString::number(p_vehicle_t->i.actual_velocity_.distance_));

		data_map.insert("i.odo_meter_.x_", QString::number(p_vehicle_t->i.odo_meter_.x_));
		data_map.insert("i.odo_meter_.y_", QString::number(p_vehicle_t->i.odo_meter_.y_));
		data_map.insert("i.odo_meter_.theta_", QString::number(p_vehicle_t->i.odo_meter_.theta_));
		data_map.insert("i.odo_meter_.angle_", QString::number(p_vehicle_t->i.odo_meter_.angle_));
		data_map.insert("i.odo_meter_.w_", QString::number(p_vehicle_t->i.odo_meter_.w_));
		data_map.insert("i.odo_meter_.distance_", QString::number(p_vehicle_t->i.odo_meter_.distance_));

		data_map.insert("i.time_stamp_", QString::number(p_vehicle_t->i.time_stamp_));
		data_map.insert("i.is_moving_", QString::number(p_vehicle_t->i.is_moving_));
		data_map.insert("i.normal_stopped_", QString::number(p_vehicle_t->i.normal_stopped_));
		data_map.insert("i.emergency_stopped_", QString::number(p_vehicle_t->i.emergency_stopped_));
		data_map.insert("i.slow_done_", QString::number(p_vehicle_t->i.slow_done_));
		data_map.insert("i.total_odo_meter_", QString::number(p_vehicle_t->i.total_odo_meter_));

		data_map.insert("u_", QString::number(p_vehicle_t->u_));
	}
		break;
	case kVarType_Navigation:
	{
		var__navigation_t *p_navigation_t = (var__navigation_t *)data;

		data_map.insert("max_speed_", QString::number(p_navigation_t->max_speed_));
		data_map.insert("creep_speed_", QString::number(p_navigation_t->creep_speed_));
		data_map.insert("max_w_", QString::number(p_navigation_t->max_w_));
		data_map.insert("creep_w_", QString::number(p_navigation_t->creep_w_));
		data_map.insert("slow_down_speed_", QString::number(p_navigation_t->slow_down_speed_));
		data_map.insert("acc_", QString::number(p_navigation_t->acc_));
		data_map.insert("dec_", QString::number(p_navigation_t->dec_));
		data_map.insert("dec_estop_", QString::number(p_navigation_t->dec_estop_));
		data_map.insert("acc_w_", QString::number(p_navigation_t->acc_w_));
		data_map.insert("dec_w_", QString::number(p_navigation_t->dec_w_));
		data_map.insert("creep_distance_", QString::number(p_navigation_t->creep_distance_));
		data_map.insert("creep_theta_", QString::number(p_navigation_t->creep_theta_));
		data_map.insert("upl_mapping_angle_tolerance_", QString::number(p_navigation_t->upl_mapping_angle_tolerance_));
		data_map.insert("upl_mapping_dist_tolerance_", QString::number(p_navigation_t->upl_mapping_dist_tolerance_));
		data_map.insert("upl_mapping_angle_weight_", QString::number(p_navigation_t->upl_mapping_angle_weight_));
		data_map.insert("upl_mapping_dist_weight_", QString::number(p_navigation_t->upl_mapping_dist_weight_));
		data_map.insert("tracking_error_tolerance_dist_", QString::number(p_navigation_t->tracking_error_tolerance_dist_));
		data_map.insert("tracking_error_tolerance_angle_", QString::number(p_navigation_t->tracking_error_tolerance_angle_));
		data_map.insert("aim_dist_", QString::number(p_navigation_t->aim_dist_));
		data_map.insert("predict_time_", QString::number(p_navigation_t->predict_time_));
		data_map.insert("is_traj_whole_", QString::number(p_navigation_t->is_traj_whole_));
		data_map.insert("aim_angle_p_", QString::number(p_navigation_t->aim_angle_p_));
		data_map.insert("aim_angle_i_", QString::number(p_navigation_t->aim_angle_i_));
		data_map.insert("aim_angle_d_", QString::number(p_navigation_t->aim_angle_d_));
		data_map.insert("stop_tolerance_", QString::number(p_navigation_t->stop_tolerance_));
		data_map.insert("stop_tolerance_angle_", QString::number(p_navigation_t->stop_tolerance_angle_));
		data_map.insert("stop_point_trim_", QString::number(p_navigation_t->stop_point_trim_));
		data_map.insert("aim_ey_p_", QString::number(p_navigation_t->aim_ey_p_));
		data_map.insert("aim_ey_i_", QString::number(p_navigation_t->aim_ey_i_));
		data_map.insert("aim_ey_d_", QString::number(p_navigation_t->aim_ey_d_));

		data_map.insert("track_status_.command_", QString::number(p_navigation_t->track_status_.command_));
		data_map.insert("track_status_.middle_", QString::number(p_navigation_t->track_status_.middle_));
		data_map.insert("track_status_.response_", QString::number(p_navigation_t->track_status_.response_));

		data_map.insert("user_task_id_", QString::number(p_navigation_t->user_task_id_));
		data_map.insert("ato_task_id_", QString::number(p_navigation_t->ato_task_id_));

		data_map.insert("dest_upl_.edge_id_", QString::number(p_navigation_t->dest_upl_.edge_id_));
		data_map.insert("dest_upl_.percentage_", QString::number(p_navigation_t->dest_upl_.percentage_));
		data_map.insert("dest_upl_.wop_id_", QString::number(p_navigation_t->dest_upl_.wop_id_));
		data_map.insert("dest_upl_.angle_", QString::number(p_navigation_t->dest_upl_.angle_));

		data_map.insert("dest_pos_.x_", QString::number(p_navigation_t->dest_pos_.x_));
		data_map.insert("dest_pos_.y_", QString::number(p_navigation_t->dest_pos_.y_));
		data_map.insert("dest_pos_.theta_", QString::number(p_navigation_t->dest_pos_.theta_));
		data_map.insert("dest_pos_.angle_", QString::number(p_navigation_t->dest_pos_.angle_));
		data_map.insert("dest_pos_.w_", QString::number(p_navigation_t->dest_pos_.w_));
		data_map.insert("dest_pos_.distance_", QString::number(p_navigation_t->dest_pos_.distance_));

		data_map.insert("traj_ref_.count_", QString::number(p_navigation_t->traj_ref_.count_));
		data_map.insert("traj_ref_.data64_", QString::number(p_navigation_t->traj_ref_.data64_));

		data_map.insert("pos_.x_", QString::number(p_navigation_t->pos_.x_));
		data_map.insert("pos_.y_", QString::number(p_navigation_t->pos_.y_));
		data_map.insert("pos_.theta_", QString::number(p_navigation_t->pos_.theta_));
		data_map.insert("pos_.angle_", QString::number(p_navigation_t->pos_.angle_));
		data_map.insert("pos_.w_", QString::number(p_navigation_t->pos_.w_));
		data_map.insert("pos_.distance_", QString::number(p_navigation_t->pos_.distance_));

		data_map.insert("pos_time_stamp_", QString::number(p_navigation_t->pos_time_stamp_));
		data_map.insert("pos_status", QString::number(p_navigation_t->pos_status));
		data_map.insert("pos_confidence_", QString::number(p_navigation_t->pos_confidence_));

		data_map.insert("i.traj_ref_index_curr_", QString::number(p_navigation_t->i.traj_ref_index_curr_));

		data_map.insert("i.upl_.edge_id_", QString::number(p_navigation_t->i.upl_.edge_id_));
		data_map.insert("i.upl_.percentage_", QString::number(p_navigation_t->i.upl_.percentage_));
		data_map.insert("i.upl_.wop_id_", QString::number(p_navigation_t->i.upl_.wop_id_));
		data_map.insert("i.upl_.angle_", QString::number(p_navigation_t->i.upl_.angle_));

		data_map.insert("i.tracking_error_", QString::number(p_navigation_t->i.tracking_error_));

		data_map.insert("i.base_point_.x_", QString::number(p_navigation_t->i.base_point_.x_));
		data_map.insert("i.base_point_.y_", QString::number(p_navigation_t->i.base_point_.y_));
		data_map.insert("i.base_point_.theta_", QString::number(p_navigation_t->i.base_point_.theta_));
		data_map.insert("i.base_point_.angle_", QString::number(p_navigation_t->i.base_point_.angle_));
		data_map.insert("i.base_point_.w_", QString::number(p_navigation_t->i.base_point_.w_));
		data_map.insert("i.base_point_.distance_", QString::number(p_navigation_t->i.base_point_.distance_));

		data_map.insert("i.aim_point_.x_", QString::number(p_navigation_t->i.aim_point_.x_));
		data_map.insert("i.aim_point_.y_", QString::number(p_navigation_t->i.aim_point_.y_));
		data_map.insert("i.aim_point_.theta_", QString::number(p_navigation_t->i.aim_point_.theta_));
		data_map.insert("i.aim_point_.angle_", QString::number(p_navigation_t->i.aim_point_.angle_));
		data_map.insert("i.aim_point_.w_", QString::number(p_navigation_t->i.aim_point_.w_));
		data_map.insert("i.aim_point_.distance_", QString::number(p_navigation_t->i.aim_point_.distance_));

		data_map.insert("i.aim_heading_error_", QString::number(p_navigation_t->i.aim_heading_error_));

		data_map.insert("i.predict_point_.x_", QString::number(p_navigation_t->i.predict_point_.x_));
		data_map.insert("i.predict_point_.y_", QString::number(p_navigation_t->i.predict_point_.y_));
		data_map.insert("i.predict_point_.theta_", QString::number(p_navigation_t->i.predict_point_.theta_));
		data_map.insert("i.predict_point_.angle_", QString::number(p_navigation_t->i.predict_point_.angle_));
		data_map.insert("i.predict_point_.w_", QString::number(p_navigation_t->i.predict_point_.w_));
		data_map.insert("i.predict_point_.distance_", QString::number(p_navigation_t->i.predict_point_.distance_));

		data_map.insert("i.predict_point_curvature_", QString::number(p_navigation_t->i.predict_point_curvature_));
		data_map.insert("i.on_last_segment_", QString::number(p_navigation_t->i.on_last_segment_));
		data_map.insert("i.dist_to_partition_", QString::number(p_navigation_t->i.dist_to_partition_));
		data_map.insert("i.dist_to_dest_", QString::number(p_navigation_t->i.dist_to_dest_));

		data_map.insert("i.current_edge_wop_properties_.wop_id_", QString::number(p_navigation_t->i.current_edge_wop_properties_.wop_id_));
		data_map.insert("i.current_edge_wop_properties_.enabled_", QString::number(p_navigation_t->i.current_edge_wop_properties_.enabled_));

		QString wop_properties_str;

		for (int i = 0; i < 9; ++i)
		{
			wop_properties_str = "i.current_edge_wop_properties_.wop_properties_[" + QString::number(i) + "]";
			data_map.insert(wop_properties_str, QString::number(p_navigation_t->i.current_edge_wop_properties_.wop_properties_[i]));
		}

		data_map.insert("u_", QString::number(p_navigation_t->u_));
		data_map.insert("runtime_limiting_velocity_", QString::number(p_navigation_t->runtime_limiting_velocity_));
	}
		break;
	case kVarType_Operation:
	{
		var__operation_t *p_operation_t = (var__operation_t *)data;

		data_map.insert("status_.command_", QString::number(p_operation_t->status_.command_));
		data_map.insert("status_.middle_", QString::number(p_operation_t->status_.middle_));
		data_map.insert("status_.response_", QString::number(p_operation_t->status_.response_));

		data_map.insert("user_task_id_", QString::number(p_operation_t->user_task_id_));
		data_map.insert("ato_task_id_", QString::number(p_operation_t->ato_task_id_));
		data_map.insert("code_", QString::number(p_operation_t->code_));
		data_map.insert("param0_", QString::number(p_operation_t->param0_));
		data_map.insert("param1_", QString::number(p_operation_t->param1_));
		data_map.insert("param2_", QString::number(p_operation_t->param2_));
		data_map.insert("param3_", QString::number(p_operation_t->param3_));
		data_map.insert("param4_", QString::number(p_operation_t->param4_));
		data_map.insert("param5_", QString::number(p_operation_t->param5_));
		data_map.insert("param6_", QString::number(p_operation_t->param6_));
		data_map.insert("param7_", QString::number(p_operation_t->param7_));
		data_map.insert("param8_", QString::number(p_operation_t->param8_));
		data_map.insert("param9_", QString::number(p_operation_t->param9_));

		data_map.insert("i.param10_", QString::number(p_operation_t->i.param10_));
		data_map.insert("i.param11_", QString::number(p_operation_t->i.param11_));
		data_map.insert("i.param12_", QString::number(p_operation_t->i.param12_));
		data_map.insert("i.param13_", QString::number(p_operation_t->i.param13_));
		data_map.insert("i.param14_", QString::number(p_operation_t->i.param14_));
		data_map.insert("i.param15_", QString::number(p_operation_t->i.param15_));
		data_map.insert("i.param16_", QString::number(p_operation_t->i.param16_));
		data_map.insert("i.param17_", QString::number(p_operation_t->i.param17_));
		data_map.insert("i.param18_", QString::number(p_operation_t->i.param18_));
		data_map.insert("i.param19_", QString::number(p_operation_t->i.param19_));

		data_map.insert("u_", QString::number(p_operation_t->u_));
	}
		break;
	case kVarType_UserDefined:
	{
		var__usrdef_buffer_t *p_usrdef_buffer_t = (var__usrdef_buffer_t *)data;
		data_map.insert("usrbuf_", QString(p_usrdef_buffer_t->usrbuf_));
	}
		break;
	case kVarType_SWheel:
	{
		var__swheel_t *p_swheel_t = (var__swheel_t *)data;

		data_map.insert("min_angle_", QString::number(p_swheel_t->min_angle_));
		data_map.insert("max_angle_", QString::number(p_swheel_t->max_angle_));
		data_map.insert("zero_angle_", QString::number(p_swheel_t->zero_angle_));
		data_map.insert("zero_angle_enc_", QString::number(p_swheel_t->zero_angle_enc_));
		data_map.insert("max_w_", QString::number(p_swheel_t->max_w_));
		data_map.insert("control_mode_", QString::number(p_swheel_t->control_mode_));
		data_map.insert("scale_control_", QString::number(p_swheel_t->scale_control_));
		data_map.insert("scale_feedback_", QString::number(p_swheel_t->scale_feedback_));
		data_map.insert("control_cp_", QString::number(p_swheel_t->control_cp_));
		data_map.insert("control_ci_", QString::number(p_swheel_t->control_ci_));
		data_map.insert("control_cd_", QString::number(p_swheel_t->control_cd_));
		data_map.insert("enabled_", QString::number(p_swheel_t->enabled_));
		data_map.insert("actual_angle_", QString::number(p_swheel_t->actual_angle_));
		data_map.insert("actual_angle_enc_", QString::number(p_swheel_t->actual_angle_enc_));
		data_map.insert("time_stamp_", QString::number(p_swheel_t->time_stamp_));
		data_map.insert("error_code_", QString::number(p_swheel_t->error_code_));

		data_map.insert("i.enable_", QString::number(p_swheel_t->i.enable_));
		data_map.insert("i.command_angle_", QString::number(p_swheel_t->i.command_angle_));
		data_map.insert("i.command_angle_enc_", QString::number(p_swheel_t->i.command_angle_enc_));
		data_map.insert("i.command_rate_", QString::number(p_swheel_t->i.command_rate_));
		data_map.insert("i.command_rate_enc_", QString::number(p_swheel_t->i.command_rate_enc_));

		data_map.insert("u_", QString::number(p_swheel_t->u_));
	}
		break;
	case kVarType_DWheel:
	{
		var__dwheel_t *p_dwheel_t = (var__dwheel_t *)data;

		data_map.insert("max_speed_", QString::number(p_dwheel_t->max_speed_));
		data_map.insert("max_acc_", QString::number(p_dwheel_t->max_acc_));
		data_map.insert("max_dec_", QString::number(p_dwheel_t->max_dec_));
		data_map.insert("control_mode_", QString::number(p_dwheel_t->control_mode_));
		data_map.insert("scale_control_", QString::number(p_dwheel_t->scale_control_));
		data_map.insert("scale_feedback_", QString::number(p_dwheel_t->scale_feedback_));
		data_map.insert("roll_weight_", QString::number(p_dwheel_t->roll_weight_));
		data_map.insert("slide_weight_", QString::number(p_dwheel_t->slide_weight_));
		data_map.insert("enabled_", QString::number(p_dwheel_t->enabled_));
		data_map.insert("actual_velocity_", QString::number(p_dwheel_t->actual_velocity_));
		data_map.insert("actual_velocity_enc_", QString::number(p_dwheel_t->actual_velocity_enc_));
		data_map.insert("actual_position_", QString::number(p_dwheel_t->actual_position_));
		data_map.insert("actual_position_enc_", QString::number(p_dwheel_t->actual_position_enc_));
		data_map.insert("actual_current_", QString::number(p_dwheel_t->actual_current_));
		data_map.insert("time_stamp_", QString::number(p_dwheel_t->time_stamp_));
		data_map.insert("error_code_", QString::number(p_dwheel_t->error_code_));

		data_map.insert("i.enable_", QString::number(p_dwheel_t->i.enable_));
		data_map.insert("i.command_velocity_", QString::number(p_dwheel_t->i.command_velocity_));
		data_map.insert("i.command_velocity_enc_", QString::number(p_dwheel_t->i.command_velocity_enc_));
		data_map.insert("i.command_position_", QString::number(p_dwheel_t->i.command_position_));
		data_map.insert("i.command_position_enc_", QString::number(p_dwheel_t->i.command_position_enc_));
		data_map.insert("i.command_current_", QString::number(p_dwheel_t->i.command_current_));

		data_map.insert("u_", QString::number(p_dwheel_t->u_));
	}
		break;
	case kVarType_SDDExtra:
	{
		var__sdd_extra_t *p_sdd_extra_t = (var__sdd_extra_t *)data;
		data_map.insert("gauge_", QString::number(p_sdd_extra_t->gauge_));
	}
		break;
	case kVarType_DriveUnit:
		break;
	case kVarType_Map:
		break;
	case kVarType_OperationTarget:
	{
		var__operation_parameter_t *p_operation_parameter_t = (var__operation_parameter_t *)data;

		data_map.insert("ull00_", QString::number(p_operation_parameter_t->ull00_));
		data_map.insert("ull01_", QString::number(p_operation_parameter_t->ull01_));
		data_map.insert("ull02_", QString::number(p_operation_parameter_t->ull02_));
		data_map.insert("ull03_", QString::number(p_operation_parameter_t->ull03_));
		data_map.insert("ull04_", QString::number(p_operation_parameter_t->ull04_));
		data_map.insert("ull05_", QString::number(p_operation_parameter_t->ull05_));
		data_map.insert("ull06_", QString::number(p_operation_parameter_t->ull06_));
		data_map.insert("ull07_", QString::number(p_operation_parameter_t->ull07_));
		data_map.insert("ull08_", QString::number(p_operation_parameter_t->ull08_));
		data_map.insert("ull09_", QString::number(p_operation_parameter_t->ull09_));
		data_map.insert("ull10_", QString::number(p_operation_parameter_t->ull10_));
		data_map.insert("ull11_", QString::number(p_operation_parameter_t->ull11_));
		data_map.insert("ull12_", QString::number(p_operation_parameter_t->ull12_));
		data_map.insert("ull13_", QString::number(p_operation_parameter_t->ull13_));
		data_map.insert("ull14_", QString::number(p_operation_parameter_t->ull14_));
		data_map.insert("ull15_", QString::number(p_operation_parameter_t->ull15_));
		data_map.insert("ull16_", QString::number(p_operation_parameter_t->ull16_));
		data_map.insert("ull17_", QString::number(p_operation_parameter_t->ull17_));
		data_map.insert("ull18_", QString::number(p_operation_parameter_t->ull18_));
		data_map.insert("ull19_", QString::number(p_operation_parameter_t->ull19_));
	}
		break;
	case kVarType_CanBus:
		break;
	case kVarType_Copley:
		break;
	case kVarType_Elmo:
	{
		var__elmo_t *p_elmo_t = (var__elmo_t *)data;

		data_map.insert("candev_head_.canbus_", QString::number(p_elmo_t->candev_head_.canbus_));
		data_map.insert("candev_head_.canport_", QString::number(p_elmo_t->candev_head_.canport_));
		data_map.insert("candev_head_.cannode_", QString::number(p_elmo_t->candev_head_.cannode_));
		data_map.insert("candev_head_.latency_", QString::number(p_elmo_t->candev_head_.latency_));
		data_map.insert("candev_head_.merge_", QString::number(p_elmo_t->candev_head_.merge_));
		data_map.insert("candev_head_.self_rw64_", QString::number(p_elmo_t->candev_head_.self_rw64_));
		data_map.insert("candev_head_.pdocnt_", QString::number(p_elmo_t->candev_head_.pdocnt_));

		data_map.insert("profile_speed_", QString::number(p_elmo_t->profile_speed_));
		data_map.insert("profile_acc_", QString::number(p_elmo_t->profile_acc_));
		data_map.insert("profile_dec_", QString::number(p_elmo_t->profile_dec_));

		data_map.insert("status_.command_", QString::number(p_elmo_t->status_.command_));
		data_map.insert("status_.middle_", QString::number(p_elmo_t->status_.middle_));
		data_map.insert("status_.response_", QString::number(p_elmo_t->status_.response_));

		data_map.insert("control_mode_", QString::number(p_elmo_t->control_mode_));
		data_map.insert("command_velocity_", QString::number(p_elmo_t->command_velocity_));
		data_map.insert("command_position_", QString::number(p_elmo_t->command_position_));
		data_map.insert("command_current_", QString::number(p_elmo_t->command_current_));
		data_map.insert("enable_", QString::number(p_elmo_t->enable_));

		data_map.insert("i.node_state_", QString::number(p_elmo_t->i.node_state_));
		data_map.insert("i.error_code_", QString::number(p_elmo_t->i.error_code_));
		data_map.insert("i.time_stamp_", QString::number(p_elmo_t->i.time_stamp_));
		data_map.insert("i.actual_velocity_", QString::number(p_elmo_t->i.actual_velocity_));
		data_map.insert("i.actual_position_", QString::number(p_elmo_t->i.actual_position_));
		data_map.insert("i.actual_current_", QString::number(p_elmo_t->i.actual_current_));
		data_map.insert("i.enabled_", QString::number(p_elmo_t->i.enabled_));
		data_map.insert("i.di_", QString::number(p_elmo_t->i.di_));
		data_map.insert("i.do_", QString::number(p_elmo_t->i.do_));

		data_map.insert("u_", QString::number(p_elmo_t->u_));
	}
		break;
	case kVarType_DIO:
	{
		var__dio_t *p_dio_t = (var__dio_t *)data;

		data_map.insert("candev_head_.canbus_", QString::number(p_dio_t->candev_head_.canbus_));
		data_map.insert("candev_head_.canport_", QString::number(p_dio_t->candev_head_.canport_));
		data_map.insert("candev_head_.cannode_", QString::number(p_dio_t->candev_head_.cannode_));
		data_map.insert("candev_head_.latency_", QString::number(p_dio_t->candev_head_.latency_));
		data_map.insert("candev_head_.merge_", QString::number(p_dio_t->candev_head_.merge_));
		data_map.insert("candev_head_.self_rw64_", QString::number(p_dio_t->candev_head_.self_rw64_));
		data_map.insert("candev_head_.pdocnt_", QString::number(p_dio_t->candev_head_.pdocnt_));

		data_map.insert("di_channel_num_", QString::number(p_dio_t->di_channel_num_));
		data_map.insert("do_channel_num_", QString::number(p_dio_t->do_channel_num_));

		data_map.insert("status_.command_", QString::number(p_dio_t->status_.command_));
		data_map.insert("status_.middle_", QString::number(p_dio_t->status_.middle_));
		data_map.insert("status_.response_", QString::number(p_dio_t->status_.response_));

		data_map.insert("do_", QString::number(p_dio_t->do_));

		QString ao_key, ao_data_key;

		for (int i = 0; i < MAXIMUM_DIO_BLOCK_COUNT; ++i)
		{
			ao_key = "ao_[" + QString::number(i) + "].start_address_";
			data_map.insert(ao_key, QString::number(p_dio_t->ao_[i].start_address_));
			ao_key = "ao_[" + QString::number(i) + "].effective_count_of_index_";
			data_map.insert(ao_key, QString::number(p_dio_t->ao_[i].effective_count_of_index_));
			ao_key = "ao_[" + QString::number(i) + "].internel_type_";
			data_map.insert(ao_key, QString::number(p_dio_t->ao_[i].internel_type_));

			for (int j = 0; j < DIO_BLOCK_DATACB; ++j)
			{
				ao_key = "ao_[" + QString::number(i) + "].data_[" + QString::number(j) + "]";
				data_map.insert(ao_key, QString::number(p_dio_t->ao_[i].data_[j]));
			}
		}

		data_map.insert("i.bus_state_", QString::number(p_dio_t->i.bus_state_));
		data_map.insert("i.error_code_", QString::number(p_dio_t->i.error_code_));
		data_map.insert("i.time_stamp_", QString::number(p_dio_t->i.time_stamp_));
		data_map.insert("i.enabled_", QString::number(p_dio_t->i.enabled_));
		data_map.insert("i.di_", QString::number(p_dio_t->i.di_));

		QString ai_key, ai_data_key;

		for (int i = 0; i < MAXIMUM_DIO_BLOCK_COUNT; ++i)
		{
			ai_key = "i.ai_[" + QString::number(i) + "].start_address_";
			data_map.insert(ai_key, QString::number(p_dio_t->i.ai_[i].start_address_));
			ai_key = "i.ai_[" + QString::number(i) + "].effective_count_of_index_";
			data_map.insert(ai_key, QString::number(p_dio_t->i.ai_[i].effective_count_of_index_));
			ai_key = "i.ai_[" + QString::number(i) + "].internel_type_";
			data_map.insert(ai_key, QString::number(p_dio_t->i.ai_[i].internel_type_));

			for (int j = 0; j < DIO_BLOCK_DATACB; ++j)
			{
				ai_key = "i.ai_[" + QString::number(i) + "].data_[" + QString::number(j) + "]";
				data_map.insert(ai_key, QString::number(p_dio_t->i.ai_[i].data_[j]));
			}
		}

		data_map.insert("i.do2_", QString::number(p_dio_t->i.do2_));

		QString ao2_key, ao2_data_key;

		for (int i = 0; i < MAXIMUM_DIO_BLOCK_COUNT; ++i)
		{
			ao2_key = "i.ao2_[" + QString::number(i) + "].start_address_";
			data_map.insert(ao2_key, QString::number(p_dio_t->i.ao2_[i].start_address_));
			ao2_key = "i.ao2_[" + QString::number(i) + "].effective_count_of_index_";
			data_map.insert(ao2_key, QString::number(p_dio_t->i.ao2_[i].effective_count_of_index_));
			ao2_key = "i.ao2_[" + QString::number(i) + "].internel_type_";
			data_map.insert(ao2_key, QString::number(p_dio_t->i.ao2_[i].internel_type_));

			for (int j = 0; j < DIO_BLOCK_DATACB; ++j)
			{
				ao2_key = "i.ao2_[" + QString::number(i) + "].data_[" + QString::number(j) + "]";
				data_map.insert(ao2_key, QString::number(p_dio_t->i.ao2_[i].data_[j]));
			}
		}

		data_map.insert("u_", QString::number(p_dio_t->u_));
	}
		break;
	case kVarType_Moons:
	{
		var__moos_t *p_moos_t = (var__moos_t *)data;

		data_map.insert("candev_head_.canbus_", QString::number(p_moos_t->candev_head_.canbus_));
		data_map.insert("candev_head_.canport_", QString::number(p_moos_t->candev_head_.canport_));
		data_map.insert("candev_head_.cannode_", QString::number(p_moos_t->candev_head_.cannode_));
		data_map.insert("candev_head_.latency_", QString::number(p_moos_t->candev_head_.latency_));
		data_map.insert("candev_head_.merge_", QString::number(p_moos_t->candev_head_.merge_));
		data_map.insert("candev_head_.self_rw64_", QString::number(p_moos_t->candev_head_.self_rw64_));
		data_map.insert("candev_head_.pdocnt_", QString::number(p_moos_t->candev_head_.pdocnt_));

		data_map.insert("profile_speed_", QString::number(p_moos_t->profile_speed_));
		data_map.insert("profile_acc_", QString::number(p_moos_t->profile_acc_));
		data_map.insert("profile_dec_", QString::number(p_moos_t->profile_dec_));

		data_map.insert("status_.command_", QString::number(p_moos_t->status_.command_));
		data_map.insert("status_.middle_", QString::number(p_moos_t->status_.middle_));
		data_map.insert("status_.response_", QString::number(p_moos_t->status_.response_));

		data_map.insert("control_mode_", QString::number(p_moos_t->control_mode_));
		data_map.insert("command_velocity_", QString::number(p_moos_t->command_velocity_));
		data_map.insert("command_position_", QString::number(p_moos_t->command_position_));
		data_map.insert("command_current_", QString::number(p_moos_t->command_current_));
		data_map.insert("enable_", QString::number(p_moos_t->enable_));

		data_map.insert("i.state_", QString::number(p_moos_t->i.state_));
		data_map.insert("i.error_code_", QString::number(p_moos_t->i.error_code_));
		data_map.insert("i.time_stamp_", QString::number(p_moos_t->i.time_stamp_));
		data_map.insert("i.actual_velocity_", QString::number(p_moos_t->i.actual_velocity_));
		data_map.insert("i.actual_position_", QString::number(p_moos_t->i.actual_position_));
		data_map.insert("i.actual_current_", QString::number(p_moos_t->i.actual_current_));
		data_map.insert("i.enabled_", QString::number(p_moos_t->i.enabled_));
		data_map.insert("i.di_", QString::number(p_moos_t->i.di_));
		data_map.insert("i.do_", QString::number(p_moos_t->i.do_));

		data_map.insert("u_", QString::number(p_moos_t->u_));
	}
		break;
	case kVarType_AngleEncoder:
	{
		var__angle_encoder_t *p_angle_encoder_t = (var__angle_encoder_t *)data;
		data_map.insert("candev_head_.canbus_", QString::number(p_angle_encoder_t->candev_head_.canbus_));
		data_map.insert("candev_head_.canport_", QString::number(p_angle_encoder_t->candev_head_.canport_));
		data_map.insert("candev_head_.cannode_", QString::number(p_angle_encoder_t->candev_head_.cannode_));
		data_map.insert("candev_head_.latency_", QString::number(p_angle_encoder_t->candev_head_.latency_));
		data_map.insert("candev_head_.merge_", QString::number(p_angle_encoder_t->candev_head_.merge_));
		data_map.insert("candev_head_.self_rw64_", QString::number(p_angle_encoder_t->candev_head_.self_rw64_));
		data_map.insert("candev_head_.pdocnt_", QString::number(p_angle_encoder_t->candev_head_.pdocnt_));

		data_map.insert("encoder_type_", QString::number(p_angle_encoder_t->encoder_type_));

		data_map.insert("i.state_", QString::number(p_angle_encoder_t->i.state_));
		data_map.insert("i.actual_angle_", QString::number(p_angle_encoder_t->i.actual_angle_));
		data_map.insert("i.error_code_", QString::number(p_angle_encoder_t->i.error_code_));
		data_map.insert("i.time_stamp_", QString::number(p_angle_encoder_t->i.time_stamp_));

		data_map.insert("u_", QString::number(p_angle_encoder_t->u_));
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
		var__safety_t *p_safety_t = (var__safety_t *)data;

		data_map.insert("enable_", QString::number(p_safety_t->enable_));
		data_map.insert("enabled_", QString::number(p_safety_t->enabled_));
		data_map.insert("cur_bank_id_", QString::number(p_safety_t->cur_bank_id_));
		data_map.insert("cur_bank_level", QString::number(p_safety_t->cur_bank_level));
		data_map.insert("safety_reslut_", QString::number(p_safety_t->safety_reslut_));

		data_map.insert("sensor_trrigered_.__src_dev_id", QString::number(p_safety_t->sensor_trrigered_.__src_dev_id));
		data_map.insert("sensor_trrigered_.__src_dev_type", QString::number(p_safety_t->sensor_trrigered_.__src_dev_type));

		QString data_src_channel_key;

		for (int i = 0; i < kSafetyProtectIdex_MaxNum; ++i)
		{
			data_src_channel_key = "sensor_trrigered_.__data_src_channel[" + QString::number(i) + "]";
			data_map.insert(data_src_channel_key, QString::number(p_safety_t->sensor_trrigered_.__data_src_channel[i]));
		}

		data_map.insert("sensor_trrigered_.__dev_data_type", QString::number(p_safety_t->sensor_trrigered_.__dev_data_type));
		data_map.insert("sensor_trrigered_.__reslut", QString::number(p_safety_t->sensor_trrigered_.__reslut));
		data_map.insert("sensor_trrigered_.sensor_name", QString(p_safety_t->sensor_trrigered_.sensor_name));

		data_map.insert("sensor_trrigered_.__di_mask", QString::number(p_safety_t->sensor_trrigered_.__di_mask));
		data_map.insert("sensor_trrigered_.__ai_thres", QString::number(p_safety_t->sensor_trrigered_.__ai_thres));
		data_map.insert("sensor_trrigered_.__di_cur", QString::number(p_safety_t->sensor_trrigered_.__di_cur));
		data_map.insert("sensor_trrigered_.__ai_cur", QString::number(p_safety_t->sensor_trrigered_.__ai_cur));
	}
		break;
	case kVarType_MaximumFunction:
		break;
	default:
		break;
	}
}
