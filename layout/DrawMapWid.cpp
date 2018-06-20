#include "DrawMapWid.h"
#include "NodeItemMgr.h"
#include "EdgeItemMgr.h"
#include <QtWidgets/qboxlayout.h>
#include <QtGui/QGuiApplication>

DrawMapWid::DrawMapWid()
{
	InitWid();
	InitSlot();
}


DrawMapWid::~DrawMapWid()
{
}

void DrawMapWid::dddPolygonVertex() {

	//m_pMapView->setCursor(Qt::CrossCursor);
	m_pMapScene->dddPolygonVertex();
}

void DrawMapWid::InitWid()
{
	m_pMapScene = new MapScene;
	m_pMapView = new MapView;
	m_pMapView->setDragMode(QGraphicsView::RubberBandDrag);
	if (m_pMapScene == NULL || m_pMapView == NULL)
	{
		return;
	}
	m_pMapView->setScene(m_pMapScene);
	m_nodeItemMgr = QSharedPointer<NodeItemMgr>::create(m_pMapScene);
	m_edgeItemMgr = QSharedPointer<EdgeItemMgr>::create(m_pMapScene);

	QHBoxLayout* pHboxLayout = new QHBoxLayout;
	if (pHboxLayout != NULL)
	{
		pHboxLayout->addWidget(m_pMapView);
	}
	this->setLayout(pHboxLayout);
}

void DrawMapWid::InitSlot()
{
	connect(m_pMapScene, &MapScene::sigUpdateMousePos, this, &DrawMapWid::SlotUpdateMousePos);
}

void DrawMapWid::SetOperaMode(ActionType actionType)
{
	if (m_pMapView != NULL)
	{
		m_pMapView->SetOperaType(actionType);
	}
	if (m_pMapScene)
	{
		m_pMapScene->SetActionType(actionType);
	}
}

void DrawMapWid::DrawAllMapItem(const LAYOUTDATA& layoutData, const DOCKDATA& dockData)
{
	if (m_pMapScene)
	{
		m_pMapScene->AddAllItem(layoutData, dockData);
	}

	if (nullptr != m_pMapView)
	{
		m_pMapView->LocationCenterItem();
	}
}

bool DrawMapWid::GetVehicleInfo(int iVehicleId, VEHICLEINFO& vehicleInfo)
{
	if (m_pMapScene)
	{
		return m_pMapScene->GetVehicleInfo(iVehicleId, vehicleInfo);
	}
	return false;
}

bool DrawMapWid::GetTargetInfo(int iTargetId, TARGETINFO& targetInfo)
{
	if (m_pMapScene)
	{
		return m_pMapScene->GetTargetInfo(iTargetId, targetInfo);
	}
	return false;
}

void DrawMapWid::ShowNavPath(const std::vector<UPL>& vecUpl)
{
	if (m_pMapScene)
	{
		m_pMapScene->ShowNavPath(vecUpl);
	}
}

void DrawMapWid::SetVehiclePos(int iVehicleId, const POSXYA& pos)
{
	if (m_pMapScene)
	{
		m_pMapScene->SetVehiclePos(iVehicleId,pos);
	}
}

void DrawMapWid::AddAnchor( const POSXYA& pos )
{
	if( m_pMapScene ) {

		m_pMapScene->AddAnchorToEdge( pos );
	}
}

void DrawMapWid::AddNode(const POSXYA& pos)
{
	if (m_pMapScene) {
		QPointF point;
		point.setX(pos.pos_x);
		point.setY(pos.pos_y);
		m_pMapScene->AddNodeItem(point);
	}
}

void DrawMapWid::SlotUpdateMousePos(const QPointF& pos)
{
	if (callback_mousepos)
	{
		callback_mousepos(pos);
	}
}

void DrawMapWid::SetMousePosCallback(const std::function<void(const QPointF&)>& callback)
{
	callback_mousepos = callback;
}

void DrawMapWid::DeleteSelectedItems()
{
	if (m_pMapScene){
		m_pMapScene->DeleteSelectedItems();
	}
}

void DrawMapWid::SetEdgeId(int edge_id)
{
	EdgeItem*pEdgeIt = NULL;
	if (m_pMapScene){
		pEdgeIt = m_pMapScene->SetEdgeId(edge_id);
	}
	QPointF pos= pEdgeIt->StartNode()->pos();
	pos.setY(pos.y());
	m_pMapView->centerOn(pos);
}
MapScene* DrawMapWid::GetScene()const
{
	return m_pMapScene;
}

NodeItem * DrawMapWid::GetNodeItem(int id) const
{
	if (m_nodeItemMgr.isNull())
	{
		return nullptr;
	}

	return m_nodeItemMgr->GetNodeItem(id);
}

void DrawMapWid::AddNodeItem(const NODEINFO &nodeInfo)
{
	if (m_nodeItemMgr.isNull())
	{
		return;
	}

	m_nodeItemMgr->AddNodeItem(nodeInfo);
}

void DrawMapWid::AddEdgeItem(NodeItem *pStartNodeItem, NodeItem *pEndNodeItem, const EDGEINFO &edgeInfo)
{
	if (m_edgeItemMgr.isNull())
	{
		return;
	}

	m_edgeItemMgr->AddEdgeItem(pStartNodeItem, pEndNodeItem, edgeInfo);
}

void DrawMapWid::Clear()
{
	ClearNodeItem();
	ClearEdgeItem();
}

void DrawMapWid::ClearNodeItem()
{
	if (m_nodeItemMgr.isNull())
	{
		return;
	}

	m_nodeItemMgr->Clear();
}

void DrawMapWid::ClearEdgeItem()
{
	if (m_edgeItemMgr.isNull())
	{
		return;
	}

	m_edgeItemMgr->Clear();
}

void DrawMapWid::deleteMoreItem()
{
	if (m_pMapScene) {

		m_pMapScene->deleteMoreItem();
	}
}

void DrawMapWid::editMoreNodes()
{
	if (m_pMapScene){
		m_pMapScene->editMoreNodes();
	}
}

void DrawMapWid::editMoreEdges()
{
	if (m_pMapScene){
		m_pMapScene->edieMoreEdges();
	}
}

void DrawMapWid::formatMoreItems()
{
	if (m_pMapScene){
		m_pMapScene->formatMoreItems();
	}
}
