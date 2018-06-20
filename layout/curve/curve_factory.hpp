#pragma once
#include "curve_product.hpp"
#include "GenerateVariable.h"
#include <memory>

#define CURVE_TYPE_90 0
#define CURVE_TYPE_180 1

class curve_factory
{
public:
	curve_factory(int type)
	{
		if (type == CURVE_TYPE_90)
		{
			curve_product_ptr = std::make_shared<GenerateManager>();
		}
		else if (type == CURVE_TYPE_180)
		{
			curve_product_ptr = std::make_shared<GenerateManager180>();
		}
	}
	~curve_factory()
	{
	}
public:
	void product_curve(const PointParamete& point_start, const PointParamete& point_end, std::vector<ControlPoints_>& ctrl_point)
	{
		if (curve_product_ptr)
		{
			curve_product_ptr->CurveGenerate(point_start.point_x, point_start.point_y, point_start.point_angle, 
				point_end.point_x, point_end.point_y, point_end.point_angle, ctrl_point);
		}
	}
private:
	std::shared_ptr<curve_product> curve_product_ptr = nullptr;
};