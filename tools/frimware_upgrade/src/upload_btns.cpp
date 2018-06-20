#include "upload_btns.h"

Upload_btns::Upload_btns(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

Upload_btns::~Upload_btns()
{

}
void Upload_btns::on_open_clicked()
{
	emit openClicked(ip_item->row());
}

void Upload_btns::on_start_clicked()
{
	emit startClicked(ip_item->row());
}
void Upload_btns::on_delete__clicked()
{
	emit deleteClicked(ip_item->row());
}
