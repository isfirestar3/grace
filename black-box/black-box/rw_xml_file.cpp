#include "rw_xml_file.h"
#include "os_util.hpp"

rw_xml_file::rw_xml_file()
{
}


rw_xml_file::~rw_xml_file()
{
}

int rw_xml_file::get_robot_ep(std::map<std::string, ep>&ep_list)
{
	ep_list.clear();

	ep ep1;
	ep1.ip_port = "10.10.100.1:1010";
	ep1.port_ = 1010;
	ep ep2;
	ep2.ip_port = "10.10.100.2:1010";
	ep2.port_ = 1010;
	ep_list.insert(std::make_pair(ep1.ip_port,ep1));
	ep_list.insert(std::make_pair(ep2.ip_port, ep2));
	return 0;
}

int rw_xml_file::read_input_ep(std::map<std::string, std::string>&input_ep)
{
	std::string path = nsp::os::get_module_directory<char>();
	if (path.empty())
	{
		return -1;
	}
	path = path + "\\log_select.xml";
	rapidxml::file<> *file_xml;
	rapidxml::xml_document<> doc;

	try
	{
		file_xml = new rapidxml::file<>(path.c_str());
		if (!file_xml)
		{
			return -1;
		}
	}
	catch (...)
	{
		return -1;
	}

	doc.parse<0>(file_xml->data());
	rapidxml::xml_node<> *xml_root = doc.first_node("ep");
	if (!xml_root){
		return -1;
	}
	for (rapidxml::xml_node<>*anode = xml_root->first_node(); anode != NULL; anode = anode->next_sibling()){
		if (strcmp(anode->name(), "ep") == 0){
			std::string id;
			std::string port;
			for (rapidxml::xml_attribute<>*attr = anode->first_attribute(); attr != NULL; attr = attr->next_attribute())
			{
				if (0 == strcmp("ip", attr->name())){
					id = attr->value();
				}
				else if (0 == strcmp("port", attr->name())){
					port = attr->value();
				}

			}
			input_ep.insert(std::make_pair(id, port));
		}
	}
	delete file_xml;
	return 0;
}
int rw_xml_file::write_input_ep(std::map<std::string, std::string>&input_ep)
{
	std::string path = nsp::os::get_module_directory<char>();
	if (path.empty())
	{
		return -1;
	}
	path = path + "\\log_select.xml";

	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* ep = document.allocate_node(rapidxml::node_element, "ep", NULL);

	for (auto &iter : input_ep){
		rapidxml::xml_node<>*chan = document.allocate_node(rapidxml::node_element, "ep");
		chan->append_attribute(document.allocate_attribute("ip", iter.first.c_str()));
		chan->append_attribute(document.allocate_attribute("port", iter.second.c_str()));
		ep->append_node(chan);
	}
	document.append_node(ep);
	std::ofstream outfile(path.c_str());
	outfile << document;
	outfile.close();

	return 0;
}

int rw_xml_file::write_template_type(std::map<std::string, std::vector<std::string>>&template_type)
{
	std::string path = nsp::os::get_module_directory<char>();
	if (path.empty())
	{
		return -1;
	}
	path = path + "\\template_type.xml";

	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* ep = document.allocate_node(rapidxml::node_element, "template_type", NULL);

	for (auto &iter : template_type){
		rapidxml::xml_node<>*chan = document.allocate_node(rapidxml::node_element, "template");
		chan->append_node(document.allocate_node(rapidxml::node_element, "name", iter.first.c_str()));
		for (auto& vctiter : iter.second){
			chan->append_node(document.allocate_node(rapidxml::node_element, "log", vctiter.c_str()));
		}		
		ep->append_node(chan);
	}
	document.append_node(ep);
	std::ofstream outfile(path.c_str());
	outfile << document;
	outfile.close();

	return 0;
}
int rw_xml_file::get_template_type(std::map<std::string, std::vector<std::string>>&template_type)
{
	std::string path = nsp::os::get_module_directory<char>();
	if (path.empty())
	{
		return -1;
	}
	path = path + "\\template_type.xml";
	rapidxml::file<> *file_xml;
	rapidxml::xml_document<> doc;

	try
	{
		file_xml = new rapidxml::file<>(path.c_str());
		if (!file_xml)
		{
			return -1;
		}
	}
	catch (...)
	{
		return -1;
	}

	doc.parse<0>(file_xml->data());
	rapidxml::xml_node<> *xml_root = doc.first_node("template_type");
	if (!xml_root){
		return -1;
	}
	for (rapidxml::xml_node<>*anode = xml_root->first_node(); anode != NULL; anode = anode->next_sibling()){
		std::vector<std::string> logtypes;
		if (strcmp(anode->name(), "template") == 0){
			std::string name;
			std::string templ;
			for (rapidxml::xml_node<>*attr = anode->first_node(); attr != NULL; attr = attr->next_sibling())
			{
				if (0 == strcmp("name", attr->name())){
					name = attr->value();
				}
				else if (0 == strcmp("log", attr->name())){
					templ = attr->value();
					logtypes.push_back(templ);
				}				
			}
			template_type.insert(std::make_pair(name, logtypes));
		}
	}
	delete file_xml;
	return 0;
}