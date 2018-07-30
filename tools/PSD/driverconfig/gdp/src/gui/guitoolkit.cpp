#include "guitoolkit.h"
#include <QWidget>
#include <QApplication>

namespace gdp
{
	namespace gui
	{
		GuiToolkit::GuiToolkit()
		{

		}

		GuiToolkit::~GuiToolkit()
		{

		}

		QWidget *GuiToolkit::getTopWidget()
		{
			QWidget *pStaysOnTopWidget = getStaysOnTopWidget();

			if (nullptr != pStaysOnTopWidget)
			{
				return pStaysOnTopWidget;
			}

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

		QWidget *GuiToolkit::getStaysOnTopWidget()
		{
			QWidget *pStaysOnTopWidget = nullptr;
			QWidgetList widgetList = QApplication::topLevelWidgets();

			for (auto &pWidget : widgetList)
			{
				if (nullptr == pWidget)
				{
					continue;
				}

				if (pWidget->isVisible() && pWidget->isWindow() && pWidget->windowFlags() & Qt::WindowStaysOnTopHint)
				{
					pStaysOnTopWidget = pWidget;
					break;
				}
			}

			return pStaysOnTopWidget;
		}

		QWidget *GuiToolkit::getModalWidget()
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

		QWidget *GuiToolkit::getActiveWindow()
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
	}
}
