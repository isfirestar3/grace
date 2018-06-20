#include "CtrlPointInfoWnd.h"
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qmessagebox.h>
#include "AlgorithmDefine.hpp"
#include <QtWidgets/qapplication.h>

CtrlPointInfoWnd::CtrlPointInfoWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}

CtrlPointInfoWnd::~CtrlPointInfoWnd()
{

}

void CtrlPointInfoWnd::InitWnd()
{
	resize(200, 100);
	setWindowTitle(en ? "CtrlPointInfo" : QStringLiteral("控制点信息"));
}

void CtrlPointInfoWnd::InitCtrl()
{
	m_pEditPosx = new QLineEdit;
	m_pEditPosy = new QLineEdit;
	m_pBtnConfirm = new QPushButton(en ? "Confirm" : QStringLiteral("确定"));
}

void CtrlPointInfoWnd::InitForm()
{
	QGridLayout* pGridLayout = new QGridLayout;
	if (pGridLayout)
	{
		QLabel* pLabelPosx = new QLabel(en ? "x:" : QStringLiteral("位置x(m):"));
		QLabel* pLabelPosy = new QLabel(en ? "y:" : QStringLiteral("位置y(m):"));
		pGridLayout->addWidget(pLabelPosx, 0, 0, 1, 1);
		pGridLayout->addWidget(m_pEditPosx, 0, 1, 1, 1);
		pGridLayout->addWidget(pLabelPosy, 1, 0, 1, 1);
		pGridLayout->addWidget(m_pEditPosy, 1, 1, 1, 1);
	}
	QVBoxLayout* pVLayout = new QVBoxLayout;
	if (pVLayout)
	{
		pVLayout->addLayout(pGridLayout);
		pVLayout->addWidget(m_pBtnConfirm);
	}
	setLayout(pVLayout);
}

void CtrlPointInfoWnd::InitSlot()
{
	connect(m_pBtnConfirm, &QPushButton::clicked, this, &CtrlPointInfoWnd::SlotBtnConfirm);
}

void CtrlPointInfoWnd::SlotBtnConfirm()
{
	if (GetCtrlPos(m_currentPos))
	{
		accept();
	}
}

void CtrlPointInfoWnd::SetPos(const QPointF& pos)
{
	if (m_pEditPosx)
	{
		m_pEditPosx->setText(QString::number(DataConvert::map2world(pos.x())));
	}
	if (m_pEditPosy)
	{
		m_pEditPosy->setText(QString::number(DataConvert::map2world(pos.y())));
	}
	m_currentPos = pos;
}

bool CtrlPointInfoWnd::GetCtrlPos(QPointF& pos)
{
	if (m_pEditPosx)
	{
		QString strPosx = m_pEditPosx->text();
		if (strPosx.isEmpty())
		{
			QMessageBox::warning(this, en ? "Warning" : QStringLiteral("提示"), en ? "Please Enter x" : QStringLiteral("请输入位置x"), QMessageBox::Ok);
			return false;
		}
		pos.setX(DataConvert::world2map(strPosx.toDouble()));
	}
	if (m_pEditPosy)
	{
		QString strPosy = m_pEditPosy->text();
		if (strPosy.isEmpty())
		{
			QMessageBox::warning(this, en ? "Warning" : QStringLiteral("提示"), en ? "Please Enter y" : QStringLiteral("请输入位置y"), QMessageBox::Ok);
			return false;
		}
		pos.setY(DataConvert::world2map(strPosy.toDouble()));
	}
	return true;
}