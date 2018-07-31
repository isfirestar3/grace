#include "NodesEditWnd.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>

NodesEditWnd::NodesEditWnd()
{
	InitWnd();
	InitForm();
	InitSlot();
}


NodesEditWnd::~NodesEditWnd()
{
}
void NodesEditWnd::InitWnd()
{
	resize(200, 200);
	setWindowTitle(QStringLiteral("编辑节点属性"));
}

void NodesEditWnd::InitForm()
{
	m_pBtnSaveInfo = new QPushButton(QStringLiteral("确定"));
	m_pBtnExit = new QPushButton(QStringLiteral("取消"));

	QGridLayout* pAttrLayout = new QGridLayout;
	if (pAttrLayout)
	{
		m_pSpin = new QCheckBox(QStringLiteral("是否可旋转："));
		pAttrLayout->addWidget(m_pSpin, 0, 0, 1, 1);
		m_pMove = new QCheckBox(QStringLiteral("是否移动"));
		pAttrLayout->addWidget(m_pMove, 2, 0, 1, 1);
		m_pLabel_X = new QLabel(QStringLiteral("X轴平移："));
		pAttrLayout->addWidget(m_pLabel_X, 3, 0, 1, 1);
		m_pLabel_Y = new QLabel(QStringLiteral("Y轴平移："));
		pAttrLayout->addWidget(m_pLabel_Y, 4, 0, 1, 1);
		m_pMove_X = new QLineEdit();
		pAttrLayout->addWidget(m_pMove_X, 3, 1, 1, 1);
		m_pMove_Y = new QLineEdit();
		pAttrLayout->addWidget(m_pMove_Y, 4, 1, 1, 1);
		m_pCm_X = new QLabel(QStringLiteral("cm"));
		pAttrLayout->addWidget(m_pCm_X, 3, 2, 1, 1);
		m_pCm_Y = new QLabel(QStringLiteral("cm"));
		pAttrLayout->addWidget(m_pCm_Y, 4, 2, 1, 1);
	}
	QGridLayout* pSaveLayout = new QGridLayout;
	if (pSaveLayout)
	{
		pSaveLayout->addWidget(m_pBtnSaveInfo, 0, 3, 1, 1);
		pSaveLayout->addWidget(m_pBtnExit, 0, 4, 1, 1);
	}
	QVBoxLayout* pWopLayout = new QVBoxLayout;
	if (pWopLayout)
	{
		pWopLayout->addLayout(pAttrLayout);
		pWopLayout->addLayout(pSaveLayout);
	}
	setLayout(pWopLayout);
}
void NodesEditWnd::InitSlot()
{
	connect(m_pBtnSaveInfo, &QPushButton::clicked, this, &NodesEditWnd::SlotBtnSaveData);
	connect(m_pBtnExit, &QPushButton::clicked, this, &NodesEditWnd::reject);
}
void NodesEditWnd::SlotBtnSaveData()
{
	pSpin = m_pSpin->isChecked();
	pMove = m_pMove->isChecked();
	move_x = m_pMove_X->text().toDouble();
	move_y = m_pMove_Y->text().toDouble();
	this->accept();
}