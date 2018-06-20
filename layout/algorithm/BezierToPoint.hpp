#ifndef BEZIER_TO_POINT_PANG_20161128_H_
#define BEZIER_TO_POINT_PANG_20161128_H_

#include <math.h>

typedef struct _POSITION {
	double x_;
	double y_;
	union {
		double theta_;
		double angle_;
		double w_;
		int distance_;
	};
}position_t, velocity_t;

typedef struct {
	position_t pos_ang_; //该点的位置和角度
	double curve_; //该点的曲率
	double dist_start_; //该点距离起点的长度，mm	
}track__pos_info_t;

double Getdis_bezeil(double _t, double x[9])
{
	double t_temp = 0;
	double sum_dis = 0;
	for (int i = 0; i<_t; i++)
	{
		if (i>0)
		{
			double t = i*0.01;
			double x_1 = x[0] * pow((1 - t), 3) + x[2] * 3 * pow((1 - t), 2)*t + x[4] * 3 * (1 - t)*pow(t, 2) + x[6] * pow(t, 3);// x
			double y_1 = x[1] * pow((1 - t), 3) + x[3] * 3 * pow((1 - t), 2)*t + x[5] * 3 * (1 - t)*pow(t, 2) + x[7] * pow(t, 3);// y 
			double x_2 = x[0] * pow((1 - t_temp), 3) + x[2] * 3 * pow((1 - t_temp), 2)*t_temp + x[4] * 3 * (1 - t_temp)*pow(t_temp, 2) + x[6] * pow(t_temp, 3);// x
			double y_2 = x[1] * pow((1 - t_temp), 3) + x[3] * 3 * pow((1 - t_temp), 2)*t_temp + x[5] * 3 * (1 - t_temp)*pow(t_temp, 2) + x[7] * pow(t_temp, 3);// y 
			sum_dis = sum_dis + sqrt(pow((x_2 - x_1), 2) + pow((y_2 - y_1), 2));
		}
		t_temp = i*0.01;
	}
	return sum_dis;
}

track__pos_info_t GetPosParabyt(position_t sp, position_t cp1, position_t cp2, position_t ep, double t){
	double x[9];
	x[0] = sp.x_; x[1] = sp.y_;
	x[2] = cp1.x_; x[3] = cp1.y_;
	x[4] = cp2.x_; x[5] = cp2.y_;
	x[6] = ep.x_; x[7] = ep.y_; x[8] = t;
	double Result[5] = { 0.0 };
	{
		// x,y
		Result[0] = x[0] * pow((1 - x[8]), 3) + x[2] * 3 * pow((1 - x[8]), 2)*x[8] + x[4] * 3 * (1 - x[8])*pow(x[8], 2) + x[6] * pow(x[8], 3); // x
		Result[1] = x[1] * pow((1 - x[8]), 3) + x[3] * 3 * pow((1 - x[8]), 2)*x[8] + x[5] * 3 * (1 - x[8])*pow(x[8], 2) + x[7] * pow(x[8], 3); // y
		// angle		
		double dif_x = 3 * x[6] * pow(x[8], 2) - 3 * x[4] * pow(x[8], 2) - 3 * x[0] * pow((x[8] - 1), 2) + 3 * x[2] * pow((x[8] - 1), 2) - 6 * x[4] * x[8] * (x[8] - 1) + 3 * x[2] * x[8] * (2 * x[8] - 2);
		double dif_y = 3 * x[7] * pow(x[8], 2) - 3 * x[5] * pow(x[8], 2) - 3 * x[1] * pow((x[8] - 1), 2) + 3 * x[3] * pow((x[8] - 1), 2) - 6 * x[5] * x[8] * (x[8] - 1) + 3 * x[3] * x[8] * (2 * x[8] - 2);
		Result[2] = atan2(dif_y, dif_x);
		// Cur
		Result[3] = ((6 * x[2] * x[8] - 12 * x[4] * x[8] + 6 * x[6] * x[8] - 6 * x[4] * (x[8] - 1) - 3 * x[0] * (2 * x[8] - 2) + 6 * x[2] * (2 * x[8] - 2))*(3 * x[5] * pow(x[8], 2) - 3 * x[7] * pow(x[8], 2) + 3 * x[1] * pow((x[8] - 1), 2) - 3 * x[3] * pow((x[8] - 1), 2) + 6 * x[5] * x[8] * (x[8] - 1) - 3 * x[3] * x[8] * (2 * x[8] - 2)) - (6 * x[3] * x[8] - 12 * x[5] * x[8] + 6 * x[7] * x[8] - 6 * x[5] * (x[8] - 1) - 3 * x[1] * (2 * x[8] - 2) + 6 * x[3] * (2 * x[8] - 2))*(3 * x[4] * pow(x[8], 2) - 3 * x[6] * pow(x[8], 2) + 3 * x[0] * pow((x[8] - 1), 2) - 3 * x[2] * pow((x[8] - 1), 2) + 6 * x[4] * x[8] * (x[8] - 1) - 3 * x[2] * x[8] * (2 * x[8] - 2))) / (27 * pow((pow((3 * x[4] * pow(x[8], 2) - 3 * x[6] * pow(x[8], 2) + 3 * x[0] * pow((x[8] - 1), 2) - 3 * x[2] * pow((x[8] - 1), 2) + 6 * x[4] * x[8] * (x[8] - 1) - 3 * x[2] * x[8] * (2 * x[8] - 2)), 2)*0.1111 + pow((3 * x[5] * pow(x[8], 2) - 3 * x[7] * pow(x[8], 2) + 3 * x[1] * pow((x[8] - 1), 2) - 3 * x[3] * pow((x[8] - 1), 2) + 6 * x[5] * x[8] * (x[8] - 1) - 3 * x[3] * x[8] * (2 * x[8] - 2)), 2)*0.1111), (3.0 / 2)));
		
		// dis
		double _t = x[8] * 100;
		Result[4] = Getdis_bezeil(_t, x);
	}
	track__pos_info_t pos_result;
	pos_result.pos_ang_.x_ = Result[0];
	pos_result.pos_ang_.y_ = Result[1];
	pos_result.pos_ang_.angle_ = Result[2];
	pos_result.curve_ = Result[3];
	pos_result.dist_start_ = Result[4];
	return pos_result;
}


#endif