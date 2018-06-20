#include "generateemapdialog.h"

GenerateEmapDialog::GenerateEmapDialog(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent, f),
d_ptr{ new GenerateEmapDialogPrivate(this) }
{
	ui.setupUi(this);
	initUi();
	initConnect();
}

GenerateEmapDialog::~GenerateEmapDialog()
{

}

void GenerateEmapDialog::initUi()
{
	Q_D(GenerateEmapDialog);
	d->init();
	ui.rowCountSpinBox->setRange(d->minSpinBoxValue, d->maxSpinBoxValue);
	ui.columnCountSpinBox->setRange(d->minSpinBoxValue, d->maxSpinBoxValue);
	layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void GenerateEmapDialog::initConnect()
{
	connect(ui.okBtn, &QPushButton::clicked, this, &GenerateEmapDialog::on_okBtn_clicked);
}

int GenerateEmapDialog::getRowCount() const
{
	return ui.rowCountSpinBox->value();
}

int GenerateEmapDialog::getColumnCount() const
{
	return ui.columnCountSpinBox->value();
}

void GenerateEmapDialog::on_okBtn_clicked()
{
	accept();
}

GenerateEmapDialogPrivate::GenerateEmapDialogPrivate(GenerateEmapDialog *q_ptr_)
:q_ptr{ q_ptr_ }
{
}

GenerateEmapDialogPrivate::~GenerateEmapDialogPrivate()
{
}

void GenerateEmapDialogPrivate::init()
{
	minSpinBoxValue = 0;
	maxSpinBoxValue = std::numeric_limits<int>::max();
}