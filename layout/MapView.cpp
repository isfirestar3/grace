#include "MapView.h"
#include "MapScene.h"

#define INIT_VIEWSCALE 0.3

MapView::MapView()
:m_curActioType(ActionType::ACTION_SELECT)
, m_scale_history(INIT_VIEWSCALE)
, m_scale_factor(INIT_VIEWSCALE),
m_viewportWidget{ QSharedPointer<ViewportWidget>(new ViewportWidget) }
{
	SetFeature();
	setViewport(m_viewportWidget.data());
}


MapView::~MapView()
{
}

void MapView::SetFeature()
{
	setCacheMode(CacheBackground);//背景缓存模式
	//setViewportUpdateMode(BoundingRectViewportUpdate);//视口的更新模式，
	setRenderHint(QPainter::Antialiasing);//抗锯齿
	setResizeAnchor(QGraphicsView::AnchorUnderMouse);
	setTransformationAnchor(AnchorUnderMouse);
	//setOptimizationFlags(QGraphicsView::DontSavePainterState);
	//setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	setMouseTracking(true);//设置跟踪
	setMatrix(QMatrix(1.0, 0.0, 0.0, -1.0, 0.0, 0.0), true);//转化下视图	

	//scale(INIT_VIEWSCALE, INIT_VIEWSCALE);
	centerOn(0, 0);
	//setMinimumSize(400, 400);

	setFocusPolicy(Qt::WheelFocus);
}

void MapView::SetViewScale(qreal scaleFactor)
{
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if ((m_scale_history > 10 && scaleFactor > 0) || (m_scale_history < 0.1 && scaleFactor < 0))
	{
		return;
	}

	if (scaleFactor > 0)
	{
		scale(1.3, 1.3);
	}
	else
	{
		scale(0.7, 0.7);
	}

	QTransform  transf = transform();
	m_scale_history = transf.m11() / INIT_VIEWSCALE;
	((MapScene*)scene())->UpdateScale(m_scale_history);
}

void MapView::SetOperaType(ActionType actionType)
{
	m_curActioType = actionType;
	if (actionType == ActionType::ACTION_HAND_DRAG)
	{
		setDragMode(ScrollHandDrag);
	}
	else
	{
		setDragMode(NoDrag);
	}
	SetCurorType(actionType);
}

void MapView::SetCurorType(ActionType actionType)
{
	QCursor cursor;
	switch (actionType)
	{
	case ActionType::ACTION_HAND_DRAG:
		{
			QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/cursor_hand.png"));
			QCursor cursorBrush(pixmap, 0, 15);
			cursor = cursorBrush;
		}
		break;
	case ActionType::ACTION_ADD_NODE:
		{
			QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/node.png"));
			QCursor cursorBrush(pixmap, -1, -1);
			cursor = cursorBrush;
		}
		break;
	case ActionType::ACTION_ADD_EDGE:
		{
			QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/pen.png"));
			QCursor cursorBrush(pixmap, 0, 24);
			cursor = cursorBrush;
		}
		break;
	case ActionType::ACTION_MOVE_ITEM:
		{
			QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/move.png"));
			QCursor cursorBrush(pixmap, -1, -1);
			cursor = cursorBrush;
		}
		break;
	case ActionType::ACTION_ADD_VEHICLE:
		{
			QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/vehicle_small.png"));
			QCursor cursorBrush(pixmap, -1, -1);
			cursor = cursorBrush;
		}
		break;
	case ActionType::ACTION_ENTER_NAV:
		{
			QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/navigation.png"));
			QCursor cursorBrush(pixmap, 16, 2);
			cursor = cursorBrush;
		}
		break;
	case ActionType::ACTION_FORMAT:
		{
			QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/clear.png"));
			QCursor cursorBrush(pixmap, 5, 15);
			cursor = cursorBrush;
		}
		break;
	case ActionType::ACTION_ADD_ANCHOR:
		{
			QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/anchor.png"));
			QCursor cursorBrush(pixmap, -1, -1);
			cursor = cursorBrush;
		}
		break;
	case ActionType::ACTION_LINK_CURVE:
		{
			QPixmap pixmap(QString::fromUtf8(":/newPrefix/images/curve_pen.png"));
			QCursor cursorBrush(pixmap, 0, 24);
			cursor = cursorBrush;
		}
		break;
	case ActionType::ACTION_ADD_POLYGON:
		cursor.setShape(Qt::CrossCursor);
		break;
	case ActionType::ACTION_SELECT:
		cursor.setShape(Qt::ArrowCursor);
		break;
	default:
		break;
	}
	//setCursor(cursor);
	m_viewportWidget->SetCursor(cursor);
}

ActionType MapView::GetOperaType()
{
	return m_curActioType;
}

void MapView::LocationCenterItem()
{
	MapScene *pGraphicsScene = qobject_cast<MapScene *>(scene());

	if (nullptr == pGraphicsScene)
	{
		return;
	}

	QGraphicsItem *pItem = nullptr;
	QList<QGraphicsItem *> selectedItems = pGraphicsScene->selectedItems();

	if (!selectedItems.isEmpty())
	{
		int itemCount = selectedItems.size();
		int offset = itemCount / 2;

		if (offset < 0 || offset >= itemCount)
		{
			return;
		}

		pItem = selectedItems[offset];
	}
	else
	{
		QList<QGraphicsItem *> items = pGraphicsScene->items();

		if (items.isEmpty())
		{
			return;
		}

		int itemCount = items.size();
		int offset = itemCount / 2;

		if (offset < 0 || offset >= itemCount)
		{
			return;
		}

		pItem = items[offset];
	}

	if (nullptr != pItem)
	{
		centerOn(pItem);
	}
}

void MapView::wheelEvent(QWheelEvent *event)
{
	if (event == NULL)
	{
		return;
	}

	SetViewScale(event->delta());
}