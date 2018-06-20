#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>
class EMapNode : public QGraphicsItem
{
public:
	EMapNode(int node_id, QGraphicsItem* parent = 0);

public:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void SetColor(QColor & color, bool color_valid = true);
	void SetShow(bool bShow);
private:
	static const int kNormalShapeSize_ = 40;

private:
	int node_id_;
	QColor color_;
	bool color_valid_;
	
};


