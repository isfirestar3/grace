#include "search_path.h"
#include "log.h"

NavSearch::NavSearch()
{

}

NavSearch::~NavSearch()
{
}

void NavSearch::get_user_id()
{
	user_id = PathSearch_GetUserId();
}

void NavSearch::TransformData(const LAYOUTDATA& layoutData, PathSearch_Wop*& wopArray, 
	PathSearch_LINK*& linkArray,PathSearch_EDGEINFO*& edgeArray)
{
	int i = 0;
	for (auto &edgeInfo : layoutData.edgeList)
	{
		for (auto &wopId : edgeInfo.wop_list)
		{
			WOP wop;
			DATAMNG->GetWopById(wopId, wop);
			wopArray[i].edge_id = edgeInfo.id;
			wopArray[i].wop_id = wopId;
			wopArray[i].direction = static_cast<PathSearch_Direction>(wop.direction);
			wopArray[i].angle_type = static_cast<PathSearch_AngleType>(wop.angle_type);
			wopArray[i].angle = wop.angle;
			//wopArray[i].distance = edgeInfo.length;
			if (edgeInfo.reLength > -0.01){
				wopArray[i].distance = edgeInfo.reLength;
			}
			else{
				wopArray[i].distance = edgeInfo.length;
			}
			++i;
		}

	}
	i = 0;
	for (auto &linkInfo : layoutData.linkList)
	{
		linkArray[i].link_id = linkInfo.id;
		linkArray[i].link_from.edge_id = linkInfo.link_from.edge_id;
		linkArray[i].link_from.wop_id = linkInfo.link_from.wop_id;
		linkArray[i].link_to.edge_id = linkInfo.link_to.edge_id;
		linkArray[i].link_to.wop_id = linkInfo.link_to.wop_id;
		linkArray[i].convert_cost = linkInfo.convert_cost;
		++i;
	}
	TransfromEdge(layoutData, edgeArray);
}

void NavSearch::TransfromEdge(const LAYOUTDATA& layoutData, PathSearch_EDGEINFO*& edgeArray)
{
	int i = 0;
	for (auto &edgeInfo : layoutData.edgeList)
	{
		edgeArray[i].id = edgeInfo.id;
		edgeArray[i].start_node_id = edgeInfo.start_node_id;
		edgeArray[i].end_node_id = edgeInfo.end_node_id;
		edgeArray[i].direction = edgeInfo.direction;
		edgeArray[i].backward = edgeInfo.backward;
		TransfromPoint(edgeInfo.ctrl_point1, edgeArray[i].ctrl_point1);
		TransfromPoint(edgeInfo.ctrl_point2, edgeArray[i].ctrl_point2);
		TransfromPoint(edgeInfo.termstpos, edgeArray[i].termstpos);
		TransfromPoint(edgeInfo.termendpos, edgeArray[i].termendpos);
		edgeArray[i].terminal_start_percent = edgeInfo.terminal_start_percent;
		edgeArray[i].terminal_end_percent = edgeInfo.terminal_end_percent;
		edgeArray[i].priority = edgeInfo.priority;
		edgeArray[i].type = edgeInfo.type;
		//edgeArray[i].length = edgeInfo.length;
		if (edgeInfo.reLength > -0.01){
			edgeArray[i].length = edgeInfo.reLength;
		}
		else{
			edgeArray[i].length = edgeInfo.length;
		}
		edgeArray[i].speed.clear();
		for (auto &speedInfo : edgeInfo.speed)
		{
			PathSearch_EDGESPEED edgeSpeed;
			edgeSpeed.speed = speedInfo.speed;
			edgeSpeed.percent = speedInfo.percent;
			edgeArray[i].speed.push_back(edgeSpeed);
		}
		edgeArray[i].disperse_point.clear();
		for (auto &pointInfo : edgeInfo.disperse_point)
		{
			PathSearch_POSPOINT point;
			TransfromPoint(pointInfo, point);
			edgeArray[i].disperse_point.push_back(point);
		}
		edgeArray[i].wop_list.clear();
		for (auto &wop_id : edgeInfo.wop_list)
		{
			edgeArray[i].wop_list.push_back(wop_id);
		}
		++i;
	}
}

void NavSearch::TransfromPoint(const POSPOINT& posPoint, PathSearch_POSPOINT& pos)
{
	pos.x = posPoint.x;
	pos.y = posPoint.y;
	pos.distance_to_start = posPoint.distance_to_start;
}

//初始化导航所需要的图数据，type为导航类型0
bool NavSearch::InitNavigationData(const LAYOUTDATA& layoutData, int type, const char* filePath,
	int(*FinishLevelCB)(const unsigned int iPercent, const void *pUsr), void* pUsr)
{
	int wop_num = 0;
	for (auto &edgeInfo : layoutData.edgeList)
	{
		wop_num += static_cast<int>(edgeInfo.wop_list.size());//所有边的wop总和
	}
	int edge_num = static_cast<int>(layoutData.edgeList.size());
	int link_num = static_cast<int>(layoutData.linkList.size());
	if (wop_num <= 0 || link_num < 0 || edge_num <= 0)//当只有一条边时，link为0,但是可以进行搜路，所以允许link为0
	{
		return false;
	}
	PathSearch_Wop* wopArray = NULL;
	PathSearch_LINK* linkArray = NULL;
	PathSearch_EDGEINFO* edgeArray = NULL;
	try
	{
		wopArray = new PathSearch_Wop[wop_num];
		linkArray = new PathSearch_LINK[link_num];
		edgeArray = new PathSearch_EDGEINFO[edge_num];
	}
	catch (...)
	{
		return false;
	}
	TransformData(layoutData, wopArray, linkArray,edgeArray);

	PathSearch_SetType(user_id, static_cast<PathSearch_Type>(type));//设置算法类型
	PathSearch_SetWops(user_id, wopArray, wop_num); //设置wop数据
	PathSearch_SetEdges(user_id, edgeArray, edge_num);//设置边数据
	PathSearch_SetLinks(user_id, linkArray, link_num); //设置link（节点间关系）数据
	if (type == NAV_TYPE_FLOYD)
	{
		std::string file_path(filePath);
		int last_pos = file_path.find_last_of('.');
		std::string path = file_path.substr(0, last_pos + 1);
		std::string new_path = path + "dat";//根据layout文件路径，生成floyd计算结果的文件路径
		PathSearch_StartFolyd(user_id, new_path.c_str(), FinishLevelCB, pUsr);
	}
	

	delete[] wopArray;
	wopArray = NULL;
	delete[] linkArray;
	linkArray = NULL;

	return true;
}

void NavSearch::Point2UplList(const position_t& pos_t, std::vector<PathSearch_UPL>& upl_list)
{
	UPL map_upl;
	for (auto&iter : dock_data_.anchorList){
		if (abs(pos_t.x_ - iter.pos_x)<0.05&&abs(pos_t.y_-iter.pos_y)<0.05){
			map_upl.edge_id = iter.edge_id;		
		}
	}
	if (edge_id_ != 0){
		map_upl.edge_id = edge_id_;
		edge_id_ = 0;
	}
	PT_ALG::point_mapping_upl(pos_t, map_upl);//此函数获取的upl中的wop是经过角度换算的当前最优wop
	EDGEINFO edgeInfo;
	DATAMNG->GetEdgeById(map_upl.edge_id, edgeInfo);//我们需要将点所在的所有wop全部拿出来进行搜路，从而搜出最佳路线
	for (auto wopId : edgeInfo.wop_list)
	{
		WOP wop;
		DATAMNG->GetWopById(wopId, wop);
		position_t p_on_edge;
		PT_ALG::point_on_edge(map_upl.edge_id, pos_t, p_on_edge);
		double wop_global_angle = (wop.angle >= 0.0) ? wop.angle : (wop.angle + 360.0); //wop的全局角度
		if (wop.angle_type == CONST_ANGLE)
		{
			wop_global_angle = DataConvert::radian2angle(p_on_edge.angle_) + wop.angle;
			wop_global_angle = PT_ALG::angle_to_normal(wop_global_angle);//将角度全部转化为0-360的值
		}
		double angle_err = fabs(wop_global_angle - pos_t.angle_);
		angle_err = PT_ALG::angle_to_normal180(angle_err); //转成0-180以内的角度差
		if (angle_err <= 40.0)  //只有角度差小于40度的wop才是可选的WOP
		{
			PathSearch_UPL upl;
			upl.edge_id = map_upl.edge_id;
			upl.wop_id = wopId;
			upl.fPercent = map_upl.percent;
			upl.fAoa = map_upl.aoa;

			upl_list.push_back(upl);

		}
	}
}

double NavSearch::SearchNavPath(const position_t& src_pos, const position_t& dest_pos, 
	std::vector<UPL>& path_list, PathSearch_UPL& dest_upl_best)
{
	std::vector<PathSearch_UPL> start_upl_list;
	Point2UplList(src_pos, start_upl_list);
	nspinfo << "all adaptive start upl list :";
	for (auto start_upl : start_upl_list)
	{
		nspinfo << "edge_id=" << start_upl.edge_id << ",wop_id=" << start_upl.wop_id << ",percent=" << start_upl.fPercent;
	}
	std::vector<PathSearch_UPL> target_upl_list;
	Point2UplList(dest_pos, target_upl_list);
	nspinfo << "all adaptive dest upl list :";
	for (auto dest_upl : target_upl_list)
	{
		nspinfo << "edge_id=" << dest_upl.edge_id << ",wop_id=" << dest_upl.wop_id << ",percent=" << dest_upl.fPercent;
	}

	double distance_min = DISTANCE_INFINITY;
	std::vector<UPL> path_list_temp;
	PathSearch_UPL start_upl_best;
	//遍历所有的wop可能组合，找到距离最短的最优路径组合
	for (auto start_upl : start_upl_list)
	{
		for (auto target_upl : target_upl_list)
		{
			PathSearch_Wop* pathList = nullptr;
			int iLen = 0;
			float distance = 0.0;
			int iRet = PathSearch_GetPathByWop(user_id, start_upl, target_upl, &pathList, &iLen, &distance);
			if (distance < distance_min && distance > 0.0)
			{
				path_list_temp.clear();
				distance_min = distance;
				dest_upl_best = target_upl;
				start_upl_best = start_upl;
				for (int i = 0; i < iLen; ++i)
				{
					UPL upl;
					upl.edge_id = pathList[i].edge_id;
					upl.wop_id = pathList[i].wop_id;
					upl.percent = 1.0;
					if (i == 0)
					{
						upl.percent = start_upl.fPercent;
					}
					if (i == iLen - 1)
					{
						upl.percent = target_upl.fPercent;
					}

					path_list_temp.push_back(upl);
				}
			}
			PathSearch_ReleasePath(user_id, pathList);
		}
	}
	for (auto upl : path_list_temp)
	{
		path_list.push_back(upl);
	}
	if (distance_min < DISTANCE_INFINITY)
	{
		nspinfo << "start_upl_best:edge_id=" << start_upl_best.edge_id << ",wop_id="
			<< start_upl_best.wop_id << ",percent=" << start_upl_best.fPercent;
		nspinfo << "dest_upl_best:edge_id=" << dest_upl_best.edge_id << ",wop_id="
			<< dest_upl_best.wop_id << ",percent=" << dest_upl_best.fPercent;
	}
	
	return distance_min;
}
void NavSearch::set_docks_data(DOCKDATA& dockData)
{
	dock_data_ = dockData;
}
void NavSearch::set_start_edge(int edge_id)
{
	edge_id_ = edge_id;
}
double NavSearch::SearchNavPath(const position_t& src_pos, const PathSearch_UPL& dest_upl, std::vector<UPL>& path_list)
{
	//PathSearch_Position start_pos;
	//start_pos.x = src_pos.x_;
	//start_pos.y = src_pos.y_;
	//start_pos.angle = src_pos.angle_;
	//PathSearch_Wop* pathList = nullptr;
	//int iLen = 0;
	//float distance = 0.0;
	//if (PathSearch_GetPathBySPosAndEUpl(user_id, start_pos, dest_upl, &pathList, &iLen, &distance) < 0){
	//	return -1;
	//}
	//for (int i = 0; i < iLen; ++i)
	//{
	//	UPL upl;
	//	upl.edge_id = pathList[i].edge_id;
	//	upl.wop_id = pathList[i].wop_id;
	//	upl.percent = 1.0;
	//	if (i == iLen - 1)
	//	{
	//		upl.percent = dest_upl.fPercent;
	//	}
	//	path_list.push_back(upl);
	//}
	//double distance_min = (double)distance;
	//return distance_min;

	std::vector<PathSearch_UPL> start_upl_list;
	Point2UplList(src_pos, start_upl_list);
	nspinfo << "all adaptive start upl list :";
	for (auto start_upl : start_upl_list)
	{
		nspinfo << "edge_id=" << start_upl.edge_id << ",wop_id=" << start_upl.wop_id << ",percent=" << start_upl.fPercent;
	}
	double distance_min = DISTANCE_INFINITY;
	std::vector<UPL> path_list_temp;
	PathSearch_UPL start_upl_best;
	//遍历所有的wop可能组合，找到距离最短的最优路径组合
	for (auto start_upl : start_upl_list)
	{
		PathSearch_Wop* pathList = nullptr;
		int iLen = 0;
		float distance = 0.0;
		int iRet = PathSearch_GetPathByWop(user_id, start_upl, dest_upl, &pathList, &iLen, &distance);
		if (iRet < 0)
		{
			continue;
		}
		if (distance < distance_min && distance > 0.0)
		{
			distance_min = distance;
			path_list_temp.clear();
			start_upl_best = start_upl;
			for (int i = 0; i < iLen; ++i)
			{
				UPL upl;
				upl.edge_id = pathList[i].edge_id;
				upl.wop_id = pathList[i].wop_id;
				upl.percent = 1.0;
				if (i == 0)
				{
					upl.percent = start_upl.fPercent;
				}
				if (i == iLen - 1)
				{
					upl.percent = dest_upl.fPercent;
				}
				path_list_temp.push_back(upl);
			}
		}
		PathSearch_ReleasePath(user_id, pathList);
	}
	for (auto upl : path_list_temp)
	{
		path_list.push_back(upl);
	}
	if (distance_min < DISTANCE_INFINITY)
	{
		nspinfo << "start_upl_best:edge_id=" << start_upl_best.edge_id << ",wop_id="
			<< start_upl_best.wop_id << ",percent=" << start_upl_best.fPercent;
	}
	return distance_min;
}


