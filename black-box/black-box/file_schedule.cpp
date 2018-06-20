#include "file_schedule.h"

file_schedule::file_schedule(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags = Qt::Dialog;
	//flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::MSWindowsFixedSizeDialogHint;
	setWindowFlags(flags);

}

file_schedule::~file_schedule()
{

}

void file_schedule::closeEvent(QCloseEvent * event)
{
	this->accept();
}
