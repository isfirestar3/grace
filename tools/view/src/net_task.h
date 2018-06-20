#pragma once

#include "vartypes.h"
#include "endpoint.h"
#include <QtCore/QString>
#include <QtCore/QObject>
#include <stdint.h>
#include <vector>
#include <memory>
#include <functional>
#include "mntypes.h"

class net_task
{
public:
	bool disconnect_to_host(uint32_t robot_id);
	bool connect_to_host(const QString &ip, int port, uint32_t &robot_id, std::function<void(uint32_t, void *, int)> to_notify);
	bool post_dbg_varls_request_(uint32_t robot_id, std::vector<mn::var_item> &items);
	bool post_common_read_dbg_var(uint32_t robot_id, const mn::common_title &common_vec, var__types type, bool entire, std::function<void(void *)> &data_cb);
};