#pragma once

#include "QtWidgets/qgraphicsview.h"
#include "QtGui/qevent.h"

class EMapView : public QGraphicsView
{
	Q_OBJECT

public:
	EMapView(QWidget *parent = 0);

public:
	void FitInView();
	void SetEMapRect(QRectF rect);

protected:
	void wheelEvent(QWheelEvent *event);

private:
	double min_scaled_;
	QRectF map_rect_;
};

