#ifndef POLYGONITEM_H
#define POLYGONITEM_H

#include <QtWidgets/qgraphicsitem.h>
#include <QtCore/qvector.h>
#include "EdgeListShowWnd.h"
#include "BasicDefine.h"

extern int polygonItemCounter;
class PolygonVertexItem;
class PolygonItem : public QGraphicsPolygonItem {

public:
    PolygonItem(QGraphicsItem* parentItem = 0);
	int type() const { return ItemType::ITEM_PolygonItem; }
	~PolygonItem();
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	int id = -1;
	void setPolygon(const QPolygonF &polygon);
	void updateVertexPos();
private:
	EdgeListShowWnd* edgeListShowWnd = 0;
	QVector<PolygonVertexItem*> vertexItems;
};

#endif // POLYGONITEM_H
