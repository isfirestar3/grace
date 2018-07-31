#ifndef APPLICATIONCURSOR_H
#define APPLICATIONCURSOR_H
#include "gdp_global.h"

class QCursor;

namespace gdp
{
	namespace core
	{
		class GDP_EXPORT ApplicationCursor
		{
		public:
			ApplicationCursor(const QCursor &cursor);
			~ApplicationCursor();
		};
	}
}

#endif // APPLICATIONCURSOR_H
