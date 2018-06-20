#include "agv_single.h"
#include "toolkit.h"


agv_single::agv_single()
{
}

agv_single::~agv_single()
{
}

agv_detail* agv_single::create_detail(const std::map<std::string, std::string > map_attr,int vhid) const
{
	agv_detail * detail = nullptr;
	try{
		detail = new agv_detail;
		if (detail){
			detail->vhid = vhid;
			detail->count_of_attrs = map_attr.size();
			if (!create_attr(&(detail->attrs), map_attr)){
				delete detail;
				return nullptr;
			}
			return detail;
		}
	}
	catch (...){
		return nullptr;
	}
	return detail;
}

bool agv_single::create_attr(agv_attribute **attrs, const std::map<std::string, std::string > &map_attr) const {
	try{
		*attrs = new agv_attribute[map_attr.size()];
		int index = 0;
		for (auto iter : map_attr){
			nsp::toolkit::posix_strncpy<char>((*attrs)[index].name, sizeof((*attrs)[index].name), iter.first.c_str(), sizeof((*attrs)[index].name) - 1);
			nsp::toolkit::posix_strncpy<char>((*attrs)[index].describe, sizeof((*attrs)[index].describe), iter.second.c_str(), sizeof((*attrs)[index].describe) - 1);
			index++;
		}
	}
	catch(...){
		return false;
	}
	return true;
}

agv_info * agv_single::create_agvinfo(const inner_agvinfo &agvinfo, const std::map<std::string, std::string > &map_attr) const
{
	agv_info * temp = nullptr;
	try{
		temp = new agv_info;
	}
	catch (...){
		return nullptr;
	}
	temp->vhid = agvinfo.vhid;
	temp->vhtype = agvinfo.vhtype;
	temp->mtport = agvinfo.mtport;
	temp->shport = agvinfo.shport;
	temp->status = agvinfo.status;
	temp->ftsport = agvinfo.ftsport;
	nsp::toolkit::posix_strncpy(temp->inet, sizeof(temp->inet), agvinfo.strip.c_str(), sizeof(temp->inet) - 1);
	nsp::toolkit::posix_strncpy(temp->hwaddr, sizeof(temp->hwaddr), agvinfo.strmac.c_str(), sizeof(temp->hwaddr) - 1);
	temp->count_of_attrs = map_attr.size();
	temp->next = nullptr;
	temp->attrs = nullptr;

	if (!create_attr(&temp->attrs, map_attr)){
		delete temp;
		return nullptr;
	}
	temp->next = nullptr;

	return temp;
}

int agv_single::build_agvinfo(const std::vector<inner_agvinfo> &agvinfo, std::function<std::map<std::string, std::string>(int)> getattr, agv_info **agvs)
{
	agv_info * pre_node = nullptr;
	int index = 0;
	for (auto iter : agvinfo){
		if (iter.delete_type){
			continue;
		}

		std::map<std::string, std::string> map_attr ;
		if (getattr){
			map_attr = getattr(index);
		}
		agv_info* pNew = nsp::toolkit::singleton<agv_single>::instance()->create_agvinfo(iter, map_attr);
		if (!pNew){
			release_agvinfo(*agvs);
			return -1;
		}

		if (!pre_node){
			pre_node = pNew;
			*agvs = pNew;
		}
		else{
			pre_node->next = pNew;
			pre_node = pNew;
		}
		index++;
	}
	return 0;
}
