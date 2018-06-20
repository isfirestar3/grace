#include "warning_page.h"

warning_page::warning_page(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

warning_page::~warning_page()
{

}

void warning_page::on_yesRadioButton_clicked()
{
	state_ = true;
}
void warning_page::on_noRadioButton_clicked()
{
	state_ = false;
}
