
#ifndef SYSTEMBUTTON_H
#define SYSTEMBUTTON_H


#include <QtWidgets/QPushButton>
#include <QtGui\QPainter>
#include <QtGui\QMouseEvent>

class PushButton : public QPushButton
{
	Q_OBJECT

public:
	enum class PB_State
	{
		PB_Invalid,
		PB_Enabled,
		PB_Hover,
		PB_Disabled
	};
	explicit PushButton(QWidget *parent = 0);
	void SetPixmap(const QString &pixmap);

protected:
	void changeEvent(QEvent *e);
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	void paintEvent(QPaintEvent *);
private:
	PB_State m_state;
	QString m_pixmapEnabled;
	QString m_pixmapHover;
	QString m_pixmapDisabled;
};

#endif //SYSTEMBUTTON_H

