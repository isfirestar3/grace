#pragma once
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>

class EMapAgv : public QGraphicsItem
{
public:
	EMapAgv(int car_id, QGraphicsItem *parent = 0);

	// µ¥Î»m
	void SetAgvSize(double agv_width, double agv_head, double agv_tail);

public:
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	static const int kAgvWidth = 120;
	static const int kAgvHead = 150;
	static const int kAgvTail = 150;

private:
	int car_id_;
	
	int agv_width_;
	int agv_head_;
	int agv_tail_;

	QImage *agv_image_;
};

