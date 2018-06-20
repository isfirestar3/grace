#include "realtimemonitormodel.h"
#include "angle_encoder.h"
#include "dio.h"
#include "elmo.h"
#include "moons.h"
#include "navigation.h"
#include "operation.h"
#include "optpar.h"
#include "safety.h"
#include "usrdef.h"
#include "vehicle.h"
#include "wheel.h"
#include "dbg_vars_impls.h"
#include "net_task.h"
#include "common/watcherwaiter.hpp"
#include <QtCore/QSet>
#include <QtCore/QMetaObject>

RealtimeMonitorModel::RealtimeMonitorModel(QObject *parent)
	: StandardItemModel(parent)
{
	qRegisterMetaType<var__types>("var__types");
}

RealtimeMonitorModel::~RealtimeMonitorModel()
{

}

void RealtimeMonitorModel::InitModel()
{
	if (!Clear())
	{
		return;
	}

	qMultiMapFree(m_dataMap);
	int id;
	uint32_t type;
	QString typeString;
	QString objectStr;
	QString properties;
	QSet<QPair<int, uint32_t>> objectSet = m_choosedMap.keys().toSet();
	QList<QString> propertiesList;
	QStandardItem *pParentItem = nullptr;
	QStandardItem *pChildItem = nullptr;

	for (auto &object : objectSet)
	{
		id = object.first;
		type = object.second;
		typeString = nsp::toolkit::singleton<dbg_vars_impls>::instance()->var_type_to_string(static_cast<var__types>(type));
		objectStr = QString::number(id) + OBJECT_SPLITER + typeString;
		pParentItem = new(std::nothrow) QStandardItem(objectStr);

		if (nullptr == pParentItem)
		{
			return;
		}

		appendRow(pParentItem);
		propertiesList = m_choosedMap.values(object);

		for (int i = 0; i < propertiesList.size(); ++i)
		{
			properties = propertiesList[i];
			pChildItem = new(std::nothrow) QStandardItem(properties);

			if (nullptr == pChildItem)
			{
				return;
			}

			pParentItem->setChild(i, RM_NAME, pChildItem);
			pChildItem = new(std::nothrow) QStandardItem("?");

			if (nullptr == pChildItem)
			{
				return;
			}

			pChildItem->setData(id, RM_ID_ROLE);
			pChildItem->setData(properties, RM_PROPERTIES_ROLE);
			pParentItem->setChild(i, RM_DATA, pChildItem);
			m_dataMap.insert(id, pChildItem);
		}
	}
}

bool RealtimeMonitorModel::Clear()
{
	int rc = rowCount();

	if (0 == rc)
	{
		return true;
	}

	return removeRows(0, rc);
}

void RealtimeMonitorModel::StartWatch()
{
	if (mn::kNetworkStatus_Connected > m_netStatus)
	{
		return;
	}

	int id;
	uint32_t type;
	var__types var_type;
	QSet<QPair<int, uint32_t>> objectSet = m_choosedMap.keys().toSet();
	QMap<QString, mn::common_title_item> propertiesMap;
	QList<QString> propertiesList;

	for (auto &object : objectSet)
	{
		id = object.first;
		type = object.second;
		var_type = static_cast<var__types>(type);
		propertiesList = m_choosedMap.values(object);
		propertiesMap = nsp::toolkit::singleton<dbg_vars_impls>::instance()->properties_from_type(id, var_type);

		bool(net_task::*func)(uint32_t, const mn::common_title &, var__types, bool, std::function<void(void *)> &) = &net_task::post_common_read_dbg_var;
		net_task nt;
		mn::common_title common_vec;//std::vector<mn::common_title_item> common_vec;
		std::function<void(void *)> data_cb;

		for (int i = 0; i < propertiesList.size(); ++i)
		{
			common_vec.items.push_back(propertiesMap.value(propertiesList[i]));
		}

		switch (var_type)
		{
			case kVarType_MotionTemplateFramwork:
			{

			}
				break;
			case kVarType_Vehicle:
			{
				data_cb = [&](void *pData)->void{
					var__vehicle_t *p_src = (var__vehicle_t *)pData;
					std::shared_ptr<var__vehicle_t> p_dst = std::make_shared<var__vehicle_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id), 
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_Navigation:
			{
				data_cb = [&](void *pData)->void{
					var__navigation_t *p_src = (var__navigation_t *)pData;
					std::shared_ptr<var__navigation_t> p_dst = std::make_shared<var__navigation_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_Operation:
			{
				data_cb = [&](void *pData)->void{
					var__operation_t *p_src = (var__operation_t *)pData;
					std::shared_ptr<var__operation_t> p_dst = std::make_shared<var__operation_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_UserDefined:
			{
				data_cb = [&](void *pData)->void{
					var__usrdef_buffer_t *p_src = (var__usrdef_buffer_t *)pData;
					std::shared_ptr<var__usrdef_buffer_t> p_dst = std::make_shared<var__usrdef_buffer_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_SWheel:
			{
				data_cb = [&](void *pData)->void{
					var__swheel_t *p_src = (var__swheel_t *)pData;
					std::shared_ptr<var__swheel_t> p_dst = std::make_shared<var__swheel_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_DWheel:
			{
				data_cb = [&](void *pData)->void{
					var__dwheel_t *p_src = (var__dwheel_t *)pData;
					std::shared_ptr<var__dwheel_t> p_dst = std::make_shared<var__dwheel_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_SDDExtra:
			{
				data_cb = [&](void *pData)->void{
					var__sdd_extra_t *p_src = (var__sdd_extra_t *)pData;
					std::shared_ptr<var__sdd_extra_t> p_dst = std::make_shared<var__sdd_extra_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_DriveUnit:
			{

			}
				break;
			case kVarType_Map:
			{

			}
				break;
			case kVarType_OperationTarget:
			{
				data_cb = [&](void *pData)->void{
					var__operation_parameter_t *p_src = (var__operation_parameter_t *)pData;
					std::shared_ptr<var__operation_parameter_t> p_dst = std::make_shared<var__operation_parameter_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_CanBus:
			{
				//data_cb = [&](void *pData)->void{
				//	var__canbus_t *p_src = (var__canbus_t *)pData;
				//	std::shared_ptr<var__canbus_t> p_dst = std::make_shared<var__canbus_t>();

				//	if (nullptr == p_dst.get())
				//	{
				//		return;
				//	}

				//	memcpy(p_dst.get(), p_src, sizeof(*p_src));
				//	QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
				//		QGenericReturnArgument(),
				//		Q_ARG(int, id),
				//		Q_ARG(var__types, var_type),
				//		Q_ARG(void *, p_dst.get()));
				//};
			}
				break;
			case kVarType_Copley:
			{

			}
				break;
			case kVarType_Elmo:
			{
				data_cb = [&](void *pData)->void{
					var__elmo_t *p_src = (var__elmo_t *)pData;
					std::shared_ptr<var__elmo_t> p_dst = std::make_shared<var__elmo_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_DIO:
			{
				data_cb = [&](void *pData)->void{
					var__dio_t *p_src = (var__dio_t *)pData;
					std::shared_ptr<var__dio_t> p_dst = std::make_shared<var__dio_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_Moons:
			{
				data_cb = [&](void *pData)->void{
					var__moos_t *p_src = (var__moos_t *)pData;
					std::shared_ptr<var__moos_t> p_dst = std::make_shared<var__moos_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_AngleEncoder:
			{
				data_cb = [&](void *pData)->void{
					var__angle_encoder_t *p_src = (var__angle_encoder_t *)pData;
					std::shared_ptr<var__angle_encoder_t> p_dst = std::make_shared<var__angle_encoder_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_Curtis:
			{

			}
				break;
			case kVarType_VoiceDevice:
			{

			}
				break;
			case kVarType_OmronPLC:
			{

			}
				break;
			case kVarType_ModBus_TCP:
			{

			}
				break;
			case kVarType_ErrorHandler:
			{

			}
				break;
			case kVarType_SafetyProtec:
			{
				data_cb = [&](void *pData)->void{
					var__safety_t *p_src = (var__safety_t *)pData;
					std::shared_ptr<var__safety_t> p_dst = std::make_shared<var__safety_t>();

					if (nullptr == p_dst.get())
					{
						return;
					}

					memcpy(p_dst.get(), p_src, sizeof(*p_src));
					QMetaObject::invokeMethod(this, "Update", Qt::BlockingQueuedConnection,
						QGenericReturnArgument(),
						Q_ARG(int, id),
						Q_ARG(var__types, var_type),
						Q_ARG(void *, p_dst.get()));
				};
			}
				break;
			case kVarType_MaximumFunction:
			{

			}
				break;
			default:
			{
				Q_ASSERT(false);
			}
				break;
		}

		nsp::toolkit::singleton<WatcherWaiter>::instance()->run(&nt, func, 1, std::ref(common_vec), var_type, false, data_cb);
	}
}

Q_INVOKABLE void RealtimeMonitorModel::Update(int id, var__types type, void *pData)
{
	QMap<QString, mn::common_title_item> propertiesMap = nsp::toolkit::singleton<dbg_vars_impls>::instance()->properties_from_type(id, type);
	QMap<QString, QString> dataMap;
	nsp::toolkit::singleton<dbg_vars_impls>::instance()->data_map_from_properties(type, pData, dataMap);
	QList<QStandardItem *> itemList = m_dataMap.values(id);
	QString properties;
	QString text;

	for (auto &pItem : itemList)
	{
		if (nullptr == pItem)
		{
			continue;
		}

		properties = pItem->data(RM_PROPERTIES_ROLE).toString();
		text = dataMap.value(properties);
		pItem->setText(text);
	}
}
