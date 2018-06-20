#ifndef GLOBAL_VARIABLES_ZHH_20170112_
#define GLOBAL_VARIABLES_ZHH_20170112_

struct PointParamete
{
	double point_x = 0.0;
	double point_y = 0.0;
	double point_angle = 0.0;
};

struct ParameteSituation
{
	int quadrant = 0;
	int length_grade = 0;
};

struct PointInit
{
	PointParamete origin;
	PointParamete termina;
};

struct ControlPoints_
{
	double cp_x = 0.0;
	double cp_y = 0.0;
	ControlPoints_& operator = (const ControlPoints_& other)
	{
		if (this != &other)
		{
			cp_x = other.cp_x;
			cp_y = other.cp_y;
		}
		return *this;
	}
};
#endif