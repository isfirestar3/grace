#ifndef REALTIMEMONITORMODEL_H
#define REALTIMEMONITORMODEL_H

#include "common/standarditemmodel.h"
#include "common_impls.hpp"
#include <cstdint>
#include "view_net.h"

class RealtimeMonitorModel : public StandardItemModel
{
	Q_OBJECT

public:
	enum RmFeild
	{
		RM_NAME,
		RM_DATA
	};

	enum RmDataRole
	{
		RM_ID_ROLE = Qt::UserRole + 1,
		RM_PROPERTIES_ROLE
	};
public:
	RealtimeMonitorModel(QObject *parent = nullptr);
	~RealtimeMonitorModel();

	void UpdateNetStatus(int &netStatus, int &robotId)
	{
		m_netStatus = netStatus;
	}

	const QMultiMap<QPair<int, uint32_t>, QString> &GetChoosedMap() const
	{
		return m_choosedMap;
	}

	void SetChoosedMap(const QMultiMap<QPair<int, uint32_t>, QString> &choosedMap)
	{
		qMultiMapFree(m_choosedMap);
		m_choosedMap = choosedMap;
	}

	void InitModel();
	bool Clear();
	void StartWatch();
	Q_INVOKABLE void Update(int id, var__types type, void *pData);
private:
	int m_netStatus{ mn::kNetworkStatus_Closed };
	QMultiMap<QPair<int, uint32_t>, QString> m_choosedMap;
	QMultiMap<int, QStandardItem *> m_dataMap;
};

#endif // REALTIMEMONITORMODEL_H
