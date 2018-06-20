#ifndef LOGIN_WID_H
#define LOGIN_WID_H

#include <QDialog>
#include "ui_login_wid.h"

class login_wid : public QDialog
{
	Q_OBJECT

public:
	login_wid(QWidget *parent = 0);
	~login_wid();

private:
	Ui::login_wid ui;
};

#endif // LOGIN_WID_H
