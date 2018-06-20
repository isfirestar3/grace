#ifndef WARNING_PAGE_H
#define WARNING_PAGE_H

#include <QDialog>
#include "ui_warning_page.h"

class warning_page : public QDialog
{
	Q_OBJECT

public:
	warning_page(QWidget *parent = 0);
	~warning_page();
	bool state_ = false;

private slots:
	void on_yesRadioButton_clicked();
	void on_noRadioButton_clicked();
private:
	Ui::warning_page ui;
	
};

#endif // WARNING_PAGE_H
