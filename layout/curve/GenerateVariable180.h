#ifndef INIT180_LIU_2016_01213_
#define INIT180_LIU_2016_01213_

#include "Matrix.h"
#include <iostream>
#include <vector>
#include "curve_product.hpp"
using namespace std;

class GenerateManager180 : public curve_product
{
public:
	GenerateManager180(){}

	~GenerateManager180(){
	}

public:
	ParameteSituation param_situation;
	PointInit ControlPoints;    // 标准情况下
	PointInit OrignPoints;      // 角度转换前的原始点
	PointInit _Points_;          // 角度转换后的原始点

	int beziertype;

	double x_m,y_q;
public:
	PointInit ParameteInit_(PointParamete FirstPoint,PointParamete SecondPoint);
	PointParamete CreatePoint_(double p_x,double p_y,double p_angle);
	PointParamete init_1_(PointInit Points_);

	void CurveGenerate(double p1_x,double p1_y,double p1_angle,double p2_x,double p2_y,double p2_angle,vector<ControlPoints_>& _ControlPoints_);

	int ParamQuadrant_(PointParamete FirstPoint,PointParamete SecondPoint);
	int ParamLengthgrade_(double _x_m,double _y_n,double _x_p,double _y_q);
	double PointRotation_(double m_,double n_);

	/* *************贝塞尔控制点计算************ */
	double Get_Cur(double a,double b,double c,double d,double e,double f,double m,double n,double t);
	double Get_Port_Cur_a_(CMatrix X);
	double Get_Port_Cur_b_(CMatrix X);
	double GetMaxCur_a_(CMatrix X);
	double GetMaxCur_b_(CMatrix X);
	double GetMaxRate_a_(CMatrix X);
	double GetMaxRate_b_(CMatrix X);
	double Fscore_( CMatrix X);
	void DataTran_(double a_init[2]);
	void DataOut(double a_init[2],vector<ControlPoints_>& _ControlPoints_);
	void DoOptimize_(CMatrix Xinit,bool& val,CMatrix & Xresult,vector<ControlPoints_>& _ControlPoints_);
};
#endif