#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include "ui_waitdialog.h"
#include <QtCore/QSharedPointer>
#include <QtGui/QMovie>
#include <QtWidgets/QDialog>

namespace shr
{
	namespace gui
	{
		class WaitDialog : public QDialog
		{
		public:
			WaitDialog(QWidget *parent = 0);

			void InitUi();
			void StartMovie();
		private:
			Ui::WaitDialog ui;
			QSharedPointer<QMovie> m_movie;
		};
	}
}

#endif // WAITDIALOG_H
