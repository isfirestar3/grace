#include "download_btns.h"

Download_btns::Download_btns(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

Download_btns::~Download_btns()
{

}

void Download_btns::on_start_clicked()
{
	emit startClicked(row_);
}

void Download_btns::on_restart_clicked()
{
	emit restartClicked(row_);
}
void Download_btns::on_reset_clicked()
{
	emit resetClicked(row_);
}
