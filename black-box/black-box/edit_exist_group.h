#ifndef EDIT_EXIST_GROUP_H
#define EDIT_EXIST_GROUP_H

#include <QDialog>
#include "ui_edit_exist_group.h"

class edit_exist_group : public QDialog
{
	Q_OBJECT

public:
	edit_exist_group(QWidget *parent = 0);
	~edit_exist_group();

	std::string group_name_;
	std::map<std::string, std::string>map_log_types_;
	std::vector<std::string>template_type_;
	bool is_delete = false;

	void init_edit_group();
private slots:
	void on_okButton_clicked();
	void on_deleteButton_clicked();
	void on_checkBox_clicked(bool state);
private:
	Ui::edit_exist_group ui;
};

#endif // EDIT_EXIST_GROUP_H
