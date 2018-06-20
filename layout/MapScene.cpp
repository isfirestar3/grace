#include "MapScene.h"
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgraphicsview.h>
#include <math.h>
#include "point_algorithm.hpp"
#include "log.h"
#include "SegmentPosWnd.h"
#include "polygonitem.h"
#include "MessageBox.h"
#include <QtWidgets/qapplication.h>
#include "LocationOfCurve.h"
#include "NodesEditWnd.h"
#include "EdgesEditWnd.h"

MapScene::MapScene()
:m_scale_factor(1.0)
, m_actionType(ActionType::ACTION_SELECT)
, m_pSelectItem(NULL)
{
	setItemIndexMethod(QGraphicsScene::NoIndex);
	setSceneRect(-100000, -100000, 200000, 200000);
	m_pNodeItemMng = std::shared_ptr<NodeItemManage>(new NodeItemManage(this));
	m_pEdgeItemMng = std::shared_ptr<EdgeItemManage>(new EdgeItemManage(this));
	m_pVehicleItemMng = std::shared_ptr<VehicleItemManage>(new VehicleItemManage(this));
	m_pTargetItemMng = std::shared_ptr<TargetItemManage>(new TargetItemManage(this));
	m_pAnchorItemMng = std::shared_ptr<AnchorItemManage>(new AnchorItemManage(this));
	m_pQRcodeItemMng = std::shared_ptr<QRcodeItemManage>(new QRcodeItemManage(this));
	m_popup_menu = new QMenu;
	m_en = qApp->property("language").toInt() == 1;
	m_actionDelete = new QAction(m_en ? "Delete" : QStringLiteral("删除"),this);
	m_en = qApp->property("language").toInt() == 1;
	m_actionLocat = new QAction(m_en ? "location" : QStringLiteral("曲线中间二位码位置"), this);
	if (m_popup_menu)
	{
		m_popup_menu->addAction(m_actionDelete);
		m_popup_menu->addAction(m_actionLocat);
	}
	connect(m_actionDelete, &QAction::triggered, this, &MapScene::SlotActionDelete);
	connect(m_actionLocat, &QAction::triggered, this, &MapScene::SlotActionLocation);
	
}


MapScene::~MapScene()
{
}

void MapScene::dddPolygonVertex() {

	m_dddPolygonVertex = true;
}

void MapScene::SetActionType(const ActionType& actionType)
{
	m_actionType = actionType;
	if (m_actionType != ActionType::ACTION_MOVE_ITEM && m_pEdgeItemMng)
	{
		m_pEdgeItemMng->HideAllEdgeCtrlPoint();
	}
	if (m_actionType != ActionType::ACTION_ENTER_NAV && m_pTargetItemMng)
	{
		m_pTargetItemMng->DeleteAllTarget();//删除所有的目标点
		if (m_pAnchorItemMng)
		{
			m_pAnchorItemMng->RemoveAllTargetItem();//将停靠点的目标点属性去除
		}
	}
	if (m_actionType != ActionType::ACTION_LINK_CURVE && m_pEdgeItemMng)
	{
		m_pEdgeItemMng->RemoveAllSelectEdgeItem();
	}
	if (m_actionType != ActionType::ACTION_FORMAT)
	{
		ClearFormatInfo();
	}
}

void MapScene::UpdateScale(double scaleFactor)
{
	m_scale_factor = scaleFactor;
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

	QPointF mousePos = event->scenePos();
	if (event->button() == Qt::RightButton)//右键点击的话，弹出菜单
	{
		QGraphicsScene::mousePressEvent(event);
		ProcessPopMenu(event);
		return;
	}
	SetItemMoveable(false);
	switch (m_actionType)
	{
	case ActionType::ACTION_ADD_NODE:
		if (m_pNodeItemMng)
		{
			EdgeItem* pEdgeItem = FindEdgeByPos(mousePos);
			if (!pEdgeItem){
				m_pNodeItemMng->AddNodeItem(mousePos);
			}
			else{//如果点击的地方有边，那么对边进行分割
				QPointF segmentPos;
				if (GetSegmentPos(mousePos, segmentPos) == 0)// 点击了确定按钮才分割边
				{
					SegmentEdge(segmentPos, pEdgeItem);
				}
				else{
					m_pNodeItemMng->AddNodeItem(mousePos);
				}
			}
		}
		break;
	case ActionType::ACTION_ADD_EDGE:
		if (m_pEdgeItemMng)
		{
			NodeItem* selectNode = FindNodeByPos(mousePos);
			m_pEdgeItemMng->SetEdgeSrcNode(selectNode);
			m_pEdgeItemMng->AddLineItem(mousePos, mousePos);
		}
		break;
	case ActionType::ACTION_MOVE_ITEM:
		{
			EdgeItem* pEdgeItem = FindEdgeByPos(mousePos);
			if (pEdgeItem)
			{
				pEdgeItem->ShowControlPoint(true);
			}
		}
		break;
	case ActionType::ACTION_ADD_VEHICLE:
		if (m_pVehicleItemMng)
		{
			EdgeItem* pSelectEdgeItem = FindEdgeByPos(mousePos);
			if (pSelectEdgeItem)
			{
				EDGEINFO edgeInfo;
				pSelectEdgeItem->GetEdgeInfo(edgeInfo);
				m_pVehicleItemMng->AddVehicleItem(event->scenePos(),edgeInfo);
			}
		}
		break;
	case ActionType::ACTION_ENTER_NAV:
		if (m_pTargetItemMng)
		{
			AnchorItem* pAnchorItem = FindAnchorByPos(mousePos);
			if (pAnchorItem)//如果点击的是停靠点，则不新加目标点,而是将停靠点设为目标点
			{
				pAnchorItem->ChangeToTarget(true);
				break;
			}
			EdgeItem* pSelectEdgeItem = FindEdgeByPos(mousePos);
			if (pSelectEdgeItem)
			{
				EDGEINFO edgeInfo;
				pSelectEdgeItem->GetEdgeInfo(edgeInfo);
				m_pTargetItemMng->AddTargetItem(mousePos,edgeInfo);
			}
		}
		break;
	case ActionType::ACTION_ADD_ANCHOR:
		if (m_pAnchorItemMng)
		{
			EdgeItem* pSelectEdgeItem = FindEdgeByPos(mousePos);
			if (pSelectEdgeItem)
			{
				EDGEINFO edgeInfo;
				pSelectEdgeItem->GetEdgeInfo(edgeInfo);
				m_pAnchorItemMng->AddAnchorItem(mousePos, edgeInfo);
			}
		}
		break;
	case ActionType::ACTION_LINK_CURVE:
		if (m_pEdgeItemMng)
		{
			ItemType select_item_type = CheckItemTypeByPos(mousePos);
			if (select_item_type == ItemType::ITEM_EDGE)
			{
				EdgeItem* pSelectItem = FindEdgeByPos(mousePos);//如果选择了边，则先保存起来
				if (pSelectItem)
				{
					m_pEdgeItemMng->AddSelectEdgeItem(pSelectItem);
					_select_node_item = NULL;
				}
			}
			else if (select_item_type == ItemType::ITEM_NODE)
			{
				if (m_pEdgeItemMng->GetSelectEdgeNum() >= 2) //选择了两条以上的边以后才能画曲线
				{
					_select_node_item = FindNodeByPos(mousePos);//如果选择了节点，认为即将开始画曲线
					_mouse_pos_count = 0;//将鼠标移动位置计数置0开始计数
					if (_select_node_item)
					{
						m_pEdgeItemMng->AddLineItem(mousePos, mousePos);//先画一条虚线
					}
				}
			}
		}
		break;
	case ActionType::ACTION_SELECT:
		{
			m_pSelectItem = FindEdgeByPos(mousePos);
			__select_rect.setTopLeft(mousePos);
			__select_rect.setWidth(0.0);
			__select_rect.setHeight(0.0);
			__mouse_press = true;
			QList<QGraphicsItem*> all_items = items(mousePos);
			if (all_items.size() <= 0){
				if (!__select_rect_item){
					__select_rect_item = new QGraphicsRectItem(__select_rect);
					addItem(__select_rect_item);
				}
				
			}
		}
		break;
	default:
		break;
	}

	if ((event->button() == Qt::LeftButton) && m_dddPolygonVertex) {

		QGraphicsEllipseItem* polygonVertex = new QGraphicsEllipseItem;
		polygonVertex->setBrush(Qt::green);
		polygonVertex->setRect(-5, -5, 10, 10);
		polygonVertex->setPos(event->scenePos());
		polygonVertexes << polygonVertex;
		if (polygonEdge)
			polygonEdge = 0;
		addItem(polygonVertex);
	}

	QGraphicsScene::mousePressEvent(event);
	if (selectedItems().count() == 1) {
		
		QGraphicsItem* item = selectedItems().at(0);
		if (item->type() == ItemType::ITEM_NODE) {

			NodeItem* node = static_cast<NodeItem*>(item);
			if (m_node_id_label){
				m_node_id_label->setText(QStringLiteral("Node ID： %1").arg(node->GetNodeId()));
			}
		}
	}

}

void MapScene::AddAnchorToEdge( const POSXYA& posXya)
{
	QPointF pos;
	pos.setX( posXya.pos_x );
	pos.setY( posXya.pos_y );

	EdgeItem* pSelectEdgeItem = FindEdgeByPos( pos );
	if( pSelectEdgeItem ) {
		EDGEINFO edgeInfo;
		pSelectEdgeItem->GetEdgeInfo( edgeInfo );
		m_pAnchorItemMng->AddAnchorItem( pos, edgeInfo );
	}
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF mousePos = event->scenePos();
	emit sigUpdateMousePos(mousePos); //发送更新鼠标位置的信号
	switch (m_actionType)
	{
	case ActionType::ACTION_ADD_EDGE:
		if (m_pEdgeItemMng)
		{
			m_pEdgeItemMng->DrawMoveLine(FindNodeByPos(mousePos), mousePos);
		}
		break;
	case ActionType::ACTION_MOVE_ITEM:
		SetItemMoveable(true);
	case ActionType::ACTION_LINK_CURVE:
		if (m_pEdgeItemMng)
		{
			m_pEdgeItemMng->DrawMoveLine(_select_node_item, FindNodeByPos(mousePos), mousePos);
			if (_mouse_pos_count++ == 10)//如果计数等于10，则取此位置的点信息
			{
				_pos_confirm_direct = mousePos;
			}
		}
		break;
	case ActionType::ACTION_SELECT:
		if (__mouse_press)
		{
			__select_rect.setBottomRight(mousePos);
			if (__select_rect_item){
				__select_rect_item->setPen(QPen(Qt::black, 1, Qt::DashLine));
				__select_rect_item->setRect(__select_rect);
				//addItem(__select_rect_item);
			}
		}
		break;
	default:
		break;
	}

	if (m_dddPolygonVertex && !polygonVertexes.isEmpty()) {

		if (!polygonEdge) {
			polygonEdge = new QGraphicsLineItem;
			polygonEdge->setZValue(-1);
			addItem(polygonEdge);
			polygonEdges << polygonEdge;
		}
		polygonEdge->setLine(QLineF(polygonVertexes.last()->pos(), event->scenePos()));

	}

	QGraphicsScene::mouseMoveEvent(event);
}

void MapScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {

	if (polygonVertexes.count() >= 3) {

		if (!polygonEdge) {

			polygonEdge = new QGraphicsLineItem;
			addItem(polygonEdge);
			polygonEdges << polygonEdge;
		}
		polygonEdge->setLine(QLineF(polygonVertexes.last()->pos(), polygonVertexes.first()->pos()));
		polygonEdge = 0;
		m_dddPolygonVertex = false;

		QPolygonF polygon;
		QVectorIterator<QGraphicsEllipseItem*> i(polygonVertexes);
		while (i.hasNext())
			polygon << i.next()->pos();

		PolygonItem* polygonItem = new PolygonItem;
		addItem(polygonItem);
		polygonItem->id = ++polygonItemCounter;
		polygonItem->setPolygon(polygon);
		polygonItems << polygonItem;
		qDeleteAll(polygonEdges);
		qDeleteAll(polygonVertexes);
		polygonEdges.clear();
		polygonVertexes.clear();
		views().at(0)->setCursor(Qt::ArrowCursor);

	}
	QGraphicsScene::mouseDoubleClickEvent(event);
}

void MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (!event)
	{
		return;
	}
	QPointF mousePos = event->scenePos();
	switch (m_actionType)
	{
	case ActionType::ACTION_ADD_EDGE:
		if (m_pEdgeItemMng)
		{
			m_pEdgeItemMng->DeleteLineItem();
		}
		break;
	case ActionType::ACTION_FORMAT:
		{
			EdgeItem* pFormatItem = FindEdgeByPos(mousePos);
			if (pFormatItem)
			{
				FormatBrushEdge(pFormatItem);
			}
		}
		break;
	case ActionType::ACTION_LINK_CURVE:
		if (m_pEdgeItemMng)
		{
			m_pEdgeItemMng->DeleteLineItem();
			ItemType select_item_type = CheckItemTypeByPos(mousePos);
			if (select_item_type == ITEM_NODE)
			{
				double angle = mouse_move_direction();
				m_pEdgeItemMng->CreateLinkCurve(_select_node_item, FindNodeByPos(mousePos),angle);
			}
		}
		break;
	case ActionType::ACTION_SELECT:
		if (__mouse_press)
		{
			if (__select_rect_item){
				removeItem(__select_rect_item);
				delete __select_rect_item;
				__select_rect_item = nullptr;
			}
			__select_rect.setBottomRight(mousePos);
			QList<QGraphicsItem*> select_items = items(__select_rect);
			foreach(QGraphicsItem* item, select_items){
				item->setSelected(true);
			}
			__mouse_press = false;
		}
		if (is_Format){//批量格式刷
			FormatBrushEdges();
			m_pSelectFormat=NULL;
			is_Format = false;
		}
		break;
	default:
		break;
	}

	QGraphicsScene::mouseReleaseEvent(event);
}

NodeItem* MapScene::FindNodeByPos(const QPointF& pos)
{
	NodeItem* pNodeItem = NULL;
	QList<QGraphicsItem*> allItem = items(pos);
	foreach (QGraphicsItem* pItem , allItem)
	{
		if (pItem && pItem->type() == ItemType::ITEM_NODE)
		{
			pNodeItem = qgraphicsitem_cast<NodeItem*>(pItem);
			break;
		}
	}
	return pNodeItem;
}

EdgeItem* MapScene::FindEdgeByPos(const QPointF& pos)
{
	EdgeItem* pEdgeItem = NULL;
	QList<QGraphicsItem*> allItem = items(pos);
	foreach(QGraphicsItem* pItem, allItem)
	{
		if (pItem && pItem->type() == ItemType::ITEM_EDGE)
		{
			pEdgeItem = qgraphicsitem_cast<EdgeItem*>(pItem);
			break;
		}
	}
	return pEdgeItem;
}

EdgeItem*MapScene::FindEdgeByEdgeId(const int EdgeId)
{
	EdgeItem* pEdgeItem = NULL;
	if (m_pEdgeItemMng)
	{
		pEdgeItem=m_pEdgeItemMng->GetEdgeItem(EdgeId);
	}
	return pEdgeItem;
}

AnchorItem* MapScene::FindAnchorByPos(const QPointF& pos)
{
	AnchorItem* pAnchorItem = NULL;
	QList<QGraphicsItem*> allItem = items(pos);
	foreach(QGraphicsItem* pItem, allItem)
	{
		if (pItem && pItem->type() == ItemType::ITEM_ANCHOR)
		{
			pAnchorItem = qgraphicsitem_cast<AnchorItem*>(pItem);
			break;
		}
	}
	return pAnchorItem;
}

VehicleItem* MapScene::FindVehicleByPos(const QPointF& pos)
{
	VehicleItem* pVehicleItem = NULL;
	QList<QGraphicsItem*> allItem = items(pos);
	foreach(QGraphicsItem* pItem, allItem)
	{
		if (pItem && pItem->type() == ItemType::ITEM_VEHICLE)
		{
			pVehicleItem = qgraphicsitem_cast<VehicleItem*>(pItem);
			break;
		}
	}
	return pVehicleItem;
}

ItemType MapScene::CheckItemTypeByPos(const QPointF& pos)
{
	bool is_node = false;
	bool is_edge = false;
	bool is_anchor = false;
	bool is_target = false;
	bool is_vehicle = false;
	QList<QGraphicsItem*> allItem = items(pos);
	foreach(QGraphicsItem* pItem, allItem){
		if (pItem && pItem->type() == ItemType::ITEM_VEHICLE){
			is_vehicle = true;
		}
		if (pItem && pItem->type() == ItemType::ITEM_TARGET){
			is_target = true;
		}
		if (pItem && pItem->type() == ItemType::ITEM_ANCHOR){
			is_anchor = true;
		}
		if (pItem && pItem->type() == ItemType::ITEM_NODE){
			is_node = true;
		}
		if (pItem && pItem->type() == ItemType::ITEM_EDGE){
			is_edge = true;
		}
	}
	//元素从上到下依次为vehicle，anchor，node，edge，所以点击时总是返回最上面的元素类型
	if (is_vehicle){
		return ItemType::ITEM_VEHICLE;
	}
	if (is_target){
		return ItemType::ITEM_TARGET;
	}
	if (is_anchor){
		return ItemType::ITEM_ANCHOR;
	}
	if (is_node){
		return ItemType::ITEM_NODE;
	}
	if (is_edge){
		return ItemType::ITEM_EDGE;
	}
	return ItemType::ITEM_NULL;
}

void MapScene::GetAllNodeItem(std::vector<NodeItem*>& vecNodeItem)
{
	if (m_pNodeItemMng)
	{
		m_pNodeItemMng->GetAllNodeItem(vecNodeItem);
	}
}
void MapScene::GetAllPosItem(std::vector<QRcodeItem*>&vctPosItem)
{
	if (m_pQRcodeItemMng)
	{
		m_pQRcodeItemMng->GetAllCodeItem(vctPosItem);
	}
}
void MapScene::AddAllItem(const LAYOUTDATA& layoutData, const DOCKDATA& dockData)
{
	if (m_pNodeItemMng)
	{
		m_pNodeItemMng->SetNodeInfo(layoutData.nodeList);
	}
	if (m_pEdgeItemMng)
	{
		m_pEdgeItemMng->SetEdgeInfo(layoutData.edgeList);
	}
	if (m_pAnchorItemMng)
	{
		m_pAnchorItemMng->SetAnchorInfo(dockData.anchorList);
	}
	

	QVector<POLYGON> polygonList_ = QVector<POLYGON>::fromStdVector(layoutData.polygonList);
	QVectorIterator<POLYGON> i(polygonList_);
	while (i.hasNext()) {

		POLYGON const& polygon = i.next();
		PolygonItem* item = new PolygonItem;
		addItem(item);
		item->id = polygon.id;
		polygonItemCounter = qMax(polygonItemCounter, polygon.id);
		QVector<QPointF> points;
		QVector<PolygonVertex> vertexes_ = QVector<PolygonVertex>::fromStdVector(polygon.vertexes);
		QVectorIterator<PolygonVertex> pi(vertexes_);
		while (pi.hasNext()) {

			PolygonVertex const& onevertex = pi.next();
			points << QPointF(onevertex.x, onevertex.y);
		}
		item->setPolygon(points);
		polygonItems << item;
	}
}

NodeItem* MapScene::FindNodeItem(int iItemId)
{
	if (m_pNodeItemMng)
	{
		return m_pNodeItemMng->GetNodeItem(iItemId);
	}
	return nullptr;
}

QRcodeItem* MapScene::FindPosItem(int iItemId)
{
	if (m_pQRcodeItemMng)
	{
		return m_pQRcodeItemMng->GetCodeItem(iItemId);
	}
	return nullptr;
}

void MapScene::SetItemMoveable(bool bMoveable)
{
	if (m_pNodeItemMng)
	{
		m_pNodeItemMng->SetItemMoveable(bMoveable);
	}
	if (m_pEdgeItemMng)
	{
		m_pEdgeItemMng->SetItemMoveable(bMoveable);
	}
}

bool MapScene::GetVehicleInfo(int iVehicleId, VEHICLEINFO& vehicleInfo)
{
	if (m_pVehicleItemMng)
	{
		return m_pVehicleItemMng->GetVehicleInfo(iVehicleId, vehicleInfo);
	}
	return false;
}

bool MapScene::GetTargetInfo(int iTartgetId, TARGETINFO& targetInfo)
{
	if (m_pTargetItemMng)
	{
		return m_pTargetItemMng->GetTargetInfo(iTartgetId, targetInfo);
	}
	return false;
}

VehicleItem * MapScene::GetCurrentVehicleItem() const
{
	VehicleItem *viPtr = nullptr;
	QList<QGraphicsItem *> itemPtrList = items();

	foreach(QGraphicsItem *itemPtr, itemPtrList)
	{
		if (nullptr != itemPtr && ItemType::ITEM_VEHICLE == itemPtr->type())
		{
			viPtr = qgraphicsitem_cast<VehicleItem *>(itemPtr);
			break;
		}
	}

	return viPtr;
}

void MapScene::ShowNavPath(const std::vector<UPL>& vecUpl)
{
	if (m_pEdgeItemMng)
	{
		m_pEdgeItemMng->ShowNavPath(vecUpl);
	}
}

void MapScene::FormatBrushEdge(EdgeItem* pFormatItem)
{
	if (!m_pSelectItem)
	{
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("请先选择一条边作为参考边!"), QMessageBox::Ok);
		return;
	}
	std::vector<int> vecWopId;
	m_pSelectItem->GetEdgeWopList(vecWopId);
	EDGEINFO edgeInfo;
	m_pSelectItem->GetEdgeInfo(edgeInfo);
	if (pFormatItem)
	{
		pFormatItem->SetEdgeWopList(vecWopId);
		pFormatItem->SetEdgeSpeed(edgeInfo.speed);
		pFormatItem->SetEdgeWopAttr(edgeInfo.wop_attr_list);
	}
}

void MapScene::ClearFormatInfo()
{
	m_pSelectItem = NULL;
}

void MapScene::SetVehiclePos(int iVehicleId, const POSXYA& pos)
{
	if (m_pVehicleItemMng)
	{
		m_pVehicleItemMng->SetVehiclePos(iVehicleId, pos);
	}
}

QGraphicsItem* MapScene::FindItemByPos(const QPointF& pos)
{
	QGraphicsItem* pItemFind = NULL;
	QList<QGraphicsItem*> allItem = items(pos);
	foreach(QGraphicsItem* pItem, allItem)
	{
		if (pItem && (pItem->type() == ItemType::ITEM_ANCHOR || 
			pItem->type() == ItemType::ITEM_NODE || pItem->type() == ItemType::ITEM_EDGE || pItem->type() == ItemType::ITEM_POS))
		{
			pItemFind = pItem;
			break;
		}
	}
	return pItemFind;
}

void MapScene::DeleteEdgeItem(EdgeItem* pEdgeItem)
{
	int edge_id = pEdgeItem->GetId();
	if (m_pEdgeItemMng)
	{
		m_pEdgeItemMng->RemoveEdgeItem(edge_id);
	}
}

void MapScene::ProcessPopMenu(QGraphicsSceneMouseEvent *event)
{
	if (!m_popup_menu || !event)
	{
		return;
	}
	if (m_actionType == ACTION_HAND_DRAG || m_actionType == ACTION_SELECT)
	{
		m_selectItem = FindItemByPos(event->scenePos());
		if (m_selectItem)
		{
			m_popup_menu->exec(event->screenPos());
		}
	}
}
void MapScene::SlotActionLocation()
{
	LocationOfCurve locOfCur;
	if (m_selectItem->type() == ITEM_EDGE)
	{
		m_pSelectItem = dynamic_cast<EdgeItem*>(m_selectItem);
		EDGEINFO edgeInfo;
		m_pSelectItem->GetEdgeInfo(edgeInfo);

		locOfCur.edgeInfo_ = edgeInfo;
		locOfCur.id_ = posId_;
		if (locOfCur.exec() == QDialog::Accepted)
		{
			DATAMNG->m_vctCodeInfo.insert(DATAMNG->m_vctCodeInfo.end(), locOfCur.vct_pos_.begin(), locOfCur.vct_pos_.end());
			posId_ += locOfCur.vct_pos_.size();
		}
	}
	if (m_pQRcodeItemMng)
	{
		m_pQRcodeItemMng->SetCodeInfo(locOfCur.vct_pos_);
	}
}
void MapScene::SlotActionDelete()
{
	if (!m_selectItem)
	{
		return;
	}
	if(m_selectItem->type() == ITEM_ANCHOR)
	{
		 MessageBox::StandardButton sb = MessageBox::Warning(nullptr, m_en ? "Warning!" : QStringLiteral("警告！"), m_en ? "Are you sure to delete the anchor?" : QStringLiteral("确定删除停靠点吗？"));

		 if (MessageBox::Ok == sb)
		 {
			 int anchor_id = dynamic_cast<AnchorItem*>(m_selectItem)->GetAnchorId();
			 if (m_pAnchorItemMng)
			 {
				 m_pAnchorItemMng->RemoveAnchorItem(anchor_id);
			 }
		 }
	}
	else if (m_selectItem->type() == ITEM_EDGE)
	{
		MessageBox::StandardButton sb = MessageBox::Warning(nullptr, m_en ? "Warning!" : QStringLiteral("警告！"), m_en ? "Are you sure to delete the edge?" : QStringLiteral("确定删除边吗？"));

		if (MessageBox::Ok == sb)
		{
			DeleteEdgeItem(dynamic_cast<EdgeItem*>(m_selectItem));
		}
	}
	else if (m_selectItem->type() == ITEM_NODE)
	{
		MessageBox::StandardButton sb = MessageBox::Warning(nullptr, m_en ? "Warning!" : QStringLiteral("警告！"), m_en ? "Are you sure to delete the node?" : QStringLiteral("确定删除节点吗？"));

		if (MessageBox::Ok == sb)
		{
			int node_id = dynamic_cast<NodeItem*>(m_selectItem)->GetNodeId();
			if (m_pNodeItemMng)
			{
				m_pNodeItemMng->RemoveNodeItem(node_id);
			}
		}
	}
	else if (m_selectItem->type() == ITEM_POS)
	{
		MessageBox::StandardButton sb = MessageBox::Warning(nullptr, m_en ? "Warning!" : QStringLiteral("警告！"), m_en ? "Are you sure to delete the node?" : QStringLiteral("确定删除二维码点吗？"));

		if (MessageBox::Ok == sb)
		{
			int node_id = dynamic_cast<QRcodeItem*>(m_selectItem)->GetCodeId();
			if (m_pQRcodeItemMng)
			{
				m_pQRcodeItemMng->RemoveCodeItem(node_id);
			}
		}
	}
}

NodeItem* MapScene::AddNodeItem(const QPointF& pos)
{
	if (m_pNodeItemMng)
	{
		return m_pNodeItemMng->AddNodeItem(pos);
	}
	return NULL;
}

double MapScene::mouse_move_direction()
{
	double angle = -1.0;
	if (!_select_node_item)
	{
		return -1.0;
	}
	QPointF node_pos = _select_node_item->scenePos();
	angle = atan2(_pos_confirm_direct.y() - node_pos.y(), _pos_confirm_direct.x() - node_pos.x());
	angle = PT_ALG::angle_to_normal(angle * 180.0 / PI);
	return angle;
}

void MapScene::DeleteSelectedItems()
{
	QList<QGraphicsItem*> select_items = selectedItems();
	foreach(QGraphicsItem* item, select_items){
		if (!item){
			continue;
		}
		if (item->type() == ItemType::ITEM_EDGE){
			EdgeItem* edge_item = reinterpret_cast<EdgeItem*>(item);
			int edge_id = edge_item->GetId();
			if (m_pEdgeItemMng){
				m_pEdgeItemMng->RemoveEdgeItem(edge_id);
			}
		}
		else if (item->type() == ItemType::ITEM_PolygonItem) {

			PolygonItem* polygonItem = static_cast<PolygonItem*>(item);
			polygonItems.removeOne(polygonItem);
			delete polygonItem;
		}
	}
	QList<QGraphicsItem*> select_nodes = selectedItems();
	foreach(QGraphicsItem* item, select_nodes){
		if (!item){
			continue;
		}
		if (item->type() == ItemType::ITEM_NODE){
			NodeItem* node_item = reinterpret_cast<NodeItem*>(item);
			int node_id = node_item->GetNodeId();
			if (m_pNodeItemMng){
				m_pNodeItemMng->RemoveNodeItem(node_id);
			}
		}
	}
}

int MapScene::GetSegmentPos(const QPointF& mousePos, QPointF& segPos)
{
	SegmentPosWnd segPosWnd;
	segPosWnd.SetPos(mousePos);
	if (segPosWnd.exec() == QDialog::Accepted){
		segPosWnd.GetPos(segPos);
		return 0;
	}
	return -1;
}

void MapScene::SegmentEdge(const QPointF& pos, EdgeItem* pEdgeItem)
{
	if (!m_pEdgeItemMng){
		return;
	}
	int startId = pEdgeItem->StartNode()->GetNodeId();
	int endId = pEdgeItem->EndNode()->GetNodeId();
	int edgeId = pEdgeItem->GetId();
	EDGEINFO edgeInfo;
	pEdgeItem->GetEdgeInfo(edgeInfo);
	m_pEdgeItemMng->RemoveEdgeItem(pEdgeItem->GetId());
	NodeItem* pNodeItem = m_pNodeItemMng->AddNodeItem(pos);
	if (pNodeItem){
		int newNodeId = pNodeItem->GetNodeId();
		EdgeItem* newEdge1 = m_pEdgeItemMng->AddEdgeItem(startId, newNodeId, edgeId);
		if (newEdge1){
			newEdge1->UpdateEdgeAttr(edgeInfo);
		}
		EdgeItem* newEdge2 = m_pEdgeItemMng->AddEdgeItem(newNodeId, endId);
		if (newEdge2){
			newEdge2->UpdateEdgeAttr(edgeInfo);
		}
		if (m_pAnchorItemMng){
			std::vector<ANCHORINFO> vecAnchor;
			m_pAnchorItemMng->GetAnchorInfoOnEdge(edgeId, vecAnchor);
			UpdateAnchorInfo(vecAnchor);//跟新原有边上所有的停靠点信息
		}
	}
}

void MapScene::UpdateAnchorInfo(const std::vector<ANCHORINFO>& vecAnchor)
{
	for (auto iter : vecAnchor){
		QPointF anchorPos;
		anchorPos.setX(iter.pos_x);
		anchorPos.setY(iter.pos_y);
		EdgeItem* pEdgeItem = FindEdgeByPos(anchorPos);
		if (pEdgeItem){
			ANCHORINFO anchorInfo = iter;
			anchorInfo.edge_id = pEdgeItem->GetId();
			position_t pos_t;
			pos_t.x_ = anchorInfo.pos_x;
			pos_t.y_ = anchorInfo.pos_y;
			pos_t.angle_ = anchorInfo.angle;
			UPL anchor_upl;
			PT_ALG::point_on_edge_upl(anchorInfo.edge_id, pos_t, anchor_upl);
			anchorInfo.percent = anchor_upl.percent;
			if (m_pAnchorItemMng){
				m_pAnchorItemMng->UpdateAnchorInfo(anchorInfo);
			}
		}
	}
}

EdgeItem* MapScene::SetEdgeId(int edge_id)
{
	EdgeItem* pEdgeItem = NULL;
	if (pSelectEdgeItem){
		pSelectEdgeItem->setSelected(false);
	}
	pEdgeItem = FindEdgeByEdgeId(edge_id);
	pSelectEdgeItem = pEdgeItem;
	if (pEdgeItem){
		pSelectEdgeItem->setSelected(true);
	}
	return pSelectEdgeItem;
}

void MapScene::SetNodeIdLabel(QLabel*label)
{
	m_node_id_label = label;
}

void MapScene::AdjustAllEdgeProperty(const std::vector<int>& vecWopId)
{
	EdgeItem *edgeItemPtr = nullptr;
	QList<QGraphicsItem *> itemPtrList = items();

	foreach(QGraphicsItem *itemPtr, itemPtrList)
	{
		if (nullptr != itemPtr && ItemType::ITEM_EDGE == itemPtr->type())
		{
			edgeItemPtr = qgraphicsitem_cast<EdgeItem *>(itemPtr);

			if (nullptr != edgeItemPtr)
			{
				edgeItemPtr->SetEdgeWopList(vecWopId);
			}
		}
	}
}

void MapScene::deleteMoreItem()
{
	QList<QGraphicsItem*> all_items = selectedItems();
	if (all_items.size() == 0) return;
	do{
		QList<QGraphicsItem*>::iterator iter;
		iter = all_items.begin();
		QGraphicsItem* item = *iter;
		if (item){
			if (item->type() == ItemType::ITEM_ANCHOR) {
				AnchorItem* node = static_cast<AnchorItem*>(item);
				int anchor_id = dynamic_cast<AnchorItem*>(item)->GetAnchorId();
				if (m_pAnchorItemMng)
				{
					m_pAnchorItemMng->RemoveAnchorItem(anchor_id);
				}
			}
			else if (item->type() == ItemType::ITEM_EDGE) {
				EdgeItem* node = static_cast<EdgeItem*>(item);
				if (m_pEdgeItemMng)
				{
					DeleteEdgeItem(dynamic_cast<EdgeItem*>(item));
				}
			}
			else if (item->type() == ItemType::ITEM_NODE) {
				NodeItem* node = static_cast<NodeItem*>(item);
				int node_id = dynamic_cast<NodeItem*>(item)->GetNodeId();
				if (m_pNodeItemMng)
				{
					m_pNodeItemMng->RemoveNodeItem(node_id);
				}
			}
			else if (item->type() == ItemType::ITEM_POS) {
				QRcodeItem* node = static_cast<QRcodeItem*>(item);
				int node_id = dynamic_cast<QRcodeItem*>(item)->GetCodeId();
				if (m_pQRcodeItemMng)
				{
					m_pQRcodeItemMng->RemoveCodeItem(node_id);
				}
			}
		}

		all_items = selectedItems();
	} while (all_items.size()>0);

}

void MapScene::editMoreNodes()
{
	QList<QGraphicsItem*> all_items = selectedItems();
	if (all_items.size() == 0){
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("未框选任何节点!"), QMessageBox::Ok);
		return;
	}
	NodesEditWnd editWnd;
	if (editWnd.exec() == QDialog::Accepted)
	{	
		QList<QGraphicsItem*>::iterator iter;
		for (iter = all_items.begin(); iter != all_items.end();iter++){
		QGraphicsItem* item = *iter;
		if (item){
			if (item->type() == ItemType::ITEM_NODE) {
				NodeItem* node = static_cast<NodeItem*>(item);
				int node_id = dynamic_cast<NodeItem*>(item)->GetNodeId();
				if (m_pNodeItemMng)
				{
					m_pNodeItemMng->UpdateNodeItem(node_id, editWnd.pSpin);
				}
			}
		}

		} 
	}
}

void MapScene::edieMoreEdges()
{
	QList<QGraphicsItem*> all_items = selectedItems();
	if (all_items.size() == 0){
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("未框选任何边!"), QMessageBox::Ok);
		return;
	}
	EdgesEditWnd editWnd;
	EDGEINFO m_edgeInfo;
	editWnd.SetEdgeInfo(m_edgeInfo);
	if (editWnd.exec() == QDialog::Accepted)
	{
		
		QList<QGraphicsItem*>::iterator iter;
		editWnd.GetEdgeInfo(m_edgeInfo);
		for (iter = all_items.begin(); iter != all_items.end(); iter++)
		{
			QGraphicsItem* item = *iter;
			if (item->type() == ItemType::ITEM_EDGE){
				EdgeItem*edge = static_cast<EdgeItem*>(item);
				edge->UpdateEdgeInfo(m_edgeInfo);
			}
			
		}
	}
}

void MapScene::formatMoreItems()
{
	m_pSelectFormat = m_pSelectItem;
	if (!m_pSelectFormat)
	{
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("请先选择一条边作为参考边!"), QMessageBox::Ok);
		return;
	}
	if (m_pSelectFormat){
		is_Format = true;
	}
}

void MapScene::FormatBrushEdges()
{
	QList<QGraphicsItem*> all_items = selectedItems();
	for (auto iter = all_items.begin(); iter != all_items.end(); iter++){
		QGraphicsItem* item = *iter;
		if (item->type() == ItemType::ITEM_EDGE){
			EdgeItem*edge = static_cast<EdgeItem*>(item);
			if (!m_pSelectFormat)
			{
				QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("请先选择一条边作为参考边!"), QMessageBox::Ok);
				return;
			}
			std::vector<int> vecWopId;
			m_pSelectFormat->GetEdgeWopList(vecWopId);
			EDGEINFO edgeInfo;
			m_pSelectFormat->GetEdgeInfo(edgeInfo);
			if (edge)
			{
				edge->SetEdgeWopList(vecWopId);
				edge->SetEdgeSpeed(edgeInfo.speed);
				edge->SetEdgeWopAttr(edgeInfo.wop_attr_list);
			}
		}
	}
}