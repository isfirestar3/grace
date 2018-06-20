#ifndef INIT_LIU_2016_01213_
#define INIT_LIU_2016_01213_

#include "GenerateVariable180.h"
#include "Matrix.h"

class GenerateManager : public curve_product
{
public:
	GenerateManager(){}

	~GenerateManager(){
	}

public:
    ParameteSituation param_situation;
	PointInit ControlPoints;
	PointInit OrignPoints;      // 角度转换前的原始点

	int beziertype;

	double x_m,y_q;

public:
	PointInit ParameteInit(PointParamete& FirstPoint,PointParamete& SecondPoint);
	PointParamete CreatePoint(double p_x,double p_y,double p_angle);
    PointParamete init_1(PointInit Points_);
    PointParamete init_2(PointInit Points_);
	PointParamete init_3(PointInit Points_);

	void CurveGenerate(double p1_x,double p1_y,double p1_angle,double p2_x,double p2_y,double p2_angle,vector<ControlPoints_>& _ControlPoints_90);

	int ParamQuadrant(bool first_param,bool second_param,PointParamete& FirstPoint,PointParamete& SecondPoint);
	int ParamLengthgrade(double _x_m,double _y_n,double _x_p,double _y_q);
	double PointRotation(double m_,double n_);
    
	/* *************贝塞尔控制点计算************ */
	double Get_Port_Cur_a(CMatrix X);
    double Get_Port_Cur_b(CMatrix X);
	double GetMaxCur_a(CMatrix X);
	double GetMaxCur_b(CMatrix X);
	double GetMaxRate_a(CMatrix X);
	double GetMaxRate_b(CMatrix X);
	double Fscore( CMatrix X);
	void DataTran(double a_init[6]);
	void DoOptimize(CMatrix Xinit,bool& val,CMatrix & Xresult,vector<ControlPoints_>& _ControlPoints_90);
};
#endif