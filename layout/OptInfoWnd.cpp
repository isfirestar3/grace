#include "OptInfoWnd.h"
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qmessagebox.h>
#include "net_interface.h"
#include <QtWidgets/qapplication.h>

OptInfoWnd::OptInfoWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


OptInfoWnd::~OptInfoWnd()
{
}

void OptInfoWnd::InitWnd()
{
	resize(400, 300);
	setWindowTitle(en ? "OptInfo" : QStringLiteral("操作信息"));
}

void OptInfoWnd::InitCtrl()
{
	m_pOptId = new QLineEdit;
	if (m_pOptId)
	{
		m_pOptId->setEnabled(false);
	}
	m_pOptType = new QLineEdit;
	for (int i = 0; i < OPT_PARAM_NUM; ++i)
	{
		QLineEdit* pEditParam = new QLineEdit;
		m_pOptParam.push_back(pEditParam);
	}
	m_pBtnUpdate = new QPushButton(en ? "Update" : QStringLiteral("获取"));
	m_pBtnSend = new QPushButton(en ? "Send" : QStringLiteral("发送"));
	m_pBtnConfirm = new QPushButton(en ? "Confirm" : QStringLiteral("确定"));
}

void OptInfoWnd::InitForm()
{
	QGridLayout* pGridLayout = new QGridLayout;
	if (pGridLayout)
	{
		QLabel* pLabelId = new QLabel(en ? "Id:" : QStringLiteral("ID："));
		QLabel* pLabelType = new QLabel(en ? "Type:" : QStringLiteral("Type："));
		QLabel* pLabelParams = new QLabel(en ? "Params:" : QStringLiteral("参数列表"));
		pGridLayout->addWidget(pLabelId, 0, 0, 1, 1);
		pGridLayout->addWidget(m_pOptId, 0, 1, 1, 1);
		pGridLayout->addWidget(pLabelType, 0, 2, 1, 1);
		pGridLayout->addWidget(m_pOptType, 0, 3, 1, 1);
		pGridLayout->addWidget(pLabelParams, 1, 0, 1, 1);

		int index = 0;
		for (int i = 0; i < OPT_PARAM_NUM; ++i)
		{
			index = i / 3;
			QString strName = "p" + QString::number(i);
			QLabel* pLabelParam = new QLabel(strName);
			pGridLayout->addWidget(pLabelParam, index + 2, (i % 3)*2, 1, 1);
			pGridLayout->addWidget(m_pOptParam[i], index + 2, (i%3)*2 + 1, 1, 1);
		}
		pGridLayout->addWidget(m_pBtnConfirm, index + 3, 0, 1, 2);
		pGridLayout->addWidget(m_pBtnUpdate, index + 3, 2, 1, 2);
		pGridLayout->addWidget(m_pBtnSend, index + 3, 4, 1, 2);
	}
	setLayout(pGridLayout);
}

void OptInfoWnd::InitSlot()
{
	connect(m_pBtnConfirm, &QPushButton::clicked, this, &OptInfoWnd::SlotBtnClickConfirm);
	connect(m_pBtnUpdate, &QPushButton::clicked, this, &OptInfoWnd::SlotBtnClickGetOptParam);
	connect(m_pBtnSend, &QPushButton::clicked, this, &OptInfoWnd::SlotBtnSendOptParam);
}

void OptInfoWnd::SetOptInfo(const OPTINFO& optInfo)
{
	m_optInfo = optInfo;
	ShowCtrlData();
}

void OptInfoWnd::ShowCtrlData()
{
	if (m_pOptId)
	{
		m_pOptId->setText(QString::number(m_optInfo.id));
	}
	if (m_pOptType)
	{
		m_pOptType->setText(QString::number(m_optInfo.type));
	}
	for (size_t i = 0; i < OPT_PARAM_NUM; ++i)
	{
		if (m_pOptParam[i])
		{
			m_pOptParam[i]->setText(QString::number((double)((int64_t)m_optInfo.params[i]) / 1000));
		}
	}
}

void OptInfoWnd::SlotBtnClickConfirm()
{
	if (m_pOptType)
	{
		m_optInfo.type = m_pOptType->text().toInt();
	}
	for (size_t i = 0; i < OPT_PARAM_NUM; ++i)
	{
		if (m_pOptParam[i] && !m_pOptParam[i]->text().isEmpty())
		{
			double param = m_pOptParam[i]->text().toDouble();
			m_optInfo.params[i] = (int64_t)(param * 1000);
		}
	}
	accept();
}

void OptInfoWnd::SlotBtnClickGetOptParam()
{
	int ret = nsp::toolkit::singleton<net_interface>::instance()->get_opt_param(m_optInfo);
	if (ret != QUERY_SUCCESS)
	{
		QMessageBox::warning(this, en ? "Warning" : QStringLiteral("提示"), en ? "Failed" : QStringLiteral("获取操作参数失败！"), QMessageBox::Ok);
		return;
	}
	ShowCtrlData();
}

void OptInfoWnd::SlotBtnSendOptParam()
{
	
}

void OptInfoWnd::GetCtrlData()
{
	if (m_pOptType)
	{
		m_optInfo.type = m_pOptType->text().toInt();
	}
	for (size_t i = 0; i < OPT_PARAM_NUM; ++i)
	{
		if (m_pOptParam[i])
		{
			double param = m_pOptParam[i]->text().toDouble();
			m_optInfo.params[i] = (int64_t)(param * 1000);
		}
	}
}
