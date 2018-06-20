#pragma once

static const char* status_table[] = {
	"kStatusDescribe_Unknown",
	"kStatusDescribe_Idle",
	"kStatusDescribe_PrepareFinish",
	"kStatusDescribe_PendingFunction",
	// 通常状态
	"kStatusDescribe_Running",
	"kStatusDescribe_Hang",
	"kStatusDescribe_Alertable",
	// 命令
	"kStatusDescribe_Startup",
	"kStatusDescribe_Cancel",
	"kStatusDescribe_Pause",
	"kStatusDescribe_Resume",
	// 最终状态， 当 response_ 处于最
	"kStatusDescribe_FinalFunction",
	"kStatusDescribe_Completed",
	"kStatusDescribe_Terminated",
	"kStatusDescribe_Error"
};

static const char* elmo_control_mode[] = {
	"kDriverControlMode_Unknown",
	"kDriverControlMode_SpeedMode",
	"kDriverControlMode_PositionMode",
	"kDriverControlMode_CurrentMode"
};

static const char* vehicle_control_mode[] = {
	"kVehicleControlMode_Navigation",
	"kVehicleControlMode_Manual",
	"kVehicleControlMode_Calibrated"		// 微调模式
};

static const char* angle_encoder_type[] {
	"kAngleEncoderType_ABS",		// 绝对角度
	"kAngleEncoderType_REL"			// 相对角度
};

static const char* dwheel_ctrlmod[] {
	"kDWheelCtrlMod_Speed",								// 速度模式
	"kDWheelCtrlMod_Dist",									// 电流模式
	"kDWheelCtrlMod_Position"								// 位置模式
};

static const char* swheel_ctrlmod[] {
	"kSWheelCtrlMod_Angle",								// 目标角度
	"kSWheelCtrlMod_DiscreteRate",							// 离散变化率
	"kSWheelCtrlMod_ContinueRate"							// 连续变化率
};