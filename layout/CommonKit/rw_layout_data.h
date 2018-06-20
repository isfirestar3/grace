#pragma once
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "BasicDefine.h"
#include "StringTool.hpp"
#include "AlgorithmDefine.hpp"

#define SUCCESS 0           //数据读取或者写入成功 
#define PATH_EMPTY -1       //文件路径为空
#define MEM_ERROR -2        //new rapidxml::file出错
#define FILE_FAILED -3      //读取文件出错
#define ROOT_FAILED -4      //读取根节点出错
#define SECTION_FAILED -5   //读取xml节点出错
#define ALLOC_FAILED -6     //创建xml节点出错
#define NO_POLYGON -7       //不存在polygon

#define STRING_SIZE 32
#define OPT_COUNT 20

class RWXml
{
public:
	RWXml();
	~RWXml();
public:
	static int get_layout_data(const std::string& filePath, LayoutData& layoutData);
	static int write_layout_data(const std::string& filePath, const LayoutData& layoutData);
	static int get_config_data(const std::string& filePath, CONFIGDATA& configData);
	static int write_config_data(const std::string& filePath, const CONFIGDATA& configData);
	static int get_oper_data(const std::string& filePath, DOCKDATA& dockData);
	static int write_oper_data(const std::string& filePath, const DOCKDATA& dockData);
	static int get_docks_data(const std::string& filePath, DOCKDATA& dockData);
	static int write_docks_data(const std::string& filePath, const DOCKDATA& dockData);
	static int write_QRcode_data(const std::string& filePath, const std::vector<CODEPOSXYA>&codeData);
private:
	static int get_xml_type(rapidxml::xml_node<>* root, LayoutData& layoutData);
	static int get_xml_nodes(rapidxml::xml_node<>* root, LayoutData& layoutData);
	static int get_xml_edges(rapidxml::xml_node<>* root, LayoutData& layoutData);
	static int get_xml_wops(rapidxml::xml_node<>* root, LayoutData& layoutData);
	static int get_xml_links(rapidxml::xml_node<>* root, LayoutData& layoutData);
	static int get_xml_polygons(rapidxml::xml_node<>* root, LayoutData& layoutData);
	static int get_xml_anchors(rapidxml::xml_node<>* root, std::vector<ANCHORINFO>& vecAnchor);
	static int get_xml_vehicles(rapidxml::xml_node<>* root, std::vector<VEHICLEINFO>& vecVehicle);
	static int get_xml_search_type(rapidxml::xml_node<>* root, int& search_type);
	static int get_xml_docks(rapidxml::xml_node<>* root, std::vector<ANCHORINFO>& vecAnchor);
	static int get_xml_opts(rapidxml::xml_node<>* parent, ANCHORINFO& anchorInfo);

	static rapidxml::xml_node<>* create_xml_type(rapidxml::xml_document<>& document, const LayoutData& layoutData);
	static rapidxml::xml_node<>* create_xml_nodes(rapidxml::xml_document<>& document, const LayoutData& layoutData);
	static rapidxml::xml_node<>* create_xml_edges(rapidxml::xml_document<>& document, const LayoutData& layoutData);
	static rapidxml::xml_node<>* create_xml_wops(rapidxml::xml_document<>& document, const LayoutData& layoutData);
	static rapidxml::xml_node<>* create_xml_links(rapidxml::xml_document<>& document, const LayoutData& layoutData);
	static rapidxml::xml_node<>* create_xml_polygons(rapidxml::xml_document<>& document, const LayoutData& layoutData);
	static rapidxml::xml_node<>* create_xml_anchors(rapidxml::xml_document<>& document, const std::vector<ANCHORINFO>& vecAnchor);
	static rapidxml::xml_node<>* create_xml_vehicles(rapidxml::xml_document<>& document, const std::vector<VEHICLEINFO>& vecVehicle);
	static rapidxml::xml_node<>* create_xml_search_type(rapidxml::xml_document<>& document, int search_type);
	static rapidxml::xml_node<>* create_xml_docks(rapidxml::xml_document<>& document, const std::vector<ANCHORINFO>& vecAnchor);
	static rapidxml::xml_node<>* create_xml_opts(rapidxml::xml_document<>& document, const ANCHORINFO& anchorInfo);
	
};