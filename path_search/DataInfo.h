#ifndef DataInfo_20161228_H
#define DataInfo_20161228_H

#include"singleton.hpp"
#include"PathSearch.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include <map>

class DataInfo
{
public:
	DataInfo();
	~DataInfo();
//	friend class nsp::toolkit::singleton<DataInfo>;
private:
	int get_xml_type( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData );
	int get_xml_nodes( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData );
	int get_xml_edges( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData );
	int get_xml_wops( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData );
	int get_xml_links( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData );

	rapidxml::xml_node<>* create_xml_paths(const char* pFilePath, rapidxml::xml_document<>& document,
		std::map<unsigned int, std::map<unsigned int, float>> mapWopDistances,
		std::map<unsigned int, std::map<unsigned int, int>> mapWopPaths);

	rapidxml::xml_node<>* insert_xml_paths(rapidxml::xml_document<>& document,
		std::map<unsigned int, std::map<unsigned int, float>> mapWopDistances,
		std::map<unsigned int, std::map<unsigned int, int>> mapWopPaths);

	int get_xml_paths(rapidxml::xml_node<>* root,
		std::map<unsigned int, std::map<unsigned int, float>>& mapWopDistances,
		std::map<unsigned int, std::map<unsigned int, int>>& mapWopPaths);

public:
	int OpenMap( const char* pFilePath, PathSearch_LayoutData* pLayoutData );

};

#endif

