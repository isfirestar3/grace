#include "GenerateVariable.h"

#define N 2
/* ****************贝塞尔控制点参数初始化******************** */
double GenerateManager::PointRotation(double m_,double n_)  // n是对称轴 m是待镜像坐标值
{
	m_=2*n_-m_;
	return m_;
}

int GenerateManager::ParamQuadrant(bool first_param,bool second_param,PointParamete& FirstPoint,PointParamete& SecondPoint)
{
	int quadrant = 0;
	if (!first_param&&second_param)                                                          // 第一象限 
	{
		quadrant=(FirstPoint.point_angle<SecondPoint.point_angle)?1:3;
		if (quadrant == 3)
		{
           PointParamete PointTemp;
		   PointTemp = FirstPoint;
		   FirstPoint = SecondPoint;
		   SecondPoint = PointTemp;
		}
	}
	if (first_param&&second_param)
	{ 
		quadrant=(FirstPoint.point_angle<SecondPoint.point_angle)?2:4;
		if (quadrant == 4)
		{
			PointParamete PointTemp;
			PointTemp = FirstPoint;
			FirstPoint = SecondPoint;
			SecondPoint = PointTemp;
		}
	}
	if (first_param&&!second_param)
	{ 
		quadrant=(FirstPoint.point_angle>SecondPoint.point_angle)?3:1;
		if (quadrant == 1)
		{
			PointParamete PointTemp;
			PointTemp = FirstPoint;
			FirstPoint = SecondPoint;
			SecondPoint = PointTemp;
		}
	}
	if (!first_param&&!second_param)
	{ 
		quadrant=(FirstPoint.point_angle<SecondPoint.point_angle)?4:2;
		if (quadrant == 2)
		{
			PointParamete PointTemp;
			PointTemp = FirstPoint;
			FirstPoint = SecondPoint;
			SecondPoint = PointTemp;
		}
	}

	OrignPoints.origin=FirstPoint;
	OrignPoints.termina=SecondPoint;
	return quadrant;
}

int GenerateManager::ParamLengthgrade(double _x_m,double _y_n,double _x_p,double _y_q)
{
   int data_mode = 0;
   double length = abs(_x_p-_x_m);
   if (length==0.5)
   {
	   data_mode=1;           //0.5级别
   }
   if(length>0.5&&length<=1)
   {
	   data_mode=2;           //0.5-1级别
   }
   if (length>1)
   {
	   data_mode=3;          //1级别
   }
   x_m=_x_m;
   y_q=_y_q;
   return data_mode;
}

PointInit GenerateManager::ParameteInit(PointParamete& FirstPoint,PointParamete& SecondPoint)
{
	PointInit Points;

	bool first_param = (FirstPoint.point_x>SecondPoint.point_x)?true:false;
	bool second_param = (FirstPoint.point_y>SecondPoint.point_y)?true:false;

	param_situation.quadrant=ParamQuadrant(first_param,second_param,FirstPoint,SecondPoint);
	switch(param_situation.quadrant)                                                       //先都给旋转到标准情况 即1象限
	{
	case 1:
		break;
	case 2:
		SecondPoint.point_x=PointRotation(SecondPoint.point_x,FirstPoint.point_x);                                           
		break;
	case 3:
		FirstPoint.point_y=PointRotation(FirstPoint.point_y,SecondPoint.point_y);
		SecondPoint.point_x=PointRotation(SecondPoint.point_x,FirstPoint.point_x);
		break;
	case 4:
		FirstPoint.point_y=PointRotation(FirstPoint.point_y,SecondPoint.point_y);
		break;
	}
	param_situation.length_grade=ParamLengthgrade(FirstPoint.point_x,FirstPoint.point_y,SecondPoint.point_x,SecondPoint.point_y);
	Points.origin=FirstPoint;
	Points.termina=SecondPoint;
	return Points;
} 

PointParamete GenerateManager::CreatePoint(double p_x,double p_y,double p_angle)
{
   PointParamete Point;
   Point.point_x = p_x;
   Point.point_y = p_y;
   Point.point_angle = p_angle;
   return Point;
}
//    ++0.5*0.5++
PointParamete GenerateManager::init_1(PointInit Points_)
{
	PointParamete XParam;
	ControlPoints.origin.point_x=0; //
	ControlPoints.origin.point_y=Points_.origin.point_y-Points_.termina.point_y;
	ControlPoints.termina.point_x=Points_.termina.point_x-Points_.origin.point_x;
	ControlPoints.termina.point_y=0;

	XParam.point_x=0.3555*abs(Points_.termina.point_x-Points_.origin.point_x);             //对称情况下
	XParam.point_y=0.5139*abs(Points_.termina.point_x-Points_.origin.point_x);

	beziertype=1;
	return XParam;
}
//    >0.5 <1
PointParamete GenerateManager::init_2(PointInit Points_)
{
	PointParamete XParam;
	ControlPoints.origin.point_x=0; //
	ControlPoints.origin.point_y=Points_.origin.point_y-Points_.termina.point_y;
	ControlPoints.termina.point_x=Points_.termina.point_x-Points_.origin.point_x;
	ControlPoints.termina.point_y=0;

	XParam.point_x=0.25*abs(Points_.termina.point_x-Points_.origin.point_x);             //对称情况下
	XParam.point_y=0.375*abs(Points_.termina.point_x-Points_.origin.point_x);

	beziertype=2;
	return XParam;
}
//         1情况
PointParamete GenerateManager::init_3(PointInit Points_)
{
	PointParamete XParam;
	ControlPoints.origin.point_x=0; //
	ControlPoints.origin.point_y=Points_.origin.point_y-Points_.termina.point_y;
	ControlPoints.termina.point_x=Points_.termina.point_x-Points_.origin.point_x;
	ControlPoints.termina.point_y=0;

	XParam.point_x=0.3*abs(Points_.termina.point_x-Points_.origin.point_x);             //对称情况下
	XParam.point_y=0.6*abs(Points_.termina.point_x-Points_.origin.point_x);

	beziertype=3;
	return XParam;
}
/* ****************贝塞尔控制点计算******************** */
double GenerateManager::Get_Port_Cur_a(CMatrix X)
{
	double t_temp=0.99;
	double t=1;

	double _x_3=0.5*(X(1,0)+ControlPoints.termina.point_x);
	double _y_3=0.5*(X(1,0)+2*ControlPoints.origin.point_y-ControlPoints.termina.point_x);
	double _y_1=X(1,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;
	double _y_2=X(0,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;

	double x_1=ControlPoints.origin.point_x*pow((1-t_temp),3)+X(0,0)*3*pow((1-t_temp),2)*t_temp+X(1,0)*3*(1-t_temp)*t_temp*t_temp+_x_3*pow(t_temp,3);
	double y_1=ControlPoints.origin.point_y*pow((1-t_temp),3)+ControlPoints.origin.point_y*3*pow((1-t_temp),2)*t_temp+ControlPoints.origin.point_y*3*(1-t_temp)*t_temp*t_temp+_y_3*pow(t_temp,3);

	double x_2=ControlPoints.origin.point_x*pow((1-t),3)+X(0,0)*3*pow((1-t),2)*t+X(1,0)*3*(1-t)*t*t+_x_3*pow(t,3);
	double y_2=ControlPoints.origin.point_y*pow((1-t),3)+ControlPoints.origin.point_y*3*pow((1-t),2)*t+ControlPoints.origin.point_y*3*(1-t)*t*t+_y_3*pow(t,3);

	double CurDis=sqrt(pow((x_2-x_1),2)+pow((y_2-y_1),2));

	double Cur_1=abs((3*_y_3*t*t - 3*ControlPoints.origin.point_y*t*t - 6*ControlPoints.origin.point_y*t*(t - 1) + 3*ControlPoints.origin.point_y*t*(2*t - 2))*(6*X(0,0)*t - 12*X(1,0)*t + 6*_x_3*t - 6*X(1,0)*(t - 1) + 6*X(0,0)*(2*t - 2) - 3*ControlPoints.origin.point_x*(2*t - 2)) + (3*ControlPoints.origin.point_y*(2*t - 2) + 6*_y_3*t - 6*ControlPoints.origin.point_y*t - 6*ControlPoints.origin.point_y*(t - 1))*(3*X(1,0)*t*t - 3*_x_3*t*t - 3*X(0,0)*pow((t - 1),2) + 3*ControlPoints.origin.point_x*pow((t - 1),2) + 6*X(1,0)*t*(t - 1) - 3*X(0,0)*t*(2*t - 2)))/(27*pow((pow((3*X(1,0)*t*t - 3*_x_3*t*t - 3*X(0,0)*pow((t - 1),2) + 3*ControlPoints.origin.point_x*pow((t - 1),2) + 6*X(1,0)*t*(t - 1) - 3*X(0,0)*t*(2*t - 2)),2.0)*0.11111 + pow((3*_y_3*t*t - 3*ControlPoints.origin.point_y*t*t - 6*ControlPoints.origin.point_y*t*(t - 1) + 3*ControlPoints.origin.point_y*t*(2*t - 2)),2.0)*0.11111),3.0/2));
	double Cur_2=abs((3*_y_3*t_temp*t_temp - 3*ControlPoints.origin.point_y*t_temp*t_temp - 6*ControlPoints.origin.point_y*t_temp*(t_temp - 1) + 3*ControlPoints.origin.point_y*t_temp*(2*t_temp - 2))*(6*X(0,0)*t_temp - 12*X(1,0)*t_temp + 6*_x_3*t_temp - 6*X(1,0)*(t_temp - 1) + 6*X(0,0)*(2*t_temp - 2) - 3*ControlPoints.origin.point_x*(2*t_temp - 2)) + (3*ControlPoints.origin.point_y*(2*t_temp - 2) + 6*_y_3*t_temp - 6*ControlPoints.origin.point_y*t_temp - 6*ControlPoints.origin.point_y*(t_temp - 1))*(3*X(1,0)*t_temp*t_temp - 3*_x_3*t_temp*t_temp - 3*X(0,0)*pow((t_temp - 1),2) + 3*ControlPoints.origin.point_x*pow((t_temp - 1),2) + 6*X(1,0)*t_temp*(t_temp - 1) - 3*X(0,0)*t_temp*(2*t_temp - 2)))/(27*pow((pow((3*X(1,0)*t_temp*t_temp - 3*_x_3*t_temp*t_temp - 3*X(0,0)*pow((t_temp - 1),2) + 3*ControlPoints.origin.point_x*pow((t_temp - 1),2) + 6*X(1,0)*t_temp*(t_temp - 1) - 3*X(0,0)*t_temp*(2*t_temp - 2)),2.0)*0.11111 + pow((3*_y_3*t_temp*t_temp - 3*ControlPoints.origin.point_y*t_temp*t_temp - 6*ControlPoints.origin.point_y*t_temp*(t_temp - 1) + 3*ControlPoints.origin.point_y*t_temp*(2*t_temp - 2)),2.0)*0.11111),3.0/2));

	double Port_rate_a=abs(Cur_1-Cur_2)/CurDis;
	return Port_rate_a;
}

double GenerateManager::Get_Port_Cur_b(CMatrix X)
{
	double t_temp=0;
	double t=0.01;

	double _x_3=0.5*(X(1,0)+ControlPoints.termina.point_x);
	double _y_3=0.5*(X(1,0)+2*ControlPoints.origin.point_y-ControlPoints.termina.point_x);
	double _y_1=X(1,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;
	double _y_2=X(0,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;

	double x_1=_x_3*pow((1-t_temp),3)+ControlPoints.termina.point_x*3*pow((1-t_temp),2)*t_temp+ControlPoints.termina.point_x*3*(1-t_temp)*t_temp*t_temp+ControlPoints.termina.point_x*pow(t_temp,3);
	double y_1=_y_3*pow((1-t_temp),3)+_y_1*3*pow((1-t_temp),2)*t_temp+_y_2*3*(1-t_temp)*t_temp*t_temp+ControlPoints.termina.point_y*pow(t_temp,3);

	double x_2=_x_3*pow((1-t),3)+ControlPoints.termina.point_x*3*pow((1-t),2)*t+ControlPoints.termina.point_x*3*(1-t)*t*t+ControlPoints.termina.point_x*pow(t,3);
	double y_2=_y_3*pow((1-t),3)+_y_1*3*pow((1-t),2)*t+_y_2*3*(1-t)*t*t+ControlPoints.termina.point_y*pow(t,3);

	double CurDis=sqrt(pow((x_2-x_1),2)+pow((y_2-y_1),2));

	double Cur_1=abs((6*ControlPoints.termina.point_x*(t - 1) - 6*ControlPoints.termina.point_x*(2*t - 2) + 3*_x_3*(2*t - 2))*(3*_y_2*t*t - 3*ControlPoints.termina.point_y*t*t + 3*_y_3*pow((t - 1),2) - 3*_y_1*pow((t - 1),2) + 6*_y_2*t*(t - 1) - 3*_y_1*t*(2*t - 2)) - (3*ControlPoints.termina.point_x*pow((t - 1),2) - 3*_x_3*pow((t - 1),2) - 6*ControlPoints.termina.point_x*t*(t - 1) + 3*ControlPoints.termina.point_x*t*(2*t - 2))*(6*_y_1*t - 12*_y_2*t + 6*ControlPoints.termina.point_y*t - 6*_y_2*(t - 1) - 3*_y_3*(2*t - 2) + 6*_y_1*(2*t - 2)))/(27*pow((pow((3*ControlPoints.termina.point_x*pow((t - 1),2)- 3*_x_3*pow((t - 1),2) - 6*ControlPoints.termina.point_x*t*(t - 1) + 3*ControlPoints.termina.point_x*t*(2*t - 2)),2.0)*0.11111 + pow((3*_y_2*t*t - 3*ControlPoints.termina.point_y*t*t + 3*_y_3*pow((t - 1),2) - 3*_y_1*pow((t - 1),2) + 6*_y_2*t*(t - 1) - 3*_y_1*t*(2*t - 2)),2.0)*0.111111),3.0/2));
	double Cur_2=abs((6*ControlPoints.termina.point_x*(t_temp - 1) - 6*ControlPoints.termina.point_x*(2*t_temp - 2) + 3*_x_3*(2*t_temp - 2))*(3*_y_2*t_temp*t_temp - 3*ControlPoints.termina.point_y*t_temp*t_temp + 3*_y_3*pow((t_temp - 1),2) - 3*_y_1*pow((t_temp - 1),2) + 6*_y_2*t_temp*(t_temp - 1) - 3*_y_1*t_temp*(2*t_temp - 2)) - (3*ControlPoints.termina.point_x*pow((t_temp - 1),2) - 3*_x_3*pow((t_temp - 1),2) - 6*ControlPoints.termina.point_x*t_temp*(t_temp - 1) + 3*ControlPoints.termina.point_x*t_temp*(2*t_temp - 2))*(6*_y_1*t_temp - 12*_y_2*t_temp + 6*ControlPoints.termina.point_y*t_temp - 6*_y_2*(t_temp - 1) - 3*_y_3*(2*t_temp - 2) + 6*_y_1*(2*t_temp - 2)))/(27*pow((pow((3*ControlPoints.termina.point_x*pow((t_temp - 1),2)- 3*_x_3*pow((t_temp - 1),2) - 6*ControlPoints.termina.point_x*t_temp*(t_temp - 1) + 3*ControlPoints.termina.point_x*t_temp*(2*t_temp - 2)),2.0)*0.11111 + pow((3*_y_2*t_temp*t_temp - 3*ControlPoints.termina.point_y*t_temp*t_temp + 3*_y_3*pow((t_temp - 1),2) - 3*_y_1*pow((t_temp - 1),2) + 6*_y_2*t_temp*(t_temp - 1) - 3*_y_1*t_temp*(2*t_temp - 2)),2.0)*0.111111),3.0/2));

	double Port_rate_b=abs(Cur_1-Cur_2)/CurDis;

	return Port_rate_b;
}

//  获取最大曲率值
double GenerateManager::GetMaxCur_a(CMatrix X)
{
	double MaxCur_a=0;
	double CurTemp_a=0;

	double _x_3=0.5*(X(1,0)+ControlPoints.termina.point_x);
	double _y_3=0.5*(X(1,0)+2*ControlPoints.origin.point_y-ControlPoints.termina.point_x);
	double _y_1=X(1,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;
	double _y_2=X(0,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;

	for (double i=0;i<100;i++)
	{
		double t=i/100;

		//MaxCur_a=abs((15*t - 6*_y_3*t)*(3*pow((t - 1),2) + 3*X(1,0)*t*t - 3*_x_3*t*t - 3*X(0,0)*pow((t - 1),2) + 6*X(1,0)*t*(t - 1) - 3*X(0,0)*t*(2*t - 2)) - ((15*t*(2*t - 2))*0.5 - 2*t*((15*t)*0.5 - 7.5) + 3*_y_3*t*t - (15*t*t)*0.5)*(6*X(0,0)*t - 6*t - 12*X(1,0)*t + 6*_x_3*t - 6*X(1,0)*(t - 1) + 6*X(0,0)*(2*t - 2) + 6))/pow((pow(((15*t*(2*t - 2))*0.5 - 2*t*((15*t)*0.5 - 7.5) + 3*_y_3*t*t - (15*t*t)*0.5),2) + pow((3*pow((t - 1),2) + 3*X(1,0)*t*t - 3*_x_3*t*t - 3*X(0,0)*pow((t - 1),2) + 6*X(1,0)*t*(t - 1) - 3*X(0,0)*t*(2*t - 2)),2)),3.0/2);
		MaxCur_a=abs((3*_y_3*t*t - 3*ControlPoints.origin.point_y*t*t - 6*ControlPoints.origin.point_y*t*(t - 1) + 3*ControlPoints.origin.point_y*t*(2*t - 2))*(6*X(0,0)*t - 12*X(1,0)*t + 6*_x_3*t - 6*X(1,0)*(t - 1) + 6*X(0,0)*(2*t - 2) - 3*ControlPoints.origin.point_x*(2*t - 2)) + (3*ControlPoints.origin.point_y*(2*t - 2) + 6*_y_3*t - 6*ControlPoints.origin.point_y*t - 6*ControlPoints.origin.point_y*(t - 1))*(3*X(1,0)*t*t - 3*_x_3*t*t - 3*X(0,0)*pow((t - 1),2) + 3*ControlPoints.origin.point_x*pow((t - 1),2) + 6*X(1,0)*t*(t - 1) - 3*X(0,0)*t*(2*t - 2)))/(27*pow((pow((3*X(1,0)*t*t - 3*_x_3*t*t - 3*X(0,0)*pow((t - 1),2) + 3*ControlPoints.origin.point_x*pow((t - 1),2) + 6*X(1,0)*t*(t - 1) - 3*X(0,0)*t*(2*t - 2)),2.0)*0.11111 + pow((3*_y_3*t*t - 3*ControlPoints.origin.point_y*t*t - 6*ControlPoints.origin.point_y*t*(t - 1) + 3*ControlPoints.origin.point_y*t*(2*t - 2)),2.0)*0.11111),3.0/2));
		if (CurTemp_a<MaxCur_a)
		{
			CurTemp_a=MaxCur_a;
		}
	}
	return CurTemp_a;
}

double GenerateManager::GetMaxCur_b(CMatrix X)
{
	double MaxCur_b=0;
	double CurTemp_b=0;

	double _x_3=0.5*(X(1,0)+ControlPoints.termina.point_x);
	double _y_3=0.5*(X(1,0)+2*ControlPoints.origin.point_y-ControlPoints.termina.point_x);
	double _y_1=X(1,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;
	double _y_2=X(0,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;

	for (double i=0;i<100;i++)
	{
		double t=i/100;
		//MaxCur_b=abs(((15*t*(2*t - 2))*0.5 - 2*t*((15*t)*0.5 - 7.5) + (15*pow((t - 1),2))*0.5 - 3*_x_3*pow((t - 1),2))*(6*t + 6*_y_1*t - 12*_y_2*t - 6*_y_2*(t - 1) - 3*_y_3*(2*t - 2) + 6*_y_1*(2*t - 2)) - (3*_x_3*(2*t - 2) - 15*t + 15)*(3*_y_2*t*t - 3*t*t + 3*_y_3*pow((t - 1),2) - 3*_y_1*pow((t - 1),2) + 6*_y_2*t*(t - 1) - 3*_y_1*t*(2*t - 2)))/pow((pow(((15*t*(2*t - 2))*0.5 - 2*t*((15*t)*0.5 - 7.5) + (15*pow((t - 1),2))*0.5 - 3*_x_3*pow((t - 1),2)),2) + pow((3*_y_2*t*t - 3*t*t + 3*_y_3*pow((t - 1),2) - 3*_y_1*pow((t - 1),2) + 6*_y_2*t*(t - 1) - 3*_y_1*t*(2*t - 2)),2)),3.0/2);
		MaxCur_b=abs((6*ControlPoints.termina.point_x*(t - 1) - 6*ControlPoints.termina.point_x*(2*t - 2) + 3*_x_3*(2*t - 2))*(3*_y_2*t*t - 3*ControlPoints.termina.point_y*t*t + 3*_y_3*pow((t - 1),2) - 3*_y_1*pow((t - 1),2) + 6*_y_2*t*(t - 1) - 3*_y_1*t*(2*t - 2)) - (3*ControlPoints.termina.point_x*pow((t - 1),2) - 3*_x_3*pow((t - 1),2) - 6*ControlPoints.termina.point_x*t*(t - 1) + 3*ControlPoints.termina.point_x*t*(2*t - 2))*(6*_y_1*t - 12*_y_2*t + 6*ControlPoints.termina.point_y*t - 6*_y_2*(t - 1) - 3*_y_3*(2*t - 2) + 6*_y_1*(2*t - 2)))/(27*pow((pow((3*ControlPoints.termina.point_x*pow((t - 1),2)- 3*_x_3*pow((t - 1),2) - 6*ControlPoints.termina.point_x*t*(t - 1) + 3*ControlPoints.termina.point_x*t*(2*t - 2)),2.0)*0.11111 + pow((3*_y_2*t*t - 3*ControlPoints.termina.point_y*t*t + 3*_y_3*pow((t - 1),2) - 3*_y_1*pow((t - 1),2) + 6*_y_2*t*(t - 1) - 3*_y_1*t*(2*t - 2)),2.0)*0.111111),3.0/2));
		if (CurTemp_b<MaxCur_b)
		{
			CurTemp_b=MaxCur_b;
		}
	}
	return CurTemp_b;
}
//  获取最大曲率变化率值
double GenerateManager::GetMaxRate_a(CMatrix X)
{
	double MaxRate_a=0;
	double MaxRate_temp=0;
	double t_temp=0;

	double _x_3=0.5*(X(1,0)+ControlPoints.termina.point_x);
	double _y_3=0.5*(X(1,0)+2*ControlPoints.origin.point_y-ControlPoints.termina.point_x);
	double _y_1=X(1,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;
	double _y_2=X(0,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;

	for (double i=0;i<100;i++)
	{
		double t=i/100;

		if (i>0)
		{
			double x_1=ControlPoints.origin.point_x*pow((1-t_temp),3)+X(0,0)*3*pow((1-t_temp),2)*t_temp+X(1,0)*3*(1-t_temp)*t_temp*t_temp+_x_3*pow(t_temp,3);
			double y_1=ControlPoints.origin.point_y*pow((1-t_temp),3)+ControlPoints.origin.point_y*3*pow((1-t_temp),2)*t_temp+ControlPoints.origin.point_y*3*(1-t_temp)*t_temp*t_temp+_y_3*pow(t_temp,3);

			double x_2=ControlPoints.origin.point_x*pow((1-t),3)+X(0,0)*3*pow((1-t),2)*t+X(1,0)*3*(1-t)*t*t+_x_3*pow(t,3);
			double y_2=ControlPoints.origin.point_y*pow((1-t),3)+ControlPoints.origin.point_y*3*pow((1-t),2)*t+ControlPoints.origin.point_y*3*(1-t)*t*t+_y_3*pow(t,3);

			double CurDis=sqrt(pow((x_2-x_1),2)+pow((y_2-y_1),2));

			double Cur_1=abs((3*_y_3*t*t - 3*ControlPoints.origin.point_y*t*t - 6*ControlPoints.origin.point_y*t*(t - 1) + 3*ControlPoints.origin.point_y*t*(2*t - 2))*(6*X(0,0)*t - 12*X(1,0)*t + 6*_x_3*t - 6*X(1,0)*(t - 1) + 6*X(0,0)*(2*t - 2) - 3*ControlPoints.origin.point_x*(2*t - 2)) + (3*ControlPoints.origin.point_y*(2*t - 2) + 6*_y_3*t - 6*ControlPoints.origin.point_y*t - 6*ControlPoints.origin.point_y*(t - 1))*(3*X(1,0)*t*t - 3*_x_3*t*t - 3*X(0,0)*pow((t - 1),2) + 3*ControlPoints.origin.point_x*pow((t - 1),2) + 6*X(1,0)*t*(t - 1) - 3*X(0,0)*t*(2*t - 2)))/(27*pow((pow((3*X(1,0)*t*t - 3*_x_3*t*t - 3*X(0,0)*pow((t - 1),2) + 3*ControlPoints.origin.point_x*pow((t - 1),2) + 6*X(1,0)*t*(t - 1) - 3*X(0,0)*t*(2*t - 2)),2.0)*0.11111 + pow((3*_y_3*t*t - 3*ControlPoints.origin.point_y*t*t - 6*ControlPoints.origin.point_y*t*(t - 1) + 3*ControlPoints.origin.point_y*t*(2*t - 2)),2.0)*0.11111),3.0/2));
			double Cur_2=abs((3*_y_3*t_temp*t_temp - 3*ControlPoints.origin.point_y*t_temp*t_temp - 6*ControlPoints.origin.point_y*t_temp*(t_temp - 1) + 3*ControlPoints.origin.point_y*t_temp*(2*t_temp - 2))*(6*X(0,0)*t_temp - 12*X(1,0)*t_temp + 6*_x_3*t_temp - 6*X(1,0)*(t_temp - 1) + 6*X(0,0)*(2*t_temp - 2) - 3*ControlPoints.origin.point_x*(2*t_temp - 2)) + (3*ControlPoints.origin.point_y*(2*t_temp - 2) + 6*_y_3*t_temp - 6*ControlPoints.origin.point_y*t_temp - 6*ControlPoints.origin.point_y*(t_temp - 1))*(3*X(1,0)*t_temp*t_temp - 3*_x_3*t_temp*t_temp - 3*X(0,0)*pow((t_temp - 1),2) + 3*ControlPoints.origin.point_x*pow((t_temp - 1),2) + 6*X(1,0)*t_temp*(t_temp - 1) - 3*X(0,0)*t_temp*(2*t_temp - 2)))/(27*pow((pow((3*X(1,0)*t_temp*t_temp - 3*_x_3*t_temp*t_temp - 3*X(0,0)*pow((t_temp - 1),2) + 3*ControlPoints.origin.point_x*pow((t_temp - 1),2) + 6*X(1,0)*t_temp*(t_temp - 1) - 3*X(0,0)*t_temp*(2*t_temp - 2)),2.0)*0.11111 + pow((3*_y_3*t_temp*t_temp - 3*ControlPoints.origin.point_y*t_temp*t_temp - 6*ControlPoints.origin.point_y*t_temp*(t_temp - 1) + 3*ControlPoints.origin.point_y*t_temp*(2*t_temp - 2)),2.0)*0.11111),3.0/2));

			MaxRate_a=abs(Cur_1-Cur_2)/CurDis;

			if (MaxRate_a>MaxRate_temp)
			{
				MaxRate_temp=MaxRate_a;
			}
		}
		t_temp=t;
	}
	return MaxRate_temp;
}

double GenerateManager::GetMaxRate_b(CMatrix X)
{
	double MaxRate_b=0;
	double MaxRate_temp=0;
	double t_temp=0;

	double _x_3=0.5*(X(1,0)+ControlPoints.termina.point_x);
	double _y_3=0.5*(X(1,0)+2*ControlPoints.origin.point_y-ControlPoints.termina.point_x);
	double _y_1=X(1,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;
	double _y_2=X(0,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;

	for (double i=0;i<100;i++)
	{
		double t=i/100;

		if (i>0)
		{
			double x_1=_x_3*pow((1-t_temp),3)+ControlPoints.termina.point_x*3*pow((1-t_temp),2)*t_temp+ControlPoints.termina.point_x*3*(1-t_temp)*t_temp*t_temp+ControlPoints.termina.point_x*pow(t_temp,3);
			double y_1=_y_3*pow((1-t_temp),3)+_y_1*3*pow((1-t_temp),2)*t_temp+_y_2*3*(1-t_temp)*t_temp*t_temp+ControlPoints.termina.point_y*pow(t_temp,3);

			double x_2=_x_3*pow((1-t),3)+ControlPoints.termina.point_x*3*pow((1-t),2)*t+ControlPoints.termina.point_x*3*(1-t)*t*t+ControlPoints.termina.point_x*pow(t,3);
			double y_2=_y_3*pow((1-t),3)+_y_1*3*pow((1-t),2)*t+_y_2*3*(1-t)*t*t+ControlPoints.termina.point_y*pow(t,3);

			double CurDis=sqrt(pow((x_2-x_1),2)+pow((y_2-y_1),2));

			double Cur_1=abs((6*ControlPoints.termina.point_x*(t - 1) - 6*ControlPoints.termina.point_x*(2*t - 2) + 3*_x_3*(2*t - 2))*(3*_y_2*t*t - 3*ControlPoints.termina.point_y*t*t + 3*_y_3*pow((t - 1),2) - 3*_y_1*pow((t - 1),2) + 6*_y_2*t*(t - 1) - 3*_y_1*t*(2*t - 2)) - (3*ControlPoints.termina.point_x*pow((t - 1),2) - 3*_x_3*pow((t - 1),2) - 6*ControlPoints.termina.point_x*t*(t - 1) + 3*ControlPoints.termina.point_x*t*(2*t - 2))*(6*_y_1*t - 12*_y_2*t + 6*ControlPoints.termina.point_y*t - 6*_y_2*(t - 1) - 3*_y_3*(2*t - 2) + 6*_y_1*(2*t - 2)))/(27*pow((pow((3*ControlPoints.termina.point_x*pow((t - 1),2)- 3*_x_3*pow((t - 1),2) - 6*ControlPoints.termina.point_x*t*(t - 1) + 3*ControlPoints.termina.point_x*t*(2*t - 2)),2.0)*0.11111 + pow((3*_y_2*t*t - 3*ControlPoints.termina.point_y*t*t + 3*_y_3*pow((t - 1),2) - 3*_y_1*pow((t - 1),2) + 6*_y_2*t*(t - 1) - 3*_y_1*t*(2*t - 2)),2.0)*0.111111),3.0/2));
			double Cur_2=abs((6*ControlPoints.termina.point_x*(t_temp - 1) - 6*ControlPoints.termina.point_x*(2*t_temp - 2) + 3*_x_3*(2*t_temp - 2))*(3*_y_2*t_temp*t_temp - 3*ControlPoints.termina.point_y*t_temp*t_temp + 3*_y_3*pow((t_temp - 1),2) - 3*_y_1*pow((t_temp - 1),2) + 6*_y_2*t_temp*(t_temp - 1) - 3*_y_1*t_temp*(2*t_temp - 2)) - (3*ControlPoints.termina.point_x*pow((t_temp - 1),2) - 3*_x_3*pow((t_temp - 1),2) - 6*ControlPoints.termina.point_x*t_temp*(t_temp - 1) + 3*ControlPoints.termina.point_x*t_temp*(2*t_temp - 2))*(6*_y_1*t_temp - 12*_y_2*t_temp + 6*ControlPoints.termina.point_y*t_temp - 6*_y_2*(t_temp - 1) - 3*_y_3*(2*t_temp - 2) + 6*_y_1*(2*t_temp - 2)))/(27*pow((pow((3*ControlPoints.termina.point_x*pow((t_temp - 1),2)- 3*_x_3*pow((t_temp - 1),2) - 6*ControlPoints.termina.point_x*t_temp*(t_temp - 1) + 3*ControlPoints.termina.point_x*t_temp*(2*t_temp - 2)),2.0)*0.11111 + pow((3*_y_2*t_temp*t_temp - 3*ControlPoints.termina.point_y*t_temp*t_temp + 3*_y_3*pow((t_temp - 1),2) - 3*_y_1*pow((t_temp - 1),2) + 6*_y_2*t_temp*(t_temp - 1) - 3*_y_1*t_temp*(2*t_temp - 2)),2.0)*0.111111),3.0/2));

			MaxRate_b=abs(Cur_1-Cur_2)/CurDis;

			if (MaxRate_b>MaxRate_temp)
			{
				MaxRate_temp=MaxRate_b;
			}
		}
		t_temp=t;
	}
	return MaxRate_temp;
}

double GenerateManager::Fscore( CMatrix X )
{
	double result=0;
	double Max_Cur_a=GetMaxCur_a(X);  // 最大曲率a
	double Max_Cur_b=GetMaxCur_b(X);  // 最大曲率b  

	double Max_Cur_Rat_a=GetMaxRate_a(X);// 最大曲率变化率a
	double Max_Cur_Rat_b=GetMaxRate_b(X);// 最大曲率变化率b

	double Port_Rate_a=Get_Port_Cur_a(X);// 连接处曲率变化率a
	double Port_Rate_b=Get_Port_Cur_b(X);// 连接处曲率变化率b

	double _x_3=0.5*(X(1,0)+ControlPoints.termina.point_x);
	double _y_3=0.5*(X(1,0)+2*ControlPoints.origin.point_y-ControlPoints.termina.point_x);
	double _y_1=X(1,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;
	double _y_2=X(0,0)+ControlPoints.origin.point_y-ControlPoints.termina.point_x;

	double Cur_1=(2*pow((pow(abs((X(0,0) - X(1,0))*(_y_3 - ControlPoints.origin.point_y)),2)),1.0/2))/(3*pow((pow(abs(_y_3 - ControlPoints.origin.point_y),2) + pow(abs(X(1,0) - _x_3),2)),3.0/2));
	double Cur_2=(2*pow((pow(abs((_y_1 - _y_2)*(_x_3 - ControlPoints.termina.point_x)),2)),1.0/2))/(3*pow((pow(abs(_x_3 - ControlPoints.termina.point_x),2) + pow(abs(_y_3 - _y_1),2)),3.0/2));

	double th_1=(3*_x_3-3*X(1,0))/sqrt(pow((3*_x_3-3*X(1,0)),2)+pow((3*_y_3-3*ControlPoints.origin.point_y),2));
	double th_2=(3*ControlPoints.termina.point_x-3*_x_3)/sqrt(pow((3*ControlPoints.termina.point_x-3*_x_3),2)+pow((3*_y_1-3*_y_3),2));  //tips 9.20 pow切勿使用分数!!!

	// result式即用于搜索算法的计算公式
	if (beziertype==1.5)  //0.6之间
	{
		result=0.2*abs(Cur_1-Cur_2)+abs(th_1-th_2)+0.02*abs(Max_Cur_Rat_b-Max_Cur_Rat_a)+0.003*Max_Cur_Rat_b+0.003*Max_Cur_Rat_a;
		result=result*1000;    //tips 0.5*0.5情况 实际测试中0.5*0.5间距较短 参数及其容易变形 这里是实际测试中效果较好的经验算式 注意0.5时result*100是不够的
	}

	if (beziertype==2)  //0.7-1之间
	{
		// result=0.3*abs(Cur_1-Cur_2)+0.9*abs(th_1-th_2)+0.05*abs(Max_Cur_Rat_b-Max_Cur_Rat_a)+0.001*Max_Cur_Rat_b+0.001*Max_Cur_Rat_a;
		result=0.3*abs(Cur_1-Cur_2)+0.8*abs(th_1-th_2)+0.02*abs(Max_Cur_Rat_b-Max_Cur_Rat_a)+0.003*Max_Cur_Rat_b+0.003*Max_Cur_Rat_a;
		result=result*1000;    //tips 0.5*0.5情况 实际测试中0.5*0.5间距较短 参数及其容易变形 这里是实际测试中效果较好的经验算式 注意0.5时result*100是不够的
	}

	if (beziertype==1)//   0.5一组  
	{
		result=0.3*abs(Cur_1-Cur_2)+0.8*abs(th_1-th_2)+0.006*Max_Cur_b+0.006*Max_Cur_a+0.05*abs(Max_Cur_Rat_b-Max_Cur_Rat_a);
		result=result*1000;
	}

	if(beziertype==3) //    1 一组
	{
		//result=0.3*abs(Cur_1-Cur_2)+0.8*abs(th_1-th_2)+0.05*Max_Cur_Rat_a+0.05*Max_Cur_Rat_b+0.1*(Max_Cur_b-Max_Cur_a);
		result=0.3*abs(Cur_1-Cur_2)+0.8*abs(th_1-th_2)+0.07*Max_Cur_Rat_a+0.07*Max_Cur_Rat_b+0.1*abs(Max_Cur_b-Max_Cur_a);//+0.01*Port_Rate_a+0.01*Port_Rate_b;//
		result=result*1000;   //实际测试了1*1以及1.5*1俩种情况 并且得到了良好效果 在这种范围的通用性有待验证
	}

	if(beziertype==4) //    >1 一组 1.1 1.2 1.3
	{
		result=0.3*abs(Cur_1-Cur_2)+0.8*abs(th_1-th_2)+0.09*Max_Cur_Rat_a+0.09*Max_Cur_Rat_b+0.1*abs(Max_Cur_b-Max_Cur_a)+0.01*Port_Rate_a+0.01*Port_Rate_b;
		result=result*100;   //实际测试了1*1以及1.5*1俩种情况 并且得到了良好效果 在这种范围的通用性有待验证
	}

	return result;
}

void GenerateManager::DataTran(double a_init[6])
{
	int _situation=param_situation.quadrant;

	a_init[2]=0.5*(a_init[1]+ControlPoints.termina.point_x);
	a_init[3]=0.5*(a_init[1]+2*ControlPoints.origin.point_y-ControlPoints.termina.point_x);
	a_init[4]=a_init[1]+ControlPoints.origin.point_y-ControlPoints.termina.point_x;
	a_init[5]=a_init[0]+ControlPoints.origin.point_y-ControlPoints.termina.point_x;

	double x_0=a_init[0];
	double x_1=a_init[1];
	double x_2=a_init[2];

	double y_3=a_init[3];
	double y_1=a_init[4];
	double y_2=a_init[5];

	switch(_situation)
	{
	case 1:
		a_init[3]=a_init[3]+y_q;
		a_init[4]=a_init[4]+y_q;
		a_init[5]=a_init[5]+y_q;

		a_init[0]=a_init[0]+x_m;
		a_init[1]=a_init[1]+x_m;
		a_init[2]=a_init[2]+x_m;
		break;
	case 2:
		a_init[3]=a_init[3]+y_q;
		a_init[4]=a_init[4]+y_q;
		a_init[5]=a_init[5]+y_q;

		a_init[0]=a_init[0]+x_m;
		a_init[1]=a_init[1]+x_m;
		a_init[2]=a_init[2]+x_m;

		a_init[0]=2*x_m-a_init[0];
		a_init[1]=2*x_m-a_init[1];
		a_init[2]=2*x_m-a_init[2];
		break;
	case 3:
		a_init[3]=a_init[3]+y_q;
		a_init[4]=a_init[4]+y_q;
		a_init[5]=a_init[5]+y_q;

		a_init[0]=a_init[0]+x_m;
		a_init[1]=a_init[1]+x_m;
		a_init[2]=a_init[2]+x_m;

		a_init[3]=2*y_q-a_init[3];
		a_init[4]=2*y_q-a_init[4];
		a_init[5]=2*y_q-a_init[5];

		a_init[0]=2*x_m-a_init[0];
		a_init[1]=2*x_m-a_init[1];
		a_init[2]=2*x_m-a_init[2];

		break;
	case 4:
		a_init[3]=a_init[3]+y_q;
		a_init[4]=a_init[4]+y_q;
		a_init[5]=a_init[5]+y_q;

		a_init[0]=a_init[0]+x_m;
		a_init[1]=a_init[1]+x_m;
		a_init[2]=a_init[2]+x_m;

		a_init[3]=2*y_q-a_init[3];
		a_init[4]=2*y_q-a_init[4];
		a_init[5]=2*y_q-a_init[5];

		break;
	default:
		break;
	}
}

void GenerateManager::DoOptimize(CMatrix Xinit,bool& val,CMatrix & Xresult,vector<ControlPoints_>& _ControlPoints_90)
{
	double data_out[14];
	val=false;
	Xresult.Init(N,1);    // N = 6
	//开始搜索算法
	//step1: 定义初始变量
	double delta,alfa,beta,uplon,prob_end;
	//delta=0.01; alfa=1.1; beta=0.95; uplon=1e-5;  1.03
	delta=0.01; alfa=1.03; beta=0.95; uplon=1e-5;
	double *datas=new double[N];
	double a_init[6];
	for(int i=0;i<N;i++){
		datas[i]=Xinit(i,0);   //  datas为初始值
	}

	CMatrix pos_now(N,1,datas);	
	CMatrix pos_last(N,1),E_axis_temp(N,1);
	CMatrix pos_end;
	CMatrix spos_axis[N+1],E_axis[N];
	CMatrix posaxis_inc(N,1),posaxis_dec(N,1);

	pos_last=pos_now;

	for (int i=0; i<N+1; i++)
	{
		spos_axis[i].Init(N,1);
		if (i<N)
			E_axis[i].Init(N,1);
	}

	for (int i=0;i<N;i++)
	{
		for (int j=0;j<N;j++)
		{
			datas[j]=0;
		}
		datas[i]=1;
		E_axis[i].SetData(datas);
	}

	//循环变量的初始化
	int k=0;
	int j=0;
	int num_cal=0; //计算得分的次数
	bool stop=false;
	spos_axis[0]=pos_now;
	double prob_now=0;

	while (1)
	{
		if(stop) break; 
		if (k>1000)
		{
			cout<<"1000次触发"<<endl;
			break;
		}		
		prob_now = Fscore(pos_now);
		num_cal++;   //计算得分次数

		while(1)
		{		
			E_axis_temp=E_axis[j]*delta;
			posaxis_inc=spos_axis[j]+E_axis_temp;   // 增加 
			posaxis_dec=spos_axis[j]-E_axis_temp;	// 减少 

			if (Fscore(posaxis_inc)<prob_now)
			{
				spos_axis[j+1]=posaxis_inc;
				num_cal++;
			}
			else
			{
				if (Fscore(posaxis_dec)<prob_now)
				{
					spos_axis[j+1]=posaxis_dec; num_cal++;
				}
				else
					spos_axis[j+1]=spos_axis[j];
			}

			if (j<N-1)
			{
				j++;
			}
			else
			{
				break;
			}
		}

		if (Fscore(spos_axis[N])<prob_now)
		{
			pos_last=pos_now;
			pos_now=spos_axis[N];
			spos_axis[0]=pos_now+(pos_now-pos_last)*alfa;
			j=0;	
			k++;
			num_cal++;
		}
		else
		{
			if (delta<uplon)
			{
				stop=true;
				pos_end=pos_now;
				prob_end=Fscore(pos_end);
				cout<<"最终得分 "<<prob_end<<"  最后步长: "<<delta<<endl;
			}
			else
			{
				delta=beta*delta;
				spos_axis[0]=pos_now;	
				pos_last=pos_now;
				j=0;
				k++;
			}
		}
	}

	if (k<1000)
	{
		val=true;
		Xresult=pos_end;			
		cout<<"优化的最终结果:  ";
		(pos_end.Transpose()).print(a_init);
	}	
	else
	{
		val=false;
	}
	DataTran(a_init);

	data_out[0]=OrignPoints.origin.point_x;data_out[1]=OrignPoints.origin.point_y;
	data_out[2]=a_init[0];data_out[3]=OrignPoints.origin.point_y;
	data_out[4]=a_init[1];data_out[5]=OrignPoints.origin.point_y;
	data_out[6]=a_init[2];data_out[7]=a_init[3];

	data_out[8]=OrignPoints.termina.point_x;data_out[9]=a_init[4];
	data_out[10]=OrignPoints.termina.point_x;data_out[11]=a_init[5];
 	data_out[12]=OrignPoints.termina.point_x;data_out[13]=OrignPoints.termina.point_y;

	ControlPoints_ ControlPoint_;
	for (int i =0;i<14;i++)
	{
		if (i%2==0)
		{
			ControlPoint_.cp_x=data_out[i];
		}else
		{
			ControlPoint_.cp_y=data_out[i];
			_ControlPoints_90.push_back(ControlPoint_);
		}
	}
}

void GenerateManager::CurveGenerate(double p1_x,double p1_y,double p1_angle,double p2_x,double p2_y,double p2_angle,vector<ControlPoints_>& _ControlPoints_90)
{
   PointParamete Point1st = CreatePoint(p1_x,p1_y,p1_angle);
   PointParamete Point2ed = CreatePoint(p2_x,p2_y,p2_angle);

   PointInit Points_ = ParameteInit(Point1st,Point2ed);

   PointParamete XParam_;

   switch(param_situation.length_grade)
   {//初始点的读取
   case 1:
	   XParam_=init_1(Points_);
	   break;
   case 2:
	   XParam_=init_2(Points_);
	   break;
   case 3:
	   XParam_=init_3(Points_);
	   break;
   default:
	   break;
   }
   
   CMatrix Xinit(2,1);
   Xinit.Init(2,1);
   Xinit(0,0)=XParam_.point_x;Xinit(1,0)=XParam_.point_y;
   CMatrix X_opti;
   bool val_opti;
   DoOptimize(Xinit,val_opti,X_opti,_ControlPoints_90);
}