#include "OneKeyAdjustDialog.h"
#include "ui_OneKeyAdjustDialog.h"
#include "DataManage.h"

OneKeyAdjustDialog::OneKeyAdjustDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OneKeyAdjustDialog)
{
    ui->setupUi(this);
	m_en = qApp->property("language").toInt() == 1;

	RetranslateUi();
	InitUiData();
	InitUiControl();
	InitSlots();
}

OneKeyAdjustDialog::~OneKeyAdjustDialog()
{
    delete ui;
}

void OneKeyAdjustDialog::RetranslateUi()
{
	ui->twoWayRadioBtn->setText(m_en ? "Adjust all edge to two way" : QStringLiteral("调整所有边为双向边"));
	ui->oneWayRadioBtn->setText(m_en ? "Adjust all edge to one way" : QStringLiteral("调整所有边为单向边"));
	ui->okBtn->setText(m_en ? "Ok" : QStringLiteral("确定"));
	ui->cancelBtn->setText(m_en ? "Cancel" : QStringLiteral("取消"));
}

void OneKeyAdjustDialog::InitUiData()
{
	std::vector<WOP> vecWopInfo;
	DATAMNG->GetAllWopInfo(vecWopInfo);

	for (auto wopInfo : vecWopInfo)
	{
		ui->wopIdCmb->addItem(QString::number(wopInfo.id), wopInfo.id);
	}
}

void OneKeyAdjustDialog::InitUiControl()
{
	ui->twoWayRadioBtn->setChecked(true);
	SlotTwoWayRadioBtnClicked();
	setWindowTitle(m_en ? "Ajust all edge property" : QStringLiteral("调整所有边属性"));
	layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void OneKeyAdjustDialog::InitSlots()
{
	connect(ui->twoWayRadioBtn, &QRadioButton::clicked, this, &OneKeyAdjustDialog::SlotTwoWayRadioBtnClicked);
	connect(ui->oneWayRadioBtn, &QRadioButton::clicked, this, &OneKeyAdjustDialog::SlotOneWayRadioBtnClicked);
	connect(ui->okBtn, &QPushButton::clicked, this, &OneKeyAdjustDialog::SlotUpdateEdgeProperty);
	connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void OneKeyAdjustDialog::SlotTwoWayRadioBtnClicked()
{
	ui->wopIdCmb->setEnabled(false);
}

void OneKeyAdjustDialog::SlotOneWayRadioBtnClicked()
{
	ui->wopIdCmb->setEnabled(true);
}

void OneKeyAdjustDialog::SlotUpdateEdgeProperty()
{
	if (ui->twoWayRadioBtn->isChecked())
	{
		AdjustAllEdgeToTwo();
	}
	else if (ui->oneWayRadioBtn->isChecked())
	{
		AdjustAllEdgeToOne();
	}

	accept();
}

void OneKeyAdjustDialog::AdjustAllEdgeToTwo()
{
	std::vector<WOP> vecWopInfo;
	DATAMNG->GetAllWopInfo(vecWopInfo);
	std::vector<int> vecWopId;

	for (auto wopInfo : vecWopInfo)
	{
		vecWopId.push_back(wopInfo.id);
	}

	m_vecWopId.swap(vecWopId);
}

void OneKeyAdjustDialog::AdjustAllEdgeToOne()
{
	int wopId = ui->wopIdCmb->itemData(ui->wopIdCmb->currentIndex()).toInt();
	std::vector<int> vecWopId;
	vecWopId.push_back(wopId);
	m_vecWopId.swap(vecWopId);
}
