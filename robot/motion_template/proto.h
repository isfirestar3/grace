#if !defined NSP_PROTO_HEAD
#define NSP_PROTO_HEAD

#include "pkthrd.h"
#include "vartypes.h"
#include "hash.h"
#include "proto_typedef.h"
#include "nis.h"

#include <stdint.h>

#if defined PROTO_SESSION_KEY_LENGTH
#undef PROTO_SESSION_KEY_LENGTH
#endif

#define PROTO_SESSION_KEY_LENGTH  (32)

#pragma pack(push ,1)

typedef nsp__packet_head_t nsp__discover_robot_t;

typedef struct {
    nsp__packet_head_t head_;
    uint32_t ipv4_;
    uint16_t port_;
    uint32_t robot_id_;
    uint32_t robot_name_length_;
    char robot_name_[VAR_NAME_LENGTH];
    uint32_t robot_type_;
} nsp__discover_robot_ack_t;

typedef struct {
    nsp__packet_head_t head_;
    int size_;
    unsigned char key_[PROTO_SESSION_KEY_LENGTH];
} nsp__pre_login_t;

typedef struct {
    nsp__packet_head_t head_;
    enum nsp__controlor_type_t cct_;
    int acquire_access_;
    int original_buffer_size_;
    unsigned char original_buffer_[PROTO_SESSION_KEY_LENGTH];
    int encrypted_md5_size_;
    unsigned char encrypted_md5_[16];
} nsp__login_robot_t;
typedef nsp__packet_head_t nsp__login_robot_ack_t;

/* 通用读写相关协议 */
typedef struct {
    int var_id_;
    int var_type_;
    int offset_;
    int length_;
    char data_[0];
} nsp__common_protocol_item_t;

typedef struct {
    nsp__packet_head_t head_;
    int count_;
    char items[0]; /* nsp__common_protocol_item_t */
} nsp__common_protocol_t;

typedef nsp__packet_head_t nsp__common_protocol_write_ack_t;

typedef struct {
    nsp__packet_head_t head_;
    uint64_t task_id_;
    int vartype_;
    int status_;
    int usrcb_;
    char usrdat_[0];
} nsp__common_report_t;
typedef nsp__packet_head_t nsp__common_report_ack_t;

/*订阅取消主动读协议 */
typedef struct {
    int var_id_;
	int event_cycle_;
	int vartype_;
    int offset_;
    int length_;
    char data_[0];
} nsp__initiactive_protocol_item_t;

typedef struct {
    nsp__packet_head_t head_;
	uint64_t ack_time_stamp_;
    int count_;
    char items[0]; /* nsp__initiactive_protocol_item_t */
} nsp__initiactive_protocol_t;

typedef nsp__packet_head_t nsp__initiactive_protocol_cancle;

/* 导航任务及其相关操作协议 */
typedef struct {
    nsp__packet_head_t head_;
    uint64_t set_task_id_;
    upl_t dest_upl_;
    position_t dest_pos_;
    int is_traj_whole_;
    int cnt_trajs_;
    char traj_[0]; /* trail_t */
} nsp__allocate_navigation_task_t;

typedef struct {
    nsp__packet_head_t head_;
    uint64_t task_id_;
} nsp__control_navigation_task_t;

typedef struct {
    nsp__packet_head_t head_;
    uint64_t task_id_;
    int status_;
} nsp__control_navigation_task_ack_t;

typedef nsp__control_navigation_task_ack_t nsp__allocate_navigation_task_ack_t;
typedef nsp__control_navigation_task_t nsp__cancel_navigation_task_t;
typedef nsp__control_navigation_task_t nsp__pause_navigation_task_t;
typedef nsp__control_navigation_task_t nsp__resume_navigation_task_t;
typedef nsp__control_navigation_task_t nsp__query_navigation_task_status_t;

typedef struct {
    nsp__packet_head_t head_;
    uint64_t task_id_;
    int traj_index_current_;
    int track_eno_;
    enum prc_t track_prc_;
    position_t pos_;
    uint64_t pos_time_stamp_;
    double pos_confidence_;
    upl_t upl_;
    int vehcile_control_mode_;
} nsp__query_navigation_task_status_ack_t;

typedef nsp__packet_head_t nsp__query_navigation_task_traj_t;

typedef struct {
    nsp__packet_head_t head_;
     uint64_t task_id_;
    int cnt_trajs_;
    char traj_[0];
} nsp__query_navigation_task_traj_ack_t;

typedef struct {
    nsp__packet_head_t head_;
     uint64_t task_id_;
     int begin_index_;       /* 按起始索引追加导航边, 允许使用 -1, 如果 begin_index_ 为-1, 则在当前导航序列最后追加导航边 */
    int is_traj_whole_;
    int cnt_trajs_;
    char traj_[0];
} nsp__additional_navigation_trajectory_t;
typedef nsp__packet_head_t nsp__additional_navigation_trajectory_ack_t;

/* 操作参数相关协议 */
typedef nsp__packet_head_t nsp__query_operation_parameters_t, nsp__query_operation_parameters_ack_t;
typedef struct {
    nsp__packet_head_t head_;
    enum status_describe_t checking_low_;
    enum status_describe_t checking_high_;
    enum status_describe_t target_;
    uint32_t code_;
    int cnt_pars_;
    uint64_t pars[10];
    int mask_;
} nsp__set_operation_t;
typedef nsp__packet_head_t nsp__set_operation_ack_t;

/*  辅助调试信息相关协议 */
typedef struct {
    nsp__packet_head_t head_;
    uint32_t length_;
    char logstr_[0];
} nsp__report_dbglog_t; /* 报告日志信息  */

typedef nsp__packet_head_t nsp__query_varlist_t;
typedef struct {
    int id_;
    uint32_t/*enum var__types*/ type_;
} nsp__var_report_item_t;

typedef struct {
    nsp__packet_head_t head_;
    int item_count_;
    nsp__var_report_item_t items[0];
} nsp__query_varlist_ack_t;

typedef nsp__packet_head_t nsp__dbg_clear_fault_t;
typedef nsp__packet_head_t nsp__dbg_clear_fault_ack_t;

typedef nsp__packet_head_t nsp__get_mtver_t;
typedef struct {
    nsp__packet_head_t head_;
    uint16_t major;
    uint16_t sub;
    uint16_t extended;
    int namlen;
    char uname[128];
    int vcunamlen;
    char vcu[32];
} nsp__get_mtver_ack_t;

enum nsp__common_condition_checking_method {
    kConditionCheck_And = 0,
    kConditionCheck_Or = 1,
};

struct nsp__common_compare_write {
    nsp__packet_head_t head_;
    enum nsp__common_condition_checking_method method_; /* 与或判定条件 */
    int var_id_;
};

struct nsp__canio_msg {
    nsp__packet_head_t head_;
    uint32_t id;
    uint32_t var_type_;
    uint32_t command_;
    uint32_t middle_;
    uint32_t response_;
};

struct nsp__report_status_msg {
    nsp__packet_head_t head_;
    uint64_t task_id_;
    uint32_t vartype_;
    uint32_t status_;
    uint32_t usrcb_;
    char usrdat_[0];
};

typedef nsp__packet_head_t nsp__report_status_msg_ack;

/* protocols for operation task */
typedef struct {
    nsp__packet_head_t head_;
    uint64_t task_id_;
    int optcode_;
    uint64_t params_[10];
} nsp__allocate_operation_task_t;

typedef struct {
    nsp__packet_head_t head_;
    uint64_t task_id_;
}nsp__allocate_operation_task_ack_t;

typedef struct {
    nsp__packet_head_t head_;
    uint64_t task_id_;
} nsp__control_operation_task_t;
typedef nsp__control_operation_task_t nsp__cancel_operation_task_t;
typedef nsp__control_operation_task_t nsp__pause_operation_task_t;
typedef nsp__control_operation_task_t nsp__resume_operation_task_t;

typedef struct {
    nsp__packet_head_t head_;
    uint64_t task_id_;
    int optcode_;
}nsp__control_operation_task_ack_t;

typedef struct {
	nsp__packet_head_t head_;
	char address_[16];
	uint16_t port_;
}nsp__keepalive_udp_t;
typedef nsp__keepalive_udp_t nsp__keepalive_udp_ack_t;

typedef struct {
    nsp__packet_head_t head_;
    uint8_t blob_[128];
}nsp__localization_cfgwrite_t;
typedef nsp__localization_cfgwrite_t nsp__localization_cfgread_ack_t;
typedef nsp__packet_head_t nsp__localization_cfgwrite_ack_t;

typedef struct {
    nsp__packet_head_t head_;
    int unit_id;
}nsp__wheels_of_driveunit;

typedef struct {
    nsp__packet_head_t head_;
    int unit_id;
    int count_;
    int ids_[0];
}nsp__wheels_of_driveunit_ack;

#pragma pack(pop)

extern int nsp__on_tcp_recvdata(HTCPLINK link, const char *data, int cb);
extern int nsp__on_udp_recvdata(HTCPLINK link, char *ip, uint16_t port, const char *data, int cb);

#endif