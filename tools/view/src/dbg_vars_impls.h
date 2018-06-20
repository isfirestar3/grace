#pragma once
#pragma execution_character_set("utf-8")
#include "vartypes.h"
#include "mntypes.h"
#include "singleton.hpp"
#include <QtCore/QObject>
#include <QtCore/QMap>

static const bool IS_EN = false;
static const QString OBJECT_SPLITER(":");
static const int DEFAULT_REFRESH_TIME = 1000;

class dbg_vars_impls
{
	friend nsp::toolkit::singleton<dbg_vars_impls>;
public:
	QString &var_type_to_string(var__types vt);
	QMap<QString, mn::common_title_item> &properties_from_type(int id, var__types vt);
	void data_map_from_properties(var__types type, void *data, QMap<QString, QString> &data_map) const;

	void set_refresh_time(const int &refresh_time)
	{
		m_refreshTime = refresh_time;
	}

	int get_refresh_time() const
	{
		return m_refreshTime;
	}
private:
	dbg_vars_impls(){}
	dbg_vars_impls(const dbg_vars_impls &) = delete;
	dbg_vars_impls &operator=(const dbg_vars_impls &) = delete;

	QString m_vtString;
	QMap<QString, mn::common_title_item> m_propertiesMap;
	std::atomic<int> m_refreshTime{ DEFAULT_REFRESH_TIME };
};