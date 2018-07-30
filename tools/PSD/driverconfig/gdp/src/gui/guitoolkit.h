#ifndef GUITOOLKIT_H
#define GUITOOLKIT_H

#include "gdp_global.h"
#include "singleton.hpp"

class QWidget;

namespace gdp
{
	namespace gui
	{
		class GDP_EXPORT GuiToolkit
		{
			friend class nsp::toolkit::singleton<GuiToolkit>;
		public:
			~GuiToolkit();

			QWidget *getTopWidget();
		private:
			QWidget *getStaysOnTopWidget();
			QWidget *getModalWidget();
			QWidget *getActiveWindow();

			GuiToolkit();
		};
	}
}

#endif // GUITOOLKIT_H
