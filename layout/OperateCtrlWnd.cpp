#include "OperateCtrlWnd.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qmessagebox.h>
#include "net_interface.h"
#include <QtWidgets/qapplication.h>

OperateCtrlWnd::OperateCtrlWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


OperateCtrlWnd::~OperateCtrlWnd()
{
}

void OperateCtrlWnd::InitWnd()
{
	resize(400, 250);
	setWindowTitle(en ? "OperateCtrl" : QStringLiteral("操作控制"));
}
void OperateCtrlWnd::InitCtrl()
{
	m_pOptCode = new QLineEdit;
	for (int i = 0; i < OPT_CTRL_NUM; ++i)
	{
		QLineEdit* pEditParam = new QLineEdit;
		m_pOptParam.push_back(pEditParam);
	}
	m_pBtnUpdate = new QPushButton(en ? "Update" : QStringLiteral("获取"));
	m_pBtnSend = new QPushButton(en ? "Send" : QStringLiteral("发送"));
}
void OperateCtrlWnd::InitForm()
{
	QGridLayout* pGridLayout = new QGridLayout;
	if (pGridLayout)
	{
		QLabel* pLabelType = new QLabel(QStringLiteral("Code："));
		QLabel* pLabelParams = new QLabel(en ? "Params" : QStringLiteral("参数列表"));
		pGridLayout->addWidget(pLabelType, 0, 0, 1, 1);
		pGridLayout->addWidget(m_pOptCode, 0, 1, 1, 1);
		pGridLayout->addWidget(pLabelParams, 1, 0, 1, 2);

		int index = 0;
		for (int i = 0; i < OPT_CTRL_NUM; ++i)
		{
			index = i / 3;
			QString strName = "p" + QString::number(i);
			QLabel* pLabelParam = new QLabel(strName);
			pGridLayout->addWidget(pLabelParam, index + 2, (i % 3) * 2, 1, 1);
			pGridLayout->addWidget(m_pOptParam[i], index + 2, (i % 3) * 2 + 1, 1, 1);
		}
		pGridLayout->addWidget(m_pBtnUpdate, index + 3, 0, 1, 2);
		pGridLayout->addWidget(m_pBtnSend, index + 3, 4, 1, 2);
	}
	setLayout(pGridLayout);
}
void OperateCtrlWnd::InitSlot()
{
	connect(m_pBtnSend, &QPushButton::clicked, this, &OperateCtrlWnd::SlotBtnClickSend);
}

void OperateCtrlWnd::SlotBtnClickSend()
{
	OPTCTRLINFO optCtrlInfo;
	GetCtrlData(optCtrlInfo);
}

void OperateCtrlWnd::GetCtrlData(OPTCTRLINFO& optCtrlInfo)
{
	if (m_pOptCode)
	{
		optCtrlInfo.code = m_pOptCode->text().toInt();
	}
	for (int i = 0; i < OPT_CTRL_NUM; ++i)
	{
		if (m_pOptParam[i] && !m_pOptParam[i]->text().isEmpty())
		{
			optCtrlInfo.params[i] = m_pOptParam[i]->text().toULongLong();
		}
		optCtrlInfo.mask |= (unsigned int)(1 << i);
	}
}