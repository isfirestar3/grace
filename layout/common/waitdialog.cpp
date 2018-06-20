#include "waitdialog.h"

namespace shr
{
	namespace gui
	{
		WaitDialog::WaitDialog(QWidget *parent)
			: QDialog(parent, Qt::FramelessWindowHint),
			m_movie{ QSharedPointer<QMovie>::create(":/images/wait.gif") }
		{
			ui.setupUi(this);
			InitUi();
			StartMovie();
		}

		void WaitDialog::InitUi()
		{
			setAttribute(Qt::WA_TranslucentBackground);
		}

		void WaitDialog::StartMovie()
		{
			if (m_movie.isNull())
			{
				return;
			}

			ui.label->setMovie(m_movie.data());
			m_movie->start();
		}
	}
}
