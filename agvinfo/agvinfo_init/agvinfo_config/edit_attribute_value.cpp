#include "edit_attribute_value.h"
#include "toolkit.h"
#include <QtWidgets\QMessageBox.h>
#include "agv_single.h"

edit_attribute_value::edit_attribute_value(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	init_ui();
	init_slot();
}

edit_attribute_value::edit_attribute_value(int id, std::map<std::string, std::string> map_attr, QWidget *parent /*= 0*/)
{
	id_ = id;
	init_attribute(map_attr);
	ui.setupUi(this);
	init_ui();
	init_slot();
}

edit_attribute_value::~edit_attribute_value()
{

}

void edit_attribute_value::init_ui()
{
	int row = 0;
	for (auto iter : map_attribute_){
		ui.tableWidget->insertRow(row);

		QWidget *widget = new QWidget(ui.tableWidget);
		QCheckBox * pCheckBox = new QCheckBox();
		pCheckBox->setCheckState(Qt::Unchecked);
		QHBoxLayout *hLayout = new QHBoxLayout();
		hLayout->addWidget(pCheckBox);
		hLayout->setMargin(0);                          // 必须添加, 否则CheckBox不能正常显示
		hLayout->setAlignment(pCheckBox, Qt::AlignCenter);
		widget->setLayout(hLayout);
		ui.tableWidget->setCellWidget(row, 0, widget);

		QTableWidgetItem* vename = new QTableWidgetItem;
		vename->setText(QString::fromLocal8Bit(iter.first.c_str()));
		ui.tableWidget->setItem(row, 1, vename);
		vename->setFlags(vename->flags() & (~Qt::ItemIsEditable));//不可编辑
		ui.tableWidget->item(row, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		QTableWidgetItem* value_attr = new QTableWidgetItem;
		value_attr->setText(QString::fromLocal8Bit(iter.second.c_str()));
		ui.tableWidget->setItem(row, 2, value_attr);
		ui.tableWidget->item(row, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		row++;
	}
}

void edit_attribute_value::init_slot(){
	connect(ui.checkBox, SIGNAL(stateChanged(int)), SLOT(stateChanged_checkbox(int)));
}

void edit_attribute_value::stateChanged_checkbox( int checked ){
	bool bCheck = ui.checkBox->isChecked();
	for (int index = 0; index < ui.tableWidget->rowCount(); index++){
		setcheck(index, bCheck);
	}
}

void edit_attribute_value::clear(){
	for (int index = 0; index < ui.tableWidget->rowCount(); index++){
		QWidget*widget = ui.tableWidget->cellWidget(index, 0);
		if (widget){
			QLayout * layout = widget->layout();
			if (layout){
				for (int num = 0; num < layout->count(); num++){
					QLayoutItem *it = layout->itemAt(num);
					QWidget *widget = it->widget();
					if (widget){
						delete widget;
					}
				}
				delete layout;
			}
			delete widget;
		}

		QTableWidgetItem *attr_name = ui.tableWidget->item(index, 1);
		if (attr_name){
			delete attr_name;
		}
		QTableWidgetItem* attr_value = ui.tableWidget->item(index, 2);
		if (attr_value){
			delete attr_value;
		}
	}
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(0);
}

bool edit_attribute_value::ischecked(int index){
	QWidget*widget = ui.tableWidget->cellWidget(index, 0);
	if (widget){
		QLayout * layout = widget->layout();
		if (layout){
			QCheckBox * pcheck = (QCheckBox *)layout->itemAt(0)->widget();
			if (pcheck){
				return pcheck->isChecked();
			}
		}
	}

	return false;
}

void edit_attribute_value::setcheck(int index, bool bCheck){
	QWidget*widget = ui.tableWidget->cellWidget(index, 0);
	if (widget){
		QLayout * layout = widget->layout();
		if (layout){
			QCheckBox * pcheck = (QCheckBox *)layout->itemAt(0)->widget();
			if (pcheck){
				pcheck->setChecked(bCheck);
			}
		}
	}
}

void edit_attribute_value::on_yesButton_clicked()
{
	bool change = false;
	for (int index = 0; index < ui.tableWidget->rowCount(); index++){
		bool bcheck = ischecked(index);
		if (bcheck){
			QTableWidgetItem*attr_name = ui.tableWidget->item(index, 1);
			QTableWidgetItem*attr_value = ui.tableWidget->item(index, 2);
			auto iter_map = map_attribute_.find(attr_name->text().toStdString());
			if (iter_map!= map_attribute_.end() && iter_map->second.compare(attr_value->text().toStdString())){
				map_attribute_[attr_name->text().toStdString()] = attr_value->text().toStdString();
				change = true;
			}
		}
	}
	std::map<std::string, std::string > map_attr;
	for (auto iter : map_attribute_){
		if (iter.first.compare("mtport") != 0 && iter.first.compare("shport") != 0 && iter.first.compare("ftsport") != 0){
			map_attr.emplace(iter.first.c_str(),iter.second.c_str());
		}
	}

	int mtport = atoi(map_attribute_["mtport"].c_str());
	int shport = atoi(map_attribute_["shport"].c_str());;
	int ftsport = atoi(map_attribute_["ftsport"].c_str());;
	bool bSingle = ui.singleButton->isChecked();
	agv_info * agvinfo = nullptr;
	if (bSingle){
		if (change){
			bool ex_change = false;
			
			if (vec_agvinfo_[sel_index_].mtport != mtport ||
				vec_agvinfo_[sel_index_].shport != shport ||
				vec_agvinfo_[sel_index_].ftsport != ftsport || -1 == id_ )
			{
				vec_agvinfo_[sel_index_].mtport = atoi(map_attribute_["mtport"].c_str());
				vec_agvinfo_[sel_index_].shport = atoi(map_attribute_["shport"].c_str());
				vec_agvinfo_[sel_index_].ftsport = atoi(map_attribute_["ftsport"].c_str());

				if (!nsp::toolkit::singleton<agv_single>::instance()->build_agvinfo(vec_agvinfo_, [&](int index)->std::map<std::string, std::string>{
					return map_attr;
				}, &agvinfo)){
					if (cover_agvinfo(agvinfo, LAM_Server)){
						QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("failed to update data."), QMessageBox::Ok);
					}
				}
				release_agvinfo(agvinfo);
			}
			else{
				agv_detail * detail = nsp::toolkit::singleton<agv_single>::instance()->create_detail(map_attr, id_);
				if (detail != nullptr){
					if (set_agvdetail(id_, detail,LAM_Server)){
						QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("failed to update data."), QMessageBox::Ok);
					}
				}
				release_agvdetail(detail);
			}
		}
	}
	else{
		for (auto iter = vec_agvinfo_.begin(); iter != vec_agvinfo_.end(); iter++){
			iter->mtport = atoi(map_attribute_["mtport"].c_str());
			iter->shport = atoi(map_attribute_["shport"].c_str());
			iter->ftsport = atoi(map_attribute_["ftsport"].c_str());
		}

		if (!nsp::toolkit::singleton<agv_single>::instance()->build_agvinfo(vec_agvinfo_, [&](int index)->std::map<std::string, std::string>{
			return map_attr;
		}, &agvinfo)){
			if (cover_agvinfo(agvinfo, LAM_Server)){
				QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("failed to update data."), QMessageBox::Ok);
			}
		}
		release_agvinfo(agvinfo);
	}
}
