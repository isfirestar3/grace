#include "applicationcursor.h"
#include <QApplication>

namespace gdp
{
	namespace core
	{
		ApplicationCursor::ApplicationCursor(const QCursor &cursor)
		{
			QApplication::setOverrideCursor(cursor);
		}

		ApplicationCursor::~ApplicationCursor()
		{
			QApplication::restoreOverrideCursor();
		}
	}
}
