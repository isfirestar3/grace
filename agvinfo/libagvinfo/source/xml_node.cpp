#include "xml_node.h"
#include "os_util.hpp"
#include "toolkit.h"
#include "log.h"

xml_node::xml_node()
{
	char buffer[1024] = { 0 };
	std::string strpath = nsp::os::get_module_directory<char>();
	file_path_ = strpath+ "/agv_info.xml";

	set_agvinfo_.insert("id");
	set_agvinfo_.insert("type");
	set_agvinfo_.insert("ip");
	set_agvinfo_.insert("port");
	set_agvinfo_.insert("shell_port");
	set_agvinfo_.insert("fts_port");
	set_agvinfo_.insert("mac_addr");
}

xml_node::~xml_node()
{
}

rapidxml::xml_node<>* xml_node::find_first_child_node(rapidxml::xml_node<> * root, std::string str_search)
{
	if (!root){
		return nullptr;
	}
	auto node = root->first_node();
	for (; node != nullptr; node = node->next_sibling())
	{
		if (!str_search.compare(node->name())){
			break;
		}
	}

	return node;
}

rapidxml::xml_node<>* xml_node::find_child_node_by_attr(rapidxml::xml_node<> * parent ,std::string strkey,std::string strValue)
{
	if (!parent){
		return nullptr;
	}

	auto node = parent->first_node();
	while (node)
	{
		for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute()){
			if ((0 == strkey.compare(attr->name())) && (0 == strValue.compare(attr->value()))){
				return node;
			}
		}
		node = node->next_sibling();
	}

	return nullptr;
}

int xml_node::attach_file(rapidxml::xml_document<> &doc, rapidxml::file<> *&file )
{
	rapidxml::file<> * pfile = nullptr;
	try{
		pfile = new rapidxml::file<>(file_path_.c_str());
	}
	catch (...)
	{
		nsperror << "load xml fail";
		return -1;
	}
	try{
		doc.parse<0>(pfile->data());
	}
	catch (...){
		delete pfile;
		return -1;
	}
	
	file = pfile;
	return 0;
}

void xml_node::detatch_file(rapidxml::file<> *file)
{
	if (file){
		delete file;
	}
}

void xml_node::refresh_file(rapidxml::xml_document<> &doc, std::string filepath )
{
	std::ofstream out_put(filepath.c_str());
	out_put << doc;
	out_put.close();
}

int xml_node::load_agvinfo(std::vector<agv_info_inner>& vec_agvinfo)
{
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file = nullptr;
	if (attach_file(doc, file)){
		return -1;
	}

	rapidxml::xml_node<>* agv_info_node = doc.first_node();
	rapidxml::xml_node<>*anode = find_first_child_node(agv_info_node, "agvs");
	if (!anode){
		return -1;
	}

	for (rapidxml::xml_node<>*agvnode = anode->first_node(); agvnode != NULL; agvnode = agvnode->next_sibling()){
		agv_info_inner agvinfo;
		if (0 == strcmp("agv", agvnode->name())){
			rapidxml::xml_attribute<char> * attr = agvnode->first_attribute();
			while (attr)
			{
				if (0 == strcmp("id", attr->name())){
					agvinfo.id_ = atoi(attr->value());
				}
				else if (0 == strcmp("type", attr->name())){
					agvinfo.type_ = atoi(attr->value());
				}
				else if (0 == strcmp("ip", attr->name())){
					agvinfo.inet_ = attr->value();
				}
				else if (0 == strcmp("port", attr->name())){
					agvinfo.mtport_ = atoi(attr->value());
				}
				else if (0 == strcmp("shell_port", attr->name())){
					agvinfo.shport_ = atoi(attr->value());
				}
				else if (0 == strcmp("fts_port", attr->name())){
					agvinfo.ftsport_ = atoi(attr->value());
				}
				else if (0 == strcmp("mac_addr", attr->name())){
					agvinfo.hwaddr_ = attr->value();
				}
				else
				{
					agv_attribute_inner attribute;
					attribute.name_ = attr->name();
					attribute.describe_ = attr->value();
					agvinfo.vec_attrs_.push_back(attribute);
				}
				attr = attr->next_attribute();
			}
			vec_agvinfo.push_back(agvinfo);
		}
	}

	detatch_file(file);
	return 0;
}

int xml_node::conver_agvinfo(std::vector<agv_info_inner> vec_agvinfo)
{
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file = nullptr;
	rapidxml::xml_node<>* agv_info_node = nullptr;
	if (!attach_file(doc, file)){
		agv_info_node = doc.first_node();
		rapidxml::xml_node<>*anode = find_first_child_node(agv_info_node, "agvs");
		if (!anode){
			return -1;
		}
		agv_info_node->remove_node(anode);
	}
	else{
		agv_info_node = doc.allocate_node(rapidxml::node_element, "agv_info");
		doc.append_node(agv_info_node);
	}

	rapidxml::xml_node<>* agv_node = doc.allocate_node(rapidxml::node_element, "agvs");
	for (auto iter : vec_agvinfo){
		if ((uint32_t)-1 == iter.id_){
			continue;
		}

		rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "agv");
		node->append_attribute(doc.allocate_attribute("id", doc.allocate_string(nsp::toolkit::to_string<char>(iter.id_).c_str())));
		node->append_attribute(doc.allocate_attribute("type", doc.allocate_string(nsp::toolkit::to_string<char>(iter.type_).c_str())));
		node->append_attribute(doc.allocate_attribute("ip", doc.allocate_string(iter.inet_.c_str(), iter.inet_.length()+1)));
		node->append_attribute(doc.allocate_attribute("port", doc.allocate_string(nsp::toolkit::to_string<char>(iter.mtport_).c_str())));
		node->append_attribute(doc.allocate_attribute("shell_port", doc.allocate_string(nsp::toolkit::to_string<char>(iter.shport_).c_str())));
		node->append_attribute(doc.allocate_attribute("fts_port", doc.allocate_string(nsp::toolkit::to_string<char>(iter.ftsport_).c_str())));
		node->append_attribute(doc.allocate_attribute("mac_addr", doc.allocate_string(iter.hwaddr_.c_str(), iter.hwaddr_.length()+1)));
		for (auto attr_node : iter.vec_attrs_){
			node->append_attribute(doc.allocate_attribute(doc.allocate_string(attr_node.name_.c_str()), doc.allocate_string(attr_node.describe_.c_str())));
		}
		agv_node->append_node(node);
	}

	agv_info_node->append_node(agv_node);

	refresh_file(doc, file_path_.c_str());
	detatch_file(file);
	return 0;
}

int xml_node::get_detail(uint32_t vhid, std::vector<agv_attribute_inner> &vec_attr)
{
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file = nullptr;
	if (attach_file(doc, file)){
		return -1;
	}

	rapidxml::xml_node<>* agv_info_node = doc.first_node();
	rapidxml::xml_node<>*anode = find_first_child_node(agv_info_node, "agvs");
	if (!anode){
		detatch_file(file);
		return -1;
	}

	rapidxml::xml_node<>* node = find_child_node_by_attr(anode,"id",nsp::toolkit::to_string<char>(vhid));
	if (node){
		rapidxml::xml_attribute<char> * attr = node->first_attribute();
		while (attr)
		{
			if (set_agvinfo_.find(attr->name()) == set_agvinfo_.end()){
				agv_attribute_inner agv_attr;
				agv_attr.name_ = attr->name();
				agv_attr.describe_ = attr->value();
				vec_attr.push_back(agv_attr);
			}
			attr = attr->next_attribute();
		}
	}
	else{
		detatch_file(file);
		return -1;
	}
	detatch_file(file);

	return 0;
}

int xml_node::set_detail(uint32_t vhid, std::vector<agv_attribute_inner> detail)
{
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file = nullptr;
	if (attach_file(doc, file)){
		return -1;
	}

	rapidxml::xml_node<> * agv_info_node = doc.first_node();
	rapidxml::xml_node<>*anode = find_first_child_node(agv_info_node, "agvs");
	if (!anode){
		detatch_file(file);
		return -1;
	}

	rapidxml::xml_node<>* agv_node = find_child_node_by_attr(anode, "id", nsp::toolkit::to_string<char>(vhid));
	if (!agv_node){
		return -1;
	}

	//agv_node->remove_all_attributes();
	rapidxml::xml_attribute<char> * attr = agv_node->first_attribute();
	while (attr)
	{
		auto attr_node = attr;
		attr = attr->next_attribute();
		if (set_agvinfo_.find(attr_node->name()) == set_agvinfo_.end()){
			agv_node->remove_attribute(attr_node);
			continue;
		}
	}

	for (auto iter : detail){
		agv_node->append_attribute(doc.allocate_attribute(doc.allocate_string(iter.name_.c_str()), doc.allocate_string(iter.describe_.c_str())));
	}

	refresh_file(doc, file_path_.c_str());
	detatch_file(file);

	return 0;
}