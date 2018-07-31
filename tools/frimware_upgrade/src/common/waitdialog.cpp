#include "waitdialog.h"
#include "waitdialog_p.h"

namespace gdp
{
	namespace gui
	{
		WaitDialog::WaitDialog(QWidget *parent)
			: QDialog(parent),
			d_ptr{ QSharedPointer<WaitDialogPrivate>::create(this) }
		{
			Q_D(WaitDialog);

			if (nullptr == d)
			{
				return;
			}

			d->ui.setupUi(this);
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

			d->ui.label->setMovie(d->m_movie.data());
			d->m_movie->start();
		}
	}
}
