#ifndef NETCONFIGFRAME_H
#define NETCONFIGFRAME_H

#include <QWidget>
#include "ui_netconfigframe.h"

class netconfigframe : public QWidget
{
	Q_OBJECT

public:
	netconfigframe(QWidget *parent = 0);
	~netconfigframe();

private:
	Ui::netconfigframe ui;
};

#endif // NETCONFIGFRAME_H
