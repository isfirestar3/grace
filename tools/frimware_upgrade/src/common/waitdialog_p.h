#ifndef WAITDIALOG_P_H
#define WAITDIALOG_P_H
#include "waitdialog.h"
#include "ui_waitdialog.h"
#include <QtGui/QMovie>

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
			Ui::WaitDialog ui;
			WaitDialog *q_ptr{nullptr};
			QSharedPointer<QMovie> m_movie;
		};
	}
}


#endif // WAITDIALOG_P_H
