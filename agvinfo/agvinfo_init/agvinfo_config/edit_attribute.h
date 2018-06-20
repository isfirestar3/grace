#ifndef EDIT_ATTRIBUTE_H
#define EDIT_ATTRIBUTE_H

#include <QDialog>
#include "ui_edit_attribute.h"
#include "libagvinfo.h"
#include <QtWidgets>
#include "agv_single.h"

class edit_attribute : public QDialog
{
	Q_OBJECT

public:
	edit_attribute(QWidget *parent = 0);
	~edit_attribute();

	void init_tablewidget();

	void init_agvinfo(std::vector<inner_agvinfo> vecagvinfo){
		vec_agvinfo_ = std::move(vecagvinfo);
	};

	void init_attr(std::map<std::string, std::string> map_attr){
		map_attribute_ = std::move(map_attr);
	}

private:
	void add_attribute(int index, std::string strattr = "", bool bCheck = false);
private slots:
	void on_addButton_clicked();
	void on_deleteButton_clicked();

	void on_yesButton_clicked();

	void item_changed(QTableWidgetItem * item);

	void item_changed();

	void set_item_edit();
private:
	Ui::edit_attribute ui;
	std::vector<inner_agvinfo> vec_agvinfo_;
	std::map<std::string, std::string> map_attribute_;
};

#endif // EDIT_ATTRIBUTE_H
