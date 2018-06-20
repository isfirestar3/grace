#include "viewportwidget.h"
#include <QtGui/QGuiApplication>

ViewportWidget::ViewportWidget(QWidget *parent)
    : QWidget(parent)
{

}

void ViewportWidget::enterEvent(QEvent *event)
{
	if (nullptr == event)
	{
		return;
	}

	QGuiApplication::setOverrideCursor(m_cursor);
	QWidget::enterEvent(event);
}

void ViewportWidget::leaveEvent(QEvent *event)
{
	if (nullptr == event)
	{
		return;
	}

	QGuiApplication::restoreOverrideCursor();
	QWidget::leaveEvent(event);
}

