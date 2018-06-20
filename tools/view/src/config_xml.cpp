#include "config_xml.h"

config_load::config_load()
{

}

config_load::~config_load()
{

}

void config_load::get_struct_offset(std::map<std::string, offsetinfo>& map_elmo_var, std::map<std::string, offsetinfo>& map_dwheel_var,
	std::map<std::string, offsetinfo>& map_swheel_var, std::map<std::string, offsetinfo>& map_moos_var, 
	std::map<std::string, offsetinfo>& map_angle_encoder_var)
{
	offsetinfo off_info;
	off_info.len = msizeof(var__elmo_t, status_.command_);
	off_info.offset = offsetof_data(var__elmo_t, status_.command_);
	map_elmo_var.insert(std::make_pair("command_", off_info));
	off_info.len = msizeof(var__elmo_t, status_.middle_);
	off_info.offset = offsetof_data(var__elmo_t, status_.middle_);
	map_elmo_var.insert(std::make_pair("middle_", off_info));
	off_info.len = msizeof(var__elmo_t, status_.response_);
	off_info.offset = offsetof_data(var__elmo_t, status_.response_);
	map_elmo_var.insert(std::make_pair("response_", off_info));
	off_info.len = msizeof(var__elmo_t, i.error_code_);
	off_info.offset = offsetof_data(var__elmo_t, i.error_code_);
	map_elmo_var.insert(std::make_pair("error_code_", off_info));
	off_info.len = msizeof(var__elmo_t, i.time_stamp_);
	off_info.offset = offsetof_data(var__elmo_t, i.time_stamp_);
	map_elmo_var.insert(std::make_pair("time_stamp_", off_info));
	off_info.len = msizeof(var__elmo_t, control_mode_);
	off_info.offset = offsetof_data(var__elmo_t, control_mode_);
	map_elmo_var.insert(std::make_pair("control_mode_", off_info));
	off_info.len = msizeof(var__elmo_t, command_velocity_);
	off_info.offset = offsetof_data(var__elmo_t, command_velocity_);
	map_elmo_var.insert(std::make_pair("command_velocity_", off_info));
	off_info.len = msizeof(var__elmo_t, command_position_);
	off_info.offset = offsetof_data(var__elmo_t, command_position_);
	map_elmo_var.insert(std::make_pair("command_position_", off_info));
	off_info.len = msizeof(var__elmo_t, command_current_);
	off_info.offset = offsetof_data(var__elmo_t, command_current_);
	map_elmo_var.insert(std::make_pair("command_current_", off_info));
	off_info.len = msizeof(var__elmo_t, i.node_state_);
	off_info.offset = offsetof_data(var__elmo_t, i.node_state_);
	map_elmo_var.insert(std::make_pair("node_state_", off_info));
	off_info.len = msizeof(var__elmo_t, i.actual_velocity_);
	off_info.offset = offsetof_data(var__elmo_t, i.actual_velocity_);
	map_elmo_var.insert(std::make_pair("actual_velocity_", off_info));
	off_info.len = msizeof(var__elmo_t, i.actual_position_);
	off_info.offset = offsetof_data(var__elmo_t, i.actual_position_);
	map_elmo_var.insert(std::make_pair("actual_position_", off_info));
	off_info.len = msizeof(var__elmo_t, i.actual_current_);
	off_info.offset = offsetof_data(var__elmo_t, i.actual_current_);
	map_elmo_var.insert(std::make_pair("actual_current_", off_info));
	off_info.len = msizeof(var__elmo_t, enable_);
	off_info.offset = offsetof_data(var__elmo_t, enable_);
	map_elmo_var.insert(std::make_pair("enable_", off_info));
	off_info.len = msizeof(var__elmo_t, i.enabled_);
	off_info.offset = offsetof_data(var__elmo_t, i.enabled_);
	map_elmo_var.insert(std::make_pair("enabled_", off_info));
	off_info.len = msizeof(var__elmo_t, i.di_);
	off_info.offset = offsetof_data(var__elmo_t, i.di_);
	map_elmo_var.insert(std::make_pair("di_", off_info));
	off_info.len = msizeof(var__elmo_t, i.do_);
	off_info.offset = offsetof_data(var__elmo_t, i.do_);
	map_elmo_var.insert(std::make_pair("do_", off_info));

	off_info.len = msizeof(_var__dwheel_t, i.enable_);
	off_info.offset = offsetof_data(_var__dwheel_t, i.enable_);
	map_dwheel_var.insert(std::make_pair("enable_", off_info));
	off_info.len = msizeof(_var__dwheel_t, i.command_velocity_);
	off_info.offset = offsetof_data(_var__dwheel_t, i.command_velocity_);
	map_dwheel_var.insert(std::make_pair("command_velocity_", off_info));
	off_info.len = msizeof(_var__dwheel_t, i.command_velocity_enc_);
	off_info.offset = offsetof_data(_var__dwheel_t, i.command_velocity_enc_);
	map_dwheel_var.insert(std::make_pair("command_velocity_enc_", off_info));
	off_info.len = msizeof(_var__dwheel_t, i.command_position_);
	off_info.offset = offsetof_data(_var__dwheel_t, i.command_position_);
	map_dwheel_var.insert(std::make_pair("command_position_", off_info));
	off_info.len = msizeof(_var__dwheel_t, i.command_position_enc_);
	off_info.offset = offsetof_data(_var__dwheel_t, i.command_position_enc_);
	map_dwheel_var.insert(std::make_pair("command_position_enc_", off_info));
	off_info.len = msizeof(_var__dwheel_t, i.command_current_);
	off_info.offset = offsetof_data(_var__dwheel_t, i.command_current_);
	map_dwheel_var.insert(std::make_pair("command_current_", off_info));
	off_info.len = msizeof(_var__dwheel_t, enabled_);
	off_info.offset = offsetof_data(_var__dwheel_t, enabled_);
	map_dwheel_var.insert(std::make_pair("enabled_", off_info));
	off_info.len = msizeof(_var__dwheel_t, actual_velocity_);
	off_info.offset = offsetof_data(_var__dwheel_t, actual_velocity_);
	map_dwheel_var.insert(std::make_pair("actual_velocity_", off_info));
	off_info.len = msizeof(_var__dwheel_t, actual_velocity_enc_);
	off_info.offset = offsetof_data(_var__dwheel_t, actual_velocity_enc_);
	map_dwheel_var.insert(std::make_pair("actual_velocity_enc_", off_info));
	off_info.len = msizeof(_var__dwheel_t, actual_position_);
	off_info.offset = offsetof_data(_var__dwheel_t, actual_position_);
	map_dwheel_var.insert(std::make_pair("actual_position_", off_info));
	off_info.len = msizeof(_var__dwheel_t, actual_position_enc_);
	off_info.offset = offsetof_data(_var__dwheel_t, actual_position_enc_);
	map_dwheel_var.insert(std::make_pair("actual_position_enc_", off_info));
	off_info.len = msizeof(_var__dwheel_t, actual_current_);
	off_info.offset = offsetof_data(_var__dwheel_t, actual_current_);
	map_dwheel_var.insert(std::make_pair("actual_current_", off_info));
	off_info.len = msizeof(_var__dwheel_t, time_stamp_);
	off_info.offset = offsetof_data(_var__dwheel_t, time_stamp_);
	map_dwheel_var.insert(std::make_pair("time_stamp_", off_info));
	off_info.len = msizeof(_var__dwheel_t, error_code_);
	off_info.offset = offsetof_data(_var__dwheel_t, error_code_);
	map_dwheel_var.insert(std::make_pair("error_code_", off_info));

	off_info.len = msizeof(var__swheel_t, enabled_);
	off_info.offset = offsetof_data(var__swheel_t, enabled_);
	map_swheel_var.insert(std::make_pair("enabled_", off_info));
	off_info.len = msizeof(var__swheel_t, i.enable_);
	off_info.offset = offsetof_data(var__swheel_t, i.enable_);
	map_swheel_var.insert(std::make_pair("enable_", off_info));
	off_info.len = msizeof(var__swheel_t, i.command_angle_);
	off_info.offset = offsetof_data(var__swheel_t, i.command_angle_);
	map_swheel_var.insert(std::make_pair("command_angle_", off_info));
	off_info.len = msizeof(var__swheel_t, i.command_angle_enc_);
	off_info.offset = offsetof_data(var__swheel_t, i.command_angle_enc_);
	map_swheel_var.insert(std::make_pair("command_angle_enc_", off_info));
	off_info.len = msizeof(var__swheel_t, i.command_rate_);
	off_info.offset = offsetof_data(var__swheel_t, i.command_rate_);
	map_swheel_var.insert(std::make_pair("command_rate_", off_info));
	off_info.len = msizeof(var__swheel_t, i.command_rate_enc_);
	off_info.offset = offsetof_data(var__swheel_t, i.command_rate_enc_);
	map_swheel_var.insert(std::make_pair("command_rate_enc_", off_info));
	off_info.len = msizeof(var__swheel_t, actual_angle_);
	off_info.offset = offsetof_data(var__swheel_t, actual_angle_);
	map_swheel_var.insert(std::make_pair("actual_angle_", off_info));
	off_info.len = msizeof(var__swheel_t, actual_angle_enc_);
	off_info.offset = offsetof_data(var__swheel_t, actual_angle_enc_);
	map_swheel_var.insert(std::make_pair("actual_angle_enc_", off_info));
	off_info.len = msizeof(var__swheel_t, time_stamp_);
	off_info.offset = offsetof_data(var__swheel_t, time_stamp_);
	map_swheel_var.insert(std::make_pair("time_stamp_", off_info));
	off_info.len = msizeof(var__swheel_t, error_code_);
	off_info.offset = offsetof_data(var__swheel_t, error_code_);
	map_swheel_var.insert(std::make_pair("error_code_", off_info));

	off_info.len = msizeof(var__moos_t, status_.command_);
	off_info.offset = offsetof_data(var__moos_t, status_.command_);
	map_moos_var.insert(std::make_pair("command_", off_info));
	off_info.len = msizeof(var__moos_t, status_.middle_);
	off_info.offset = offsetof_data(var__moos_t, status_.middle_);
	map_moos_var.insert(std::make_pair("middle_", off_info));
	off_info.len = msizeof(var__moos_t, status_.response_);
	off_info.offset = offsetof_data(var__moos_t, status_.response_);
	map_moos_var.insert(std::make_pair("response_", off_info));
	off_info.len = msizeof(var__moos_t, i.state_);
	off_info.offset = offsetof_data(var__moos_t, i.state_);
	map_moos_var.insert(std::make_pair("state_", off_info));
	off_info.len = msizeof(var__moos_t, i.error_code_);
	off_info.offset = offsetof_data(var__moos_t, i.error_code_);
	map_moos_var.insert(std::make_pair("error_code_", off_info));
	off_info.len = msizeof(var__moos_t, i.time_stamp_);
	off_info.offset = offsetof_data(var__moos_t, i.time_stamp_);
	map_moos_var.insert(std::make_pair("time_stamp_", off_info));
	off_info.len = msizeof(var__moos_t, control_mode_);
	off_info.offset = offsetof_data(var__moos_t, control_mode_);
	map_moos_var.insert(std::make_pair("control_mode_", off_info));
	off_info.len = msizeof(var__moos_t, command_velocity_);
	off_info.offset = offsetof_data(var__moos_t, command_velocity_);
	map_moos_var.insert(std::make_pair("command_velocity_", off_info));
	off_info.len = msizeof(var__moos_t, command_position_);
	off_info.offset = offsetof_data(var__moos_t, command_position_);
	map_moos_var.insert(std::make_pair("command_position_", off_info));
	off_info.len = msizeof(var__moos_t, command_current_);
	off_info.offset = offsetof_data(var__moos_t, command_current_);
	map_moos_var.insert(std::make_pair("command_current_", off_info));
	off_info.len = msizeof(var__moos_t, i.actual_velocity_);
	off_info.offset = offsetof_data(var__moos_t, i.actual_velocity_);
	map_moos_var.insert(std::make_pair("actual_velocity_", off_info));
	off_info.len = msizeof(var__moos_t, i.actual_position_);
	off_info.offset = offsetof_data(var__moos_t, i.actual_position_);
	map_moos_var.insert(std::make_pair("actual_position_", off_info));
	off_info.len = msizeof(var__moos_t, i.actual_current_);
	off_info.offset = offsetof_data(var__moos_t, i.actual_current_);
	map_moos_var.insert(std::make_pair("actual_current_", off_info));
	off_info.len = msizeof(var__moos_t, enable_);
	off_info.offset = offsetof_data(var__moos_t, enable_);
	map_moos_var.insert(std::make_pair("enable_", off_info));
	off_info.len = msizeof(var__moos_t, i.enabled_);
	off_info.offset = offsetof_data(var__moos_t, i.enabled_);
	map_moos_var.insert(std::make_pair("enabled_", off_info));
	off_info.len = msizeof(var__moos_t, i.di_);
	off_info.offset = offsetof_data(var__moos_t, i.di_);
	map_moos_var.insert(std::make_pair("di_", off_info));
	off_info.len = msizeof(var__moos_t, i.do_);
	off_info.offset = offsetof_data(var__moos_t, i.do_);
	map_moos_var.insert(std::make_pair("do_", off_info));


	off_info.len = msizeof(var__angle_encoder_t, i.actual_angle_);
	off_info.offset = offsetof_data(var__angle_encoder_t, i.actual_angle_);
	map_angle_encoder_var.insert(std::make_pair("actual_angle_", off_info));
	off_info.len = msizeof(var__angle_encoder_t, i.error_code_);
	off_info.offset = offsetof_data(var__angle_encoder_t, i.error_code_);
	map_angle_encoder_var.insert(std::make_pair("error_code_", off_info));
	off_info.len = msizeof(var__angle_encoder_t, i.state_);
	off_info.offset = offsetof_data(var__angle_encoder_t, i.state_);
	map_angle_encoder_var.insert(std::make_pair("state_", off_info));
	off_info.len = msizeof(var__angle_encoder_t, i.time_stamp_);
	off_info.offset = offsetof_data(var__angle_encoder_t, i.time_stamp_);
	map_angle_encoder_var.insert(std::make_pair("time_stamp_", off_info));
}

void config_load::get_navigation_struct(std::vector<std::string>& vct_str)
{
	vct_str.push_back("max_speed");
	vct_str.push_back("creep_speed");
	vct_str.push_back("max_w");
	vct_str.push_back("creep_w");
	vct_str.push_back("slow_down_speed");
	vct_str.push_back("acc");
	vct_str.push_back("dec");
	vct_str.push_back("dec_estop");
	vct_str.push_back("acc_w");
	vct_str.push_back("dec_w");
	vct_str.push_back("creep_distance");
	vct_str.push_back("creep_theta");
	vct_str.push_back("upl_mapping_angle_tolerance");
	vct_str.push_back("upl_mapping_dist_tolerance");
	vct_str.push_back("upl_mapping_angle_weight");
	vct_str.push_back("upl_mapping_dist_weight");
	vct_str.push_back("tracking_error_tolerance_dist");
	vct_str.push_back("tracking_error_tolerance_angle");
	vct_str.push_back("aim_dist");
	vct_str.push_back("predict_time");
	vct_str.push_back("aim_angle_p");
	vct_str.push_back("aim_angle_i");
	vct_str.push_back("aim_angle_d");
	vct_str.push_back("stop_tolerance");
	vct_str.push_back("stop_tolerance_angle");
	vct_str.push_back("stop_point_trim");
	vct_str.push_back("aim_ey_p");
	vct_str.push_back("aim_ey_i");
	vct_str.push_back("aim_ey_d");

	vct_str.push_back("track_status: command");
	vct_str.push_back("track_status: middle");
	vct_str.push_back("track_status: response");

	vct_str.push_back("dest_upl:edge_id");
	vct_str.push_back("dest_upl:percentage");
	vct_str.push_back("dest_upl:wop_id");
	vct_str.push_back("dest_pos: x");
	vct_str.push_back("dest_pos: y");
	vct_str.push_back("dest_pos: w");
	
	vct_str.push_back("pos: x");
	vct_str.push_back("pos: y");
	vct_str.push_back("pos: angle");
	vct_str.push_back("pos_time_stamp");
	vct_str.push_back("pos_confidence");

	vct_str.push_back("traj_ref_index_curr");
	vct_str.push_back("upl:enge_id");
	vct_str.push_back("upl:percentage");
	vct_str.push_back("upl:angle");
	vct_str.push_back("upl:wop_id");
	vct_str.push_back("tracking_error");
	vct_str.push_back("base_point: x");
	vct_str.push_back("base_point: y");
	vct_str.push_back("base_point: angle");
	vct_str.push_back("aim_point_: x");
	vct_str.push_back("aim_point_: y");
	vct_str.push_back("aim_point_: angle");
	vct_str.push_back("aim_heading_error");
	vct_str.push_back("predict_point: x");
	vct_str.push_back("predict_point: y");
	vct_str.push_back("predict_point: angle");
	vct_str.push_back("predict_point_curvature");
	vct_str.push_back("on_last_segment");
	vct_str.push_back("dist_to_dest");
	vct_str.push_back("current_task_id");
}

void config_load::get_vehicle_struct(std::vector<std::string>& vct_str)
{
	vct_str.push_back("vehicle_type");
	vct_str.push_back("vehicle_id");
	vct_str.push_back("chassis_type");
	vct_str.push_back("max_speed");
	vct_str.push_back("creep_speed");
	vct_str.push_back("max_acc");
	vct_str.push_back("max_dec");
	vct_str.push_back("max_w");
	vct_str.push_back("creep_w");
	vct_str.push_back("max_acc_w");
	vct_str.push_back("max_dec_w");
	vct_str.push_back("steer_angle_error_tolerance");

	vct_str.push_back("manual_velocity_x");
	vct_str.push_back("manual_velocity_y");
	vct_str.push_back("manual_velocity_w");
	vct_str.push_back("stop_normal");
	vct_str.push_back("stop_emergency");
	
	//fault_stop
	vct_str.push_back("fault_stop");
	
	vct_str.push_back("slow_down");
	vct_str.push_back("enable");
	vct_str.push_back("control_mode");

	vct_str.push_back("command_velocity_x");
	vct_str.push_back("command_velocity_y");
	vct_str.push_back("command_velocity_w");
	vct_str.push_back("ref_velocity_x");
	vct_str.push_back("ref_velocity_y");
	vct_str.push_back("ref_velocity_w");
	
	//actual_command_velocity_
	vct_str.push_back("actual_command_velocity_x");
	vct_str.push_back("actual_command_velocity_y");
	vct_str.push_back("actual_command_velocity_w");
	
	vct_str.push_back("actual_velocity_x");
	vct_str.push_back("actual_velocity_y");
	vct_str.push_back("actual_velocity_w");
	vct_str.push_back("odo_meter_x");
	vct_str.push_back("odo_meter_y");
	vct_str.push_back("odo_meter_angle");
	vct_str.push_back("time_stamp");
	vct_str.push_back("is_moving");

	vct_str.push_back("normal_stopped");
	vct_str.push_back("emergency_stopped");
	vct_str.push_back("slow_done");
	
	//total_odo_meter_
	vct_str.push_back("total_odo_meter");
}

void config_load::get_elmo_struct(std::vector<std::string>& vct_str)
{
	vct_str.push_back("canbus");
	vct_str.push_back("canport");
	vct_str.push_back("cannode");
	vct_str.push_back("latency");
	vct_str.push_back("merge");
	vct_str.push_back("profile_speed");
	vct_str.push_back("profile_acc");
	vct_str.push_back("profile_dec");

	vct_str.push_back("command");
	vct_str.push_back("middle");
	vct_str.push_back("response");
	vct_str.push_back("control_mode");
	vct_str.push_back("command_velocity");
	vct_str.push_back("command_position");
	vct_str.push_back("command_current");
	vct_str.push_back("enable");
	vct_str.push_back("state");
	vct_str.push_back("error_code");
	vct_str.push_back("time_stamp");
	vct_str.push_back("actual_velocity");
	vct_str.push_back("actual_position");
	vct_str.push_back("actual_current");
	vct_str.push_back("enabled");
	vct_str.push_back("di");
	vct_str.push_back("do");

}

void config_load::get_moos_struct(std::vector<std::string>& vct_str)
{
	vct_str.push_back("canbus");
	vct_str.push_back("canport");
	vct_str.push_back("cannode");
	vct_str.push_back("latency");
	vct_str.push_back("merge");
	vct_str.push_back("profile_speed");
	vct_str.push_back("profile_acc");
	vct_str.push_back("profile_dec");
	vct_str.push_back("command");
	vct_str.push_back("middle");
	vct_str.push_back("response");
	vct_str.push_back("control_mode");
	vct_str.push_back("command_velocity");
	vct_str.push_back("command_position");
	vct_str.push_back("command_current");
	vct_str.push_back("enable");
	vct_str.push_back("state");
	vct_str.push_back("error_code");
	vct_str.push_back("time_stamp");
	vct_str.push_back("actual_velocity");
	vct_str.push_back("actual_position");
	vct_str.push_back("actual_current");
	vct_str.push_back("enabled");
	vct_str.push_back("di");
	vct_str.push_back("do");
}

void config_load::get_angle_encoder_struct(std::vector<std::string>& vct_str)
{
	vct_str.push_back("canbus");
	vct_str.push_back("canport");
	vct_str.push_back("cannode");
	vct_str.push_back("latency");
	vct_str.push_back("merge");
	vct_str.push_back("encoder_type");
	vct_str.push_back("state");
	vct_str.push_back("actual_angle");
	vct_str.push_back("error_code");
	vct_str.push_back("time_stamp");
}

void config_load::get_dwheel_struct(std::vector<std::string>& vct_str)
{
	vct_str.push_back("max_speed");
	vct_str.push_back("max_acc");
	vct_str.push_back("max_dec");
	vct_str.push_back("control_mode");
	vct_str.push_back("scale_control");
	vct_str.push_back("scale_feedback");
	vct_str.push_back("roll_weight");
	vct_str.push_back("slide_weight");
	vct_str.push_back("enabled");
	vct_str.push_back("actual_velocity");
	vct_str.push_back("actual_velocity_enc");
	vct_str.push_back("actual_position");
	vct_str.push_back("actual_position_enc");
	vct_str.push_back("actual_current");
	vct_str.push_back("time_stamp");
	vct_str.push_back("error_code_");
	vct_str.push_back("enable");
	vct_str.push_back("command_velocity");
	vct_str.push_back("command_velocity_enc");
	vct_str.push_back("command_position");
	vct_str.push_back("command_position_enc");
	vct_str.push_back("command_current");
}

void config_load::get_swheel_struct(std::vector<std::string>& vct_str)
{
	vct_str.push_back("min_angle");
	vct_str.push_back("max_angle");
	vct_str.push_back("zero_angle");
	vct_str.push_back("zero_angle_enc");
	vct_str.push_back("max_w");
	vct_str.push_back("control_mode");
	vct_str.push_back("scale_control");
	vct_str.push_back("scale_feedback");
	vct_str.push_back("control_cp");
	vct_str.push_back("control_ci");
	vct_str.push_back("control_cd");
	vct_str.push_back("enabled");
	vct_str.push_back("actual_angle");
	vct_str.push_back("actual_angle_enc");
	vct_str.push_back("time_stamp");
	vct_str.push_back("error_code");
	vct_str.push_back("enable");
	vct_str.push_back("command_angle");
	vct_str.push_back("command_angle_enc");
	vct_str.push_back("command_rate");
	vct_str.push_back("command_rate_enc");

}

void config_load::get_sddex_struct(std::vector<std::string>& vct_str)
{
	vct_str.push_back("gauge");
}

void config_load::get_operation_struct(std::vector<std::string>&vct_str)
{
	vct_str.push_back("command");
	vct_str.push_back("middle");
	vct_str.push_back("response");
	vct_str.push_back("code");
	vct_str.push_back("param0");
	vct_str.push_back("param1");
	vct_str.push_back("param2");
	vct_str.push_back("param3");
	vct_str.push_back("param4");
	vct_str.push_back("param5");
	vct_str.push_back("param6");
	vct_str.push_back("param7");
	vct_str.push_back("param8");
	vct_str.push_back("param9");
	vct_str.push_back("param10");
	vct_str.push_back("param11");
	vct_str.push_back("param12");
	vct_str.push_back("param13");
	vct_str.push_back("param14");
	vct_str.push_back("param15");
	vct_str.push_back("param16");
	vct_str.push_back("param17");
	vct_str.push_back("param18");
	vct_str.push_back("param19");
	vct_str.push_back("current_task_id");
}

void config_load::get_optpar_struct(std::vector<std::string>&vct_str)
{
	vct_str.push_back("ull00");
	vct_str.push_back("ull01");
	vct_str.push_back("ull02");
	vct_str.push_back("ull03");
	vct_str.push_back("ull04");
	vct_str.push_back("ull05");
	vct_str.push_back("ull06");
	vct_str.push_back("ull07");
	vct_str.push_back("ull08");
	vct_str.push_back("ull09"); 
	vct_str.push_back("ull10");
	vct_str.push_back("ull12");
	vct_str.push_back("ull13");
	vct_str.push_back("ull14");
	vct_str.push_back("ull15");
	vct_str.push_back("ull16");
	vct_str.push_back("ull17");
	vct_str.push_back("ull18");
	vct_str.push_back("ull19");
}

void config_load::get_dio_struct(std::vector<std::string>&vct_str)
{
	vct_str.push_back("canbus");
	vct_str.push_back("canport");
	vct_str.push_back("cannode");
	vct_str.push_back("latency");
	vct_str.push_back("merge");
	vct_str.push_back("command");
	vct_str.push_back("middle");
	vct_str.push_back("response");
	vct_str.push_back("do");
	vct_str.push_back("ao");
	vct_str.push_back("ao_norm");
	vct_str.push_back("ao_scale_coef");
	vct_str.push_back("ao_bias_coef");
	vct_str.push_back("bus_state");
	vct_str.push_back("error_stamp");
	vct_str.push_back("enable");
	vct_str.push_back("di");
	vct_str.push_back("ai");
	vct_str.push_back("ai_norm");
	vct_str.push_back("ai_scale_coef");
	vct_str.push_back("ai_bias_coef");
	vct_str.push_back("do2");
	vct_str.push_back("ao2");
	/*vct_str.push_back("ao_norm2");
	vct_str.push_back("ao_scale_coef2");
	vct_str.push_back("ao_bias_coef2");*/
}