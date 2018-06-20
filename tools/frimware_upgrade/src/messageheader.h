#pragma  once

enum  message_type
{
	kmessage_cmd_data_forward_get_software_version,
	kmessage_cmd_data_forward_get_software_version_upload,
	kmessage_cmd_data_forward_get_hardware_type,
	kmessage_cmd_data_forward_get_hardware_type_upload,
	kmessage_cmd_data_forward_initial_rom_update,
	kmessage_cmd_data_forward_download_packet,
	kmessage_cmd_data_forward_download_packet_ex,
	kmessage_cmd_data_forward_upload_packet,
	kmessage_cmd_data_forward_reset,
	kmessage_cmd_data_forward_abort_update,
	kmessage_cmd_data_forward_read_register,
	kmessage_cmd_data_forward_write_register,
	kmessage_cmd_start_data_forward,

	//kmessage_
};

enum ui_update_area
{
	kupdatearea_type,
	kupdatearea_type_upload,
	kupdatearea_version,
	kupdatearea_version_upload,
	kupdatearea_progress_download,
	kupdatearea_progress_upload,
	kupdatearea_status,
	kupdatearea_button,
	kupdatearea_reset,
	kupdatearea_data_forward
};