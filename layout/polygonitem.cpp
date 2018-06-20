#include "polygonitem.h"
#include "EdgeItem.h"
#include "MainWnd.h"
#include "PolygonVertexItem.h"
#include <QtWidgets/qdialog.h>
#include <QtGui/qpen.h>
#include <QtWidgets/qapplication.h>

int polygonItemCounter = 0;
PolygonItem::PolygonItem(QGraphicsItem* parentItem)
    :QGraphicsPolygonItem(parentItem) {

    setPen(QPen(Qt::green));
    setFlag(ItemIsSelectable);
	setZValue(-1000);
}

PolygonItem::~PolygonItem() {

	delete edgeListShowWnd;
	qDeleteAll(vertexItems);
}

void PolygonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {

	QListIterator<QGraphicsItem*> i(collidingItems());
	std::vector<EDGEINFO> vecEdgeInfo;
	while (i.hasNext()) {

		QGraphicsItem* item = i.next();
		if (item->type() == ItemType::ITEM_EDGE) {

			EdgeItem* edge = static_cast<EdgeItem*> (item);
			vecEdgeInfo.push_back(edge->m_edgeInfo);
		}
	}
	if (!edgeListShowWnd) {
		
		edgeListShowWnd = new EdgeListShowWnd;
		edgeListShowWnd->resize(1000, 700);
	}
	edgeListShowWnd->SetAllEdgesInfo(vecEdgeInfo);
	edgeListShowWnd->show();
    QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}

void PolygonItem::setPolygon(const QPolygonF &polygon) {

	qDeleteAll(vertexItems);
	QGraphicsPolygonItem::setPolygon(polygon);
	QVectorIterator<QPointF> i(polygon);
	while (i.hasNext()) {

		PolygonVertexItem* polygonVertex = new PolygonVertexItem(this);
		vertexItems << polygonVertex;
		polygonVertex->setPos(i.next());
		scene()->addItem(polygonVertex);
	}
}

void PolygonItem::updateVertexPos() {

	QVectorIterator<PolygonVertexItem*> i(vertexItems);
	QPolygonF polygon;
	while (i.hasNext())
		polygon << i.next()->pos();
	QGraphicsPolygonItem::setPolygon(polygon);
}