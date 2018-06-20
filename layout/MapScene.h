#pragma once
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qmenu.h>
#include "NodeItemManage.h"
#include "EdgeItemManage.h"
#include "VehicleItemManage.h"
#include "TargetItemManage.h"
#include "AnchorItemManage.h"
#include "BasicDefine.h"
#include "QRcodeItemManage.h"

class QLabel;
class PolygonItem;
class MapScene : public QGraphicsScene
{
	Q_OBJECT
public:
	MapScene();
	~MapScene();
public:
	void dddPolygonVertex();
	void SetActionType(const ActionType& actionType);
	ActionType GetActionType(){ return m_actionType; }
	void UpdateScale(double scaleFactor);
	void GetAllNodeItem(std::vector<NodeItem*>& vecNodeItem);
	void GetAllPosItem(std::vector<QRcodeItem*>&vctPosItem);
	void AddAllItem(const LAYOUTDATA& layoutData, const DOCKDATA& dockData);
	NodeItem* FindNodeItem(int iItemId);
	QRcodeItem* FindPosItem(int iItemId);
	void SetItemMoveable(bool bMoveable);
	bool GetVehicleInfo(int iVehicleId, VEHICLEINFO& vehicleInfo);
	bool GetTargetInfo(int iTartgetId, TARGETINFO& targetInfo);
	VehicleItem *GetCurrentVehicleItem() const;
	void ShowNavPath(const std::vector<UPL>& vecUpl);
	void ClearFormatInfo();
	void SetVehiclePos(int iVehicleId, const POSXYA& pos);
	void DeleteEdgeItem(EdgeItem* pEdgeItem);
	void AddAnchorToEdge( const POSXYA& posXya );
	NodeItem* AddNodeItem(const QPointF& pos);
	void DeleteSelectedItems();
	EdgeItem* SetEdgeId(int edge_id);
	void SetNodeIdLabel(QLabel*label);
	QList<PolygonItem*> polygonItems;
	void AdjustAllEdgeProperty(const std::vector<int>& vecWopId);
	void deleteMoreItem();
	void editMoreNodes();
	void edieMoreEdges();
	void formatMoreItems();
	void FormatBrushEdges();
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

	void SlotActionDelete();
	void SlotActionLocation();
private:
	NodeItem* FindNodeByPos(const QPointF& pos);
	EdgeItem* FindEdgeByPos(const QPointF& pos);
	AnchorItem* FindAnchorByPos(const QPointF& pos);
	VehicleItem* FindVehicleByPos(const QPointF& pos);
	QGraphicsItem* FindItemByPos(const QPointF& pos);
	ItemType CheckItemTypeByPos(const QPointF& pos);
	void FormatBrushEdge(EdgeItem* pFormatItem);
	void ProcessPopMenu(QGraphicsSceneMouseEvent *event);
	double mouse_move_direction();
	int GetSegmentPos(const QPointF& mousePos, QPointF& segPos);//获取分割点
	void SegmentEdge(const QPointF& pos, EdgeItem* pEdgeItem);
	void UpdateAnchorInfo(const std::vector<ANCHORINFO>& vecAnchor);

	EdgeItem*FindEdgeByEdgeId(const int EdgeId);
signals:
	void sigUpdateMousePos(const QPointF& pos);

private:
	std::shared_ptr<NodeItemManage> m_pNodeItemMng;
	std::shared_ptr<EdgeItemManage> m_pEdgeItemMng;
	std::shared_ptr<VehicleItemManage> m_pVehicleItemMng;
	std::shared_ptr<TargetItemManage> m_pTargetItemMng;
	std::shared_ptr<AnchorItemManage> m_pAnchorItemMng;
	std::shared_ptr<QRcodeItemManage> m_pQRcodeItemMng;
	bool m_en;
	ActionType m_actionType;//当前操作
	double m_scale_factor; //分辨率
	EdgeItem* m_pSelectItem; //记录选中的边item
	EdgeItem* m_pSelectFormat;//记录格式刷，先选择的一条边
	bool is_Format = false;//鼠标指针状态，是否要批量格式刷
	QMenu* m_popup_menu;
	QAction* m_actionDelete;
	QAction* m_actionLocat;
	QGraphicsItem* m_selectItem = nullptr;
	NodeItem* _select_node_item = nullptr;
	int _mouse_pos_count = 0;
	QPointF _pos_confirm_direct;
	bool  __mouse_press = false;//鼠标是否按下
	QRectF __select_rect;//框选的矩形区域
	QGraphicsRectItem* __select_rect_item = nullptr;
	EdgeItem* pSelectEdgeItem = NULL;
	QLabel*m_node_id_label=nullptr;
	
	// add by fengqiang 2017 07 25
	bool m_dddPolygonVertex = false;
	QVector<QGraphicsEllipseItem*> polygonVertexes;
	QGraphicsLineItem* polygonEdge = 0;
	QVector<QGraphicsLineItem*> polygonEdges;
	//std::vector<CODEPOSXYA>vct_locaPos_;
	int posId_ = 0;
	
	// 
};

