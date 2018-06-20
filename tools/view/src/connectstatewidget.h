#ifndef CONNECTSTATEWIDGET_H
#define CONNECTSTATEWIDGET_H

#include <QtWidgets/QWidget>
#include "ui_connectstatewidget.h"

class ConnectStateWidget : public QWidget
{
	Q_OBJECT

public:
	ConnectStateWidget(QWidget *parent = nullptr);

	void InitUi();
	void OnUpdateConnectState(bool connect, const QString &ipAddr = QString(), int port = -1);
private:
	Ui::ConnectStateWidget ui;
};

#endif // CONNECTSTATEWIDGET_H
