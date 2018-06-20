#include "wid_setting.h"

wid_setting::wid_setting(QWidget* parent)
:m_otherSetWidget{QSharedPointer<OtherSettingWidget>::create()}
{
	init_form();
	init_slot();
}

wid_setting::~wid_setting(){

}

void wid_setting::AfterShow()
{
	/*if (nullptr != net_setting_)
	{
		net_setting_->GetNetworkSettingWidget()->SetApplicationNotify();
	}*/
}

void wid_setting::init_form(){
	/*net_setting_ = new wid_network_setting();
	net_setting_->set_setting_callbck(std::bind(&wid_setting::update_netstatus, this, std::placeholders::_1,std::placeholders::_2));*/
	net_setting_ = new NetworkSettingWidget();
	net_setting_->set_setting_callbck(std::bind(&wid_setting::update_netstatus, this, std::placeholders::_1, std::placeholders::_2));

	other = new QWidget;

	QStringListModel*slm = new QStringListModel(this);
	QStringList ls;
	ls << QStringLiteral("网络设置") << QStringLiteral("其他设置");
	slm->setStringList(ls);
	list_view_ = new QListView;
	list_view_->setModel(slm);
	list_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
	list_view_->setCurrentIndex(slm->index(0, 0));

	update_stackwidget_ = new QStackedWidget;
	update_stackwidget_->addWidget(net_setting_);
	//update_stackwidget_->addWidget(other);
	update_stackwidget_->addWidget(m_otherSetWidget.data());

	splitter_ = new QSplitter(Qt::Horizontal);
	splitter_->addWidget(list_view_);
	splitter_->addWidget(update_stackwidget_);
	splitter_->setStretchFactor(1, 2);
	splitter_->setHandleWidth(1);

	QHBoxLayout * layout_info = new QHBoxLayout;
	layout_info->addWidget(splitter_);
	this->setLayout(layout_info);

	this->setAutoFillBackground(true);

	QPalette palette;
	palette.setColor(QPalette::Background, QColor(221, 225, 230));
	this->setPalette(palette);
}

void wid_setting::init_slot(){
	connect(list_view_, SIGNAL(clicked(const QModelIndex &)), this, SLOT(list_clicked(const QModelIndex&)));
	QTimer::singleShot(0, this, &wid_setting::AfterShow);
}

void wid_setting::list_clicked(const QModelIndex& index){

	switch (index.row())
	{
	case NET_SETTING:
		update_stackwidget_->setCurrentIndex(0);
		break;
	case OTHER_SETTING:
		update_stackwidget_->setCurrentIndex(1);
		break;
	default:
		break;
	}
}

void wid_setting::update_netstatus(int status,int robot_id){
	if (status_function_){
		status_function_(status, robot_id);
	}
}

void wid_setting::set_setiing_callback(const std::function<void(int,int)>& func){
	if (!status_function_){
		status_function_ = func;
	}
}