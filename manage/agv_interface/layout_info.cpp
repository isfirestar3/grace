#include "layout_info.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "os_util.hpp"
#include <iostream>

#include "log.h"

#ifndef PI
#define PI  3.141592653
#endif

layout_info::layout_info()
{
}


layout_info::~layout_info()
{
}

int searchCB(const unsigned int iPercent, const void *pUsr)
{
	std::cout << "map loading " << iPercent << "%" << std::endl;
	return 0;
}

int layout_info::load_file(std::string layout_name /*= "/layout.xml"*/, std::string dock_name /*= "/docks.xml"*/, int search_type)
{
	//path
	std::string file_path = nsp::os::get_module_directory<char>();
	std::string layout_path = file_path + layout_name;

	__userid = PathSearch_GetUserId();
	//PathSearch_SetType(__userid,PathSearch_DIJKSTRA);
    PathSearch_SetType(__userid, (PathSearch_Type)search_type);
    loinfo("layoutinfo") << "PathSearch_SetType " << search_type;

	int r = PathSearch_OpenMap(layout_path.c_str(), &layout_data_);
	if (r < 0)
	{
		loerror("layoutinfo") << "Open Layout " << layout_path << " Failed...";
		return -1;
	}
	loinfo("layoutinfo") << "Open Layout " << layout_path << " Success...";

	r = PathSearch_LoadData(__userid, &layout_data_);
	if (r < 0)
	{
		loerror("layoutinfo") << "Load Layout " << layout_path << " Failed...";
		return -1;
	}
	loinfo("layoutinfo") << "Load Layout " << layout_path << " Success...";

    if (search_type == 0)
    {
        std::string folyd_file_path = file_path + "/etc/layout.dat";
        r = PathSearch_StartFolyd(__userid, folyd_file_path.c_str(), searchCB, nullptr);
        if (r < 0)
        {
            loerror("layoutinfo") << "Start Folyd " << folyd_file_path << " Failed...";
            return -1;
        }
        loinfo("layoutinfo") << "Start Folyd " << folyd_file_path << " Success...";
    }


	//dock

	std::string dock_path = file_path + dock_name;
	rapidxml::xml_document<> doc;
	rapidxml::file<char> *file = nullptr;
	try {
		file = new rapidxml::file<char>(dock_path.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node("docks");
		if (root)
		{
			for (rapidxml::xml_node<>* dock = root->first_node(); dock; dock = dock->next_sibling())
			{
				DocksInfo anchorInfo;
				rapidxml::xml_node<>* id = dock->first_node("id");
				if (id)
				{
					anchorInfo.id = atoi(id->value());
				}
				rapidxml::xml_node<>* pos = dock->first_node("pos");
				if (pos)
				{
					rapidxml::xml_attribute<char>* attr = pos->first_attribute("x");
					if (attr)
					{
						anchorInfo.pos.x_ = atof(attr->value());
					}
					attr = pos->first_attribute("y");
					if (attr)
					{
						anchorInfo.pos.y_ = atof(attr->value());
					}
					attr = pos->first_attribute("angle");
					if (attr)
					{
						anchorInfo.pos.angle_ = atof(attr->value());
					}
				}
				rapidxml::xml_node<>* upl = dock->first_node("upl");
				if (upl)
				{
					rapidxml::xml_attribute<char>* attr = upl->first_attribute("edge_id");
					if (attr)
					{
						anchorInfo.upl.edge_id_ = atoi(attr->value());
					}
					attr = upl->first_attribute("percentage");
					if (attr)
					{
						anchorInfo.upl.percentage_ = atof(attr->value());
					}
					attr = upl->first_attribute("aoa");
					if (attr)
					{
						//anchorInfo.upl.angle_ = atof(attr->value()) * PI / 180.0;
						anchorInfo.upl.angle_ = atof(attr->value());
					}
				}

				rapidxml::xml_node<>* opts = dock->first_node("opts");
				if (opts)
				{
					for (rapidxml::xml_node<>* opt = opts->first_node(); opt; opt = opt->next_sibling())
					{
						if (!opt)
						{
							continue;
						}
						op_info optInfo;
						rapidxml::xml_node<>* id = opt->first_node("id");
						if (id)
						{
							optInfo.id = atoi(id->value());
						}
						rapidxml::xml_node<>* type = opt->first_node("type");
						if (type)
						{
							optInfo.type = atoi(type->value());
						}
						//int i = 0;
						//for (rapidxml::xml_node<>* param = opt->first_node("p0"); param; param = param->next_sibling())
						//{
						//	if (!param) continue;
						//	double val = atof(param->value());
						//	optInfo.params[i++] = val;
						//}

						anchorInfo.opt[optInfo.id] = optInfo;
					}
				}

				map_dock_[anchorInfo.id] = anchorInfo;
			}

		}
		delete file;
	}
	catch (...) {
		delete file;
		loerror("layoutinfo") << "Load Dock " << dock_path << " Failed...";
		return -1;
	}

	loinfo("layoutinfo") << "Load Dock " << dock_path << " Success...";
	return 0;
}

PathSearch_LayoutData layout_info::get_layout_data()
{
	return layout_data_;
}

int layout_info::get_dest_by_dockid(int dock_id, upl_t& destUpl, position_t& destPos)
{
	std::map<int, DocksInfo>::iterator itr = map_dock_.find(dock_id);
	if (itr == map_dock_.end())
	{
		//std::cout << "err dock id = " << dock_id << std::endl;
		return -1;
	}
	destUpl = itr->second.upl;
	destPos = itr->second.pos;

	return 0;
}

int layout_info::path_search(upl_t curUpl, upl_t destUpl, std::vector<trail_t>& pathUpl, double& dis)
{
	//搜路
	PathSearch_Upl cur_upl;
	cur_upl.edge_id = curUpl.edge_id_;
	cur_upl.fAoa = (float)curUpl.angle_;
	cur_upl.wop_id = 0;
    cur_upl.fPercent = (float)curUpl.percentage_;

	PathSearch_Upl des_upl;
	des_upl.edge_id = destUpl.edge_id_;
    des_upl.fAoa = (float)destUpl.angle_;
	des_upl.wop_id = 0;
    des_upl.fPercent = (float)destUpl.percentage_;
	PathSearch_Wop *pwop = NULL;
	int num = 0;
	float dist = 0.0;
	int ret = PathSearch_GetPath(__userid,cur_upl, des_upl, &pwop, &num, &dist);
	if (ret < 0)
	{
		assert(!pwop);
		loerror("layoutinfo") << "path search failed! upl:" << cur_upl.edge_id << "," << cur_upl.wop_id << "->" << des_upl.edge_id << "," << des_upl.wop_id;
		return -1;
	}
    if (!pwop)
    {
        loerror("layoutinfo") << "path search failed! PathSearch_Wop empty;";
        return -1;
    }
	dis = dist;
	loinfo("layoutinfo") << (int64_t)nsp::os::gettid() << " path search result: num = " << num << " dis=" << dis;

	pathUpl.clear();
	//std::vector<trail_t> pathUpl;
	for (int i = 0; i < num; i++)
	{
		trail_t path;
		path.edge_id_ = pwop[i].edge_id;
		path.wop_id_ = pwop[i].wop_id;
		loinfo("layoutinfo") << "edge id:" << path.edge_id_ << " wop_id" << path.wop_id_;
		pathUpl.push_back(path);
	}

	dis = dist;
    
    PathSearch_ReleasePath(__userid,pwop);
	return 0;
}

int layout_info::get_opt_type(int dock_id, int opt_id,int &type)
{
	type = -1;
	std::map<int, DocksInfo>::iterator itr = map_dock_.find(dock_id);
	if (itr == map_dock_.end())
	{
		//std::cout << "err dock id = " << dock_id << std::endl;
		return -1;
	}
	
	std::map<int, op_info>::iterator itr_opt = itr->second.opt.find(opt_id);
	if (itr_opt == itr->second.opt.end())
	{
		return -1;
	}

	type = itr_opt->second.type;

	return 0;
}
