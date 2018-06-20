#include "DataInfo.h"
#include "Calculate.hpp"
#include "log.h"
#include <iostream>
#include "posix_string.h"

DataInfo::DataInfo()
{
}

DataInfo::~DataInfo()
{
}

int DataInfo::OpenMap( const char* pFilePath,  PathSearch_LayoutData* pLayoutData )
{
	if( !pFilePath || !pLayoutData ) {
		loerror("path_search") << "PathSearch_OpenMap: pFilePath is NULL. or pLayoutData is NULL.";
		return -1;
	}

	rapidxml::xml_document<> doc;
	rapidxml::file<> *xmlfile = nullptr;
	try {
		xmlfile = new rapidxml::file<>( pFilePath );
		if( !xmlfile ) {
			loerror("path_search") << "PathSearch_OpenMap: Failed to Open LayoutData xml file.";
			return -1;
		}
	} catch( ... ) {
		loerror("path_search") << "PathSearch_OpenMap: Failed to Open LayoutData xml file.";
		return -1;
	}

	doc.parse<0>( xmlfile->data() );
	rapidxml::xml_node<>* root = doc.first_node();

	int iRet = 0;
	do {
		if( !root ) {
			iRet = -1;
			loerror("path_search") << "PathSearch_OpenMap: Failed to Read LayoutData xml file:root.";
			break;
		}
		iRet = get_xml_type( root, pLayoutData );
		
		if( iRet != 0 ) {
			loerror("path_search") << "PathSearch_OpenMap: Failed to Read LayoutData xml file:type.";
			break;
		}
		iRet = get_xml_nodes( root, pLayoutData );
		if( iRet != 0 ) {
			loerror("path_search") << "PathSearch_OpenMap: Failed to Read LayoutData xml file:nodes.";
			break;
		}
		iRet = get_xml_edges( root, pLayoutData );
		if( iRet != 0 ) {
			loerror("path_search") << "PathSearch_OpenMap: Failed to Read LayoutData xml file:edges.";
			break;
		}
		iRet = get_xml_wops( root, pLayoutData );
		if( iRet != 0 ) {
			loerror("path_search") << "PathSearch_OpenMap: Failed to Read LayoutData xml file:wops.";
			break;
		}
		iRet = get_xml_links( root, pLayoutData );
		if( iRet != 0 ) {
			loerror("path_search") << "PathSearch_OpenMap: Failed to Read LayoutData xml file:links.";
			break;
		}
	} while( false );

	delete xmlfile;
	return iRet;
}

int DataInfo::get_xml_type( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData )
{
	rapidxml::xml_node<>* type = root->first_node( "type" );
	if( !type ) {
		return -1;
	}
	layoutData->type = atoi( type->value( ) );
	return 0;
}

int DataInfo::get_xml_nodes( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData )
{
	rapidxml::xml_node<>* nodes = root->first_node( "nodes" );
	if( !nodes ) {
		return -1;
	}

	for( rapidxml::xml_node<>* node = nodes->first_node( ); node; node = node->next_sibling( ) )//获取node下的所有属性
	{
		//int i = 0;
		double table[5];
		for( rapidxml::xml_attribute<char> * attr = node->first_attribute( ); attr; attr = attr->next_attribute( ) ) {
			if (0 == strcmp("id", attr->name())){
				table[0] = atof(attr->value());
			}
			else if (0 == strcmp("x", attr->name())){
				table[1] = atof(attr->value());
			}
			else if (0 == strcmp("y", attr->name())){
				table[2] = atof(attr->value());
			}
			else if (0 == strcmp("spin", attr->name())){
				table[3] = atof(attr->value());
			}
		}
		PathSearch_NODEINFO nodeInfo;
		nodeInfo.node_id = table[0];
		nodeInfo.pos_x = DataCalculate::world2map( table[1] );
		nodeInfo.pos_y = DataCalculate::world2map( table[2] );

		layoutData->nodeList.push_back( nodeInfo );
	}
	return 0;
}

int DataInfo::get_xml_edges( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData )
{
	rapidxml::xml_node<>* edges = root->first_node( "edges" );
	if( !edges ) {
		return -1;
	}

	for( rapidxml::xml_node<>* edge = edges->first_node( ); edge; edge = edge->next_sibling( ) )//获取edges节点下的每个edge
	{
		PathSearch_EDGEINFO edgeInfo;
		rapidxml::xml_node<>* id = edge->first_node( );
		if( id ) {
			edgeInfo.id = atoi( id->value( ) );
		}
		rapidxml::xml_node<>* start_node_id = edge->first_node( "start_node_id" );
		if( start_node_id ) {
			edgeInfo.start_node_id = atoi( start_node_id->value( ) );
		}
		rapidxml::xml_node<>* end_node_id = edge->first_node( "end_node_id" );
		if( end_node_id ) {
			edgeInfo.end_node_id = atoi( end_node_id->value( ) );
		}
		rapidxml::xml_node<>* ctrl_point1 = edge->first_node( "ctrl_point1" );
		if( ctrl_point1 ) {
			for (rapidxml::xml_attribute<char> * attr = ctrl_point1->first_attribute(); attr; attr = attr->next_attribute()){
				if (0 == strcmp("x", attr->name()))
				{
					edgeInfo.ctrl_point1.x = DataCalculate::world2map(atof(attr->value()));
				}
				else if (0 == strcmp("y", attr->name()))
				{
					edgeInfo.ctrl_point1.y = DataCalculate::world2map(atof(attr->value()));
				}
			}
		}

		rapidxml::xml_node<>* ctrl_point2 = edge->first_node( "ctrl_point2" );
		if( ctrl_point2 ) {
			for (rapidxml::xml_attribute<char> * attr = ctrl_point2->first_attribute(); attr; attr = attr->next_attribute()){
				if (0 == strcmp("x", attr->name()))
				{
					edgeInfo.ctrl_point2.x = DataCalculate::world2map(atof(attr->value()));
				}
				else if (0 == strcmp("y", attr->name()))
				{
					edgeInfo.ctrl_point2.y = DataCalculate::world2map(atof(attr->value()));
				}
			}
		}

		rapidxml::xml_node<>* term_point1 = edge->first_node( "start_node" );
		if( term_point1 ) {
			for (rapidxml::xml_attribute<char> * attr = term_point1->first_attribute(); attr; attr = attr->next_attribute()){
				if (0 == strcmp("x", attr->name()))
				{
					edgeInfo.termstpos.x = DataCalculate::world2map(atof(attr->value()));
				}
				else if (0 == strcmp("y", attr->name()))
				{
					edgeInfo.termstpos.y = DataCalculate::world2map(atof(attr->value()));
				}
			}
		}

		rapidxml::xml_node<>* term_point2 = edge->first_node( "end_node" );
		if( term_point2 ) {
			for (rapidxml::xml_attribute<char> * attr = term_point2->first_attribute(); attr; attr = attr->next_attribute()){
				if (0 == strcmp("x", attr->name()))
				{
					edgeInfo.termendpos.x = DataCalculate::world2map(atof(attr->value()));
				}
				else if (0 == strcmp("y", attr->name()))
				{
					edgeInfo.termendpos.y = DataCalculate::world2map(atof(attr->value()));
				}
			}
		}

		rapidxml::xml_node<>* distance = edge->first_node( "distance" );
		if( distance ) {
			edgeInfo.length = DataCalculate::world2map( atof( distance->value( ) ) );
		}
		double tmp;
		rapidxml::xml_node<>* realdistance = edge->first_node("real_distance");
		if (realdistance)	{
			tmp = atof(realdistance->value());
			if (tmp >= -0.01){
				edgeInfo.length = DataCalculate::world2map(atof(realdistance->value()));
			}
		}
		
		rapidxml::xml_node<>* speed_limit = edge->first_node( "speed_limit" );
		if( speed_limit ) {
			rapidxml::xml_node<>* segment = speed_limit->first_node( );
			for( ; segment; segment = segment->next_sibling( ) ) {
				rapidxml::xml_attribute<char> * attr = segment->first_attribute( );
				PathSearch_EDGESPEED speed_seg;
				for (rapidxml::xml_attribute<char> * attr = segment->first_attribute(); attr; attr = attr->next_attribute()){
					if (0 == strcmp("percent", attr->name()))
					{
						speed_seg.percent = atof(attr->value());
					}
					else if (0 == strcmp("speed", attr->name()))
					{
						speed_seg.speed = atof(attr->value());
					}
				}
				edgeInfo.speed.push_back( speed_seg );
			}
		}

		rapidxml::xml_node<>* disperse_point = edge->first_node( "disperse_point" );
		if( disperse_point ) {
			rapidxml::xml_node<>* point = disperse_point->first_node( );
			for( ; point; point = point->next_sibling( ) ) {
				PathSearch_POSPOINT pt_temp;
				for (rapidxml::xml_attribute<char> * attr = point->first_attribute(); attr; attr = attr->next_attribute()){
					if (0 == strcmp("x", attr->name()))
					{
						pt_temp.x = DataCalculate::world2map(atof(attr->value()));
					}
					else if (0 == strcmp("y", attr->name()))
					{
						pt_temp.y = DataCalculate::world2map(atof(attr->value()));
					}
					else if (0 == strcmp("distance_to_start", attr->name()))
					{
						pt_temp.distance_to_start = DataCalculate::world2map(atof(attr->value()));
					}
				}
				edgeInfo.disperse_point.push_back( pt_temp );
			}
		}
		rapidxml::xml_node<>* wops = edge->first_node( "wops" );
		if( wops ) {
			rapidxml::xml_node<>* wop = wops->first_node( );
			for( ; wop; wop = wop->next_sibling( ) ) {
				edgeInfo.wop_list.push_back( atoi( wop->value( ) ) );
			}
		}
		layoutData->edgeList.push_back( edgeInfo );
	}
	return 0;
}

int DataInfo::get_xml_wops( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData )
{
	rapidxml::xml_node<>* wops = root->first_node( "wops" );
	if( !wops ) {
		return -1;
	}
	for( rapidxml::xml_node<>* wop = wops->first_node( ); wop; wop = wop->next_sibling( ) )//获取wop下的所有属性
	{
		int i = 0;
		double table[4];
		for( rapidxml::xml_attribute<char> * attr = wop->first_attribute( ); attr; attr = attr->next_attribute( ) ) {
			/*table[i] = atof( attr->value( ) );
			i++;*/
			if (0 == strcmp("id", attr->name())){
				table[0] = atof(attr->value());
			}
			else if (0 == strcmp("direction", attr->name())){
				table[1] = atof(attr->value());
			}
			else if (0 == strcmp("angle_type", attr->name())){
				table[2] = atof(attr->value());
			}
			else if (0 == strcmp("angle", attr->name())){
				table[3] = atof(attr->value());
			}
		}
		PathSearch_Wop wop_tmp;
		wop_tmp.wop_id = ( int )table[0];
		wop_tmp.direction = static_cast<PathSearch_Direction>( ( int )table[1] );
		wop_tmp.angle_type = static_cast<PathSearch_AngleType>( ( int )table[2] );
		wop_tmp.angle = DataCalculate::radian2angle( table[3] );
		layoutData->wopList.push_back( wop_tmp );
	}


	return 0;
}

int DataInfo::get_xml_links( rapidxml::xml_node<>* root, PathSearch_LayoutData* layoutData )
{
	rapidxml::xml_node<>* links = root->first_node( "links" );
	if( !links ) {
		return -1;
	}
	for( rapidxml::xml_node<>* link = links->first_node( ); link; link = link->next_sibling( ) )//获取links下的所有节点
	{
		int i = 0;
		double table[6];
		for( rapidxml::xml_attribute<char> * attr = link->first_attribute( ); attr; attr = attr->next_attribute( ) ) {
			/*table[i] = atof( attr->value( ) );
			i++;*/
			if (0 == strcmp("id", attr->name())){
				table[0] = atof(attr->value());
			}
			else if (0 == strcmp("from_edge", attr->name())){
				table[1] = atof(attr->value());
			}
			else if (0 == strcmp("from_wop", attr->name())){
				table[2] = atof(attr->value());
			}
			else if (0 == strcmp("to_edge", attr->name())){
				table[3] = atof(attr->value());
			}
			else if (0 == strcmp("to_wop", attr->name())){
				table[4] = atof(attr->value());
			}
			else if (0 == strcmp("cost", attr->name())){
				table[5] = atof(attr->value());
			}
		}
		PathSearch_LINK linkInfo;
		linkInfo.link_id = static_cast<int>( table[0] );
		linkInfo.link_from.edge_id = static_cast<int>( table[1] );
		linkInfo.link_from.wop_id = static_cast<int>( table[2] );
		linkInfo.link_to.edge_id = static_cast<int>( table[3] );
		linkInfo.link_to.wop_id = static_cast<int>( table[4] );
		linkInfo.convert_cost = DataCalculate::world2map( table[5] );
		layoutData->linkList.push_back( linkInfo );
	}
	return 0;
}

rapidxml::xml_node<>* DataInfo::create_xml_paths(const char* pFilePath, rapidxml::xml_document<>& document,
	std::map<unsigned int, std::map<unsigned int, float>> mapWopDistances,
	std::map<unsigned int, std::map<unsigned int, int>> mapWopPaths)
{
	rapidxml::xml_node<>* paths = document.allocate_node(rapidxml::node_element, pFilePath, NULL);
	if (!paths) {
		return NULL;
	}
	for (size_t startIndex = 0; startIndex < mapWopPaths.size(); startIndex++) {
		for (size_t endIndex = 0; endIndex < mapWopPaths.size(); endIndex++) {
			rapidxml::xml_node<>* path = document.allocate_node(rapidxml::node_element, "path", NULL);

			if (path) {
				char chText[32] = { 0 };
				posix__sprintf(chText, 32, "%d", startIndex);
				path->append_attribute(document.allocate_attribute("S", document.allocate_string(chText)));
				posix__sprintf(chText, 32, "%d", endIndex);
				path->append_attribute(document.allocate_attribute("E", document.allocate_string(chText)));
				posix__sprintf(chText, 32, "%d", mapWopPaths[startIndex][endIndex]);
				path->append_attribute(document.allocate_attribute("path", document.allocate_string(chText)));
				posix__sprintf(chText, 32, "%lf", mapWopDistances[startIndex][endIndex]);
				path->append_attribute(document.allocate_attribute("distance", document.allocate_string(chText)));

				paths->append_node(path);
			}
		}
	}
	return paths;
}



int DataInfo::get_xml_paths(rapidxml::xml_node<>* paths,
	std::map<unsigned int, std::map<unsigned int, float>>& mapWopDistances,
	std::map<unsigned int, std::map<unsigned int, int>>& mapWopPaths)
{
	//rapidxml::xml_node<>* paths = root->first_node("paths");
	//if (!paths) {
	//	return -1;
	//}

	for (rapidxml::xml_node<>* path = paths->first_node(); path; path = path->next_sibling())//获取node下的所有属性
	{
		int i = 0;
		double table[5];
		for (rapidxml::xml_attribute<char> * attr = path->first_attribute(); attr; attr = attr->next_attribute()) {
			table[i] = atof(attr->value());
			i++;
		}
		unsigned int startIndex = table[0];
		unsigned int endIndex = table[1];
		mapWopPaths[startIndex][endIndex] = table[2];
		mapWopDistances[startIndex][endIndex] = table[3];
	}
	return 0;
}
