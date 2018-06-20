#ifndef GROUP_TAB_H
#define GROUP_TAB_H

#include <QDialog>
#include "ui_group_tab.h"
#include "template_manage.h"
#include "mainMind.h"

class group_tab : public QDialog
{
	Q_OBJECT

public:
	group_tab(QWidget *parent = 0);
	~group_tab();

	void set_group_data(std::vector<std::string>&group);

	void set_group_check(bool state);
	std::string group_name_;

	Ui::group_tab ui;
private slots:
	void on_radioButton_clicked(bool state);
private:
	log_select *parent_page_;
};

#endif // GROUP_TAB_H
