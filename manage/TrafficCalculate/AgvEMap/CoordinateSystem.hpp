#pragma once

#include <QtCore/qpoint.h>

class CoordinateSystem
{
public:
	static double Map2Scene(double pos) {
		return pos * resolution_;
	}

	static double Scene2Map(double pos) {
		return pos / resolution_;
	}

	static QPointF Map2Scene(QPointF point) {
		return point * resolution_;
	}

	static QPointF Scene2Map(QPointF point) {
		return point / resolution_;
	}

private:
	static const int resolution_ = 500;
};
