#include "EMapView.h"
#include <iostream>

EMapView::EMapView(QWidget *parent)
	: QGraphicsView(parent)
{
	setParent(parent);
	setCacheMode(CacheBackground);//背景缓存模式
	setViewportUpdateMode(BoundingRectViewportUpdate);//视口的更新模式，
	setRenderHint(QPainter::Antialiasing);//抗锯齿
	setTransformationAnchor(AnchorUnderMouse);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	setMouseTracking(true);//设置跟踪
	setMatrix( QMatrix(1.0, 0.0 , 0.0 , -1.0 , 0.0 , 0.0) , true );//转化下视图

	setDragMode(QGraphicsView::ScrollHandDrag);
	setWindowTitle(tr("EMap"));
}

void EMapView::FitInView()
{
	QRectF m_rect = map_rect_;
	centerOn(m_rect.center());
	fitInView(m_rect, Qt::KeepAspectRatio);
	min_scaled_ = transform().m11();
}

void EMapView::SetEMapRect(QRectF rect)
{
	map_rect_ = rect;
}

void EMapView::wheelEvent(QWheelEvent *event)
{
	if (event->delta() > 0) {
		scale(1.3, 1.3);
	}
	else {
		double m_value = transform().m11();
		if (m_value * 0.7 > min_scaled_) {
			scale(0.7, 0.7);
		}
		else {
			this->FitInView();
		}
	}
	//QGraphicsView::wheelEvent(event);
}
