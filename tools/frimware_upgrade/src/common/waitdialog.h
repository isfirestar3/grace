#ifndef WAITDIALOG_H
#define WAITDIALOG_H
#include <QtWidgets/QDialog>

namespace gdp
{
	namespace gui
	{
		class WaitDialogPrivate;

		class WaitDialog : public QDialog
		{
			Q_OBJECT
			Q_DECLARE_PRIVATE(WaitDialog);
			Q_DISABLE_COPY(WaitDialog);
		public:
			WaitDialog(QWidget *parent = nullptr);

			void startMovie();
		private:
			QSharedPointer<WaitDialogPrivate> d_ptr;
		};
	}
}

#endif // WAITDIALOG_H
