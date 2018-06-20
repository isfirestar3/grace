#ifndef TEMPLATE_MANAGE_H
#define TEMPLATE_MANAGE_H

#include <QDialog>
#include "ui_template_manage.h"

class template_manage : public QDialog
{
	Q_OBJECT

public:
	template_manage(QWidget *parent = 0);
	~template_manage();

	void init_page();

	std::map<std::string, std::string>map_log_types_;
	std::map<std::string, std::vector<std::string>>template_type_;
private slots:
	void on_pushButton_clicked();
public:
	Ui::template_manage ui;
};

#endif // TEMPLATE_MANAGE_H
