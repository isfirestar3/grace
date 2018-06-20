#include "NodeInfoWnd.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qcheckbox.h>
#include "AlgorithmDefine.hpp"
#include <QtWidgets/qapplication.h>

NodeInfoWnd::NodeInfoWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


NodeInfoWnd::~NodeInfoWnd()
{
}

void NodeInfoWnd::InitWnd()
{
	resize(200, 150);
	setWindowTitle(en ? "NodeInfo" : QStringLiteral("节点属性编辑"));
}
void NodeInfoWnd::InitCtrl()
{
	m_pEditeNodeId = new QLineEdit;
	m_pEditNodePosx = new QLineEdit;
	m_pEditNodePosy = new QLineEdit;
	m_pBtnComfirm = new QPushButton(en ? "Comfirm" : QStringLiteral("确定"));
	m_pBtnCancel = new QPushButton(en ? "Cancel" : QStringLiteral("取消"));
	if (m_pEditeNodeId)
	{
		m_pEditeNodeId->setEnabled(false);
	}
}

void NodeInfoWnd::InitForm()
{
	QGridLayout* pMainLayout = new QGridLayout;
	if (pMainLayout)
	{
		QLabel* pLabelId = new QLabel(en ? "Id:" : QStringLiteral("ID:"));
		QLabel* pLabelPosx = new QLabel(en ? "x:" : QStringLiteral("X:"));
		QLabel* pLabelPosy = new QLabel(en ? "y:" : QStringLiteral("Y:"));
		m_pSpin = new QCheckBox(en ? "Spin" : QStringLiteral("是否可旋转"));
		pMainLayout->addWidget(pLabelId, 0, 0, 1, 1);
		pMainLayout->addWidget(m_pEditeNodeId, 0, 1, 1, 1);
		pMainLayout->addWidget(pLabelPosx, 1, 0, 1, 1);
		pMainLayout->addWidget(m_pEditNodePosx, 1, 1, 1, 1);
		pMainLayout->addWidget(pLabelPosy, 2, 0, 1, 1);
		pMainLayout->addWidget(m_pEditNodePosy, 2, 1, 1, 1);
		pMainLayout->addWidget(m_pSpin, 3, 1, 1, 1);
		pMainLayout->addWidget(m_pBtnComfirm, 4, 0, 1, 1);
		pMainLayout->addWidget(m_pBtnCancel, 4, 1, 1, 1);
	}
	setLayout(pMainLayout);
}
void NodeInfoWnd::InitSlot()
{
	connect(m_pBtnComfirm, &QPushButton::clicked, this, &NodeInfoWnd::SlotBtnComfirm);
	connect(m_pBtnCancel, &QPushButton::clicked, this, &NodeInfoWnd::SlotBtnCancel);
}

void NodeInfoWnd::SetNodeInfo(const NodeInfo& nodeInfo)
{
	if (m_pEditeNodeId)
	{
		m_pEditeNodeId->setText(QString::number(nodeInfo.node_id));
	}
	if (m_pEditNodePosx)
	{
		m_pEditNodePosx->setText(QString::number(DataConvert::map2world(nodeInfo.pos_x)));
	}
	if (m_pEditNodePosy)
	{
		m_pEditNodePosy->setText(QString::number(DataConvert::map2world(nodeInfo.pos_y)));
	}
	if (m_pSpin) 
	{
		m_pSpin->setChecked(nodeInfo.spin);
	}
	m_nodeInfo = nodeInfo;
}

void NodeInfoWnd::GetNodeInfo(NodeInfo& nodeInfo)
{
	nodeInfo = m_nodeInfo;
}

void NodeInfoWnd::SlotBtnComfirm()
{
	if (m_pEditNodePosx)
	{
		QString strPosx = m_pEditNodePosx->text();
		m_nodeInfo.pos_x = DataConvert::world2map(strPosx.toDouble());
	}
	if (m_pEditNodePosy)
	{
		QString strPosy = m_pEditNodePosy->text();
		m_nodeInfo.pos_y = DataConvert::world2map(strPosy.toDouble());
	}
	if (m_pSpin)
	{
		m_nodeInfo.spin = m_pSpin->isChecked() ? 1 : 0;
	}
	accept();
}

void NodeInfoWnd::SlotBtnCancel()
{
	reject();
}
