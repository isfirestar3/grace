#ifndef DBGVARMODEL_H
#define DBGVARMODEL_H

#include "common_impls.hpp"
#include "common/standarditemmodel.h"
#include "mntypes.h"

class DbgVarModel : public StandardItemModel
{
	Q_OBJECT

public:
	enum DataRole
	{
		DBM_ID_ROLE = Qt::UserRole + 1,
		DBM_TYPE_ROLE,
		DBM_PROPERTIES_ROLE
	};
public:
	DbgVarModel(QObject *parent = nullptr);
	~DbgVarModel();

	void SetReportItems(std::vector<mn::var_item> &items)
	{
		std_vector_free<mn::var_item>(m_items);
		m_items.swap(items);
	}

	QMultiMap<QPair<int, uint32_t>, QString> &GetChoosedMap()
	{
		return m_choosedMap;
	}

	void SetCheckedMap(QMultiMap<QPair<int, uint32_t>, QString> checkedMap)
	{
		qMultiMapFree(m_checkedMap);
		m_checkedMap.swap(checkedMap);
	}

	void InitObjects();
	void UpdateAllCheckState();
	void AdjustSingleChoosedItem(QStandardItem *pItem);
	void AdjustChoosedItems();
	void UpdateCheckState(int id, uint32_t type, QStandardItem *pItem);
	QStandardItem *GetChildItem(int id, uint32_t type, const QString &properties);
private:
	std::vector<mn::var_item> m_items;
	QMap<int, QStandardItem *> m_objectMap;
	QMultiMap<int, QStandardItem *> m_childMap;
	QMultiMap<QPair<int, uint32_t>, QString> m_choosedMap;
	QMultiMap<QPair<int, uint32_t>, QString> m_checkedMap;
};

#endif // DBGVARMODEL_H
