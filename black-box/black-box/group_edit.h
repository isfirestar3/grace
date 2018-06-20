#ifndef GROUP_EDIT_H
#define GROUP_EDIT_H

#include <QDialog>
#include "ui_group_edit.h"
#include "template_manage.h"

class group_edit : public QDialog
{
	Q_OBJECT

public:
	group_edit(QWidget *parent = 0);
	~group_edit();

	void set_group_data(std::vector<std::string>&group);
	std::string group_name_;
	std::map<std::string, std::string>map_log_types_;
	std::map<std::string, std::vector<std::string>>template_type_;
	int index;
private slots:
	void on_pushButton_clicked();
private:
	Ui::group_edit ui;
	template_manage*tem_manage_;
};

#endif // GROUP_EDIT_H
