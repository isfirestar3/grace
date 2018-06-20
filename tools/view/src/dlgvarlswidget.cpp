#include "dlgvarlswidget.h"
#include "common/watcherwaiter.hpp"
#include "net_task.h"
#include "serialize.hpp"
#include "dbg_vars_impls.h"
#include "singleton.hpp"
#include "data_type.h"
#include "common/MessageBox.h"
#include <functional>

DlgVarLsWidget::DlgVarLsWidget(QWidget *parent)
: QWidget(parent),
m_commonAckModel{QSharedPointer<CommonAckModel>::create()}
{
	ui.setupUi(this);
	InitUiData();
	InitUi();
	InitSlots();
}

DlgVarLsWidget::~DlgVarLsWidget()
{

}

void DlgVarLsWidget::InitUiData()
{
	m_commonAckModel->SetHorizontalHeader(QStringList() << tr("name") << tr("value"));
}

void DlgVarLsWidget::InitUi()
{
	ui.tableView->setModel(m_commonAckModel.data());
}

void DlgVarLsWidget::InitSlots()
{
	connect(ui.typeCmb, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DlgVarLsWidget::TypeCmbIndexChanged);
	connect(ui.idCmb, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DlgVarLsWidget::IdCmbIndexChanged);
}

void DlgVarLsWidget::PostDdgVarLsRequest()
{
	int net_status;
	//agv_network_api_.get_net_status(net_status); //nsp::toolkit::singleton<viewer_net>::instance()->get_net_status(status);
	if (mn::kNetworkStatus_Connected > net_status)
	{
		CMessageBox::Critical(this, tr("Critical!"), tr("The network is not connect!"), CMessageBox::Ok);

		return;
	}

	bool(net_task::*func)(uint32_t, std::vector<mn::var_item> &) = &net_task::post_dbg_varls_request_;
	std::function<void(bool)> cb = std::bind(&DlgVarLsWidget::PostDdgVarLsRequestFinished, this, std::placeholders::_1);
	net_task nt;
	nsp::toolkit::singleton<WatcherDialog>::instance()->run(&nt, func, m_robotId, std::ref(m_items), cb, getTopWidget());
}

void DlgVarLsWidget::PostDdgVarLsRequestFinished(bool successed)
{
	if (successed)
	{
		qMultiMapFree<int, uint32_t>(m_typeIdMap);

		for (auto &item : m_items)
		{
			m_typeIdMap.insert(item.type_, item.id_);
		}

		InitTypeCmb(m_typeIdMap);
	}
	else
	{
		CMessageBox::Critical(this, tr("Critical!"), tr("Post dbg var list request failed!"), CMessageBox::Ok);
	}
}

void DlgVarLsWidget::PostCommonReadVehicleFinished(bool successed)
{
	if (successed)
	{
		QList<QStringList> commonAckList;
		QStringList strList;
		strList << QString("vehicle_id_") << QString::number(m_vehicle.vehicle_id_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("vehicle_name_") << QString(m_vehicle.vehicle_name_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("vehicle_type_") << QString(m_vehicle.vehicle_type_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("chassis_type_") << QString(m_vehicle.chassis_type_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("max_speed_") << QString::number(m_vehicle.max_speed_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("creep_speed_") << QString::number(m_vehicle.creep_speed_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("max_acc_") << QString::number(m_vehicle.max_acc_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("max_dec_") << QString::number(m_vehicle.max_dec_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("max_w_") << QString::number(m_vehicle.max_w_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("creep_w_") << QString::number(m_vehicle.creep_w_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("max_acc_w_") << QString::number(m_vehicle.max_acc_w_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("max_dec_w_") << QString::number(m_vehicle.max_dec_w_);
		commonAckList.append(strList);
		strList.clear();
		strList << QString("steer_angle_error_tolerance_") << QString::number(m_vehicle.steer_angle_error_tolerance_);
		commonAckList.append(strList);
		m_commonAckModel->SetCommonAckList(commonAckList);
		ui.tableView->resizeColumnsToContents();
	}
	else
	{
		CMessageBox::Critical(this, tr("Critical!"), tr("Post common read vehicle failed!"), CMessageBox::Ok);
	}
}

void DlgVarLsWidget::PostCommonReadNavigationFinished(bool successed)
{
	if (successed)
	{
		double acc = m_navigation.acc_;
	}
}

void DlgVarLsWidget::InitTypeCmb(const QMultiMap<int, uint32_t> &map)
{
	ui.typeCmb->clear();
	QSet<int> keys = map.keys().toSet();
	QString type;

	for (auto &key : keys)
	{
		type = nsp::toolkit::singleton<dbg_vars_impls>::instance()->var_type_to_string(static_cast<var__types>(key));
		ui.typeCmb->addItem(type, key);
	}
}

void DlgVarLsWidget::InitIdCmb(const QList<uint32_t> &values)
{
	ui.idCmb->clear();

	for (auto &value : values)
	{
		ui.idCmb->addItem(QString::number(value), value);
	}
}

void DlgVarLsWidget::TypeCmbIndexChanged(int index)
{
	if (INVALID_INDEX == index)
	{
		return;
	}

	bool ok = false;
	int type = ui.typeCmb->itemData(index).toInt(&ok);

	if (!ok)
	{
		return;
	}

	QList<uint32_t> values = m_typeIdMap.values(type);
	InitIdCmb(values);
}

void DlgVarLsWidget::IdCmbIndexChanged(int index)
{
	m_commonAckModel->Clear();

	if (INVALID_INDEX == index)
	{
		return;
	}

	bool ok = false;
	var__types type = static_cast<var__types>(ui.typeCmb->itemData(ui.typeCmb->currentIndex()).toInt(&ok));

	if (!ok)
	{
		return;
	}

	int id = ui.idCmb->itemData(index).toInt(&ok);

	if (!ok)
	{
		return;
	}

	switch (type)
	{
		case kVarType_MotionTemplateFramwork:
			break;
		case kVarType_Vehicle:
		{
			bool(net_task::*func)(uint32_t, const mn::common_title &, var__types, bool, std::function<void(void *)> &) = &net_task::post_common_read_dbg_var;
			std::function<void(bool)> cb = std::bind(&DlgVarLsWidget::PostCommonReadVehicleFinished, this, std::placeholders::_1);
			net_task nt;

			mn::common_title common_vec;
			/*mn::common_data_item vehicle_id{ id, offsetof_data(var__vehicle_t, vehicle_id_), msizeof(var__vehicle_t, vehicle_id_) };
			common_vec.items.push_back(vehicle_id);
			mn::common_data_item vehicle_name_{ id, offsetof_data(var__vehicle_t, vehicle_name_), msizeof(var__vehicle_t, vehicle_name_) };
			common_vec.items.push_back(vehicle_name_);
			mn::common_data_item chassis_type_{ id, offsetof_data(var__vehicle_t, chassis_type_), msizeof(var__vehicle_t, chassis_type_) };
			common_vec.items.push_back(chassis_type_);
			mn::common_data_item max_speed_{ id, offsetof_data(var__vehicle_t, max_speed_), msizeof(var__vehicle_t, max_speed_) };
			common_vec.items.push_back(max_speed_);*/
			mn::common_title_item vehicle_t{ id, type, 0, sizeof(var__vehicle_t) };
			common_vec.items.push_back(vehicle_t);

			std::function<void(void *)> data_cb = [&](void *pData)->void{
				var__vehicle_t vehicle_t = *(var__vehicle_t *)pData;
			};

			nsp::toolkit::singleton<WatcherWaiter>::instance()->run(&nt, func, m_robotId, std::ref(common_vec), kVarType_Vehicle, false, data_cb, cb);
		}
			break;
		case kVarType_Navigation:
		{
			bool(net_task::*func)(uint32_t, const mn::common_title &, var__types, bool, std::function<void(void *)> &) = &net_task::post_common_read_dbg_var;
			std::function<void(bool)> cb = std::bind(&DlgVarLsWidget::PostCommonReadNavigationFinished, this, std::placeholders::_1);
			net_task nt;
			mn::common_title common_vec;
			mn::common_title_item navigation_t{ id, type, sizeof(var__navigation_t) };
			common_vec.items.push_back(navigation_t);

			std::function<void(void *)> data_cb = [&](void *pData)->void{
				var__navigation_t navigation_t = *(var__navigation_t *)pData;
			};

			nsp::toolkit::singleton<WatcherWaiter>::instance()->run(&nt, func, m_robotId, std::ref(common_vec), kVarType_Navigation, true, data_cb, cb);
		}
			break;
		case kVarType_Operation:
			break;
		case kVarType_UserDefined:
			break;
		case kVarType_SWheel:
			break;
		case kVarType_DWheel:
			break;
		case kVarType_SDDExtra:
			break;
		case kVarType_DriveUnit:
			break;
		case kVarType_Map:
			break;
		case kVarType_OperationTarget:
			break;
		case kVarType_CanBus:
			break;
		case kVarType_Copley:
			break;
		case kVarType_Elmo:
			break;
		case kVarType_DIO:
			break;
		case kVarType_Moons:
			break;
		case kVarType_AngleEncoder:
			break;
		case kVarType_Curtis:
			break;
		case kVarType_VoiceDevice:
			break;
		case kVarType_OmronPLC:
			break;
		case kVarType_ModBus_TCP:
			break;
		case kVarType_ErrorHandler:
			break;
		case kVarType_SafetyProtec:
			break;
		case kVarType_MaximumFunction:
			break;
		default:
		{
			Q_ASSERT(false);
		}
			break;
	}
}