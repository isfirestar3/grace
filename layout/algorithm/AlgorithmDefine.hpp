#ifndef ALGORITHM_DEFINE_ZHH_20161124
#define ALGORITHM_DEFINE_ZHH_20161124

#include <QtCore/qpoint.h>
#include "map.h"
#include "BasicDefine.h"

class DataConvert
{
public:
	static double map2world(double loc)
	{
		return loc / 100;
	};
	static double world2map(double world)
	{
		return world * 100;
	};
	static double angle2radian(double angle)
	{
		return angle * 3.14159 / 180;
	}
	static double radian2angle(double radian)
	{
		return radian * 180 / 3.14159;
	}
};
class GeometryCompute
{
public:
	static double CalDisTowPoint(QPointF &pot1, QPointF &pot2) 
	{
		double distance = sqrt((pot1.x() - pot2.x()) * (pot1.x() - pot2.x()) + (pot1.y()
			- pot2.y()) * (pot1.y() - pot2.y()));
		return distance;
	};

};

typedef struct NearPoint
{
	QPointF  point_near;
	double percent_near = 0.0;
	double min_distance = 0.0;
	NearPoint& operator = (const NearPoint& other)
	{
		if (this != &other)
		{
			point_near = other.point_near;
			percent_near = other.percent_near;
			min_distance = other.min_distance;
		}
		return *this;
	}
}NEARPOINT;

typedef struct Upl
{
	int edge_id = 0;
	int wop_id = 0;
	double percent = 0.0;
	double aoa = 0.0; //¹¥½Ç
	Upl& operator = (const Upl& other)
	{
		if (this != &other)
		{
			edge_id = other.edge_id;
			wop_id = other.wop_id;
			percent = other.percent;
			aoa = other.aoa;
		}
		return *this;
	}
}UPL;

class PT_C
{
public:
	static position_t PT(const POSPOINT& point)
	{
		position_t pos;
		pos.x_ = point.x;
		pos.y_ = point.y;

		return pos;
	}

	static void ED(const EDGEINFO& edgeInfo,var__edge_t& edge_t)
	{
		edge_t.id_ = edgeInfo.id;
		edge_t.start_node_id_ = edgeInfo.start_node_id;
		edge_t.end_node_id_ = edgeInfo.end_node_id;
		edge_t.control_point_1_ = PT(edgeInfo.ctrl_point1);
		edge_t.control_point_2_ = PT(edgeInfo.ctrl_point2);
		edge_t.start_point_ = PT(edgeInfo.termstpos);
		edge_t.end_point_ = PT(edgeInfo.termendpos);
		edge_t.length_ = edgeInfo.length;

		edge_t.disperse_points_.count_ = edgeInfo.disperse_point.size();
		if (edge_t.disperse_points_.count_ > 0){
			edge_t.disperse_points_.data_ = new position_t[edge_t.disperse_points_.count_];
			memcpy(edge_t.disperse_points_.data_, &edgeInfo.disperse_point[0], sizeof(position_t)* edge_t.disperse_points_.count_);
		}
	}
	static var__way_of_pass_t WP(const WOP& wop)
	{
		var__way_of_pass_t way_of_pass;
		way_of_pass.id_ = wop.id;
		way_of_pass.direction_ = wop.direction;
		way_of_pass.angle_type_ = wop.angle_type;
		way_of_pass.angle_ = wop.angle;

		return way_of_pass;
	}
};



#endif