#ifndef VIEWPORTWIDGET_H
#define VIEWPORTWIDGET_H

#include <QtWidgets/QWidget>

class ViewportWidget : public QWidget
{
public:
    explicit ViewportWidget(QWidget *parent = nullptr);

	void SetCursor(const QCursor &cursor)
	{
		m_cursor = cursor;
	}
protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
private:
	QCursor m_cursor{Qt::ArrowCursor};
};

#endif // VIEWPORTWIDGET_H
