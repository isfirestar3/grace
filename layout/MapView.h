#pragma once
#include <QtWidgets/qgraphicsview.h>
#include <QtGui/qevent.h>
#include "BasicDefine.h"
#include "viewportwidget.h"

class MapView : public QGraphicsView
{
public:
	MapView();
	~MapView();
public:
	void SetFeature();
	void SetViewScale(qreal scaleFactor);
	void SetOperaType(ActionType actionType);
	void SetCurorType(ActionType actionType);
	ActionType GetOperaType();
	void LocationCenterItem();
protected:
	void wheelEvent(QWheelEvent *event);
private:
	ActionType m_curActioType;
	qreal m_scale_factor;
	qreal m_scale_history;
	QSharedPointer<ViewportWidget> m_viewportWidget;
};

