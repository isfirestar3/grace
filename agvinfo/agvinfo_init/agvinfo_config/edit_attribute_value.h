#ifndef EDIT_ATTRIBUTE_VALUE_H
#define EDIT_ATTRIBUTE_VALUE_H

#include <QDialog>
#include "ui_edit_attribute_value.h"
#include "libagvinfo.h"
#include "agv_single.h"

class edit_attribute_value : public QDialog
{
	Q_OBJECT

public:	
	edit_attribute_value(int vchid, std::map<std::string, std::string> map_attr, QWidget *parent = 0);
	edit_attribute_value(QWidget *parent = 0);
	~edit_attribute_value();

	void init_page();

	void init_attribute(std::map<std::string, std::string> map_attribute)
	{
		map_attribute_ = std::move(map_attribute);
	}

	void init_agvinfo(std::vector<inner_agvinfo> vecagvinfo,int index ){
		vec_agvinfo_ = std::move(vecagvinfo);
		sel_index_ = index;
	};

signals:
	void signal_refresh_window();
private:
	void clear();

	void init_ui();

	bool ischecked(int index);

	void setcheck(int index, bool bCheck);

	void init_slot();
private slots:
	void on_yesButton_clicked();

	void stateChanged_checkbox( int );
private:
	Ui::edit_attribute_value ui;
	std::map<std::string, std::string> map_attribute_;
	int id_;
	std::vector<inner_agvinfo> vec_agvinfo_;
	int sel_index_;
};

#endif // EDIT_ATTRIBUTE_VALUE_H
