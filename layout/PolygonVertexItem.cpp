#include "PolygonVertexItem.h"
#include "polygonitem.h"
#include <QtGui/qbrush.h>

PolygonVertexItem::PolygonVertexItem(PolygonItem* parentItem)
	:QGraphicsEllipseItem(), m_parentItem(parentItem) {

	setBrush(Qt::yellow);
	setRect(-5, -5, 10, 10);
	setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsScenePositionChanges);
}

QVariant PolygonVertexItem::itemChange(GraphicsItemChange change, const QVariant &value) {

	if (change == ItemPositionHasChanged)
		m_parentItem->updateVertexPos();
	return QGraphicsEllipseItem::itemChange(change, value);
}