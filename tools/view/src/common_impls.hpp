#pragma once
#include "os_util.hpp"
#include "log.h"
#include <QtWidgets/QWidget>
#include <QtCore/QMultiMap>
#include <QtWidgets/QApplication>
#include <vector>

static const int RET_FAILED = -1;
static const int TIMEOUT = 5000;
static const int INVALID_INDEX = -1;

class waitable_wait
{
public:
	waitable_wait(nsp::os::waitable_handle *waitable_handle)
		:_waitable_handle(waitable_handle)
	{
	
	}

	~waitable_wait()
	{
		if (nullptr != _waitable_handle)
		{
			_waitable_handle = nullptr;
		}
	}

	bool wait(uint32_t timeo = TIMEOUT)
	{
		if (nullptr == _waitable_handle)
		{
			return false;
		}

		if (-1 == _waitable_handle->wait(timeo))
		{
			nsperror << __FUNCTION__ << "wait failed.";

			return false;
		}

		return true;
	}
private:
	nsp::os::waitable_handle *_waitable_handle;
};

class waitable_sig
{
public:
	waitable_sig(nsp::os::waitable_handle *waitable_handle)
		:_waitable_handle(waitable_handle)
	{
	
	}

	~waitable_sig()
	{
		if (nullptr == _waitable_handle)
		{
			return;
		}

		_waitable_handle->sig();
		_waitable_handle = nullptr;
	}
private:
	nsp::os::waitable_handle *_waitable_handle;
};

template <typename T>
void qListFree(QList<T> &list)
{
	QList<T> tmpList;
	list.swap(tmpList);
}

template <typename Key, typename Value>
void qMapFree(QMap<Key, Value> &map)
{
	QMap<Key, Value> tmpMap;
	map.swap(tmpMap);
}

template <typename Key, typename Value>
void qMultiMapFree(QMultiMap<Key, Value> &map)
{
	QMultiMap<Key, Value> tmpMap;
	map.swap(tmpMap);
}

template <typename T>
void std_vector_free(std::vector<T> &vec)
{
	std::vector<T> tmpVec;
	tmpVec.swap(vec);
}

static QWidget *getModalWidget()
{
	QWidget *pModalWidget = nullptr;
	QWidgetList widgetList = QApplication::topLevelWidgets();

	for (auto &pWidget : widgetList)
	{
		if (nullptr == pWidget)
		{
			continue;
		}

		if (pWidget->isVisible() && pWidget->isWindow() && ((Qt::WindowModal == pWidget->windowModality()) || (Qt::ApplicationModal == pWidget->windowModality())))
		{
			pModalWidget = pWidget;
			break;
		}
	}

	return pModalWidget;
}

static QWidget *getActiveWindow()
{
	QWidget *pActiveWindow = nullptr;
	QWidgetList widgetList = QApplication::topLevelWidgets();

	for (auto &pWidget : widgetList)
	{
		if (nullptr == pWidget)
		{
			continue;
		}

		if (pWidget->isVisible() && pWidget->isWindow() && pWidget->isActiveWindow())
		{
			pActiveWindow = pWidget;
			break;
		}
	}

	return pActiveWindow;
}

static QWidget *getTopWidget()
{
	QWidget *pModalWidget = getModalWidget();

	if (nullptr != pModalWidget)
	{
		return pModalWidget;
	}

	QWidget *pActiveWindow = getActiveWindow();

	if (nullptr != pActiveWindow)
	{
		return pActiveWindow;
	}

	QWidget *pTopWidget = nullptr;
	QWidgetList widgetList = QApplication::topLevelWidgets();

	for (auto &pWidget : widgetList)
	{
		if (nullptr == pWidget)
		{
			continue;
		}

		if (pWidget->isVisible() && pWidget->isWindow())
		{
			pTopWidget = pWidget;
			break;
		}
	}

	return pTopWidget;
}