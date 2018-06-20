#include "GenerateVariable180.h"


#define N 2
/* ****************贝塞尔控制点参数初始化******************** */
double GenerateManager180::PointRotation_(double m_,double n_)  // n是对称轴 m是待镜像坐标值
{
	m_=2*n_-m_;
	return m_;
}

int GenerateManager180::ParamQuadrant_(PointParamete FirstPoint,PointParamete SecondPoint)
{
	int quadrant = 0;
	if (FirstPoint.point_angle<45&&SecondPoint.point_angle>145)          // 0 180                                                // 第一象限 
	{
		quadrant=1;
	}
	if (FirstPoint.point_angle<145&&SecondPoint.point_angle>180)         // 90 270
	{ 
		quadrant=2;
	}
	if (FirstPoint.point_angle>145&&SecondPoint.point_angle<45)          // 180 0
	{ 
		quadrant=3;
	}
	if (FirstPoint.point_angle>180&&SecondPoint.point_angle<145)         // 270 90
	{ 
		quadrant=4;
	}
	return quadrant;
}

int GenerateManager180::ParamLengthgrade_(double _x_m,double _y_n,double _x_p,double _y_q)
{
	int data_mode = 1;
	double length = abs(_y_n-_y_q);
	x_m=_x_m;                 /////////////////////////////////
	y_q=_y_q;                 /////////////////////////////////
	return data_mode;
}

PointInit GenerateManager180::ParameteInit_(PointParamete FirstPoint,PointParamete SecondPoint)
{
	PointInit Points;

    PointParamete FirstPoint_;
    PointParamete SecondPoint_;
    PointParamete MiddlePoint;

	double b=0;

	param_situation.quadrant=ParamQuadrant_(FirstPoint,SecondPoint);
	switch(param_situation.quadrant)                                                       //先都给旋转到标准情况 即1象限
	{
	case 1:
        FirstPoint_=FirstPoint;
        SecondPoint_=SecondPoint;
		break;
	case 2:
		MiddlePoint.point_x=(FirstPoint.point_x+SecondPoint.point_x)*0.5;
		MiddlePoint.point_y=(FirstPoint.point_y+SecondPoint.point_y)*0.5;
		b=MiddlePoint.point_y-MiddlePoint.point_x;
        FirstPoint_.point_x = FirstPoint.point_y-b;
		FirstPoint_.point_y = FirstPoint.point_x+b;
		SecondPoint_.point_x = SecondPoint.point_y-b;
		SecondPoint_.point_y = SecondPoint.point_x+b;
		break;
    case 3:
		FirstPoint_=FirstPoint;
		SecondPoint_=SecondPoint;
		break;
	case 4:
		MiddlePoint.point_x=(FirstPoint.point_x+SecondPoint.point_x)*0.5;
		MiddlePoint.point_y=(FirstPoint.point_y+SecondPoint.point_y)*0.5;
		b=MiddlePoint.point_y-MiddlePoint.point_x;
		FirstPoint_.point_x = FirstPoint.point_y-b;
		FirstPoint_.point_y = FirstPoint.point_x+b;
		SecondPoint_.point_x = SecondPoint.point_y-b;
		SecondPoint_.point_y = SecondPoint.point_x+b;
		break;
	}
	param_situation.length_grade=ParamLengthgrade_(FirstPoint.point_x,FirstPoint.point_y,SecondPoint.point_x,SecondPoint.point_y);
	Points.origin=FirstPoint_;
	Points.termina=SecondPoint_;
	return Points;
} 

PointParamete GenerateManager180::CreatePoint_(double p_x,double p_y,double p_angle)
{
	PointParamete Point;
	Point.point_x = p_x;
	Point.point_y = p_y;
	Point.point_angle = p_angle;
	return Point;
}
//    ++0.5*0.5++
PointParamete GenerateManager180::init_1_(PointInit Points_)
{
	PointParamete XParam;
	ControlPoints.origin.point_x=0; //
	ControlPoints.origin.point_y=Points_.origin.point_y-Points_.termina.point_y;
	ControlPoints.termina.point_x=Points_.termina.point_x-Points_.origin.point_x;
	ControlPoints.termina.point_y=0;
	XParam.point_x=0.4*abs(Points_.origin.point_y-Points_.termina.point_y);             //对称情况下
	XParam.point_y=0.6*abs(Points_.origin.point_y-Points_.termina.point_y);

	beziertype=1;
	return XParam;
}
/* ****************贝塞尔控制点计算******************** */
inline double GenerateManager180::Get_Cur(double a,double b,double c,double d,double e,double f,double m,double n,double t)
{
   double Cur=abs((6*c*t - 12*e*t + 6*m*t - 6*e*(t - 1) - 3*a*(2*t - 2) + 6*c*(2*t - 2))*(3*f*pow(t,2) - 3*n*pow(t,2) + 3*b*pow((t - 1),2) - 3*d*pow((t - 1),2) + 6*f*t*(t - 1) - 3*d*t*(2*t - 2)) - (6*d*t - 12*f*t + 6*n*t - 6*f*(t - 1) - 3*b*(2*t - 2) + 6*d*(2*t - 2))*(3*e*pow(t,2) - 3*m*pow(t,2) + 3*a*pow((t - 1),2) - 3*c*pow((t - 1),2) + 6*e*t*(t - 1) - 3*c*t*(2*t - 2)))/(27*pow((pow((3*e*pow(t,2) - 3*m*pow(t,2) + 3*a*pow((t - 1),2) - 3*c*pow((t - 1),2) + 6*e*t*(t - 1) - 3*c*t*(2*t - 2)),2)*0.1111 + pow((3*f*pow(t,2) - 3*n*pow(t,2) + 3*b*pow((t - 1),2) - 3*d*pow((t - 1),2) + 6*f*t*(t - 1) - 3*d*t*(2*t - 2)),2)*0.1111),3.0/2));
   return Cur;
};
//  获取最大曲率值
double GenerateManager180::GetMaxCur_a_(CMatrix X)
{
	double MaxCur_a=0;
	double CurTemp_a=0;

	double a=ControlPoints.origin.point_x;
	double b=ControlPoints.origin.point_y;
	double c=X(0,0);
	double d=ControlPoints.origin.point_y;
	double e=X(1,0);
	double f=ControlPoints.origin.point_y;
	double m=X(1,0);
	double n=0.5*(ControlPoints.origin.point_y+ControlPoints.termina.point_y);

	for (double i=0;i<100;i++)
	{
		double t=i/100;
		MaxCur_a=Get_Cur(a,b,c,d,e,f,m,n,t);
		/*CurTemp_a+=MaxCur_a;*/
		if (CurTemp_a<MaxCur_a)
		{
			CurTemp_a=MaxCur_a;
		}
	}
	return CurTemp_a;
}

double GenerateManager180::GetMaxCur_b_(CMatrix X)
{
	double MaxCur_b=0;
	double CurTemp_b=0;

	double a=X(1,0);
	double b=0.5*(ControlPoints.origin.point_y+ControlPoints.termina.point_y);
	double c=X(1,0);
	double d=ControlPoints.termina.point_y;
	double e=X(0,0);
	double f=ControlPoints.termina.point_y;
	double m=ControlPoints.termina.point_x;
	double n=ControlPoints.termina.point_y;

	for (double i=0;i<100;i++)
	{
		double t=i/100;
		MaxCur_b=Get_Cur(a,b,c,d,e,f,m,n,t);
		if (CurTemp_b<MaxCur_b)
		{
			CurTemp_b=MaxCur_b;
		}
	}
	return CurTemp_b;
}
//  获取最大曲率变化率值
double GenerateManager180::GetMaxRate_a_(CMatrix X)
{
	double MaxRate_a=0;
	double MaxRate_temp=0;
	double t_temp=0;

	double a=ControlPoints.origin.point_x;
	double b=ControlPoints.origin.point_y;
	double c=X(0,0);
	double d=ControlPoints.origin.point_y;
	double e=X(1,0);
	double f=ControlPoints.origin.point_y;
	double m=X(1,0);
	double n=0.5*(ControlPoints.origin.point_y+ControlPoints.termina.point_y);

	for (double i=0;i<100;i++)
	{
		double t=i/100;

		if (i>0)
		{
			double x_1=a*pow((1-t_temp),3)+c*3*pow((1-t_temp),2)*t_temp+e*3*(1-t_temp)*t_temp*t_temp+m*pow(t_temp,3);
			double y_1=b*pow((1-t_temp),3)+d*3*pow((1-t_temp),2)*t_temp+f*3*(1-t_temp)*t_temp*t_temp+n*pow(t_temp,3);

			double x_2=a*pow((1-t),3)+c*3*pow((1-t),2)*t+e*3*(1-t)*t*t+m*pow(t,3);
			double y_2=b*pow((1-t),3)+d*3*pow((1-t),2)*t+f*3*(1-t)*t*t+n*pow(t,3);

			double CurDis=sqrt(pow((x_2-x_1),2)+pow((y_2-y_1),2));

			double Cur_1=Get_Cur(a,b,c,d,e,f,m,n,t); //t_temp
			double Cur_2=Get_Cur(a,b,c,d,e,f,m,n,t_temp);

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

double GenerateManager180::GetMaxRate_b_(CMatrix X)
{
	double MaxRate_b=0;
	double MaxRate_temp=0;
	double t_temp=0;

	double a=X(1,0);
	double b=0.5*(ControlPoints.origin.point_y+ControlPoints.termina.point_y);
	double c=X(1,0);
	double d=ControlPoints.termina.point_y;
	double e=X(0,0);
	double f=ControlPoints.termina.point_y;
	double m=ControlPoints.termina.point_x;
	double n=ControlPoints.termina.point_y;

	for (double i=0;i<100;i++)
	{
		double t=i/100;

		if (i>0)
		{
			double x_1=a*pow((1-t_temp),3)+c*3*pow((1-t_temp),2)*t_temp+e*3*(1-t_temp)*t_temp*t_temp+m*pow(t_temp,3);
			double y_1=b*pow((1-t_temp),3)+d*3*pow((1-t_temp),2)*t_temp+f*3*(1-t_temp)*t_temp*t_temp+n*pow(t_temp,3);

			double x_2=a*pow((1-t),3)+c*3*pow((1-t),2)*t+e*3*(1-t)*t*t+m*pow(t,3);
			double y_2=b*pow((1-t),3)+d*3*pow((1-t),2)*t+f*3*(1-t)*t*t+n*pow(t,3);

			double CurDis=sqrt(pow((x_2-x_1),2)+pow((y_2-y_1),2));

			double Cur_1=Get_Cur(a,b,c,d,e,f,m,n,t); //t_temp
			double Cur_2=Get_Cur(a,b,c,d,e,f,m,n,t_temp);

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

double GenerateManager180::Fscore_( CMatrix X )
{
	double result=0;
	double Max_Cur_a=GetMaxCur_a_(X);  // 最大曲率a
	double Max_Cur_Rat_a=GetMaxRate_a_(X);// 最大曲率变化率a
	result=0.5*Max_Cur_a+0.5*Max_Cur_Rat_a+0.1*X(1,0);
	result=result*100;

	return result;
}

void GenerateManager180::DataOut(double a_init[2],vector<ControlPoints_>& _ControlPoints_)
{
   int _situation_=param_situation.quadrant;  // 象限情况
   double data_out[14];
   switch (_situation_)
   {
   case 1:
         data_out[0] = OrignPoints.origin.point_x;
		 data_out[1] = OrignPoints.origin.point_y;
		 data_out[2] = a_init[0];
		 data_out[3] = OrignPoints.origin.point_y;
		 data_out[4] = a_init[1];
		 data_out[5] = OrignPoints.origin.point_y;
		 data_out[6] = a_init[1];
		 data_out[7] = 0.5*(OrignPoints.origin.point_y+OrignPoints.termina.point_y);
		 data_out[8] = a_init[1];
		 data_out[9] = OrignPoints.termina.point_y;
		 data_out[10] = a_init[0];
		 data_out[11] = OrignPoints.termina.point_y;
		 data_out[12] = OrignPoints.termina.point_x;
		 data_out[13] = OrignPoints.termina.point_y;
   	     break;
   case 2:
	     data_out[0] = OrignPoints.origin.point_x;
	     data_out[1] = OrignPoints.origin.point_y;
	     data_out[2] = OrignPoints.origin.point_x;
	     data_out[3] = a_init[0];
	     data_out[4] = OrignPoints.origin.point_x;
	     data_out[5] = a_init[1];
	     data_out[6] = 0.5*(OrignPoints.origin.point_x+OrignPoints.termina.point_x);
	     data_out[7] = a_init[1];
	     data_out[8] = OrignPoints.termina.point_x;
	     data_out[9] = a_init[1];
	     data_out[10] = OrignPoints.termina.point_x;
	     data_out[11] = a_init[0];
	     data_out[12] = OrignPoints.termina.point_x;
	     data_out[13] = OrignPoints.termina.point_y;
		 break;
   case 3:
	    data_out[0] = OrignPoints.origin.point_x;
	    data_out[1] = OrignPoints.origin.point_y;
	    data_out[2] = a_init[0];
	    data_out[3] = OrignPoints.origin.point_y;
	    data_out[4] = a_init[1];
	    data_out[5] = OrignPoints.origin.point_y;
	    data_out[6] = a_init[1];
	    data_out[7] = 0.5*(OrignPoints.origin.point_y+OrignPoints.termina.point_y);
	    data_out[8] = a_init[1];
	    data_out[9] = OrignPoints.termina.point_y;
	    data_out[10] = a_init[0];
	    data_out[11] = OrignPoints.termina.point_y;
	    data_out[12] = OrignPoints.termina.point_x;
	    data_out[13] = OrignPoints.termina.point_y;
		break;
   case 4:
	    data_out[0] = OrignPoints.origin.point_x;
	    data_out[1] = OrignPoints.origin.point_y;
	    data_out[2] = OrignPoints.origin.point_x;
	    data_out[3] = a_init[0];
	    data_out[4] = OrignPoints.origin.point_x;
	    data_out[5] = a_init[1];
	    data_out[6] = 0.5*(OrignPoints.origin.point_x+OrignPoints.termina.point_x);
	    data_out[7] = a_init[1];
	    data_out[8] = OrignPoints.termina.point_x;
	    data_out[9] = a_init[1];
	    data_out[10] = OrignPoints.termina.point_x;
	    data_out[11] = a_init[0];
	    data_out[12] = OrignPoints.termina.point_x;
	    data_out[13] = OrignPoints.termina.point_y;
	    break;
   }
   /*cout<<data_out[0]<<" "<<data_out[1]<<" "<<data_out[2]<<" "<<data_out[3]<<" "<<data_out[4]<<" "<<data_out[5]<<" "<<data_out[6]<<" "<<data_out[7]<<" "<<data_out[8]<<" "<<data_out[9]<<" "<<data_out[10]<<" "<<data_out[11]<<" "<<data_out[12]<<" "<<data_out[13]<<endl;*/
   ControlPoints_ ControlPoint_;
   for (int i =0;i<14;i++)
   {
	  if (i%2==0)
	  {
		  ControlPoint_.cp_x=data_out[i];
	  }else
	  {
          ControlPoint_.cp_y=data_out[i];
		  _ControlPoints_.push_back(ControlPoint_);
	  }
   }
}

void GenerateManager180::DataTran_(double a_init[2])
{
	int _situation=param_situation.quadrant;  // 象限情况

	double b_ = 0;
	double middle_x = 0.5*(_Points_.origin.point_x+_Points_.termina.point_x); // 中间点
	double middle_y = 0.5*(_Points_.origin.point_y+_Points_.termina.point_y); 
     
	switch (_situation)
    {
    case 1:
		a_init[0]=middle_x+a_init[0];
		a_init[1]=middle_x+a_init[1];
		break;
	case 2:
		a_init[0]=middle_y+a_init[0];
		a_init[1]=middle_y+a_init[1];
        break;
	case 3:
        a_init[0]=middle_x-a_init[0];
		a_init[1]=middle_x-a_init[1];
		break;
	case 4:
		a_init[0]=middle_y-a_init[0];
		a_init[1]=middle_y-a_init[1];
		break;
	default:
		break;
    }
}

void GenerateManager180::DoOptimize_(CMatrix Xinit,bool& val,CMatrix & Xresult,vector<ControlPoints_>& _ControlPoints_)
{
	val=false;
	Xresult.Init(N,1);    // N = 6
	vector<ControlPoints_> _ControlPoints;
	//开始搜索算法
	//step1: 定义初始变量
	double delta,alfa,beta,uplon,prob_end;
	//delta=0.01; alfa=1.1; beta=0.95; uplon=1e-5;  1.03
	delta=0.1; alfa=1.3; beta=0.98; uplon=1e-3;
	double *datas=new double[N];
	double a_init[2];
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
		if (k>2000)
		{
			cout<<"1000次触发"<<endl;
			break;
		}		
		prob_now = Fscore_(pos_now);
		num_cal++;   //计算得分次数

		while(1)
		{		
			E_axis_temp=E_axis[j]*delta;
			posaxis_inc=spos_axis[j]+E_axis_temp;   // 增加 
			posaxis_dec=spos_axis[j]-E_axis_temp;	// 减少 

			if (Fscore_(posaxis_inc)<prob_now)
			{
				spos_axis[j+1]=posaxis_inc;
				num_cal++;
			}
			else
			{
				if (Fscore_(posaxis_dec)<prob_now)
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

		if (Fscore_(spos_axis[N])<prob_now)
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
				prob_end=Fscore_(pos_end);
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
        DataTran_(a_init);
        DataOut(a_init,_ControlPoints_);
	}	
	else
	{
		val=false;
	}
	cout<<a_init[0]<<" "<<a_init[1]<<endl;
	cout<<"搜索步数"<<k<<endl;
	cout<<"计算次数"<<num_cal<<endl;	
}

void GenerateManager180::CurveGenerate(double p1_x,double p1_y,double p1_angle,double p2_x,double p2_y,double p2_angle,vector<ControlPoints_>& _ControlPoints_)
{
	PointParamete Point1st = CreatePoint_(p1_x,p1_y,p1_angle);
	PointParamete Point2ed = CreatePoint_(p2_x,p2_y,p2_angle);

	OrignPoints.origin=Point1st;
	OrignPoints.termina=Point2ed;

	_Points_ = ParameteInit_(Point1st,Point2ed);
	PointParamete XParam_;

	switch(param_situation.length_grade)
	{//初始点的读取
	case 1:
		XParam_=init_1_(_Points_);
		break;
	default:
		break;
	}
	CMatrix Xinit(2,1);
	Xinit.Init(2,1);
	Xinit(0,0)=XParam_.point_x;Xinit(1,0)=XParam_.point_y;
	CMatrix X_opti;
	bool val_opti;
	DoOptimize_(Xinit,val_opti,X_opti,_ControlPoints_);
}