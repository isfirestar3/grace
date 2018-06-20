/*!
 * file layout_info.h
 * date 2017/08/15 16:30
 *
 * author chenkai
 *
 * brief 
 *
 * note
*/

#ifndef _LAYOUT_INFO_H__
#define _LAYOUT_INFO_H__
#include "operation.h"
#include <string>
#include <map>
#include "navigation.h"
#include <vector>
#include "optpar.h"
#include "PathSearch.h"
class layout_info
{
public:
	struct op_info
	{
		int id;
		int type;
		var__operation_parameter_t op;
	};
	struct DocksInfo
	{
		int id;
		upl_t upl;
		position_t pos;
		std::map<int, op_info> opt;
	};
public:
	layout_info();
	~layout_info();

	int load_file(std::string layout_name = "/layout.xml",std::string dock_name = "/docks.xml",int search_type = 0);

public:
	PathSearch_LayoutData get_layout_data();

public://dock
	int get_dest_by_dockid(int dock_id, upl_t& destUpl, position_t& destPos);
	int get_opt_type(int dock_id, int opt_id, int &type);

public://path
	int path_search(upl_t curUpl, upl_t destUpl, std::vector<trail_t>& pathUpl, double& dis);
private:

	std::map<int, DocksInfo> map_dock_;
	int __userid = -1;

private:
	PathSearch_LayoutData layout_data_;
};

#endif


