#ifndef __GRACE_ROBOT_AGVSHELL_TASK_H__
#define __GRACE_ROBOT_AGVSHELL_TASK_H__

#include "endpoint.h"
#include "icom/nisdef.h"
#include <vector>
#include <stdint.h>
#include <string>

/***************************************base_task********************************************/
class base_task {
public:
	HTCPLINK link_;
public:
	base_task(){};
	virtual ~base_task(){};
	virtual void on_task() = 0;
};
/***************************************²éÑ¯MºËĞÄÌø×´Ì¬********************************************/
class query_keepalive_status_task : public base_task {
	nsp::tcpip::endpoint vcu_endpoint_;
	std::string vcu_keepalive_status_;//vcu ·µ»Ø×´Ì¬
	
public:
	query_keepalive_status_task(HTCPLINK link);
	~query_keepalive_status_task();
	void on_task() override final;
	
};

/***************************************ÉèÖÃMºËĞÄÌø×´Ì¬********************************************/
class set_keepalive_status_task : public base_task {
	int status_;//ÉèÖÃ vcu ×´Ì¬
	nsp::tcpip::endpoint vcu_endpoint_;
	
public:
	set_keepalive_status_task(HTCPLINK link, int status);
	~set_keepalive_status_task();
	void on_task() override final;
	
};

/***************************************ÉèÖÃMºËĞÄÌø×´Ì¬********************************************/
class deal_process_cmd_task : public base_task {
	int command_;
	int process_id_all_;
	std::vector<std::string > vec_cmd_param_;
	
public:
	deal_process_cmd_task(HTCPLINK link, int cmd, int process_id);
	~deal_process_cmd_task();
	void on_task() override final;
	void add_cmd_param(std::string& param);
	
};


#endif /* __GRACE_ROBOT_AGVSHELL_TASK_H__ */
