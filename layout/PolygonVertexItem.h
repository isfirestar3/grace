#pragma once

#include <QtWidgets/qgraphicsitem.h>

class PolygonItem;
class PolygonVertexItem : public QGraphicsEllipseItem {

public:
	PolygonVertexItem(PolygonItem* parentItem = 0);
	PolygonItem* m_parentItem;

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

