#pragma once

#include "agv_base.hpp"

class agv_api : public agv_base
{
public:
	agv_api();
	~agv_api();

protected:
	virtual int ready(bool& ready);

	virtual int get_elongate_variable();

	virtual void get_elongate_variable_ack(uint32_t id, const void *data);

	virtual int goto_charge(int dock_id, int charge_on, uint64_t& task_id, std::function<void(uint64_t taskid, status_describe_t status, int err, void* user)> fn, void* user = nullptr);

	virtual int get_external_fault_describe(std::vector<std::string> & v) {
		return 0;
	}

	virtual int get_voltage(double& vol) const
	{
		return 0;
	}
};