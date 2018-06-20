#include "LocationOfCurve.h"
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include "PosandTrack.h"
#include "DataManage.h"
#include <math.h>

LocationOfCurve::LocationOfCurve()
{
	InitWnd();
	InitForm();
	InitSlot();
}


LocationOfCurve::~LocationOfCurve()
{
}

void LocationOfCurve::InitWnd()
{
	resize(300, 400);
	setWindowTitle(QStringLiteral("曲线二维码位置"));
}

void LocationOfCurve::InitForm()
{
	m_posCount = new QLineEdit;
	m_posDis = new QLineEdit;
	m_pBtnSaveInfo = new QPushButton(QStringLiteral("确定"));
	m_pBtnExit = new QPushButton(QStringLiteral("取消"));

	QGridLayout* pAttrLayout = new QGridLayout;
	if (pAttrLayout)
	{
		QLabel* p_posCount = new QLabel(QStringLiteral("需要标记点的个数："));
		QLabel* p_posDis = new QLabel(QStringLiteral("左右二维码距离："));
		QLabel* pLabeldist = new QLabel(QStringLiteral("cm"));

		pAttrLayout->addWidget(p_posCount, 0, 0, 1, 1);
		pAttrLayout->addWidget(m_posCount, 0, 1, 1, 1);
		pAttrLayout->addWidget(p_posDis, 1, 0, 1, 1);
		pAttrLayout->addWidget(m_posDis, 1, 1, 1, 1);
		pAttrLayout->addWidget(pLabeldist, 1, 2, 1, 1);
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
void LocationOfCurve::InitSlot()
{
	connect(m_pBtnSaveInfo, &QPushButton::clicked, this, &LocationOfCurve::SlotBtnSaveData);
	connect(m_pBtnExit, &QPushButton::clicked, this, &QDialog::reject);
}
void LocationOfCurve::SlotBtnSaveData()
{

	if (m_posCount->text() != "" && m_posDis->text() != ""){
		int count = m_posCount->text().toInt();
		double distance = m_posDis->text().toDouble();
		CODEPOSXYA codeinfo;
		int part = 200 / (count+1);
		for (int i = 1; i < count+1; i++){
			id_++;
			//if (edgeInfo_.disperse_point.size()>=200){
			position_t tmp;
			position_t tmp1;
			position_t tmp2;
			var__edge_t edge_t;
			DATAMNG->ConvertEdgeInfo(edgeInfo_, edge_t);
			GetposbyT((i*part - 1)*0.005, &edge_t, &tmp);//求该离散点的坐标及角度

			codeinfo.pos_x = tmp.x_;
			codeinfo.pos_y = tmp.y_;
			codeinfo.angle = DataConvert::radian2angle(tmp.angle_);
			codeinfo.pos_id = id_;
			vct_pos_.push_back(codeinfo);

			double w = 180 * tmp.w_ / Pi;

			if ((90 < w && w < 180) || (-90 < w && w < 0)){
				tmp1.x_ = tmp.x_ + abs(sin(tmp.w_)*distance);
				tmp1.y_ = tmp.y_ + abs(cos(tmp.w_)*distance);
				tmp1.w_ = tmp.w_;

				tmp2.x_ = tmp.x_ - abs(sin(tmp.w_)*distance);
				tmp2.y_ = tmp.y_ - abs(cos(tmp.w_)*distance);
				tmp2.w_ = tmp.w_;
			}
			else if ((0 < w && w < 90) || (-180 < w && w < -90)){
				tmp1.x_ = tmp.x_ - abs(sin(tmp.w_)*distance);
				tmp1.y_ = tmp.y_ + abs(cos(tmp.w_)*distance);
				tmp1.w_ = tmp.w_;

				tmp2.x_ = tmp.x_ + abs(sin(tmp.w_)*distance);
				tmp2.y_ = tmp.y_ - abs(cos(tmp.w_)*distance);
				tmp2.w_ = tmp.w_;
			}
			else if (w == 0 || w == 90 || w == 180 || w == -180 || w == -90){
				vct_pos_.clear();
				this->accept();
				return;
			}
			id_++;
			codeinfo.pos_x = tmp1.x_;
			codeinfo.pos_y = tmp1.y_;
			codeinfo.angle = DataConvert::radian2angle(tmp1.angle_);
			codeinfo.pos_id = id_;
			vct_pos_.push_back(codeinfo);

			id_++;
			codeinfo.pos_x = tmp2.x_;
			codeinfo.pos_y = tmp2.y_;
			codeinfo.angle = DataConvert::radian2angle(tmp2.angle_);
			codeinfo.pos_id = id_;
			vct_pos_.push_back(codeinfo);
			//}
		}
	}
	this->accept();
}