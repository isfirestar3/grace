#ifndef __AGV_SHELL_PROTO_H__
#define __AGV_SHELL_PROTO_H__

#define INVALID_PACK_TYPE		((uint32_t)~0)
#define PKTTYPE_UNKNOWN			( INVALID_PACK_TYPE )

enum AgvShellNetProtoType {
	kAgvShellProto_HeartBeat = 0,
	kAgvShellProto_Cmd,
	kAgvShellProto_Status,
	kAgvShellProto_Hostname,
	kAgvShellProto_Hostname_Ack,
	kAgvShellProto_StartProcess,
	kAgvShellProto_StartProcess_Ack,
	kAgvShellProto_StopProcess,
	kAgvShellProto_StopProcess_Ack,
	kAgvShellProto_RestartProcess,
	kAgvShellProto_RestartProcess_Ack,
	kAgvShellProto_RestartOS,
	kAgvShellProto_RestartOS_Ack,
	kAgvShellProto_Frimware_Info,
	kAgvShellProto_Frimware_Info_Ack,
	kAgvShellProto_Frimware_Update,
	kAgvShellProto_Frimware_Update_Ack,
	kAgvShellProto_Frimware_Download,
	kAgvShellProto_Frimware_Download_Ack,
	kAgvShellProto_Frimware_Restart,
	kAgvShellProto_Frimware_Restart_Ack,
	kAgvShellProto_Upgrade,
	kAgvShellProto_KeepAlive,
	kAgvShellProto_Login,
	kAgvShellProto_Login_Ack,
	kAgvShellProto_Query_Keepalive_Status,
	kAgvShellProto_Query_Keepalive_Status_Ack,
	kAgvShellProto_Set_Keepalive_Status,
	kAgvShellProto_Set_Keepalive_Status_Ack,
	kAgvShellProto_Process_List,
	kAgvShellProto_Process_List_Ack,
	kAgvShellProto_ProcessCmd,
	kAgvShellProto_UpdateNTP,
	kAgvShellProto_RunScript,
	kAgvShellProto_CmdList,
	kAgvShellProto_CmdList_ACK,
	kAgvShellProto_UpdateCmdList,
	kAgvShellProto_UpdateCmdList_ACK,
	kAgvShellProto_LocalInfo,
	kAgvShellProto_LocalInfo_ACK,
	kAgvShellProto_MacNotify,
	kAgvShellProto_GetLogType,
	kAgvShellProto_GetLogType_ACK,
	kAgvShellProto_TimeTypeConditon,
	kAgvShellProto_TimeTypeConditon_ACK,
	kAgvShellProto_Schedule_ACK,
	kAgvShellProto_Logs_Cancel,
	kAgvShellProto_FileMutexStatus,
	kAgvShellProto_ModifyFileMutex,
	kAgvShellProto_ModifyFileMutex_ACK,
	kAgvShellProto_VersionInfo,
	kAgvShellProto_VersionInfo_ACK,
	kAgvShellProto_Custom_Update,
	kAgvShellProto_Frimware_Custom_Info,
	kAgvShellProto_KeepAlive_ACk,
	kAgvShellProto_ShellVersionInfo,
	kAgvShellProto_BackupFiles,
	kAgvShellProto_BackupFiles_ACK,
	kAgvShellProto_BackupDemarcate,
	kAgvShellProto_BackupDemarcate_ACK,
	kAgvShellProto_ConfigVersion
};

enum AgvShellReply {
    kAgvShellReply_Waiting = -3,
    kAgvShellReply_NotSupport = -2,
    kAgvShellReply_Failed = -1,
    kAgvShellReply_OK = 0,
};

enum AgvShellCmd {
    None = -1,
    Start = 0,
    Stop,
    ReStart,
    ReBoot,
    ShutDown,
    Max_AGVCMD_NUM,
};

enum AgvShellStatus {
    AgvShellStatus_Unknow = 0,
    AgvShellStatus_Idle,
    AgvShellStatus_Running,
};

enum AgvClientIdentify {
	AgvClientType_Unknow = 0x100,
	AgvClientType_CMP = (AgvClientType_Unknow << 1), //配置管理平台
};

enum AgvShellKeepaliveStatus {
	AgvShellKeepalive_Unknow = -1,
	AgvShellKeepalive_Start = 0,
	AgvShellKeepalive_Suspend,
};

enum AgvShellKeepaliveReply {
	AgvShellKeepalive_Successful = 0,
	AgvShellKeepalive_Failure,
};

#endif
