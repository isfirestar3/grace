#include "WopAttrWnd.h"
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <math.h>
#include <QtWidgets/qapplication.h>

WopAttrWnd::WopAttrWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


WopAttrWnd::~WopAttrWnd()
{
}

void WopAttrWnd::InitWnd()
{
	resize(450, 250);
	setWindowTitle(en ? "WopAttr" : QStringLiteral("WOP属性"));
}

void WopAttrWnd::InitForm()
{
	QGridLayout* layout_value = new QGridLayout;
	if (layout_value)
	{
		for (int i = 0; i < 8; ++i){
			QString val_text = QString("val") + QString::number(i);
			QLabel* label_value = new QLabel(val_text);
			layout_value->addWidget(label_value, i / 4, (i % 4) * 2, 1, 1);
			layout_value->addWidget(m_editValue[i], i / 4, (i % 4) * 2 + 1, 1, 1);
		}
	}
	QGridLayout* layout_bit = new QGridLayout;
	if (layout_bit)
	{
		for (int i = 0; i < 32; ++i){
			layout_bit->addWidget(m_checkbox[i], i / 8, i % 8, 1, 1);
		}
	}
	QHBoxLayout* pButtonLayout = new QHBoxLayout;
	if (pButtonLayout){
		pButtonLayout->addWidget(m_btnConfirm);
		pButtonLayout->addWidget(m_btnCancel);
	}
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	if (pMainLayout)
	{
		pMainLayout->addLayout(layout_value);
		pMainLayout->addLayout(layout_bit);
		pMainLayout->addWidget(m_checkbox_enable);
		pMainLayout->addLayout(pButtonLayout);
	}
	this->setLayout(pMainLayout);
}

void WopAttrWnd::InitCtrl()
{
	for (int i = 0; i < 8; ++i){
		m_editValue[i] = new QLineEdit;
	}
	for (int i = 0; i < 32; ++i){
		QString number = QString::number(i);
		m_checkbox[i] = new QCheckBox(number);
	}
	m_btnConfirm = new QPushButton(en ? "Confirm" : QStringLiteral("确定"));
	m_btnCancel = new QPushButton(en ? "Cancel" : QStringLiteral("取消"));
	m_checkbox_enable = new QCheckBox(en ? "Enable" : QStringLiteral("是否启用"));
}

void WopAttrWnd::InitSlot()
{
	connect(m_btnConfirm, &QPushButton::clicked, this, &WopAttrWnd::SlotBtnConfirmClicked);
	connect(m_btnCancel, &QPushButton::clicked, this, &WopAttrWnd::SlotBtnCancelClicked);
}

void WopAttrWnd::SetWopAttr(const WOPATTR& wop_attr)
{
	m_wop_attr = wop_attr;
	int wop_num = static_cast<int>(wop_attr.values.size());
	for (int i = 0; i < wop_num - 1; ++i){
		if (m_editValue[i]){
			m_editValue[i]->setText(QString::number(wop_attr.values[i]));
		}
	}
	if (wop_num == 9){
		unsigned int value8 = wop_attr.values[8];
		for (int i = 0; i < 32; ++i){
			if (m_checkbox[i]){

				if (((value8 >> (31 - i)) & 1) == 1){
					m_checkbox[i]->setChecked(true);
				}
				/*if (static_cast<int>(value8 / pow(2, 31 - i)) == 1){
					m_checkbox[i]->setChecked(true);
				}
				value8 = value8 % static_cast<int>(pow(2, 31 - i));*/
			}
		}
	}
	if (m_wop_attr.enable == 1 && m_checkbox_enable){
		m_checkbox_enable->setChecked(true);
	}
}

void WopAttrWnd::SlotBtnConfirmClicked()
{
	if (m_wop_attr.values.size() < 9){
		return;
	}
	for (int i = 0; i < 8; ++i){
		if (m_editValue[i]){
			int value = m_editValue[i]->text().toInt();
			m_wop_attr.values[i] = value;
		}
	}
	int value = 0;
	for (int i = 0; i < 32; ++i){
		if (m_checkbox[i] && m_checkbox[i]->isChecked()){
			value += pow(2, 31 - i);
		}
	}
	m_wop_attr.values[8] = value;
	if (m_checkbox_enable){
		if (m_checkbox_enable->isChecked()){
			m_wop_attr.enable = 1;
		}else{
			m_wop_attr.enable = 0;
		}
	}
	accept();
}

void WopAttrWnd::SlotBtnCancelClicked()
{
	reject();
}
