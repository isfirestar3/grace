#include "DataManage.h"


DataManage::DataManage()
{
}


DataManage::~DataManage()
{

}

void DataManage::AddNodeInfo(const NODEINFO& nodeInfo)
{
	if (nodeInfo.node_id <= 0)
	{
		return;
	}
	std::lock_guard<decltype(m_mutexNodeInfo)> guard(m_mutexNodeInfo);
	m_vecNodeInfo.push_back(nodeInfo);
}

void DataManage::AddQRcodeInfo(CODEPOSXYA&posInfo)
{
	m_vctCodeInfo.push_back(posInfo);
}

void DataManage::AddEdgeInfo(EDGEINFO& edgeInfo)
{
	if (edgeInfo.id <= 0)
	{
		return;
	}
	for( auto wopType : m_vecWopInfo ) {
		if( wopType.direction == FORWARD && wopType.angle == 0 ) {
			edgeInfo.wop_list.push_back( wopType.id );
			break;
		}
	}
	std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
	m_vecEdgeInfo.push_back(edgeInfo);
}

bool DataManage::UpdateNodeInfo(const NODEINFO& nodeInfo)
{
	if (nodeInfo.node_id <= 0)
	{
		return false;
	}
	std::lock_guard<decltype(m_mutexNodeInfo)> guard(m_mutexNodeInfo);
	for (auto &node : m_vecNodeInfo)
	{
		if (node.node_id == nodeInfo.node_id)
		{
			node = nodeInfo;
			return true;
		}
	}
	return false;
}

bool DataManage::UpdateEdgeInfo(EDGEINFO& edgeInfo)
{
	if (edgeInfo.id <= 0)
	{
		return false;
	}
	std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
	std::vector<EDGEINFO>::iterator itEdge = m_vecEdgeInfo.begin();
	for (; itEdge != m_vecEdgeInfo.end(); ++itEdge)
	{
		if ((*itEdge).id == edgeInfo.id)
		{
			*itEdge = edgeInfo;
			return true;
		}
	}
	return false;
}

void DataManage::GetAllNodeInfo(std::vector<NODEINFO>& vecNodeInfo)
{
	std::lock_guard<decltype(m_mutexNodeInfo)> guard(m_mutexNodeInfo);
	vecNodeInfo = m_vecNodeInfo;
}

void DataManage::GetAllEdgeInfo(std::vector<EDGEINFO>& vecEdgeInfo)
{
	std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
	vecEdgeInfo = m_vecEdgeInfo;
}

void DataManage::GetAllCodeInfo(std::vector<CODEPOSXYA>&vecCodeInfo)
{
	vecCodeInfo = m_vctCodeInfo;
}

void DataManage::GetLayoutData(LayoutData& layoutData)
{
	GetAllNodeInfo(layoutData.nodeList);
	GetAllEdgeInfo(layoutData.edgeList);
	GetAllWopInfo(layoutData.wopList);
	GetAllLinkInfo(layoutData.linkList);
}

void DataManage::SetAllNodeInfo(const std::vector<NODEINFO>& vecNodeInfo)
{
	std::lock_guard<decltype(m_mutexNodeInfo)> guard(m_mutexNodeInfo);
	m_vecNodeInfo = vecNodeInfo;
}

void DataManage::SetAllEdgeInfo(const std::vector<EDGEINFO>& vecEdgeInfo)
{
	std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
	m_vecEdgeInfo = vecEdgeInfo;
}

void DataManage::SetAllWopInfo(const std::vector<WOP>& vecWopInfo)
{
	if( vecWopInfo.size() == 0 ) {
		return;
	}
	std::lock_guard<decltype(m_mutexWopInfo)> guard(m_mutexWopInfo);
	if( m_vecWopInfo.size() > 0 ) {
		m_vecWopInfo.clear();
	}
	m_vecWopInfo = vecWopInfo;
}

void DataManage::SetLayoutData(const LayoutData& layoutData)
{
	SetAllNodeInfo(layoutData.nodeList);
	SetAllEdgeInfo(layoutData.edgeList);
	SetAllWopInfo(layoutData.wopList);
	SetAllLinkInfo(layoutData.linkList);
}

void DataManage::SetOperData(const DOCKDATA& dockData)
{
	SetAllAnchorInfo(dockData.anchorList);
}

void DataManage::GetOperData(DOCKDATA& dockData)
{
	GetAllAnchorInfo(dockData.anchorList);
}

void DataManage::SetConfigData(const CONFIGDATA& configData)
{
	SetAllVehicleInfo(configData.vehicleList);
	m_search_type = configData.search_type;
}

void DataManage::GetConfigData(CONFIGDATA& configData)
{
	GetAllVehicleInfo(configData.vehicleList);
	configData.search_type = m_search_type;
}

bool DataManage::IsWopExist(const WOP& wop)
{
	std::lock_guard<decltype(m_mutexWopInfo)> guard(m_mutexWopInfo);
	for (auto wopInfo : m_vecWopInfo)
	{
		if (wopInfo.direction == wop.direction && wopInfo.angle_type == wop.angle_type &&
			wopInfo.angle == wop.angle)
		{
			return true;
		}
	}
	return false;
}

void DataManage::GetAllWopInfo(std::vector<WOP>& vecWopInfo)
{
	std::lock_guard<decltype(m_mutexWopInfo)> guard(m_mutexWopInfo);
	vecWopInfo = m_vecWopInfo;
}

bool DataManage::GetWopById(int id, WOP& wop)
{
	std::lock_guard<decltype(m_mutexWopInfo)> guard(m_mutexWopInfo);
	for (auto wopInfo : m_vecWopInfo)
	{
		if (wopInfo.id == id)
		{
			wop = wopInfo;
			return true;
		}
	}
	return false;
}

void DataManage::SetAllLinkInfo(const std::vector<LINK>& vecLinkInfo)
{
	std::lock_guard<decltype(m_mutexLinkInfo)> guard(m_mutexLinkInfo);
	m_vecLinkInfo = vecLinkInfo;
}

void DataManage::GetAllLinkInfo(std::vector<LINK>& vecLinkInfo)
{
	std::lock_guard<decltype(m_mutexLinkInfo)> guard(m_mutexLinkInfo);
	vecLinkInfo = m_vecLinkInfo;
}

bool DataManage::GetEdgeById(int id, EDGEINFO& edgeInfo)
{
	std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
	for (auto edge : m_vecEdgeInfo)
	{
		if (edge.id == id)
		{
			edgeInfo = edge;
			return true;
		}
	}
	return false;
}

void DataManage::ConvertEdgeInfo(const EDGEINFO& edgeInfo, var__edge_t& edge_t)
{
	edge_t.id_ = edgeInfo.id;
	edge_t.start_node_id_ = edgeInfo.start_node_id;
	edge_t.end_node_id_ = edgeInfo.end_node_id;
	edge_t.control_point_1_.x_ = edgeInfo.ctrl_point1.x;
	edge_t.control_point_1_.y_ = edgeInfo.ctrl_point1.y;
	edge_t.control_point_2_.x_ = edgeInfo.ctrl_point2.x;
	edge_t.control_point_2_.y_ = edgeInfo.ctrl_point2.y;
	edge_t.start_point_.x_ = edgeInfo.termstpos.x;
	edge_t.start_point_.y_ = edgeInfo.termstpos.y;
	edge_t.end_point_.x_ = edgeInfo.termendpos.x;
	edge_t.end_point_.y_ = edgeInfo.termendpos.y;
	edge_t.length_ = edgeInfo.length;
}

void DataManage::DispersePoint()
{
	std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
	std::vector<EDGEINFO>::iterator itEdge = m_vecEdgeInfo.begin();
	for (; itEdge != m_vecEdgeInfo.end(); ++itEdge)
	{
		(*itEdge).disperse_point.clear();
		for (int i = 0; i <= 200; ++i)
		{
			var__edge_t edge_t;
			ConvertEdgeInfo(*itEdge, edge_t);
			position_t pos;
			GetposbyT(i * 0.005, &edge_t, &pos);
			double distance; 
			GetdistbyTOrg(i * 0.005, &edge_t, &distance);

			POSPOINT point;
			point.x = pos.x_;
			point.y = pos.y_;
			point.distance_to_start = distance;
			(*itEdge).disperse_point.push_back(point);
		}
	}
}

void DataManage::DispersePoint(EDGEINFO& edge_info)
{
	edge_info.disperse_point.clear();
	for (int i = 0; i <= 200; ++i)
	{
		var__edge_t edge_t;
		ConvertEdgeInfo(edge_info, edge_t);
		position_t pos;
		GetposbyT(i * 0.005, &edge_t, &pos);
		double distance;
		GetdistbyTOrg(i * 0.005, &edge_t, &distance);

		POSPOINT point;
		point.x = pos.x_;
		point.y = pos.y_;
		point.distance_to_start = distance;
		edge_info.disperse_point.push_back(point);
	}
}

bool DataManage::SetVehiclePos(const std::string& vehicleIp, const POSXYA& pos)
{
	if (vehicleIp.empty())
	{
		return false;
	}
	std::lock_guard<decltype(m_mutexVehiclePos)> guard(m_mutexVehiclePos);
	std::map<std::string, POSXYA>::iterator itPos = m_vehicle_pos.find(vehicleIp);
	if (itPos != m_vehicle_pos.end())
	{
		itPos->second = pos;
	}
	else
	{
		m_vehicle_pos.insert(std::make_pair(vehicleIp, pos));
	}
	return true;
}

bool DataManage::GetVehiclePos(const std::string& vehicleIp, POSXYA& pos)
{
	std::lock_guard<decltype(m_mutexVehiclePos)> guard(m_mutexVehiclePos);
	std::map<std::string, POSXYA>::const_iterator itPos = m_vehicle_pos.find(vehicleIp);
	if (itPos != m_vehicle_pos.end())
	{
		pos = itPos->second;
		return true;
	}
	return false;
}

void DataManage::SetAllTargetInfo(const std::vector<TARGETINFO>& vecTargetInfo)
{
	std::lock_guard<decltype(m_mutexTargetInfo)> guard(m_mutexTargetInfo);
	m_vecTargetInfo = vecTargetInfo;
}

void DataManage::GetAllTargetInfo(std::vector<TARGETINFO>& vecTargetInfo)
{
	std::lock_guard<decltype(m_mutexTargetInfo)> guard(m_mutexTargetInfo);
	vecTargetInfo = m_vecTargetInfo;
}

bool DataManage::GetTargetInfo(int id, TARGETINFO& targetInfo)
{
	std::lock_guard<decltype(m_mutexTargetInfo)> guard(m_mutexTargetInfo);
	for (auto target : m_vecTargetInfo)
	{
		if (target.id == id)
		{
			targetInfo = target;
			return true;
		}
	}
	return false;
}

void DataManage::AddTargetInfo(const TARGETINFO& targetInfo)
{
	bool bExist = false;
	std::lock_guard<decltype(m_mutexTargetInfo)> guard(m_mutexTargetInfo);
	for (auto &target : m_vecTargetInfo)
	{
		if (target.id == targetInfo.id)
		{
			target = targetInfo;
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		m_vecTargetInfo.push_back(targetInfo);
	}
}

void DataManage::AddAnchorInfo(const ANCHORINFO& anchorInfo)
{
	bool bExist = false;
	std::lock_guard<decltype(m_mutexAnchorInfo)> guard(m_mutexAnchorInfo);
	for (auto &anchor : m_vecAnchorInfo)
	{
		if (anchor.id == anchorInfo.id)
		{
			anchor = anchorInfo;
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		m_vecAnchorInfo.push_back(anchorInfo);
	}
}

bool DataManage::DeleteAnchorInfo(int anchor_id)
{
	std::lock_guard<decltype(m_mutexAnchorInfo)> guard(m_mutexAnchorInfo);
	for (auto itAnchor = m_vecAnchorInfo.begin(); itAnchor != m_vecAnchorInfo.end(); ++itAnchor)
	{
		if ((*itAnchor).id == anchor_id)
		{
			m_vecAnchorInfo.erase(itAnchor);
			return true;
		}
	}
	return false;
}

void DataManage::GetAllAnchorInfo(std::vector<ANCHORINFO>& vecAnchor)
{
	std::lock_guard<decltype(m_mutexAnchorInfo)> guard(m_mutexAnchorInfo);
	vecAnchor = m_vecAnchorInfo;
}

void DataManage::SetAllAnchorInfo(const std::vector<ANCHORINFO>& vecAnchor)
{
	std::lock_guard<decltype(m_mutexAnchorInfo)> guard(m_mutexAnchorInfo);
	m_vecAnchorInfo = vecAnchor;
}

bool DataManage::DeleteTargetInfo(int id)
{
	std::lock_guard<decltype(m_mutexTargetInfo)> guard(m_mutexTargetInfo);
	for (auto itTarget = m_vecTargetInfo.begin(); itTarget != m_vecTargetInfo.end(); ++itTarget)
	{
		if ((*itTarget).id == id)
		{
			m_vecTargetInfo.erase(itTarget);
			return true;
		}
	}
	return false;
}

void DataManage::DeleteAllTargetInfo()
{
	std::lock_guard<decltype(m_mutexTargetInfo)> guard(m_mutexTargetInfo);
	m_vecTargetInfo.clear();
}

int DataManage::GetTargetSize()
{
	std::lock_guard<decltype(m_mutexTargetInfo)> guard(m_mutexTargetInfo);
	return static_cast<int>(m_vecTargetInfo.size());
}

bool DataManage::ChangeAnchorToTarget(int anchorId)
{
	TARGETINFO targetInfo;
	bool anchor_exist = false;
	{
		std::lock_guard<decltype(m_mutexAnchorInfo)> guard(m_mutexAnchorInfo);
		for (auto anchor : m_vecAnchorInfo)
		{
			if (anchor.id == anchorId)
			{
				targetInfo.edge_id = anchor.edge_id;
				targetInfo.wop_id = anchor.wop_id;
				targetInfo.pos_x = anchor.pos_x;
				targetInfo.pos_y = anchor.pos_y;
				targetInfo.angle = anchor.angle;

				anchor_exist = true;
				break;
			}
		}
	}
	if (!anchor_exist)
	{
		return false;
	}
	targetInfo.id = GetTargetSize() + 1;
	AddTargetInfo(targetInfo);
	return true;
}

void DataManage::SetAllVehicleInfo(const std::vector<VEHICLEINFO>& vecVehicle)
{
	std::lock_guard<decltype(m_mutexVehicle)> guard(m_mutexVehicle);
	m_vecVehicleInfo = vecVehicle;
}

void DataManage::GetAllVehicleInfo(std::vector<VEHICLEINFO>& vecVehicle)
{
	std::lock_guard<decltype(m_mutexVehicle)> guard(m_mutexVehicle);
	vecVehicle = m_vecVehicleInfo;
}

void DataManage::AddVehicleInfo(const VEHICLEINFO& vehicleInfo)
{
	std::lock_guard<decltype(m_mutexVehicle)> guard(m_mutexVehicle);
	bool exist = false;
	for (auto &iter : m_vecVehicleInfo){
		if (iter.vehicle_id == vehicleInfo.vehicle_id){
			iter = vehicleInfo;
			exist = true;
			break;
		}
	}
	if (!exist){
		m_vecVehicleInfo.push_back(vehicleInfo);
	}
}

bool DataManage::SetVehicleUpl(const std::string& vehicleIp, const UPL& upl)
{
	if (vehicleIp.empty())
	{
		return false;
	}
	std::lock_guard<decltype(m_mutexVehicleUpl)> guard(m_mutexVehicleUpl);
	std::map<std::string, UPL>::iterator itUpl = m_vecVehicleUpl.find(vehicleIp);
	if (itUpl != m_vecVehicleUpl.end())
	{
		itUpl->second = upl;
	}
	else
	{
		m_vecVehicleUpl.insert(std::make_pair(vehicleIp, upl));
	}
	return true;
}

bool DataManage::GetVehicleUpl(const std::string& vehicleIp, UPL& upl)
{
	std::lock_guard<decltype(m_mutexVehicleUpl)> guard(m_mutexVehicleUpl);
	std::map<std::string, UPL>::const_iterator itUpl = m_vecVehicleUpl.find(vehicleIp);
	if (itUpl != m_vecVehicleUpl.end())
	{
		upl = itUpl->second;
		return true;
	}
	return false;
}

bool DataManage::GetVehicleInfo(int vehicle_id, VEHICLEINFO& vehicleInfo)
{
	std::lock_guard<decltype(m_mutexVehicle)> guard(m_mutexVehicle);
	for (auto vehicle : m_vecVehicleInfo)
	{
		if (vehicle.vehicle_id == vehicle_id)
		{
			vehicleInfo = vehicle;
			return true;
		}
	}
	return false;
}

bool DataManage::DeleteEdgeInfo(int id)
{
	std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
	for (auto itEdge = m_vecEdgeInfo.begin(); itEdge != m_vecEdgeInfo.end(); ++itEdge)
	{
		if ((*itEdge).id == id)
		{
			m_vecEdgeInfo.erase(itEdge);
			return true;
		}
	}
	return false;
}

bool DataManage::DeleteNodeInfo(int id)
{
	std::lock_guard<decltype(m_mutexNodeInfo)> guard(m_mutexNodeInfo);
	for (auto itNode = m_vecNodeInfo.begin(); itNode != m_vecNodeInfo.end(); ++itNode)
	{
		if ((*itNode).node_id == id)
		{
			m_vecNodeInfo.erase(itNode);
			return true;
		}
	}
	return false;
}

bool DataManage::DeletePosInfo(int id)
{
	std::lock_guard<decltype(m_mutexPosInfo)> guard(m_mutexPosInfo);
	for (auto itPos = m_vctCodeInfo.begin(); itPos != m_vctCodeInfo.end(); ++itPos)
	{
		if ((*itPos).pos_id == id)
		{
			m_vctCodeInfo.erase(itPos);
			return true;
		}
	}
	return false;
}

void DataManage::SetDefaultWop( )
{
	std::lock_guard<decltype( m_mutexWopInfo )> guard( m_mutexWopInfo );
	WOP wopType1;
	wopType1.id = 1;
	wopType1.angle = 0;
	wopType1.angle_type = CONST_ANGLE;
	wopType1.direction = FORWARD;
	WOP wopType2;
	wopType2.id = 2;
	wopType2.angle = 180;
	wopType2.angle_type = CONST_ANGLE;
	wopType2.direction = FORWARD;
	WOP wopType3;
	wopType3.id = 3;
	wopType3.angle = 180;
	wopType3.angle_type = CONST_ANGLE;
	wopType3.direction = BACKWARD;
	WOP wopType4;
	wopType4.id = 4;
	wopType4.angle = 0;
	wopType4.angle_type = CONST_ANGLE;
	wopType4.direction = BACKWARD;
	m_vecWopInfo.push_back( wopType1 );
	m_vecWopInfo.push_back( wopType2 );
	m_vecWopInfo.push_back( wopType3 );
	m_vecWopInfo.push_back( wopType4 );
}

void DataManage::SetTaskStatus(int status)
{
	m_task_status.store(status);
}

int  DataManage::GetTaskStatus()
{
	return m_task_status.load();
}

