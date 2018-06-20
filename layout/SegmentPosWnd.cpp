#include "SegmentPosWnd.h"
#include <QtWidgets\qgridlayout.h>
#include <QtWidgets\qlabel.h>
#include "AlgorithmDefine.hpp"
#include <QtWidgets/qapplication.h>

SegmentPosWnd::SegmentPosWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}
SegmentPosWnd::~SegmentPosWnd()
{

}

void SegmentPosWnd::InitWnd()
{
	resize(200, 150);
	setWindowTitle(en ? "SegmentPos" : QStringLiteral("节点属性编辑"));
}
void SegmentPosWnd::InitCtrl()
{
	m_pEditPosx = new QLineEdit;
	m_pEditPosy = new QLineEdit;
	m_pBtnComfirm = new QPushButton(en ? "Comfirm" : QStringLiteral("确定"));
	m_pBtnCancel = new QPushButton(en ? "Cancel" : QStringLiteral("取消"));
}

void SegmentPosWnd::InitForm()
{
	QGridLayout* pMainLayout = new QGridLayout;
	if (pMainLayout)
	{
		QLabel* pLabelPosx = new QLabel(QStringLiteral("X(m):"));
		QLabel* pLabelPosy = new QLabel(QStringLiteral("Y(m):"));
		pMainLayout->addWidget(pLabelPosx, 0, 0, 1, 1);
		pMainLayout->addWidget(m_pEditPosx, 0, 1, 1, 1);
		pMainLayout->addWidget(pLabelPosy, 1, 0, 1, 1);
		pMainLayout->addWidget(m_pEditPosy, 1, 1, 1, 1);
		pMainLayout->addWidget(m_pBtnComfirm, 2, 0, 1, 1);
		pMainLayout->addWidget(m_pBtnCancel, 2, 1, 1, 1);
	}
	setLayout(pMainLayout);
}
void SegmentPosWnd::InitSlot()
{
	connect(m_pBtnComfirm, &QPushButton::clicked, this, &SegmentPosWnd::SlotBtnComfirm);
	connect(m_pBtnCancel, &QPushButton::clicked, this, &SegmentPosWnd::SlotBtnCancel);
}

void SegmentPosWnd::SetPos(const QPointF& pos)
{
	if (m_pEditPosx){
		m_pEditPosx->setText(QString::number(DataConvert::map2world(pos.x())));
	}
	if (m_pEditPosy){
		m_pEditPosy->setText(QString::number(DataConvert::map2world(pos.y())));
	}
}

void SegmentPosWnd::GetPos(QPointF& pos)
{
	if (m_pEditPosx){
		pos.setX(DataConvert::world2map(m_pEditPosx->text().toDouble()));
	}
	if (m_pEditPosy){
		pos.setY(DataConvert::world2map(m_pEditPosy->text().toDouble()));
	}
}

void SegmentPosWnd::SlotBtnComfirm()
{
	accept();
}

void SegmentPosWnd::SlotBtnCancel()
{
	reject();
}