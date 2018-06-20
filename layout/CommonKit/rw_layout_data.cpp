#include "rw_layout_data.h"

RWXml::RWXml()
{

}
RWXml::~RWXml()
{

}

int RWXml::get_layout_data(const std::string& filePath, LayoutData& layoutData)
{
	if (filePath.empty())
	{
		return PATH_EMPTY;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try
	{
		file = new rapidxml::file<>(filePath.c_str());
		if (!file)
		{
			return FILE_FAILED;
		}
	}
	catch (...)
	{
		return MEM_ERROR;
	}

	doc.parse<0>(file->data());
	rapidxml::xml_node<>* root = doc.first_node();

	int iRet = SUCCESS;
	do
	{
		if (!root)
		{
			iRet = ROOT_FAILED;
			break;
		}
		iRet = get_xml_type(root, layoutData);
		iRet = get_xml_nodes(root, layoutData);
		if (iRet != SUCCESS)
		{
			break;
		}
		iRet = get_xml_edges(root, layoutData);
		if (iRet != SUCCESS)
		{
			break;
		}
		iRet = get_xml_wops(root, layoutData);
		if (iRet != SUCCESS)
		{
			break;
		}
		iRet = get_xml_links(root, layoutData);
		if (iRet != SUCCESS)
		{
			break;
		}
		iRet = get_xml_polygons(root, layoutData);
		if (iRet != SUCCESS && iRet != NO_POLYGON)
		{
			break;
		}
	} while (false);

	delete file;
	return iRet;
}
int RWXml::write_layout_data(const std::string& filePath, const LayoutData& layoutData)
{
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* root = document.allocate_node(rapidxml::node_element, "root", NULL);
	if (!root)
	{
		return ALLOC_FAILED;
	}
	rapidxml::xml_node<>* type = create_xml_type(document, layoutData);
	root->append_node(type);
	rapidxml::xml_node<>* nodes = create_xml_nodes(document, layoutData);
	if (!nodes)
	{
		return ALLOC_FAILED;
	}
	root->append_node(nodes);
	rapidxml::xml_node<>* edges = create_xml_edges(document, layoutData);
	if (!edges)
	{
		return ALLOC_FAILED;
	}
	root->append_node(edges);
	rapidxml::xml_node<>* wops = create_xml_wops(document, layoutData);
	if (!wops)
	{
		return ALLOC_FAILED;
	}
	root->append_node(wops);
	rapidxml::xml_node<>* links = create_xml_links(document, layoutData);
	if (!links)
	{
		return ALLOC_FAILED;
	}
	root->append_node(links);
	rapidxml::xml_node<>* polygons = create_xml_polygons(document, layoutData);
	if (!polygons)
	{
		return ALLOC_FAILED;
	}
	root->append_node(polygons);
	document.append_node(root);

	/*std::string strText;
	rapidxml::print(std::back_inserter(strText), document, 0);
	std::cout << strText << std::endl;*/

	std::ofstream outfile(filePath.c_str());
	outfile << document;
	return SUCCESS;
}
int RWXml::get_config_data(const std::string& filePath, CONFIGDATA& configData)
{
	if (filePath.empty())
	{
		return PATH_EMPTY;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try
	{
		file = new rapidxml::file<>(filePath.c_str());
		if (!file)
		{
			return FILE_FAILED;
		}
	}
	catch (...)
	{
		return MEM_ERROR;
	}

	doc.parse<0>(file->data());
	rapidxml::xml_node<>* root = doc.first_node();

	int iRet = SUCCESS;
	do
	{
		if (!root)
		{
			iRet = ROOT_FAILED;
			break;
		}
		iRet = get_xml_vehicles(root, configData.vehicleList);
		if (iRet != SUCCESS)
		{
			break;
		}
		iRet = get_xml_search_type(root, configData.search_type);
		if (iRet != SUCCESS)
		{
			break;
		}
	} while (false);

	delete file;
	return iRet;
}
int RWXml::write_config_data(const std::string& filePath, const CONFIGDATA& configData)
{
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* root = document.allocate_node(rapidxml::node_element, "root", NULL);
	if (!root)
	{
		return ALLOC_FAILED;
	}
	rapidxml::xml_node<>* vehicles = create_xml_vehicles(document, configData.vehicleList);
	if (!vehicles)
	{
		return ALLOC_FAILED;
	}
	root->append_node(vehicles);
	rapidxml::xml_node<>* search_type = create_xml_search_type(document, configData.search_type);
	if (!search_type)
	{
		return ALLOC_FAILED;
	}
	root->append_node(search_type);
	document.append_node(root);

	std::ofstream outfile(filePath.c_str());
	outfile << document;
	return SUCCESS;
}

int RWXml::get_oper_data(const std::string& filePath, DOCKDATA& dockData)
{
	if (filePath.empty())
	{
		return PATH_EMPTY;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try
	{
		file = new rapidxml::file<>(filePath.c_str());
		if (!file)
		{
			return FILE_FAILED;
		}
	}
	catch (...)
	{
		return MEM_ERROR;
	}

	doc.parse<0>(file->data());
	rapidxml::xml_node<>* root = doc.first_node();

	int iRet = SUCCESS;
	do
	{
		if (!root)
		{
			iRet = ROOT_FAILED;
			break;
		}
		iRet = get_xml_anchors(root, dockData.anchorList);
		if (iRet != SUCCESS)
		{
			break;
		}
	} while (false);

	delete file;
	return iRet;
}
int RWXml::write_oper_data(const std::string& filePath, const DOCKDATA& dockData)
{
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* root = document.allocate_node(rapidxml::node_element, "root", NULL);
	if (!root)
	{
		return ALLOC_FAILED;
	}
	rapidxml::xml_node<>* anchors = create_xml_anchors(document, dockData.anchorList);
	if (!anchors)
	{
		return ALLOC_FAILED;
	}
	root->append_node(anchors);
	document.append_node(root);

	std::ofstream outfile(filePath.c_str());
	outfile << document;
	return SUCCESS;
}

int RWXml::get_docks_data(const std::string& filePath, DOCKDATA& dockData)
{
	if (filePath.empty())
	{
		return PATH_EMPTY;
	}
	rapidxml::xml_document<> doc;
	rapidxml::file<> *file;
	try
	{
		file = new rapidxml::file<>(filePath.c_str());
		if (!file)
		{
			return FILE_FAILED;
		}
	}
	catch (...)
	{
		return MEM_ERROR;
	}

	doc.parse<0>(file->data());
	rapidxml::xml_node<>* root = doc.first_node("docks");

	int iRet = SUCCESS;
	do
	{
		if (!root)
		{
			iRet = ROOT_FAILED;
			break;
		}
		iRet = get_xml_docks(root, dockData.anchorList);
	} while (false);

	delete file;
	return iRet;
}

int RWXml::write_docks_data(const std::string& filePath, const DOCKDATA& dockData)
{
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* docks = create_xml_docks(document, dockData.anchorList);
	if (!docks)
	{
		return ALLOC_FAILED;
	}
	document.append_node(docks);

	std::ofstream outfile(filePath.c_str());
	outfile << document;
	return SUCCESS;
}

int RWXml::get_xml_type(rapidxml::xml_node<>* root, LayoutData& layoutData)
{
	rapidxml::xml_node<>* type = root->first_node("type");
	if (type)
	{
		layoutData.type = atoi(type->value());
	}
	return SUCCESS;
}
int RWXml::get_xml_nodes(rapidxml::xml_node<>* root, LayoutData& layoutData)
{
	rapidxml::xml_node<>* nodes = root->first_node("nodes");
	if (!nodes)
	{
		return SECTION_FAILED;
	}

	for (rapidxml::xml_node<>* node = nodes->first_node(); node; node = node->next_sibling())//获取node下的所有属性
	{
		double table[5] = { 0.0 };
		for (rapidxml::xml_attribute<char> * attr = node->first_attribute(); attr; attr = attr->next_attribute())
		{
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
		NODEINFO nodeInfo;
		nodeInfo.node_id = table[0];
		nodeInfo.pos_x = DataConvert::world2map(table[1]);
		nodeInfo.pos_y = DataConvert::world2map(table[2]);
		nodeInfo.spin = (int)table[3];

		layoutData.nodeList.push_back(nodeInfo);
	}
	return SUCCESS;
}

int RWXml::get_xml_edges(rapidxml::xml_node<>* root, LayoutData& layoutData)
{
	rapidxml::xml_node<>* edges = root->first_node("edges");
	if (!edges)
	{
		return SECTION_FAILED;
	}

	for (rapidxml::xml_node<>* edge = edges->first_node(); edge; edge = edge->next_sibling())//获取edges节点下的每个edge
	{
		EDGEINFO edgeInfo;
		rapidxml::xml_node<>* id = edge->first_node("id");
		if (id)	{
			edgeInfo.id = atoi(id->value());
		}
		rapidxml::xml_node<>* start_node_id = edge->first_node("start_node_id");
		if (start_node_id)	{
			edgeInfo.start_node_id = atoi(start_node_id->value());
		}
		rapidxml::xml_node<>* end_node_id = edge->first_node("end_node_id");
		if (end_node_id)	{
			edgeInfo.end_node_id = atoi(end_node_id->value());
		}
		rapidxml::xml_node<>* ctrl_point1 = edge->first_node("ctrl_point1");
		if (ctrl_point1)
		{
			for (rapidxml::xml_attribute<char> * attr = ctrl_point1->first_attribute(); attr; attr = attr->next_attribute()){
				if (0 == strcmp("x", attr->name()))
				{
					edgeInfo.ctrl_point1.x = DataConvert::world2map(atof(attr->value()));
				}
				else if (0 == strcmp("y", attr->name()))
				{
					edgeInfo.ctrl_point1.y = DataConvert::world2map(atof(attr->value()));
				}
			}
			
		}

		rapidxml::xml_node<>* ctrl_point2 = edge->first_node("ctrl_point2");
		if (ctrl_point2)
		{
			for (rapidxml::xml_attribute<char> * attr = ctrl_point2->first_attribute(); attr; attr = attr->next_attribute()){
				if (0 == strcmp("x", attr->name()))
				{
					edgeInfo.ctrl_point2.x = DataConvert::world2map(atof(attr->value()));
				}
				else if ( 0 == strcmp("y", attr->name()))
				{
					edgeInfo.ctrl_point2.y = DataConvert::world2map(atof(attr->value()));
				}
			}
			
		}

		rapidxml::xml_node<>* term_point1 = edge->first_node("start_node");
		if (term_point1)
		{
			for (rapidxml::xml_attribute<char> * attr = term_point1->first_attribute(); attr; attr = attr->next_attribute()){
				if (0 == strcmp("x", attr->name()))
				{
					edgeInfo.termstpos.x = DataConvert::world2map(atof(attr->value()));				
				}
				else if (0 == strcmp("y", attr->name()))
				{
					edgeInfo.termstpos.y = DataConvert::world2map(atof(attr->value()));
				}
			}
			
		}

		rapidxml::xml_node<>* term_point2 = edge->first_node("end_node");
		if (term_point2)
		{
			for (rapidxml::xml_attribute<char> * attr = term_point2->first_attribute(); attr; attr = attr->next_attribute()){
				if (0 == strcmp("x", attr->name()))
				{
					edgeInfo.termendpos.x = DataConvert::world2map(atof(attr->value()));
				}
				else if (0 == strcmp("y", attr->name()))
				{
					edgeInfo.termendpos.y = DataConvert::world2map(atof(attr->value()));
				}
			}
			
		}

		rapidxml::xml_node<>* distance = edge->first_node("distance");
		if (distance)	{
			edgeInfo.length = DataConvert::world2map(atof(distance->value()));
		}
		rapidxml::xml_node<>* realdistance = edge->first_node("real_distance");
		if (realdistance)	{
			edgeInfo.reLength = DataConvert::world2map(atof(realdistance->value()));
		}
		rapidxml::xml_node<>* speed_limit = edge->first_node("speed_limit");
		if (speed_limit)
		{
			rapidxml::xml_node<>* segment = speed_limit->first_node();
			if (segment) //如果存在分段速度，则先将默认的速度清除
			{
				edgeInfo.speed.clear();
			}
			for (; segment; segment = segment->next_sibling())
			{
				EDGESPEED speed_seg;
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
				edgeInfo.speed.push_back(speed_seg);
			}
		}

		rapidxml::xml_node<>* disperse_point = edge->first_node("disperse_point");
		if (disperse_point)
		{
			rapidxml::xml_node<>* point = disperse_point->first_node();
			for (; point; point = point->next_sibling())
			{
				POSPOINT pt_temp;
				for (rapidxml::xml_attribute<char> * attr = point->first_attribute(); attr; attr = attr->next_attribute()){
					if (0 == strcmp("x", attr->name()))
					{
						pt_temp.x = DataConvert::world2map(atof(attr->value()));
					}
					else if (0 == strcmp("y", attr->name()))
					{
						pt_temp.y = DataConvert::world2map(atof(attr->value()));
					}
					else if (0 == strcmp("distance_to_start", attr->name()))
					{
						pt_temp.distance_to_start = DataConvert::world2map(atof(attr->value()));
					}
				}				
				
				edgeInfo.disperse_point.push_back(pt_temp);
			}
		}
		rapidxml::xml_node<>* wops = edge->first_node("wops");
		if (wops)
		{
			rapidxml::xml_node<>* wop = wops->first_node();
			for (; wop; wop = wop->next_sibling())
			{
				int wop_id = atoi(wop->value());
				edgeInfo.wop_list.push_back(wop_id);
				WOPATTR wop_attr;
				wop_attr.id = wop_id;
				rapidxml::xml_attribute<char>* attr = wop->first_attribute();
				if (attr){
					wop_attr.enable = atoi(attr->value());
					uint32_t i = 0;
					for (attr = attr->next_attribute(); attr; attr = attr->next_attribute(),++i){
						if (wop_attr.values.size() > i){
							wop_attr.values[i] = atoi(attr->value());
						}
					}
				}
				edgeInfo.wop_attr_list.push_back(wop_attr);
			}
		}
		layoutData.edgeList.push_back(edgeInfo);
	}
	return SUCCESS;
}

int RWXml::get_xml_wops(rapidxml::xml_node<>* root, LayoutData& layoutData)
{
	rapidxml::xml_node<>* wops = root->first_node("wops");
	if (!wops)
	{
		return SECTION_FAILED;
	}
	for (rapidxml::xml_node<>* wop = wops->first_node(); wop; wop = wop->next_sibling())//获取wop下的所有属性
	{
		double table[4];
		for (rapidxml::xml_attribute<char> * attr = wop->first_attribute(); attr; attr = attr->next_attribute())
		{
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
		WOP wop_tmp;
		wop_tmp.id = table[0];
		wop_tmp.direction = static_cast<Direction>((int)table[1]);
		wop_tmp.angle_type = static_cast<AngleType>((int)table[2]);
		wop_tmp.angle = DataConvert::radian2angle(table[3]);
		layoutData.wopList.push_back(wop_tmp);
	}
	return SUCCESS;
}

int RWXml::get_xml_links(rapidxml::xml_node<>* root, LayoutData& layoutData)
{
	rapidxml::xml_node<>* links = root->first_node("links");
	if (!links)
	{
		return SECTION_FAILED;
	}
	for (rapidxml::xml_node<>* link = links->first_node(); link; link = link->next_sibling())//获取links下的所有节点
	{
		int i = 0;
		double table[6];
		for (rapidxml::xml_attribute<char> * attr = link->first_attribute(); attr; attr = attr->next_attribute())
		{
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
		LINK linkInfo;
		linkInfo.id = static_cast<int>(table[0]);
		linkInfo.link_from.edge_id = static_cast<int>(table[1]);
		linkInfo.link_from.wop_id = static_cast<int>(table[2]);
		linkInfo.link_to.edge_id = static_cast<int>(table[3]);
		linkInfo.link_to.wop_id = static_cast<int>(table[4]);
		linkInfo.convert_cost = DataConvert::world2map(table[5]);
		layoutData.linkList.push_back(linkInfo);
	}
	return SUCCESS;
}
int RWXml::get_xml_polygons(rapidxml::xml_node<>* root, LayoutData& layoutData)
{
	rapidxml::xml_node<>* polygons = root->first_node("polygons");
	if (!polygons)
	{
		return NO_POLYGON;
		//return SECTION_FAILED;
	}

	for (rapidxml::xml_node<>* polygon = polygons->first_node(); polygon; polygon = polygon->next_sibling())//获取edges节点下的每个edge
	{
		POLYGON PolygonInfo;
		rapidxml::xml_node<>* id = polygon->first_node("id");
		if (id)	{
			PolygonInfo.id = atoi(id->value());
		}

		rapidxml::xml_node<>* disperse_point = polygon->first_node("polygon_vertex");
		if (disperse_point)
		{
			rapidxml::xml_node<>* point = disperse_point->first_node();
			for (; point; point = point->next_sibling())
			{
				PolygonVertex pt_temp;
				for (rapidxml::xml_attribute<char> * attr = point->first_attribute(); attr; attr = attr->next_attribute()){
					if (0 == strcmp("x", attr->name()))
					{
						pt_temp.x = DataConvert::world2map(atof(attr->value()));
					}
					else if (0 == strcmp("y", attr->name()))
					{
						pt_temp.y = DataConvert::world2map(atof(attr->value()));
					}
				}

				PolygonInfo.vertexes.push_back(pt_temp);
			}
		}
		layoutData.polygonList.push_back(PolygonInfo);
	}
	return SUCCESS;
}

rapidxml::xml_node<>* RWXml::create_xml_type(rapidxml::xml_document<>& document, const LayoutData& layoutData)
{
	char chText[STRING_SIZE] = { 0 };
	sprintf_s(chText, STRING_SIZE, "%d", layoutData.type);
	rapidxml::xml_node<>* type = document.allocate_node(rapidxml::node_element, "type", document.allocate_string(chText));
	return type;
}
rapidxml::xml_node<>* RWXml::create_xml_nodes(rapidxml::xml_document<>& document, const LayoutData& layoutData)
{
	rapidxml::xml_node<>* nodes = document.allocate_node(rapidxml::node_element, "nodes", NULL);
	if (!nodes)
	{
		return NULL;
	}
	for (auto nodeInfo : layoutData.nodeList)
	{
		rapidxml::xml_node<>* node = document.allocate_node(rapidxml::node_element, "node", NULL);
		if (node)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%d", nodeInfo.node_id);
			node->append_attribute(document.allocate_attribute("id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(nodeInfo.pos_x));
			node->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(nodeInfo.pos_y));
			node->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", nodeInfo.spin);
			node->append_attribute(document.allocate_attribute("spin", document.allocate_string(chText)));

			nodes->append_node(node);
		}
	}
	return nodes;
}
rapidxml::xml_node<>* RWXml::create_xml_edges(rapidxml::xml_document<>& document, const LayoutData& layoutData)
{
	rapidxml::xml_node<>* edges = document.allocate_node(rapidxml::node_element, "edges", NULL);
	if (!edges)
	{
		return NULL;
	}
	for (auto edgeInfo : layoutData.edgeList)
	{
		rapidxml::xml_node<>* edge = document.allocate_node(rapidxml::node_element, "edge", NULL);
		if (edge)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%d", edgeInfo.id);
			edge->append_node(document.allocate_node(rapidxml::node_element, "id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", edgeInfo.start_node_id);
			edge->append_node(document.allocate_node(rapidxml::node_element, "start_node_id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", edgeInfo.end_node_id);
			edge->append_node(document.allocate_node(rapidxml::node_element, "end_node_id", document.allocate_string(chText)));

			rapidxml::xml_node<>* ctrl_point1 = document.allocate_node(rapidxml::node_element, "ctrl_point1", NULL);
			if (ctrl_point1)
			{
				sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.ctrl_point1.x));
				ctrl_point1->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
				sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.ctrl_point1.y));
				ctrl_point1->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
				edge->append_node(ctrl_point1);
			}
			rapidxml::xml_node<>* ctrl_point2 = document.allocate_node(rapidxml::node_element, "ctrl_point2", NULL);
			if (ctrl_point2)
			{
				sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.ctrl_point2.x));
				ctrl_point2->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
				sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.ctrl_point2.y));
				ctrl_point2->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
				edge->append_node(ctrl_point2);
			}
			rapidxml::xml_node<>* term_point1 = document.allocate_node(rapidxml::node_element, "start_node", NULL);
			if (term_point1)
			{
				sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.termstpos.x));
				term_point1->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
				sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.termstpos.y));
				term_point1->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
				edge->append_node(term_point1);
			}
			rapidxml::xml_node<>* term_point2 = document.allocate_node(rapidxml::node_element, "end_node", NULL);
			if (term_point2)
			{
				sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.termendpos.x));
				term_point2->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
				sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.termendpos.y));
				term_point2->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
				edge->append_node(term_point2);
			}

			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.length));
			edge->append_node(document.allocate_node(rapidxml::node_element, "distance", document.allocate_string(chText)));

			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(edgeInfo.reLength));
			edge->append_node(document.allocate_node(rapidxml::node_element, "real_distance", document.allocate_string(chText)));

			rapidxml::xml_node<>* speed_limit = document.allocate_node(rapidxml::node_element, "speed_limit", NULL);
			if (speed_limit)
			{
				for (auto segSpeed : edgeInfo.speed)
				{
					rapidxml::xml_node<>* segment = document.allocate_node(rapidxml::node_element, "segment", NULL);
					if (segment)
					{
						sprintf_s(chText, STRING_SIZE, "%lf", segSpeed.percent);
						segment->append_attribute(document.allocate_attribute("percent", document.allocate_string(chText)));
						sprintf_s(chText, STRING_SIZE, "%lf", segSpeed.speed);
						segment->append_attribute(document.allocate_attribute("speed", document.allocate_string(chText)));
						speed_limit->append_node(segment);
					}
				}
				edge->append_node(speed_limit);
			}

			rapidxml::xml_node<>* disperse_point = document.allocate_node(rapidxml::node_element, "disperse_point", NULL);
			if (disperse_point)
			{
				for (auto pointInfo : edgeInfo.disperse_point)
				{
					rapidxml::xml_node<>* point = document.allocate_node(rapidxml::node_element, "point", NULL);
					if (point)
					{
						sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(pointInfo.x));
						point->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
						sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(pointInfo.y));
						point->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
						sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(pointInfo.distance_to_start));
						point->append_attribute(document.allocate_attribute("distance_to_start", document.allocate_string(chText)));
						disperse_point->append_node(point);
					}
				}
				edge->append_node(disperse_point);
			}

			rapidxml::xml_node<>* wops = document.allocate_node(rapidxml::node_element, "wops", NULL);
			if (wops)
			{
				for (auto wopId : edgeInfo.wop_list)
				{
					sprintf_s(chText, STRING_SIZE, "%d", wopId);
					rapidxml::xml_node<>* wop = document.allocate_node(rapidxml::node_element, "wop", document.allocate_string(chText));
					if (wop){
						for (auto wop_attr : edgeInfo.wop_attr_list){
							if (wopId == wop_attr.id){
								sprintf_s(chText, STRING_SIZE, "%d", wop_attr.enable);
								wop->append_attribute(document.allocate_attribute("enable", document.allocate_string(chText)));
								for (size_t i = 0; i < wop_attr.values.size(); ++i){
									sprintf_s(chText, STRING_SIZE, "%d", wop_attr.values[i]);
									char chName[32] = { 0 };
									sprintf_s(chName, STRING_SIZE, "val%d", i);
									wop->append_attribute(document.allocate_attribute(document.allocate_string(chName), document.allocate_string(chText)));
								}
							}
						}
						wops->append_node(wop);
					}
					
				}
				edge->append_node(wops);
			}
			edges->append_node(edge);
		}
	}
	return edges;
}

rapidxml::xml_node<>* RWXml::create_xml_wops(rapidxml::xml_document<>& document, const LayoutData& layoutData)
{
	rapidxml::xml_node<>* wops = document.allocate_node(rapidxml::node_element, "wops", NULL);
	if (!wops)
	{
		return NULL;
	}
	for (auto wopInfo : layoutData.wopList)
	{
		rapidxml::xml_node<>* wop = document.allocate_node(rapidxml::node_element, "wop", NULL);
		if (wop)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%d", wopInfo.id);
			wop->append_attribute(document.allocate_attribute("id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", wopInfo.direction);
			wop->append_attribute(document.allocate_attribute("direction", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", wopInfo.angle_type);
			wop->append_attribute(document.allocate_attribute("angle_type", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::angle2radian(wopInfo.angle));
			wop->append_attribute(document.allocate_attribute("angle", document.allocate_string(chText)));

			wops->append_node(wop);
		}
	}
	return wops;
}

rapidxml::xml_node<>* RWXml::create_xml_links(rapidxml::xml_document<>& document, const LayoutData& layoutData)
{
	rapidxml::xml_node<>* links = document.allocate_node(rapidxml::node_element, "links", NULL);
	if (!links)
	{
		return NULL;
	}
	for (auto linkInfo : layoutData.linkList)
	{
		rapidxml::xml_node<>* link = document.allocate_node(rapidxml::node_element, "link", NULL);
		if (link)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%d", linkInfo.id);
			link->append_attribute(document.allocate_attribute("id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", linkInfo.link_from.edge_id);
			link->append_attribute(document.allocate_attribute("from_edge", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", linkInfo.link_from.wop_id);
			link->append_attribute(document.allocate_attribute("from_wop", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", linkInfo.link_to.edge_id);
			link->append_attribute(document.allocate_attribute("to_edge", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", linkInfo.link_to.wop_id);
			link->append_attribute(document.allocate_attribute("to_wop", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(linkInfo.convert_cost));
			link->append_attribute(document.allocate_attribute("cost", document.allocate_string(chText)));

			links->append_node(link);
		}
	}
	return links;
}

rapidxml::xml_node<>* RWXml::create_xml_polygons(rapidxml::xml_document<>& document, const LayoutData& layoutData)
{
	rapidxml::xml_node<>* polygons = document.allocate_node(rapidxml::node_element, "polygons", NULL);
	if (!polygons)
	{
		return NULL;
	}
	for (auto polygoninfo : layoutData.polygonList)
	{
		rapidxml::xml_node<>* polygon = document.allocate_node(rapidxml::node_element, "polygon", NULL);
		if (polygon)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%d", polygoninfo.id);
			polygon->append_node(document.allocate_node(rapidxml::node_element, "id", document.allocate_string(chText)));
			
			rapidxml::xml_node<>* dispoint = document.allocate_node(rapidxml::node_element, "polygon_vertex", NULL);
			if (dispoint)
			{
				for (auto pointInfo : polygoninfo.vertexes)
				{
					rapidxml::xml_node<>* point = document.allocate_node(rapidxml::node_element, "point", NULL);
					if (point)
					{
						sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(pointInfo.x));
						point->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
						sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(pointInfo.y));
						point->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
						dispoint->append_node(point);
					}
				}
				polygon->append_node(dispoint);
			}
			polygons->append_node(polygon);
		}
	}
	return polygons;
	
			
}

rapidxml::xml_node<>* RWXml::create_xml_anchors(rapidxml::xml_document<>& document, const std::vector<ANCHORINFO>& vecAnchor)
{
	rapidxml::xml_node<>* anchors = document.allocate_node(rapidxml::node_element, "anchors", NULL);
	if (!anchors)
	{
		return NULL;
	}
	for (auto anchorInfo : vecAnchor)
	{
		rapidxml::xml_node<>* anchor = document.allocate_node(rapidxml::node_element, "anchor", NULL);
		if (anchor)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%d", anchorInfo.id);
			anchor->append_attribute(document.allocate_attribute("id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", anchorInfo.edge_id);
			anchor->append_attribute(document.allocate_attribute("edge_id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%d", anchorInfo.wop_id);
			anchor->append_attribute(document.allocate_attribute("wop_id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(anchorInfo.pos_x));
			anchor->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(anchorInfo.pos_y));
			anchor->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::angle2radian(anchorInfo.angle));
			anchor->append_attribute(document.allocate_attribute("angle", document.allocate_string(chText)));

			anchors->append_node(anchor);
		}
	}
	return anchors;
}

int RWXml::get_xml_anchors(rapidxml::xml_node<>* root, std::vector<ANCHORINFO>& vecAnchor)
{
	rapidxml::xml_node<>* anchors = root->first_node("anchors");
	if (!anchors)
	{
		return SECTION_FAILED;
	}

	for (rapidxml::xml_node<>* anchor = anchors->first_node(); anchor; anchor = anchor->next_sibling())//获取anchors下的所有属性
	{
		int i = 0;
		double table[6];
		for (rapidxml::xml_attribute<char> * attr = anchor->first_attribute(); attr; attr = attr->next_attribute())
		{
			if (i >= 6)
			{
				break;
			}
			table[i] = atof(attr->value());
			i++;
		}
		ANCHORINFO anchorInfo;
		anchorInfo.id = static_cast<int>(table[0]);
		anchorInfo.edge_id = static_cast<int>(table[1]);
		anchorInfo.wop_id = static_cast<int>(table[2]);
		anchorInfo.pos_x = DataConvert::world2map(table[3]);
		anchorInfo.pos_y = DataConvert::world2map(table[4]);
		anchorInfo.angle = DataConvert::radian2angle(table[5]);

		vecAnchor.push_back(anchorInfo);
	}
	return SUCCESS;
}

int RWXml::get_xml_vehicles(rapidxml::xml_node<>* root, std::vector<VEHICLEINFO>& vecVehicle)
{
	rapidxml::xml_node<>* vehicles = root->first_node("vehicles");
	if (!vehicles)
	{
		return SECTION_FAILED;
	}

	for (rapidxml::xml_node<>* vehicle = vehicles->first_node(); vehicle; vehicle = vehicle->next_sibling())//获取vehicles下的所有属性
	{
		VEHICLEINFO vehicleInfo;
		rapidxml::xml_attribute<char> * attr = vehicle->first_attribute("id");
		if (attr)
		{
			vehicleInfo.vehicle_id = atoi(attr->value());
		}
		attr = vehicle->first_attribute("ip");
		if (attr)
		{
			vehicleInfo.vehicle_ip = attr->value();
		}
		attr = vehicle->first_attribute("port");
		if (attr)
		{
			vehicleInfo.vehicle_port = atoi(attr->value());
		}

		vecVehicle.push_back(vehicleInfo);
	}
	return SUCCESS;
}

rapidxml::xml_node<>* RWXml::create_xml_vehicles(rapidxml::xml_document<>& document, const std::vector<VEHICLEINFO>& vecVehicle)
{
	rapidxml::xml_node<>* vehicles = document.allocate_node(rapidxml::node_element, "vehicles", NULL);
	if (!vehicles)
	{
		return NULL;
	}
	for (auto vehicleInfo : vecVehicle)
	{
		rapidxml::xml_node<>* vehicle = document.allocate_node(rapidxml::node_element, "vehicle", NULL);
		if (vehicle)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%d", vehicleInfo.vehicle_id);
			vehicle->append_attribute(document.allocate_attribute("id", document.allocate_string(chText)));
			vehicle->append_attribute(document.allocate_attribute("ip", document.allocate_string(vehicleInfo.vehicle_ip.c_str())));
			sprintf_s(chText, STRING_SIZE, "%d", vehicleInfo.vehicle_port);
			vehicle->append_attribute(document.allocate_attribute("port", document.allocate_string(chText)));

			vehicles->append_node(vehicle);
		}
	}
	return vehicles;
}

int RWXml::get_xml_search_type(rapidxml::xml_node<>* root, int& search_type)
{
	rapidxml::xml_node<>* type = root->first_node("search_type");
	if (!type)
	{
		return SECTION_FAILED;
	}

	search_type = atoi(type->value());
	return SUCCESS;
}

rapidxml::xml_node<>* RWXml::create_xml_search_type(rapidxml::xml_document<>& document, int search_type)
{
	char chText[STRING_SIZE] = { 0 };
	sprintf_s(chText, STRING_SIZE, "%d", search_type);
	rapidxml::xml_node<>* type = document.allocate_node(rapidxml::node_element, "search_type", document.allocate_string(chText));
	return type;
}

int RWXml::get_xml_docks(rapidxml::xml_node<>* root, std::vector<ANCHORINFO>& vecAnchor)
{
	for (rapidxml::xml_node<>* dock = root->first_node(); dock; dock = dock->next_sibling())
	{
		ANCHORINFO anchorInfo;
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
				anchorInfo.pos_x = DataConvert::world2map(atof(attr->value()));
			}
			attr = pos->first_attribute("y");
			if (attr)
			{
				anchorInfo.pos_y = DataConvert::world2map(atof(attr->value()));
			}
			attr = pos->first_attribute("angle");
			if (attr)
			{
				anchorInfo.angle = DataConvert::radian2angle(atof(attr->value()));
			}
		}
		rapidxml::xml_node<>* upl = dock->first_node("upl");
		if (upl)
		{
			rapidxml::xml_attribute<char>* attr = upl->first_attribute("edge_id");
			if (attr)
			{
				anchorInfo.edge_id = atoi(attr->value());
			}
			attr = upl->first_attribute("percentage");
			if (attr)
			{
				anchorInfo.percent = atof(attr->value());
			}
			attr = upl->first_attribute("aoa");
			if (attr)
			{
				anchorInfo.aoa = DataConvert::radian2angle(atof(attr->value()));
			}
		}
		get_xml_opts(dock, anchorInfo);
		vecAnchor.push_back(anchorInfo);
	}
	return SUCCESS;
}

int RWXml::get_xml_opts(rapidxml::xml_node<>* parent, ANCHORINFO& anchorInfo)
{
	rapidxml::xml_node<>* opts = parent->first_node("opts");
	if (!opts)
	{
		return SECTION_FAILED;
	}
	for (rapidxml::xml_node<>* opt = opts->first_node(); opt; opt = opt->next_sibling())
	{
		if (!opt)
		{
			continue;
		}
		OPTINFO optInfo;
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
		int i = 0;
		for (rapidxml::xml_node<>* param = opt->first_node("p0"); param; param = param->next_sibling())
		{
			if (!param) continue;
			double val = atof(param->value());
			optInfo.params[i++] = (uint64_t)(int64_t)(val * 1000);
		}
		anchorInfo.opts.push_back(optInfo);
	}
	return SUCCESS;
}

rapidxml::xml_node<>* RWXml::create_xml_docks(rapidxml::xml_document<>& document, const std::vector<ANCHORINFO>& vecAnchor)
{
	rapidxml::xml_node<>* docks = document.allocate_node(rapidxml::node_element, "docks", NULL);
	if (!docks)
	{
		return NULL;
	}
	for (auto anchorInfo : vecAnchor)
	{
		rapidxml::xml_node<>* dock = document.allocate_node(rapidxml::node_element, "dock", NULL);
		if (!dock)
		{
			continue;
		}
		char chText[STRING_SIZE] = { 0 };
		sprintf_s(chText, STRING_SIZE, "%d", anchorInfo.id);
		rapidxml::xml_node<>* id = document.allocate_node(rapidxml::node_element, "id", document.allocate_string(chText));
		dock->append_node(id);
		rapidxml::xml_node<>* pos = document.allocate_node(rapidxml::node_element, "pos", NULL);
		if (pos)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(anchorInfo.pos_x));
			pos->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(anchorInfo.pos_y));
			pos->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::angle2radian(anchorInfo.angle));
			pos->append_attribute(document.allocate_attribute("angle", document.allocate_string(chText)));

			dock->append_node(pos);
		}
		rapidxml::xml_node<>* upl = document.allocate_node(rapidxml::node_element, "upl", NULL);
		if (upl)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%d", anchorInfo.edge_id);
			upl->append_attribute(document.allocate_attribute("edge_id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", anchorInfo.percent);
			upl->append_attribute(document.allocate_attribute("percentage", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::angle2radian(anchorInfo.aoa));
			upl->append_attribute(document.allocate_attribute("aoa", document.allocate_string(chText)));

			dock->append_node(upl);
		}
		rapidxml::xml_node<>* opts = create_xml_opts(document, anchorInfo);
		dock->append_node(opts);

		docks->append_node(dock);
	}
	return docks;
}

rapidxml::xml_node<>* RWXml::create_xml_opts(rapidxml::xml_document<>& document, const ANCHORINFO& anchorInfo)
{
	rapidxml::xml_node<>* opts = document.allocate_node(rapidxml::node_element, "opts", NULL);
	if (!opts)
	{
		return NULL;
	}
	for (auto optInfo : anchorInfo.opts)
	{
		rapidxml::xml_node<>* opt = document.allocate_node(rapidxml::node_element, "opt", NULL);
		if (!opt)
		{
			continue;
		}
		char chText[STRING_SIZE] = { 0 };
		sprintf_s(chText, STRING_SIZE, "%d", optInfo.id);
		rapidxml::xml_node<>* id = document.allocate_node(rapidxml::node_element, "id", document.allocate_string(chText));
		opt->append_node(id);
		sprintf_s(chText, STRING_SIZE, "%d", optInfo.type);
		rapidxml::xml_node<>* type = document.allocate_node(rapidxml::node_element, "type", document.allocate_string(chText));
		opt->append_node(type);

		for (int i = 0; i < OPT_COUNT; ++i)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%lf",(double)((int64_t)optInfo.params[i])/1000);
			char chName[STRING_SIZE] = { 0 };
			sprintf_s(chName, "p%d", i);
			rapidxml::xml_node<>* param = document.allocate_node(rapidxml::node_element, document.allocate_string(chName),
				document.allocate_string(chText));
			opt->append_node(param);
		}

		opts->append_node(opt);
	}
	return opts;
}

int RWXml::write_QRcode_data(const std::string& filePath, const std::vector<CODEPOSXYA>&codeData)
{
	//FILE *fp = nullptr;
	//fopen_s(&fp,filePath.c_str(), "w");
	//if (fwrite(&codeData[0], sizeof(CODEPOSXYA), 1, fp) < 0){
	//	fclose(fp);
	//	return -1;
	//}
	//fclose(fp);
	//return 0;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* root = document.allocate_node(rapidxml::node_element, "root", NULL);
	if (!root)
	{
		return ALLOC_FAILED;
	}
	rapidxml::xml_node<>* nodes = document.allocate_node(rapidxml::node_element, "QRcodes", NULL);
	if (!nodes)
	{
		return ALLOC_FAILED;
	}
	for (auto codeInfo : codeData)
	{
		rapidxml::xml_node<>* node = document.allocate_node(rapidxml::node_element, "QRcode", NULL);
		if (node)
		{
			char chText[STRING_SIZE] = { 0 };
			sprintf_s(chText, STRING_SIZE, "%d", codeInfo.pos_id);
			node->append_attribute(document.allocate_attribute("id", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(codeInfo.pos_x));
			node->append_attribute(document.allocate_attribute("x", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", DataConvert::map2world(codeInfo.pos_y));
			node->append_attribute(document.allocate_attribute("y", document.allocate_string(chText)));
			sprintf_s(chText, STRING_SIZE, "%lf", codeInfo.angle);
			node->append_attribute(document.allocate_attribute("angle", document.allocate_string(chText)));

			nodes->append_node(node);
		}
	}
	root->append_node(nodes);
	document.append_node(root);

	std::ofstream outfile(filePath.c_str());
	outfile << document;
	return SUCCESS;

}