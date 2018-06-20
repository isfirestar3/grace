#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>
//#include "MapScene.h"
#include "BasicDefine.h"

class EdgeItem;
class MapScene;

class QRcodeItem : public QGraphicsItem
{
public:
	QRcodeItem(MapScene* mapScene);
	~QRcodeItem();
public:
	int type() const { return ItemType::ITEM_POS; }
	void SetFeature();
	void SetCodeInfo(const CODEPOSXYA& nodeInfo);
	int GetCodeId();
	void SetMoveable(bool bMoveable);
	void SetPosData(const QPointF& pos);

private:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void DrawCode(QPainter* painter);
protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	QVariant itemChange(GraphicsItemChange change, const QVariant& value);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
private:
	//MapScene* m_pMapScene;
	CODEPOSXYA m_posInfo;
	double m_dScale;
	bool m_bMouseHover;//是否鼠标悬停
	bool m_bMousePress;//鼠标是否按下
	bool m_bAlign;//是否启用对齐线

	QList<EdgeItem*> m_listEdgeItem;//存放与此节点关联的边
	MapScene* m_mapScene;
	
};

