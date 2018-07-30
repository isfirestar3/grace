#ifndef WAITDIALOG_P_H
#define WAITDIALOG_P_H

#include "waitdialog.h"
#include <QMovie>
#include <QSharedPointer>

namespace gdp
{
	namespace gui
	{
		class WaitDialogPrivate
		{
			Q_DECLARE_PUBLIC(WaitDialog);
		public:
			WaitDialogPrivate(WaitDialog *pWaitDialog);
			~WaitDialogPrivate();

			void init();
		private:
			WaitDialog *q_ptr{nullptr};
			QSharedPointer<QMovie> m_movie;
		};
	}
}


#endif // WAITDIALOG_P_H
