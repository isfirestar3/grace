#include "waitdialog_p.h"

namespace gdp
{
	namespace gui
	{
		WaitDialogPrivate::WaitDialogPrivate(WaitDialog *pWaitDialog)
			:q_ptr{ pWaitDialog }, 
			m_movie{ QSharedPointer<QMovie>::create(":/image/wait.gif") }
		{

		}

		WaitDialogPrivate::~WaitDialogPrivate()
		{

		}

		void WaitDialogPrivate::init()
		{
			Q_Q(WaitDialog);

			if (nullptr == q)
			{
				return;
			}

			q->setAttribute(Qt::WA_TranslucentBackground);
			q->setWindowFlags(q->windowFlags() | Qt::FramelessWindowHint);
		}
	}
}
