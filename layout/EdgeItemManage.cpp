#include "EdgeItemManage.h"
#include "MapScene.h"
#include "DataManage.h"
#include "log.h"
#include "point_algorithm.hpp"
#include "curve_factory.hpp"


EdgeItemManage::EdgeItemManage(MapScene* mapScene)
: m_pMapScene(mapScene)
, m_pSrcNode(NULL)
, m_pDestNode(NULL)
, m_pDrawLineItem(NULL)
{
}


EdgeItemManage::~EdgeItemManage()
{
	
}

void EdgeItemManage::SetEdgeInfo(const std::vector<EDGEINFO>& vecEdgeInfo)
{
	std::vector<EDGEINFO>::const_iterator itEdge = vecEdgeInfo.begin();
	for (; itEdge != vecEdgeInfo.end(); ++itEdge)
	{
		AddEdgeItem(*itEdge);
	}
}

EdgeItem* EdgeItemManage::AddEdgeItem(const EDGEINFO& edgeInfo)
{
	EdgeItem* pEdgeItem = CreateEdgeItem(edgeInfo.start_node_id, edgeInfo.end_node_id);
	if (!pEdgeItem)
	{
		return NULL;
	}
	pEdgeItem->SetEdgeInfo(edgeInfo);
	pEdgeItem->UpdateCtrlPoint(QPointF(edgeInfo.ctrl_point1.x, edgeInfo.ctrl_point1.y), QPointF(edgeInfo.ctrl_point2.x, edgeInfo.ctrl_point2.y));//修改控制点的实际位置
	pEdgeItem->Adjust();

	return pEdgeItem;
}

EdgeItem* EdgeItemManage::AddEdgeItem(int startNodeId, int endNodeId, int edgeId)
{
	EdgeItem* pEdgeItem = CreateEdgeItem(startNodeId, endNodeId);
	if (!pEdgeItem)
	{
		return NULL;
	}
	if (edgeId == -1){//如果没有指定边的ID,则边ID自增
		pEdgeItem->SetId(GetMaxEdgeId() + 1);
	}
	else{//如果指定了边ID，则直接用此Id
		pEdgeItem->SetId(edgeId);
	}
	EDGEINFO edgeInfo;
	pEdgeItem->GetEdgeInfo(edgeInfo);
	DATAMNG->AddEdgeInfo(edgeInfo);
	pEdgeItem->SetEdgeInfo( edgeInfo );

	return pEdgeItem;
}

EdgeItem* EdgeItemManage::CreateEdgeItem(int startNodeId, int endNodeId)
{
	if (!m_pMapScene)
	{
		return NULL;
	}
	NodeItem *startNode = m_pMapScene->FindNodeItem(startNodeId);
	NodeItem *endNode = m_pMapScene->FindNodeItem(endNodeId);

	if (startNode == NULL || endNode == NULL)
	{
		return NULL;
	}

	EdgeItem *pEdgeItem = NULL;
	try
	{
		pEdgeItem = new EdgeItem(startNode, endNode);
	}
	catch (...)
	{
		return NULL;
	}
	m_pMapScene->addItem(pEdgeItem);
	m_pMapScene->addItem(pEdgeItem->CtrlPointItem1());
	m_pMapScene->addItem(pEdgeItem->CtrlPointItem2());
	AddEdgeToVector(pEdgeItem);

	return pEdgeItem;
}

void EdgeItemManage::AddEdgeToVector(EdgeItem* pEdgeItem)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexEdgeItem);
	m_vecEdgeItem.push_back(pEdgeItem);
}

EdgeItem* EdgeItemManage::FindEdge(int startNodeId, int endNodeId)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexEdgeItem);
	std::vector<EdgeItem*>::const_iterator itEdge = m_vecEdgeItem.begin();
	for (; itEdge != m_vecEdgeItem.end(); ++itEdge)
	{
		EdgeItem* pEdgeItem = *itEdge;
		if (pEdgeItem == NULL)
		{
			continue;
		}
		EDGEINFO edgeInfo;
		pEdgeItem->GetEdgeInfo(edgeInfo);
		if (edgeInfo.start_node_id == startNodeId && edgeInfo.end_node_id == endNodeId)
		{
			return pEdgeItem;
		}
		if (edgeInfo.start_node_id == endNodeId && edgeInfo.end_node_id == startNodeId &&
			edgeInfo.direction == Direction::BOTH)
		{
			return pEdgeItem;
		}
	}
	return NULL;
}

EdgeItem* EdgeItemManage::FindEdgeDirect(int startNodeId, int endNodeId)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexEdgeItem);
	std::vector<EdgeItem*>::const_iterator itEdge = m_vecEdgeItem.begin();
	for (; itEdge != m_vecEdgeItem.end(); ++itEdge)
	{
		EdgeItem* pEdgeItem = *itEdge;
		if (pEdgeItem == NULL)
		{
			continue;
		}
		EDGEINFO edgeInfo;
		pEdgeItem->GetEdgeInfo(edgeInfo);
		if (edgeInfo.start_node_id == startNodeId && edgeInfo.end_node_id == endNodeId)
		{
			return pEdgeItem;
		}
	}
	return NULL;
}

void EdgeItemManage::GetAllEdgeItem(std::vector<EdgeItem*>& vecEdgeItem)
{
	std::lock_guard<decltype(m_mutexEdgeItem)> guard(m_mutexEdgeItem);
	vecEdgeItem = m_vecEdgeItem;
}

void EdgeItemManage::SetEdgeSrcNode(NodeItem* pNodeItem)
{
	m_pSrcNode = pNodeItem;
}
void EdgeItemManage::AddLineItem(const QPointF& startPos, const QPointF& endPos)
{
	if (!m_pMapScene)
	{
		return;
	}
	m_pDrawLineItem = new QGraphicsLineItem(QLineF(startPos, endPos));
	m_pMapScene->addItem(m_pDrawLineItem);
}

EdgeItem* EdgeItemManage::DrawMoveLine(NodeItem* pDestNode, const QPointF& mousePos)
{
	if (!m_pSrcNode)
	{
		return NULL;
	}
	m_pDestNode = pDestNode;
	if (m_pDestNode == m_pSrcNode)
	{
		return NULL;
	}
	if (!m_pDestNode)
	{
		UpdateLineItem(m_pSrcNode->scenePos(), mousePos);
	}
	else
	{
		UpdateLineItem(m_pDestNode->scenePos(), m_pDestNode->scenePos());
		QListIterator<EdgeItem*> i(*m_pDestNode->listEdgeItem());
		EdgeItem* edgeItem;
		while (i.hasNext()) {
			edgeItem = i.next();
			if (edgeItem->StartNode() == m_pSrcNode)
				return NULL;
		}
		EdgeItem* pEdgeItem = AddEdgeItem(m_pSrcNode->GetNodeId(), m_pDestNode->GetNodeId());
		m_pSrcNode = m_pDestNode;//将终点设为下一条边的起点
		return pEdgeItem;
	}
	return NULL;
}

void EdgeItemManage::DrawMoveLine(NodeItem* srcNode, NodeItem* pDestNode, const QPointF& mousePos)
{
	if (!srcNode){
		return;
	}
	if (srcNode == pDestNode){
		return;
	}
	if (!pDestNode)
	{
		UpdateLineItem(srcNode->scenePos(), mousePos);
	}
	else
	{
		UpdateLineItem(srcNode->scenePos(), pDestNode->scenePos());
	}
}

void EdgeItemManage::UpdateLineItem(const QPointF& startPos, const QPointF& endPos)
{
	if (m_pDrawLineItem)
	{
		QLineF newLine(startPos, endPos);
		m_pDrawLineItem->setLine(newLine);
	}
}

int EdgeItemManage::GetMaxEdgeId()
{
	int iMaxId = 0;
	for (auto pEdgeItem : m_vecEdgeItem)
	{
		if (pEdgeItem && pEdgeItem->GetId() > iMaxId)
		{
			iMaxId = pEdgeItem->GetId();
		}
	}
	return iMaxId;
}

void EdgeItemManage::DeleteLineItem()
{
	if (!m_pMapScene)
	{
		return;
	}
	if (m_pDrawLineItem)
	{
		m_pMapScene->removeItem(m_pDrawLineItem);
		delete m_pDrawLineItem;
		m_pDrawLineItem = NULL;
	}
	m_pSrcNode = NULL;
	m_pDestNode = NULL;
}

void EdgeItemManage::SetItemMoveable(bool bMoveable)
{
	for (auto pEdgeItem : m_vecEdgeItem)
	{
		if (pEdgeItem)
		{
			pEdgeItem->SetItemMoveable(bMoveable);
		}
	}
}

void EdgeItemManage::HideAllEdgeCtrlPoint()
{
	for (auto pEdgeItem : m_vecEdgeItem)
	{
		if (pEdgeItem)
		{
			pEdgeItem->ShowControlPoint(false);
		}
	}
}

void EdgeItemManage::ShowNavPath(const std::vector<UPL>& vecUpl)
{
	ResetPathColor();
	for (auto upl : vecUpl)
	{
		for (auto pEdgeItem : m_vecEdgeItem)
		{
			if (pEdgeItem && pEdgeItem->GetId() == upl.edge_id)
			{
				WOP wop;
				DATAMNG->GetWopById(upl.wop_id, wop);
				pEdgeItem->ChangeEdgeColor(1.0, wop.direction, NAV_PATH_TYPE);
			}
		}
	}
}

void EdgeItemManage::ResetPathColor()
{
	for (auto pEdgeItem : m_vecEdgeItem)
	{
		if (pEdgeItem)
		{
			pEdgeItem->ChangeEdgeColor(1.0, FORWARD, NORMAL_TYPE);
		}
	}
}

bool EdgeItemManage::RemoveEdgeItem(int edge_id)
{
	for (auto itEdge = m_vecEdgeItem.begin(); itEdge != m_vecEdgeItem.end(); ++itEdge)
	{
		EdgeItem* pEdgeItem = *itEdge;
		if (pEdgeItem && pEdgeItem->GetId() == edge_id)
		{
			pEdgeItem->RemoveFromNode();
			m_pMapScene->removeItem(pEdgeItem);
			DATAMNG->DeleteEdgeInfo(edge_id);

			m_vecEdgeItem.erase(itEdge);
			delete pEdgeItem;
			pEdgeItem = NULL;
			return true;
		}
	}
	return false;
}

void EdgeItemManage::AddSelectEdgeItem(EdgeItem* edge_item)
{
	bool exist = false;
	for (auto item : select_edge_items)
	{
		if (item && item->GetId() == edge_item->GetId())
		{
			exist = true;
			break;
		}
	}
	if (!exist)
	{
		select_edge_items.push_back(edge_item);
	}
}

void EdgeItemManage::RemoveAllSelectEdgeItem()
{
	select_edge_items.clear();
}

int EdgeItemManage::GetSelectEdgeNum()
{
	return select_edge_items.size();
}

EdgeItem* EdgeItemManage::NodeItemOnEdge(NodeItem* nodeItem)
{
	for (auto edge_item : select_edge_items)
	{
		if (edge_item && (edge_item->StartNode() == nodeItem || edge_item->EndNode() == nodeItem))
		{
			return edge_item;
		}
	}
	return NULL;
}

bool EdgeItemManage::CreateLinkCurve(NodeItem* srcNode, NodeItem* dstNode,double angle)
{
	if (!srcNode || !dstNode){
		return false;
	}
	EdgeItem* startEdge = NodeItemOnEdge(srcNode);
	if (!startEdge){
		nspinfo << "can not find start edge!";
		return false;
	}
	EdgeItem* endEdge = NodeItemOnEdge(dstNode);
	if (!endEdge){
		nspinfo << "can not find end edge!";
		return false;
	}
	PointParamete point_start;
	NodePointWithAngle(srcNode, startEdge, point_start);
	PointParamete point_end;
	NodePointWithAngle(dstNode, endEdge, point_end);

	double angle_offset = point_end.point_angle - point_start.point_angle;
	angle_offset = PT_ALG::angle_to_normal180(angle_offset);
	std::vector<ControlPoints_> ctrl_points;
	if (fabs(angle_offset - 90) < 1.0) //90度的情况
	{
		curve_factory curve(CURVE_TYPE_90);
		ConvertNodeAngle90(point_start, point_end);
		curve.product_curve(point_start, point_end, ctrl_points);
	}
	else if (fabs(angle_offset - 180) < 1.0 || fabs(angle_offset - 0.0) < 1.0) //180的情况
	{
		curve_factory curve(CURVE_TYPE_180);
		ConvertNodeAngle180(point_start, point_end,angle);
		curve.product_curve(point_start, point_end, ctrl_points);
	}
	return AddLinkCurve(srcNode,dstNode,ctrl_points);
}

void EdgeItemManage::NodePointWithAngle(NodeItem* nodeItem, EdgeItem* edgeItem, PointParamete& point)
{
	point.point_x = DataConvert::map2world(nodeItem->scenePos().x());
	point.point_y = DataConvert::map2world(nodeItem->scenePos().y());
	EDGEINFO edge_info;
	edgeItem->GetEdgeInfo(edge_info);
	if (edgeItem->StartNode() == nodeItem){
		point.point_angle = PT_ALG::point_angle_on_edge(edge_info, 0.0);
	}
	else if (edgeItem->EndNode() == nodeItem)
	{
		point.point_angle = PT_ALG::point_angle_on_edge(edge_info, 1.0);
	}
	point.point_angle = point.point_angle * 180.0 / PI;
	point.point_angle = PT_ALG::angle_to_normal(point.point_angle);
}

bool EdgeItemManage::AddLinkCurve(NodeItem* srcNode, NodeItem* dstNode, const std::vector<ControlPoints_>& ctrl_points)
{
	if (!srcNode || !dstNode)
	{
		nspinfo << "srcNode or dstNode is NULL.";
		return false;
	}
	if (ctrl_points.size() != 7)
	{
		nspinfo << "compute control point failed,control point num:" << ctrl_points.size();
		return false;
	}
	std::vector<ControlPoints_> new_ctrl_points = ctrl_points;
	ConvertPointOrder(srcNode, dstNode, new_ctrl_points);//根据起始点和终点的顺序转换控制点的顺序
	QPointF new_point_pos;
	new_point_pos.setX(DataConvert::world2map(new_ctrl_points[3].cp_x));
	new_point_pos.setY(DataConvert::world2map(new_ctrl_points[3].cp_y));
	NodeItem* new_node = m_pMapScene->AddNodeItem(new_point_pos);
	if (!new_node)
	{
		nspinfo << "add center node failed!";
		return false;
	}
	EdgeItem* edge_curve1 = AddEdgeItem(srcNode->GetNodeId(), new_node->GetNodeId());
	if (!edge_curve1)
	{
		nspinfo << "add first curve edge failed!";
		return false;
	}
	UpdateEdgeItem(edge_curve1, new_ctrl_points[1], new_ctrl_points[2]);
	EdgeItem* edge_curve2 = AddEdgeItem(new_node->GetNodeId(), dstNode->GetNodeId());
	if (!edge_curve2)
	{
		nspinfo << "add first curve edge failed!";
		return false;
	}
	UpdateEdgeItem(edge_curve2, new_ctrl_points[4], new_ctrl_points[5]);
	return true;
}

void EdgeItemManage::UpdateEdgeItem(EdgeItem* edge_item, const ControlPoints_& ctrl_point1, const ControlPoints_& ctrl_point2)
{
	if (!edge_item){
		return;
	}
	EDGEINFO edgeInfo;
	edge_item->GetEdgeInfo(edgeInfo);
	edgeInfo.ctrl_point1.x = DataConvert::world2map(ctrl_point1.cp_x);
	edgeInfo.ctrl_point1.y = DataConvert::world2map(ctrl_point1.cp_y);
	edgeInfo.ctrl_point2.x = DataConvert::world2map(ctrl_point2.cp_x);
	edgeInfo.ctrl_point2.y = DataConvert::world2map(ctrl_point2.cp_y);

	edge_item->SetEdgeInfo(edgeInfo);
	edge_item->UpdateCtrlPoint(QPointF(edgeInfo.ctrl_point1.x, edgeInfo.ctrl_point1.y), QPointF(edgeInfo.ctrl_point2.x, edgeInfo.ctrl_point2.y));//修改控制点的实际位置
	edge_item->Adjust();
}

void EdgeItemManage::ConvertNodeAngle90(PointParamete& point_start, PointParamete& point_end)
{
	//顺时针方向,第一象限
	if (point_end.point_x >= point_start.point_x && point_end.point_y <= point_start.point_y)
	{
		if (greater_than(point_start.point_angle, 135.0) && less_than(point_start.point_angle, 225.0))
		{
			point_start.point_angle = point_start.point_angle - 180.0;
		}
		if(greater_than(point_end.point_angle,45.0) && less_than(point_end.point_angle,135.0)){
			point_end.point_angle = point_end.point_angle - 180.0;
		}
	}
	//逆时针，第一象限
	if (point_end.point_x <= point_start.point_x && point_end.point_y >= point_start.point_y)
	{
		if (greater_than(point_end.point_angle, 315.0) || less_than(point_end.point_angle, 45.0))
		{
			point_end.point_angle = point_end.point_angle - 180.0;
		}
		if (greater_than(point_start.point_angle, 225.0) && less_than(point_start.point_angle, 315.0)){
			point_start.point_angle = point_start.point_angle - 180.0;
		}
	}
	//顺时针，第二象限
	if (point_start.point_x <= point_end.point_x && point_start.point_y <= point_end.point_y)
	{
		if (greater_than(point_start.point_angle, 225.0) && less_than(point_start.point_angle, 315.0))
		{
			point_start.point_angle = point_start.point_angle - 180.0;
		}
		if (greater_than(point_end.point_angle, 135.0) && less_than(point_end.point_angle, 225.0)){
			point_end.point_angle = point_end.point_angle - 180.0;
		}
	}
	//逆时针，第二象限
	if (point_start.point_x >= point_end.point_x && point_start.point_y >= point_end.point_y)
	{
		if (greater_than(point_end.point_angle, 45.0) && less_than(point_end.point_angle, 135.0))
		{
			point_end.point_angle = point_end.point_angle - 180;
		}
		if (greater_than(point_start.point_angle, 315.0) || less_than(point_start.point_angle, 45.0)){
			point_start.point_angle = point_start.point_angle - 180.0;
		}
	}
	//顺时针，第三象限
	if (point_start.point_x >= point_end.point_x && point_start.point_y <= point_end.point_y)
	{
		if (greater_than(point_start.point_angle, 315.0) || less_than(point_start.point_angle, 45.0))
		{
			point_start.point_angle = point_start.point_angle - 180.0;
		}
		if (greater_than(point_end.point_angle, 225.0) && less_than(point_end.point_angle, 315.0)){
			point_end.point_angle = point_end.point_angle - 180.0;
		}
	}
	//逆时针，第三象限
	if (point_start.point_x <= point_end.point_x && point_start.point_y >= point_end.point_y)
	{
		if (greater_than(point_end.point_angle, 135.0) && less_than(point_end.point_angle, 225.0))
		{
			point_end.point_angle = point_end.point_angle - 180.0;
		}
		if (greater_than(point_start.point_angle, 45.0) && less_than(point_start.point_angle, 135.0)){
			point_start.point_angle = point_start.point_angle - 180.0;
		}
	}
	//顺时针，第四象限
	if (point_start.point_x >= point_end.point_x && point_start.point_y >= point_end.point_y)
	{
		if (greater_than(point_start.point_angle, 45.0) && less_than(point_start.point_angle, 135.0))
		{
			point_start.point_angle = point_start.point_angle - 180.0;
		}
		if (greater_than(point_end.point_angle, 315.0) || less_than(point_end.point_angle, 45.0)){
			point_end.point_angle = point_end.point_angle - 180.0;
		}
	}
	//逆时针，第四象限
	if (point_start.point_x <= point_end.point_x && point_start.point_y <= point_end.point_y)
	{
		if (greater_than(point_end.point_angle, 225.0) && less_than(point_end.point_angle, 315.0))
		{
			point_end.point_angle = point_end.point_angle - 180.0;
		}
		if (greater_than(point_start.point_angle, 135.0) && less_than(point_start.point_angle, 225.0)){
			point_start.point_angle = point_start.point_angle - 180.0;
		}
	}
	//再将角度转回到0-360
	point_start.point_angle = PT_ALG::angle_to_normal(point_start.point_angle);
	point_end.point_angle = PT_ALG::angle_to_normal(point_end.point_angle);
}

void EdgeItemManage::ConvertNodeAngle180(PointParamete& point_start, PointParamete& point_end,double angle)
{
	//第一象限
	if (point_start.point_x >= point_end.point_x && point_start.point_y >= point_end.point_y)
	{
		if (angle >= 270.0 && angle <= 360.0)//顺时针
		{
			if ((greater_than(point_start.point_angle, 270.0) && less_than(point_start.point_angle, 360.0) ||
				fabs(point_start.point_angle - 0.0) < 1.0) &&
				(greater_than(point_end.point_angle, 90.0) && less_than(point_end.point_angle, 180.0)))
			{
				return;
			}
			if (greater_than(point_start.point_angle, 90.0) && less_than(point_start.point_angle, 180.0))
			{
				point_start.point_angle = point_start.point_angle - 180.0;
			}
			if (fabs(point_start.point_angle - point_end.point_angle) < 1.0)
			{
				point_end.point_angle = point_end.point_angle - 180.0;
			}
		}
		if (angle >= 90.0 && angle <= 180.0)//逆时针
		{
			if (greater_than(point_start.point_angle, 90.0) && less_than(point_start.point_angle, 180.0) &&
				(greater_than(point_end.point_angle, 270.0) && less_than(point_end.point_angle, 360.0) ||
				fabs(point_end.point_angle - 0.0) < 1.0))
			{
				return;
			}
			if (greater_than(point_start.point_angle, 270.0) && less_than(point_start.point_angle, 360.0) ||
				fabs(point_start.point_angle - 0.0) < 1.0)
			{
				point_start.point_angle = point_start.point_angle - 180.0;
			}
			if (fabs(point_start.point_angle - point_end.point_angle) < 1.0)
			{
				point_end.point_angle = point_end.point_angle - 180.0;
			}
		}
	}
	//第二象限
	if (point_start.point_x <= point_end.point_x && point_start.point_y >= point_end.point_y)
	{
		if (angle >= 0.0 && angle <= 90.0)//顺时针
		{
			if ((greater_than(point_start.point_angle, 0.0) && less_than(point_start.point_angle, 90.0) ||
				fabs(point_start.point_angle - 360.0) < 1.0) &&
				greater_than(point_end.point_angle, 180.0) && less_than(point_end.point_angle, 270.0))
			{
				return ;
			}
			if (greater_than(point_start.point_angle, 180.0) && less_than(point_start.point_angle, 270.0))
			{
				point_start.point_angle = point_start.point_angle - 180.0;
			}
			if (fabs(point_start.point_angle - point_end.point_angle) < 1.0)
			{
				point_end.point_angle = point_end.point_angle - 180.0;
			}
		}
		if (angle >= 180.0 && angle <= 270.0)//逆时针
		{
			if (greater_than(point_start.point_angle, 180.0) && less_than(point_start.point_angle, 270.0) &&
				(greater_than(point_end.point_angle, 0.0) && less_than(point_end.point_angle, 90.0) ||
				fabs(point_end.point_angle - 360.0) < 1.0))
			{
				return ;
			}
			if (greater_than(point_start.point_angle, 0.0) && less_than(point_start.point_angle, 90.0) ||
				fabs(point_start.point_angle - 360.0) < 1.0)
			{
				point_start.point_angle = point_start.point_angle - 180.0;
			}
			if (fabs(point_start.point_angle - point_end.point_angle) < 1.0)
			{
				point_end.point_angle = point_end.point_angle - 180.0;
			}
		}
	}
	//第三象限
	if (point_start.point_x <= point_end.point_x && point_start.point_y <= point_end.point_y)
	{
		if (angle >= 90.0 && angle <= 180.0)//顺时针
		{
			if (greater_than(point_start.point_angle, 90.0) && less_than(point_start.point_angle, 180.0) &&
				(greater_than(point_end.point_angle, 270.0) && less_than(point_end.point_angle, 360.0) ||
				fabs(point_end.point_angle - 0.0) < 1.0))
			{
				return;
			}
			if (greater_than(point_start.point_angle, 270.0) && less_than(point_start.point_angle, 360.0) ||
				fabs(point_start.point_angle - 0.0) < 1.0)
			{
				point_start.point_angle = point_start.point_angle - 180.0;
			}
			if (fabs(point_start.point_angle - point_end.point_angle) < 1.0)
			{
				point_end.point_angle = point_end.point_angle - 180.0;
			}
		}
		if (angle >= 270.0 && angle <= 360.0)//逆时针
		{
			if ((greater_than(point_start.point_angle, 270.0) && less_than(point_start.point_angle, 360.0) ||
				fabs(point_start.point_angle - 0.0) < 1.0) &&
				greater_than(point_end.point_angle, 90.0) && less_than(point_end.point_angle, 180.0))
			{
				return;
			}
			if (greater_than(point_start.point_angle, 90.0) && less_than(point_start.point_angle, 180.0))
			{
				point_start.point_angle = point_start.point_angle - 180.0;
			}
			if (fabs(point_start.point_angle - point_end.point_angle) < 1.0)
			{
				point_end.point_angle = point_end.point_angle - 180.0;
			}
		}
	}
	//第四象限
	if (point_start.point_x >= point_end.point_x && point_start.point_y <= point_end.point_y)
	{
		if (angle >= 180.0 && angle <= 270.0) //顺时针
		{
			if (greater_than(point_start.point_angle, 180.0) && less_than(point_start.point_angle, 270.0) &&
				(greater_than(point_end.point_angle, 0.0) && less_than(point_end.point_angle, 90.0) ||
				fabs(point_end.point_angle - 360.0) < 1.0))
			{
				return;
			}
			if (greater_than(point_start.point_angle, 0.0) && less_than(point_start.point_angle, 90.0) ||
				fabs(point_start.point_angle - 360.0) < 1.0)
			{
				point_start.point_angle = point_start.point_angle - 180.0;
			}
			if (fabs(point_start.point_angle - point_end.point_angle) < 1.0)
			{
				point_end.point_angle -= 180.0;
			}
		}
		if (angle > 0.0 && angle < 90.0) //逆时针
		{
			if ((greater_than(point_start.point_angle, 0.0) && less_than(point_start.point_angle, 90.0) ||
				fabs(point_start.point_angle - 360.0) < 1.0) &&
				greater_than(point_end.point_angle, 180.0) && less_than(point_end.point_angle, 270.0))
			{
				return;
			}
			if (greater_than(point_start.point_angle, 180.0) && less_than(point_start.point_angle, 270.0))
			{
				point_start.point_angle -= 180.0;
			}
			if (fabs(point_start.point_angle - point_end.point_angle) < 1.0)
			{
				point_end.point_angle -= 180.0;
			}
		}
	}
	
	//再将角度转回到0-360
	point_start.point_angle = PT_ALG::angle_to_normal(point_start.point_angle);
	point_end.point_angle = PT_ALG::angle_to_normal(point_end.point_angle);
}

bool EdgeItemManage::less_than(double val, double param)
{
	if (val < param || fabs(val - param) < 1.0)
	{
		return true;
	}
	return false;
}

bool EdgeItemManage::greater_than(double val, double param)
{
	if (val > param || fabs(val - param) < 1.0)
	{
		return true;
	}
	return false;
}

void EdgeItemManage::ConvertPointOrder(NodeItem* srcNode, NodeItem* dstNode, std::vector<ControlPoints_>& ctrl_points)
{
	QPointF srcPos = srcNode->scenePos();
	// 起始点是控制点的最后一个点，则需要将控制点的顺序反过来
	if (fabs(srcPos.x() - DataConvert::world2map(ctrl_points[6].cp_x)) < 0.001 && 
		fabs(srcPos.y() - DataConvert::world2map(ctrl_points[6].cp_y)) < 0.001)
	{
		ControlPoints_ point_tmp;
		for (int i = 0; i < 7 / 2; ++i)
		{
			point_tmp = ctrl_points[i];
			ctrl_points[i] = ctrl_points[6 - i];
			ctrl_points[6 - i] = point_tmp;
		}
	}
}

EdgeItem* EdgeItemManage::GetEdgeItem(int EdgeId)
{
	for (auto pEdgeItem : m_vecEdgeItem)
	{
		if (pEdgeItem && pEdgeItem->GetId()==EdgeId)
		{
			return pEdgeItem;
		}
	}
	return NULL;
}