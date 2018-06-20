#include "QRcodeInfoWnd.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qcheckbox.h>
#include "AlgorithmDefine.hpp"
#include <QtWidgets/qapplication.h>

QRcodeInfoWnd::QRcodeInfoWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


QRcodeInfoWnd::~QRcodeInfoWnd()
{
}

void QRcodeInfoWnd::InitWnd()
{
	resize(200, 150);
	setWindowTitle(en ? "NodeInfo" : QStringLiteral("曲线二维码位置信息"));
}
void QRcodeInfoWnd::InitCtrl()
{
	m_pEditeCodeId = new QLineEdit;
	m_pEditCodePosx = new QLineEdit;
	m_pEditCodePosy = new QLineEdit;
	m_pEditCodePosw = new QLineEdit;
	m_pBtnComfirm = new QPushButton(en ? "Comfirm" : QStringLiteral("确定"));
	m_pBtnCancel = new QPushButton(en ? "Cancel" : QStringLiteral("取消"));
	/*if (m_pEditeCodeId)
	{
		m_pEditeNodeId->setEnabled(false);
		m_pEditNodePosx->setEnabled(false);
		m_pEditNodePosy->setEnabled(false);
		m_pEditNodePosw->setEnabled(false);
	}*/
}

void QRcodeInfoWnd::InitForm()
{
	QGridLayout* pMainLayout = new QGridLayout;
	if (pMainLayout)
	{
		QLabel* pLabelId = new QLabel(en ? "Id:" : QStringLiteral("ID:"));
		QLabel* pLabelPosx = new QLabel(en ? "x:" : QStringLiteral("X:"));
		QLabel* pLabelPosy = new QLabel(en ? "y:" : QStringLiteral("Y:"));
		QLabel* pLabelPosw = new QLabel(en ? "w:" : QStringLiteral("W:"));

		pMainLayout->addWidget(pLabelId, 0, 0, 1, 1);
		pMainLayout->addWidget(m_pEditeCodeId, 0, 1, 1, 1);
		pMainLayout->addWidget(pLabelPosx, 1, 0, 1, 1);
		pMainLayout->addWidget(m_pEditCodePosx, 1, 1, 1, 1);
		pMainLayout->addWidget(pLabelPosy, 2, 0, 1, 1);
		pMainLayout->addWidget(m_pEditCodePosy, 2, 1, 1, 1);
		pMainLayout->addWidget(pLabelPosw, 3, 0, 1, 1);
		pMainLayout->addWidget(m_pEditCodePosw, 3, 1, 1, 1);
		
		pMainLayout->addWidget(m_pBtnComfirm, 4, 0, 1, 1);
		pMainLayout->addWidget(m_pBtnCancel, 4, 1, 1, 1);
	}
	setLayout(pMainLayout);
}
void QRcodeInfoWnd::InitSlot()
{
	connect(m_pBtnComfirm, &QPushButton::clicked, this, &QRcodeInfoWnd::SlotBtnComfirm);
	connect(m_pBtnCancel, &QPushButton::clicked, this, &QRcodeInfoWnd::SlotBtnCancel);
}

void QRcodeInfoWnd::SetCodeInfo(const CODEPOSXYA& nodeInfo)
{
	if (m_pEditeCodeId)
	{
		m_pEditeCodeId->setText(QString::number(nodeInfo.pos_id));
	}
	if (m_pEditCodePosx)
	{
		m_pEditCodePosx->setText(QString::number(DataConvert::map2world(nodeInfo.pos_x)));
	}
	if (m_pEditCodePosy)
	{
		m_pEditCodePosy->setText(QString::number(DataConvert::map2world(nodeInfo.pos_y)));
	}
	if (m_pEditCodePosw)
	{
		double angle = nodeInfo.angle;
		//if (angle > 22.5 && angle < 67.5)angle = 45;
		//else if (angle > 112.5 && angle < 157.5)angle = 135;
		//else if (angle > -67.5 && angle < -22.5)angle = -45;
		//else if (angle < -112.5 && angle > -157.5)angle = -135;

		m_pEditCodePosw->setText(QString::number(angle));
	}
	m_posInfo = nodeInfo;
}

void QRcodeInfoWnd::GetCodeInfo(CODEPOSXYA& nodeInfo)
{
	nodeInfo = m_posInfo;
}

void QRcodeInfoWnd::SlotBtnComfirm()
{
	accept();
}

void QRcodeInfoWnd::SlotBtnCancel()
{
	reject();
}
