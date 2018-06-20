#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qpushbutton.h>
#include "MapScene.h"
#include "MapView.h"
#include "BasicDefine.h"
#include <QtCore/QSharedPointer>
#include <functional>

class NodeItemMgr;
class EdgeItemMgr;

class DrawMapWid : public QWidget
{
public:
	DrawMapWid();
	~DrawMapWid();

	MapView *GetView() const
	{
		return m_pMapView;
	}
public:
	void dddPolygonVertex();
	void SetOperaMode(ActionType actionType);
	void DrawAllMapItem(const LAYOUTDATA& layoutData, const DOCKDATA& dockData);
	bool GetVehicleInfo(int iVehicleId, VEHICLEINFO& vehicleInfo);
	bool GetTargetInfo(int iTargetId, TARGETINFO& targetInfo);
	void ShowNavPath(const std::vector<UPL>& vecUpl);
	void SetVehiclePos(int iVehicleId, const POSXYA& pos);
	void SetMousePosCallback(const std::function<void(const QPointF&)>& callback);
	void AddAnchor( const POSXYA& pos );
	void AddNode(const POSXYA& pos);
	void DeleteSelectedItems();
	void SetEdgeId(int edge_id);
	MapScene* GetScene() const;
	NodeItem *GetNodeItem(int id) const;
	void AddNodeItem(const NODEINFO &nodeInfo);
	void AddEdgeItem(NodeItem *pStartNodeItem, NodeItem *pEndNodeItem, const EDGEINFO &edgeInfo);
	void Clear();
	void ClearNodeItem();
	void ClearEdgeItem();
	void deleteMoreItem();
	void editMoreNodes();
	void editMoreEdges();
	void formatMoreItems();
private:
	void InitWid();
	void InitSlot();
private:
	void SlotUpdateMousePos(const QPointF& pos);
private:
	std::function<void(const QPointF&)> callback_mousepos = nullptr;
private:
	MapView* m_pMapView;
	MapScene* m_pMapScene;
	QSharedPointer<NodeItemMgr> m_nodeItemMgr;
	QSharedPointer<EdgeItemMgr> m_edgeItemMgr;
};

