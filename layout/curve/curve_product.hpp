#pragma once
#include "GlobalVariables.h"
#include <vector>

class curve_product
{
public:
	curve_product()
	{

	}
	virtual ~curve_product()
	{

	}
public:
	virtual void CurveGenerate(double p1_x, double p1_y, double p1_angle, 
		double p2_x, double p2_y, double p2_angle, std::vector<ControlPoints_>& ctrl_point) = 0;
};