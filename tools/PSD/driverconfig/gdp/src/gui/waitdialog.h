#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include "gdp_global.h"
#include "ui_waitdialog.h"
#include <QDialog>
#include <QSharedPointer>

namespace gdp
{
	namespace gui
	{
		class WaitDialogPrivate;

		class GDP_EXPORT WaitDialog : public QDialog
		{
			Q_OBJECT
			Q_DECLARE_PRIVATE(WaitDialog);
			Q_DISABLE_COPY(WaitDialog);
		public:
			WaitDialog(QWidget *parent = 0);

			void startMovie();
		private:
			Ui::WaitDialog ui;
			QSharedPointer<WaitDialogPrivate> d_ptr;
		};
	}
}

#endif // WAITDIALOG_H
