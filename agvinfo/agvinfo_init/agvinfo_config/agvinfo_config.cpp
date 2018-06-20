#include "agvinfo_config.h"
#include <QHeaderView>
#include <qtimer.h>
#include <QtCore\qstring.h>
#include "item_delegate.h"
#include <QtWidgets\QMessageBox.h>
#include <qmessagebox.h>
#include "agv_single.h"
#include "toolkit.h"
#include "set"

struct agv_UserData:QObjectUserData {
	int index_;
	std::map<std::string, std::string> map_attr_;
};

Q_DECLARE_METATYPE(agv_UserData);

agvinfo_config::agvinfo_config(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);


	ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);	
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

	item_delegate *itemDelegate=new item_delegate(this);
	ui.tableWidget->setItemDelegateForColumn(1, itemDelegate);//第二列使用delegate

	//ui.attributeWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//固定宽度不可变
	ui.attributeWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui.attributeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//表格不可编辑


	//ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//固定宽度不可变
	//connect(ui.tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), ui.tableWidget, SLOT(sortByColumn(int)));//表头排序
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//表格选中一行
	QRegExp regExp("(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])");//只能输入IP格式
	ui.IPLineEdit->setValidator(new QRegExpValidator(regExp, this));
	ui.portLineEdit->setValidator(new QIntValidator(1, 65535, this));
	pMapper_ = new QSignalMapper(this);
	QHeaderView *header = ui.tableWidget->horizontalHeader();
	//SortIndicator为水平标题栏文字旁边的三角指示器
	map_soft_.emplace(1, false);
	map_soft_.emplace(2, false);
	map_soft_.emplace(3, false);
	header->setSortIndicatorShown(true);
	connect(header, SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn(int)));

	connect(pMapper_, SIGNAL(mapped(QString)), this, SLOT(btn_edit_slot(QString)));
	//connect(pMapper_, SIGNAL(mapped(QString)), this, SLOT(btn_edit_slot(QString)));
}

agvinfo_config::~agvinfo_config()
{

}

void agvinfo_config::agvinfo_regiset_callback(const std::function<void()>&func)
{
	if (func){
		regiset_callback_ = func;
	}
}

extern agvinfo_config *agv_ptr;
void fun_notify()
{
	if (agv_ptr)
	agv_ptr->notify_handler();
	//static int i = 0;
	//i++;
}

int agvinfo_config::init_agvinfo()
{
	if (ui.IPLineEdit->text() == "" || ui.portLineEdit->text() == ""){
		QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("Enter a valid IP or port"), QMessageBox::Ok);
		return -1;
	}
	if (connect_agvinfo_server(ui.IPLineEdit->text().toLocal8Bit(), ui.portLineEdit->text().toInt()) < 0){
		QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("failed to connect agvinfo_server"), QMessageBox::Ok);
		return -1;
	}
	QMessageBox::information(this, QStringLiteral("infomation："), QStringLiteral("Connect success!"), QMessageBox::Ok);

	agv_info * agvinfo = nullptr;
	if (load_agvinfo(&agvinfo, LAM_Real) < 0){
		//ui.updatePushButton->setEnabled(false);
		return 0;
	}
#ifdef debug_test
	if (load_agvinfo(&agvinfo, LAM_Local) < 0){
		//ui.updatePushButton->setEnabled(false);
		return 0;
	}
	cover_agvinfo(agvinfo, LAM_Server);
#endif // debug_test
	cover_local_agvinfo(agvinfo);
	release_agvinfo(agvinfo);
	//ui.updatePushButton->setEnabled(true);
	QTimer::singleShot(0, this, SLOT(update_table_view()));
	about_something_changed(fun_notify);
	return 0;
}

void agvinfo_config::notify_handler()
{
	agv_info *tmp_info = nullptr;
	if (load_agvinfo(&tmp_info, LAM_Real)){
		return;
	}

	cover_local_agvinfo(tmp_info);
	release_agvinfo(tmp_info);
	QTimer::singleShot(0, this, SLOT(update_table_view()));
}

void agvinfo_config::sortByColumn(int index){
	if (index == 0){
		return;
	}

	bool soft = map_soft_[index];
	if (soft){
		ui.tableWidget->sortItems(index, Qt::AscendingOrder);
	}
	else{
		ui.tableWidget->sortItems(index, Qt::DescendingOrder);
	}
	map_soft_[index] = !soft;
	soft_index_ = index;
}

void agvinfo_config::cover_local_agvinfo( const agv_info * agvs)
{
	std::lock_guard<decltype(agv_lock_)>lock(agv_lock_);
	vec_agvinfo_.clear();
	int num = 0;
	for (auto * iter = agvs; iter != nullptr; iter = iter->next){
		inner_agvinfo agvinfo;
		agvinfo.vhid = iter->vhid;
		agvinfo.vhtype = iter->vhtype;
		agvinfo.strip = iter->inet;
		agvinfo.mtport = iter->mtport;
		agvinfo.shport = iter->shport;
		agvinfo.ftsport = iter->ftsport;
		agvinfo.strmac = iter->hwaddr;
		agvinfo.status = iter->status;

		for (int index = 0; index < iter->count_of_attrs; index++){
			agvinfo.map_attr.emplace(iter->attrs[index].name, iter->attrs[index].describe);
		}
#ifdef debug_test
		num++;
		if (num < 5){
			agvinfo.strmac = "AD:FD:SF:DF:FD:FD:FD";
			agvinfo.status = 3;
		}
#endif
		vec_agvinfo_.push_back(agvinfo);
	}
}

void agvinfo_config::on_updatePushButton_clicked()
{
	std::set<int> set_vhid;
	for (int row = 0; row < ui.tableWidget->rowCount(); ++row){
		QTableWidgetItem*veid = ui.tableWidget->item(row, 1);
		int vhid = veid->text().toInt();
		std::string ipstr = ui.tableWidget->item(row, 3)->text().toStdString();
		if (set_vhid.find(vhid) != set_vhid.end() && vhid != -1){
			QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("There is a repeat ID"), QMessageBox::Ok);
			ui.tableWidget->editItem(veid);
			return;
		}
		agv_UserData userdata = veid->data(Qt::UserRole).value<agv_UserData>();

		vec_agvinfo_[userdata.index_].vhid = vhid;
		vec_agvinfo_[userdata.index_].strip = ipstr;
		set_vhid.emplace(vhid);
	}

	update_agvinfo(vec_agvinfo_);
}

void agvinfo_config::update_agvinfo( std::vector<inner_agvinfo> vec_agvinfo)
{
	agv_info * agvinfo = nullptr;
	if (!nsp::toolkit::singleton<agv_single>::instance()->build_agvinfo(vec_agvinfo, [&](int index)->std::map<std::string, std::string>{
		std::map<std::string, std::string > map_attr;
		if (index < vec_agvinfo.size()){
			map_attr = vec_agvinfo[index].map_attr;
		}
		return map_attr;
	}, &agvinfo)){
		if (cover_agvinfo(agvinfo, LAM_Server)){
			QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("failed to update data."), QMessageBox::Ok);
		}
	}
	release_agvinfo(agvinfo);
}

void agvinfo_config::update_table_view()
{
	if (ui.tableWidget->isVisible()){
		refresh_view_page();
	}
	else{
		refresh_attr_page();
	}	
}

void agvinfo_config::on_getPushButton_clicked()
{
	if (init_agvinfo() >= 0){
		ui.getPushButton->setEnabled(false);
	}

}
void agvinfo_config::on_pushButton_clicked()
{
	agv_info * agvinfo = nullptr;
	std::lock_guard<decltype(agv_lock_)>lock(agv_lock_);
	//std::vector<agv_info> vecagvinfo;
	//for (auto iter : vec_agvinfo_){
	//	iter.vhid = -1;
	//	iter.hwaddr[0] = '\0';
	//	vecagvinfo.push_back(iter);
	//}
	//if (!nsp::toolkit::singleton<agv_single>::instance()->build_agvinfo(vecagvinfo, [&](int index)->std::map<std::string, std::string>{
	//	std::map<std::string, std::string> map_attr;
	//	if (index < vec_attr_.size()){
	//		map_attr = vec_attr_[index];
	//	}
	//	return map_attr;
	//}, &agvinfo)){
	//	if (cover_agvinfo(agvinfo, LAM_Server)){
	//		QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("failed to update data."), QMessageBox::Ok);
	//	}
	//}
	std::vector<inner_agvinfo> vec_agvinfo;
	update_agvinfo(vec_agvinfo);
}
//合并agv项
void agvinfo_config::on_combPushButton_clicked()
{
	merge();
	update_table_view();
}

void agvinfo_config::on_merge_clicked()
{
	if (merge(true)){
		update_table_view();
		//update_agvinfo(vec_agvinfo_);
		on_updatePushButton_clicked();
	}
}

bool agvinfo_config::merge( bool isAll )
{
	bool bmerge = false;
	if (isAll == false){
		int combine_row = 0;
		int combined_row = 0;
		int offline_count = 0;
		int unknow_count = 0;
		for (int row = 0; row < ui.tableWidget->rowCount(); row++){
			QCheckBox *checkBox = (QCheckBox*)ui.tableWidget->cellWidget(row, 0);
			QTableWidgetItem*veid = ui.tableWidget->item(row, 1);
			agv_UserData userdata = veid->data(Qt::UserRole).value<agv_UserData>();
			if (checkBox->checkState() == Qt::Checked){
				if (ui.tableWidget->item(row, 4)->text().toLocal8Bit() == "OFFLINE"){
					combine_row = userdata.index_;
					offline_count++;
				}
				else if (ui.tableWidget->item(row, 4)->text().toLocal8Bit() == "UNKNOWN"){
					combined_row = userdata.index_;
					unknow_count++;
				}
			}
		}
		if (offline_count != 1 || unknow_count != 1){
			QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("Check Error..."), QMessageBox::Ok);
			return false;
		}
		if (combined_row < vec_agvinfo_.size()&& combine_row < vec_agvinfo_.size()){
			vec_agvinfo_[combine_row].strip = vec_agvinfo_[combined_row].strip;
			vec_agvinfo_[combine_row].strmac = vec_agvinfo_[combined_row].strmac;
			vec_agvinfo_.erase(vec_agvinfo_.begin() + combined_row);
			bmerge = true;
		}
	}
	else{
		int firstrow = find_first_unknow_row(0);
		while (firstrow != -1){
			int combinerow = find_first_offline_row(vec_agvinfo_[firstrow].strip);
			if (combinerow != -1){
				if (firstrow < vec_agvinfo_.size() && combinerow < vec_agvinfo_.size()){
					vec_agvinfo_[combinerow].strip = vec_agvinfo_[firstrow].strip;
					vec_agvinfo_[combinerow].strmac = vec_agvinfo_[firstrow].strmac;
					vec_agvinfo_.erase(vec_agvinfo_.begin() + firstrow);
					bmerge = true;
				}
			}
			if (combinerow == -1){
				++firstrow;
			}

			firstrow = find_first_unknow_row(firstrow);
		}
	}
	return bmerge;
}

int agvinfo_config::find_first_unknow_row(int nBegin)
{
	for (int index = nBegin; index < vec_agvinfo_.size(); index++){
		if (vec_agvinfo_[index].status == 3){
			return index;
		}
	}
	return -1;
}

int agvinfo_config::find_first_offline_row(std::string ipstr){
	for (int index = 0; index < vec_agvinfo_.size(); index++){
		if (vec_agvinfo_[index].status == 2 &&
			!ipstr.compare(vec_agvinfo_[index].strip.c_str())){
			return index;
		}
	}
	return -1;
}

void agvinfo_config::refresh_view_page(){
	//ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(vec_agvinfo_.size());
	int row = 0;
	bool bSuccess = true;
	for (auto iter : vec_agvinfo_){
		//ui.tableWidget->insertRow(row);
		//添check按钮
		if (!insert_row(row, iter)){
			break;
		}
		row++;
	}

	if (soft_index_ != 0){
		bool soft = map_soft_[soft_index_];
		if (!soft){
			ui.tableWidget->sortItems(soft_index_, Qt::AscendingOrder);
		}
		else{
			ui.tableWidget->sortItems(soft_index_, Qt::DescendingOrder);
		}
	}
}

bool agvinfo_config::insert_row(int row, const inner_agvinfo & agvinfo ){
	QCheckBox * check_box = nullptr;
	check_box = (QCheckBox *)ui.tableWidget->cellWidget(row, 0);
	if (!check_box){
		check_box = new QCheckBox;
		ui.tableWidget->setCellWidget(row, 0, check_box);
	}
	check_box->setChecked(false);

	QTableWidgetItem* veid = setitem(ui.tableWidget, row, 1, agvinfo.vhid);
	if (!veid){
		return false;
	}
	agv_UserData UserData;
	UserData.index_ = row;
	veid->setData(Qt::UserRole, QVariant::fromValue(UserData));

	QTableWidgetItem* vemac = setitem(ui.tableWidget, row, 2, agvinfo.strmac.c_str());
	if (!vemac){
		return false;
	}
	vemac->setFlags(vemac->flags() & (~Qt::ItemIsEditable));//空格不可编辑

	QTableWidgetItem* veip = setitem(ui.tableWidget, row, 3, agvinfo.strip.c_str());
	if (!veip){
		return false;
	}
	//veip->setFlags(veip->flags() & (~Qt::ItemIsEditable));//空格不可编辑

	std::string strtext = "";
	switch (agvinfo.status){
	case 1:
		strtext = "ONLINE";
		break;
	case 2:
		strtext = "OFFLINE";
		break;
	case 3:
		strtext = "UNKNOWN";
		break;
	default:
		break;
	}
	QTableWidgetItem* vestate = setitem(ui.tableWidget, row, 4, strtext.c_str());
	if (!vestate){
		return false;
	}
	vestate->setFlags(vestate->flags() & (~Qt::ItemIsEditable));//空格不可编辑

	return true;
}

void agvinfo_config::refresh_attr_page(){
	//ui.attributeWidget->clearContents();
	ui.attributeWidget->setRowCount(vec_agvinfo_.size());
	std::lock_guard<decltype(agv_lock_)>lock(agv_lock_);
	int row = 0;
	bool bSuccess = true;
	for (auto iter : vec_agvinfo_){
		QTableWidgetItem * veid = setitem(ui.attributeWidget, row, 0, iter.vhid);
		if (!veid){
			bSuccess = false;
			break;
		}
		agv_UserData UserData;
		UserData.map_attr_ = iter.map_attr;
		UserData.map_attr_.emplace("mtport", nsp::toolkit::to_string<char>(iter.mtport));
		UserData.map_attr_.emplace("shport", nsp::toolkit::to_string<char>(iter.shport));
		UserData.map_attr_.emplace("ftsport", nsp::toolkit::to_string<char>(iter.ftsport));
		UserData.index_ = iter.vhid;
		veid->setData(Qt::UserRole, QVariant::fromValue(UserData));

		QTableWidgetItem * vetype = setitem(ui.attributeWidget, row, 1,iter.vhtype);
		if (!veid){
			bSuccess = false;
			break;
		}

		QTableWidgetItem* vemac = setitem(ui.attributeWidget, row, 2, iter.strmac.c_str());
		if (!vemac){
			bSuccess = false;
			break;
		}

		QTableWidgetItem* veip = setitem(ui.attributeWidget, row, 3, iter.strip.c_str());
		if (!veip){
			bSuccess = false;
			break;
		}

		QTableWidgetItem* veport = setitem(ui.attributeWidget, row, 4, iter.mtport);
		if (!veport){
			bSuccess = false;
			break;
		}

		QTableWidgetItem* shellport = setitem(ui.attributeWidget, row, 5, iter.shport);
		if (!shellport){
			bSuccess = false;
			break;
		}

		QTableWidgetItem* fts_port = setitem(ui.attributeWidget, row, 6, iter.ftsport);
		if (!fts_port){
			bSuccess = false;
			break;
		}

		QPushButton * edit_btn = nullptr;
		edit_btn = (QPushButton*)ui.attributeWidget->cellWidget(row, 7);
		if (!edit_btn){
			QPushButton*edit_btn = new QPushButton;
			ui.attributeWidget->setCellWidget(row, 7, edit_btn);
			QIcon icon;
			icon.addFile(QStringLiteral(":/agvinfo_config/image/edit.png"), QSize(), QIcon::Normal, QIcon::Off);
			edit_btn->setIcon(icon);
			edit_btn->setIconSize(QSize(26, 26));
			edit_btn->setFlat(true);
			connect(edit_btn, SIGNAL(clicked()), pMapper_, SLOT(map()));//删除
			pMapper_->setMapping(edit_btn, QString::number(row, 10));
		}
		row++;
	}
	if (!bSuccess){
		ui.attributeWidget->clearContents();
	}
}

QTableWidgetItem * agvinfo_config::setitem(QTableWidget * table,int index, int subindex, QString strtext){
	if (index > table->rowCount()){
		return nullptr;
	}

	QTableWidgetItem * item = nullptr;
	item = table->item(index, subindex);
	if (!item){
		item = new QTableWidgetItem;
		table->setItem(index, subindex, item);
		item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//居中显示
	}
	item->setData(Qt::DisplayRole, strtext);
	return item;
}

QTableWidgetItem * agvinfo_config::setitem(QTableWidget * table, int index, int subindex, int strtext){
	if (index > table->rowCount()){
		return nullptr;
	}

	QTableWidgetItem * item = nullptr;
	item = table->item(index, subindex);
	if (!item){
		item = new QTableWidgetItem;
		table->setItem(index, subindex, item);
		item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//居中显示
	}
	item->setData(Qt::DisplayRole, strtext);
	return item;
}

void agvinfo_config::on_morePushButton_clicked()
{
	refresh_attr_page();
	ui.stackedWidget->setCurrentIndex(1);
}

void agvinfo_config::on_backButton_clicked()
{
	refresh_view_page();
	ui.stackedWidget->setCurrentIndex(0);
}
//修改属性值
void agvinfo_config::btn_edit_slot(QString index)
{
	int row = index.toInt();
	QTableWidgetItem *vhiditem = ui.attributeWidget->item(row, 0);
	if (vhiditem){
		agv_UserData userdata = vhiditem->data(Qt::UserRole).value<agv_UserData>();
		edit_attribute_value edit_value(userdata.index_, userdata.map_attr_, this);
		edit_value.init_agvinfo(vec_agvinfo_, row);
		edit_value.exec();
	}
}
//增删改所有的agv属性
void agvinfo_config::on_attributeButton_clicked()
{
	edit_attribute edit_page(this);
	QTableWidgetItem *vhiditem = ui.attributeWidget->item(0, 0);
	if (vhiditem){
		agv_UserData userdata = vhiditem->data(Qt::UserRole).value<agv_UserData>();
		std::map<std::string, std::string> map_attr;
		for (auto iter : userdata.map_attr_){
			if (iter.first.compare("mtport") != 0 &&
				iter.first.compare("shport") != 0 &&
				iter.first.compare("ftsport") != 0){
				map_attr.emplace(iter.first, iter.second);
			}
		}
		edit_page.init_agvinfo(vec_agvinfo_);
		edit_page.init_attr(map_attr);
		edit_page.init_tablewidget();
	}	
	if (edit_page.exec()){
	}
}

void agvinfo_config::on_pushButton_add_clicked()
{
	int row_count = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(row_count);
	inner_agvinfo agvinfo;
	vec_agvinfo_.push_back(agvinfo);
	insert_row(row_count, agvinfo);
}

void agvinfo_config::on_pushButton_delete_clicked()
{
	std::set<int> set_vhid;
	for (int row = 0; row < ui.tableWidget->rowCount(); row++){
		QCheckBox *checkBox = (QCheckBox*)ui.tableWidget->cellWidget(row, 0);
		QTableWidgetItem*veid = ui.tableWidget->item(row, 1);
		agv_UserData userdata = veid->data(Qt::UserRole).value<agv_UserData>();
		if (checkBox->checkState() == Qt::Checked){
			vec_agvinfo_[userdata.index_].delete_type = true;
			continue;
		}
		int vhid = veid->text().toInt();
		if (set_vhid.find(vhid) != set_vhid.end() && vhid != -1){
			QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("There is a repeat ID"), QMessageBox::Ok);
			ui.tableWidget->editItem(veid);
			return;
		}
		set_vhid.emplace(vhid);
	}

	update_agvinfo(vec_agvinfo_);
}
