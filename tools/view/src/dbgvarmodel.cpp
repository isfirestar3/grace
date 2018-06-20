#include "dbgvarmodel.h"
#include "dbg_vars_impls.h"
#include "singleton.hpp"
#include <QtCore/QSet>

DbgVarModel::DbgVarModel(QObject *parent)
: StandardItemModel(parent)
{

}

DbgVarModel::~DbgVarModel()
{

}

void DbgVarModel::InitObjects()
{
	int id;
	uint32_t type;
	QString typeStr;
	QMap<QString, mn::common_title_item> propertiesMap;
	QString text;
	QStandardItem *pItem = nullptr;
	QStandardItem *pChildItem = nullptr;

	for (auto item : m_items)
	{
		id = item.id_;
		type = item.type_;
		typeStr = nsp::toolkit::singleton<dbg_vars_impls>::instance()->var_type_to_string(static_cast<var__types>(type));
		propertiesMap = nsp::toolkit::singleton<dbg_vars_impls>::instance()->properties_from_type(id, static_cast<var__types>(type));

		if (propertiesMap.keys().isEmpty())
		{
			continue;
		}

		text = (QString::number(id) + OBJECT_SPLITER + typeStr);
		pItem = new(std::nothrow) QStandardItem(text);

		if (nullptr == pItem)
		{
			return;
		}

		pItem->setData(id, DBM_ID_ROLE);
		pItem->setData(type, DBM_TYPE_ROLE);
		pItem->setCheckable(true);
		appendRow(pItem);
		m_objectMap.insert(id, pItem);
		QMapIterator<QString, mn::common_title_item> mapItor(propertiesMap);

		while (mapItor.hasNext())
		{
			mapItor.next();
			pChildItem = new(std::nothrow) QStandardItem(mapItor.key());

			if (nullptr == pChildItem)
			{
				return;
			}

			pChildItem->setData(id, DBM_ID_ROLE);
			pChildItem->setData(type, DBM_TYPE_ROLE);
			pChildItem->setData(mapItor.key(), DBM_PROPERTIES_ROLE);
			pChildItem->setCheckable(true);
			pItem->appendRow(pChildItem);
			m_childMap.insert(id, pChildItem);
		}
	}
}

void DbgVarModel::UpdateAllCheckState()
{
	int id;
	uint32_t type;
	QMap<QString, mn::common_title_item> propertiesMap;
	QStandardItem *pChildItem = nullptr;

	for (auto item : m_items)
	{
		id = item.id_;
		type = item.type_;
		propertiesMap = nsp::toolkit::singleton<dbg_vars_impls>::instance()->properties_from_type(id, static_cast<var__types>(type));
		QMapIterator<QString, mn::common_title_item> mapItor(propertiesMap);

		while (mapItor.hasNext())
		{
			mapItor.next();
			pChildItem = GetChildItem(id, type, mapItor.key());

			if (nullptr == pChildItem)
			{
				return;
			}

			UpdateCheckState(id, type, pChildItem);
		}
	}
}

void DbgVarModel::AdjustSingleChoosedItem(QStandardItem *pItem)
{
	if (nullptr == pItem)
	{
		return;
	}

	bool ok = false;
	int id = pItem->data(DBM_ID_ROLE).toInt(&ok);

	if (!ok)
	{
		return;
	}

	uint32_t type = pItem->data(DBM_TYPE_ROLE).toUInt(&ok);

	if (!ok)
	{
		return;
	}

	QPair<int, uint32_t> pair(id, type);
	QStandardItem *pChildItem = nullptr;
	QString properties;

	for (int i = 0; i < pItem->rowCount(); i++)
	{
		pChildItem = pItem->child(i);

		if (Qt::Checked == pChildItem->checkState())
		{
			properties = pChildItem->data(DBM_PROPERTIES_ROLE).toString();
			m_choosedMap.insert(pair, properties);
		}
	}
}

void DbgVarModel::AdjustChoosedItems()
{
	qMapFree(m_choosedMap);
	QMapIterator<int, QStandardItem *> mapItor(m_objectMap);

	while (mapItor.hasNext())
	{
		mapItor.next();
		AdjustSingleChoosedItem(mapItor.value());
	}
}

void DbgVarModel::UpdateCheckState(int id, uint32_t type, QStandardItem *pItem)
{
	if (nullptr == pItem)
	{
		return;
	}

	QString text = pItem->text();
	QSet<QPair<int, uint32_t>> checkedSet = m_checkedMap.keys().toSet();
	int _id;
	uint32_t _type;
	QList<QString> checkedList;
	QString properties;

	for (auto &checked : checkedSet)
	{
		_id = checked.first;
		_type = checked.second;

		if (id == _id && type == _type)
		{
			checkedList = m_checkedMap.values(checked);
			properties = pItem->data(DBM_PROPERTIES_ROLE).toString();

			if (checkedList.contains(properties))
			{
				pItem->setCheckState(Qt::Checked);
			}
		}
	}
}

QStandardItem *DbgVarModel::GetChildItem(int id, uint32_t type, const QString &properties)
{
	int _id;
	int _type;
	QString _properties;
	bool ok = false;
	QList<QStandardItem *> itemList = m_childMap.values(id);

	for (auto &pItem : itemList)
	{
		if (nullptr == pItem)
		{
			continue;
		}

		_id = pItem->data(DBM_ID_ROLE).toInt(&ok);

		if (!ok)
		{
			continue;
		}

		_type = pItem->data(DBM_TYPE_ROLE).toInt(&ok);

		if (!ok)
		{
			continue;
		}

		_properties = pItem->data(DBM_PROPERTIES_ROLE).toString();

		if (_id == id && _type == type && _properties == properties)
		{
			return pItem;
		}
	}

	return nullptr;
}
