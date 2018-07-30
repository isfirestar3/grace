#include "waitdialog.h"
#include "waitdialog_p.h"

namespace gdp
{
	namespace gui
	{
		WaitDialog::WaitDialog(QWidget *parent)
			: QDialog(parent, Qt::FramelessWindowHint),
			d_ptr{ QSharedPointer<WaitDialogPrivate>::create(this) }
		{
			ui.setupUi(this);
			Q_D(WaitDialog);

			if (nullptr == d)
			{
				return;
			}

			d->init();
			startMovie();
		}

		void WaitDialog::startMovie()
		{
			Q_D(WaitDialog);

			if (nullptr == d)
			{
				return;
			}


			if (d->m_movie.isNull())
			{
				return;
			}

			ui.label->setMovie(d->m_movie.data());
			d->m_movie->start();
		}
	}
}
