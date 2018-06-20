#include "PosandTrack.h"
#include "var.h"
#include <math.h>

static
double Getdistsqure( const position_t *p1, const position_t *p2 ) { return ( p2->x_ - p1->x_ )*( p2->x_ - p1->x_ ) + ( p2->y_ - p1->y_ )*( p2->y_ - p1->y_ ); }
static
double GetdistbyPos( const position_t *p1, const position_t *p2 ) { return sqrt( Getdistsqure( p1, p2 ) ); }

int Sgn( double x ) { return ( x >= 0 ? 1 : -1 ); }

double Getdis_bezeil( double _t, double x[9] ) {
	double t_temp = 0;
	double sum_dis = 0;
	int i = 0;
	for ( i = 0; i <= _t; i++ ) {
		if ( i > 0 ) {
			double t = i*0.001;
			double x_1 = x[0] * pow( ( 1 - t ), 3 ) + x[2] * 3 * pow( ( 1 - t ), 2 )*t + x[4] * 3 * ( 1 - t )*pow( t, 2 ) + x[6] * pow( t, 3 );// x
			double y_1 = x[1] * pow( ( 1 - t ), 3 ) + x[3] * 3 * pow( ( 1 - t ), 2 )*t + x[5] * 3 * ( 1 - t )*pow( t, 2 ) + x[7] * pow( t, 3 );// y 
			double x_2 = x[0] * pow( ( 1 - t_temp ), 3 ) + x[2] * 3 * pow( ( 1 - t_temp ), 2 )*t_temp + x[4] * 3 * ( 1 - t_temp )*pow( t_temp, 2 ) + x[6] * pow( t_temp, 3 );// x
			double y_2 = x[1] * pow( ( 1 - t_temp ), 3 ) + x[3] * 3 * pow( ( 1 - t_temp ), 2 )*t_temp + x[5] * 3 * ( 1 - t_temp )*pow( t_temp, 2 ) + x[7] * pow( t_temp, 3 );// y 
			sum_dis = sum_dis + sqrt( pow( ( x_2 - x_1 ), 2 ) + pow( ( y_2 - y_1 ), 2 ) );
		}
		t_temp = i*0.001;
	}
	return sum_dis;
}


int GetTClosestOnLineBycurg( position_t current_pos, const var__edge_t * curgraph, double *output ) {
	double t_result;
	position_t* disperse_points;
	int index_mindist_sq;
	double thisdist_sq;
	double min_dist_sq;
	double ts, te, tm;//ts:迭代中的起点，te：迭代中的终止点 tm：迭代的中点
	double space_t = 0.005;
	int i;
	double eplon = 0.0001;//如果分开的二边距离差小于该值，则停止迭代
	double dts, dte, dtm;
	position_t pos;

	if ( !curgraph || !output ) {
		return -1;
	}

	disperse_points = ( position_t* ) curgraph->disperse_points_.data_; //从地图中获取离散点
	if ( !disperse_points || curgraph->disperse_points_.count_ != 201 ) {
		return -1;
	}

	//在201点中找到最近的点,带sq的表示距离平方,这201个点的遍历是少不了的
	index_mindist_sq = 0;
	thisdist_sq = 0;
	min_dist_sq = Getdistsqure( &disperse_points[index_mindist_sq], &current_pos );
	for ( i = 1; i < 201; i++ ) {
		thisdist_sq = Getdistsqure( &disperse_points[i], &current_pos );
		if ( thisdist_sq < min_dist_sq ) {
			min_dist_sq = thisdist_sq;
			index_mindist_sq = i;
		}
	}
	//用二分法找到真正最近的点		
	if ( index_mindist_sq == 0 ) {
		ts = 0;
		te = ( index_mindist_sq + 1 ) * space_t;
	} else if ( index_mindist_sq == 199 ) {
		ts = ( index_mindist_sq - 1 )*space_t;
		te = index_mindist_sq*space_t;
	} else {
		ts = ( index_mindist_sq - 1 )*space_t;
		te = ( index_mindist_sq + 1 )*space_t;
	}

	for ( int i = 0;; i++ ) {
		if ( GetposbyT( ts, curgraph, &pos ) >= 0 ) {
			dts = GetdistbyPos( &current_pos, &pos );
		}
		if ( GetposbyT( te, curgraph, &pos ) >= 0 ) {
			dte = GetdistbyPos( &current_pos, &pos );
		}
		if ( fabs( dts - dte ) < eplon || i > 10 ) {
			t_result = ( ts + te ) / 2.0;
			break;
		} else {
			tm = ( ts + te ) / 2.0;
			if ( GetposbyT( tm, curgraph, &pos ) >= 0 ) {
				dtm = GetdistbyPos( &current_pos, &pos );
			}
			if ( fabs( dts - dtm ) < fabs( dte - dtm ) ) {
				te = tm;
			} else {
				ts = tm;
			}
		}
	}

	*output = t_result;
	return 0;
}

int GetTnewbyDistandTcur( double t_cur, double dist, const var__edge_t * curgraph, double *output ) {
	double dist_cur;
	double dist_new;
	int retval;

	if ( !curgraph || !output ) return -1;

	retval = GetdistbyT( t_cur, curgraph, &dist_cur );
	if ( retval >= 0 ) {
		dist_new = dist_cur + dist;
		return GetTbydist( dist_new, curgraph, output );
	}
	return -1;
}

int GetTbydist( double dist, const var__edge_t * curgraph, double *output ) {
	position_t* disperse_points;
	int is = 0;
	int ie = 200;
	int im = 0;
	double ts, te;

	if ( !curgraph || !output ) return -1;

	if ( dist <= 0 ) {
		*output = 0;
		return 0;
	} 
	
	disperse_points = ( position_t* ) curgraph->disperse_points_.data_;
	if ( !disperse_points || curgraph->disperse_points_.count_ != 201) {
		return -1;
	}

	if ( dist >= disperse_points[200].distance_ ) {
		*output = 1.0;
		return 0;
	}

	for ( int i = 0;; i++ ) {
		if ( is - ie >= -1 ) {
			break;
		}
		im = ( is + ie ) / 2;
		if ( dist <= disperse_points[im].distance_ ) {
			ie = im;
		} else {
			is = im;
		}
	}
	ts = is / 200.0;
	te = ie / 200.0;

	//求加权平均
	*output = ( ts*( disperse_points[ie].distance_ - dist ) + te*( dist - disperse_points[is].distance_ ) )
		/ ( disperse_points[ie].distance_ - disperse_points[is].distance_ );
	return 0;
}

int GetdistbyT( double t, const var__edge_t * curgraph, double *output ) {
	double dist_result;
	position_t* disperse_points;
	int is, ie;

	if ( !output || !curgraph ) {
		return -1;
	}

	disperse_points = ( position_t* ) curgraph->disperse_points_.data_;
	if ( !disperse_points ||  curgraph->disperse_points_.count_ != 201) {
		return -1;
	}

	if ( t <= 0 ) {
		*output = 0;
		return 0;
	} else if ( t >= 1 ) {
		*output = disperse_points[200].distance_;
		return 0;
	}

	is = ( int ) ( t * 200.0 );
	if ( is >= 200 ) {
		*output = disperse_points[200].distance_;
		return 0;
	}

	ie = is + 1;
	dist_result = ( ( ie*0.005 - t )*disperse_points[is].distance_ + ( t - is*0.005 )*disperse_points[ie].distance_ )*200.0;
	*output = dist_result;
	return 0;
}

int GetposbyT( double t, const var__edge_t * curgraph, position_t *pos ) {
	double x[9], dif_x, dif_y;

	if ( !curgraph || !pos ) return -1;

	x[0] = curgraph->start_point_.x_;     x[1] = curgraph->start_point_.y_;
	x[2] = curgraph->control_point_1_.x_; x[3] = curgraph->control_point_1_.y_;
	x[4] = curgraph->control_point_2_.x_; x[5] = curgraph->control_point_2_.y_;
	x[6] = curgraph->end_point_.x_;       x[7] = curgraph->end_point_.y_;
	x[8] = t;
	pos->x_ = x[0] * pow( ( 1 - x[8] ), 3 ) + x[2] * 3 * pow( ( 1 - x[8] ), 2 )*x[8] + x[4] * 3 * ( 1 - x[8] )*pow( x[8], 2 ) + x[6] * pow( x[8], 3 ); // x
	pos->y_ = x[1] * pow( ( 1 - x[8] ), 3 ) + x[3] * 3 * pow( ( 1 - x[8] ), 2 )*x[8] + x[5] * 3 * ( 1 - x[8] )*pow( x[8], 2 ) + x[7] * pow( x[8], 3 ); // y
	dif_x = 3 * x[6] * pow( x[8], 2 ) - 3 * x[4] * pow( x[8], 2 ) - 3 * x[0] * pow( ( x[8] - 1 ), 2 ) + 3 * x[2] * pow( ( x[8] - 1 ), 2 ) - 6 * x[4] * x[8] * ( x[8] - 1 ) + 3 * x[2] * x[8] * ( 2 * x[8] - 2 );
	dif_y = 3 * x[7] * pow( x[8], 2 ) - 3 * x[5] * pow( x[8], 2 ) - 3 * x[1] * pow( ( x[8] - 1 ), 2 ) + 3 * x[3] * pow( ( x[8] - 1 ), 2 ) - 6 * x[5] * x[8] * ( x[8] - 1 ) + 3 * x[3] * x[8] * ( 2 * x[8] - 2 );
	pos->angle_ = atan2( dif_y, dif_x );
	return 0;
}

int GetPosInfobyTdistExt( double t, double dist, const var__edge_t *curgragh, track__pos_info_t *pos ) {
	double dist_now;
	double dist_new;
	position_t pos_start, pos_end;
	double a_ext, dist_ext, t_new;
	int retval;

	if ( !curgragh || !pos ) return -1;

	retval = GetdistbyT( t, curgragh, &dist_now );
	if ( retval < 0 ) {
		return -1;
	}
	dist_new = dist_now + dist;

	if ( dist_new < 0 ) {
		retval = GetposbyT( 0, curgragh, &pos_start );
		if ( retval < 0 ) {
			return -1;
		}

		a_ext = pos_start.angle_ + Pi;
		dist_ext = -dist_new;
		pos->pos_ang_.x_ = pos_start.x_ + dist_ext*cos( a_ext );
		pos->pos_ang_.y_ = pos_start.y_ + dist_ext*sin( a_ext );
		pos->pos_ang_.angle_ = NormalAngle( a_ext );
		pos->curve_ = 0;
		pos->dist_start_ = dist_new;
		pos->percent_ = 0;
	} else if ( dist_new>curgragh->length_ ) {
		retval = GetposbyT( 1, curgragh, &pos_end );
		if ( retval < 0 ) {
			return -1;
		}

		a_ext = pos_end.angle_;
		dist_ext = dist_new - curgragh->length_;
		pos->pos_ang_.x_ = pos_end.x_ + dist_ext*cos( a_ext );
		pos->pos_ang_.y_ = pos_end.y_ + dist_ext*sin( a_ext );
		pos->pos_ang_.angle_ = a_ext;
		pos->curve_ = 0;
		pos->dist_start_ = dist_new;
		pos->percent_ = 0;
	} else {
		retval = GetTbydist( dist_new, curgragh, &t_new );
		if (  retval < 0 ) {
			return -1;
		}

		retval = GetPosAllInfobyT( t_new, curgragh, pos );
	}

	return retval;
}

int GetAnglebyT( double t, const var__edge_t * curgraph, double *output ) {
	double x[9], dif_x, dif_y;

	if ( !curgraph || !output ) return -1;

	x[0] = curgraph->start_point_.x_;     x[1] = curgraph->start_point_.y_;
	x[2] = curgraph->control_point_1_.x_; x[3] = curgraph->control_point_1_.y_;
	x[4] = curgraph->control_point_2_.x_; x[5] = curgraph->control_point_2_.y_;
	x[6] = curgraph->end_point_.x_;       x[7] = curgraph->end_point_.y_;
	x[8] = t;
	dif_x = 3 * x[6] * pow( x[8], 2 ) - 3 * x[4] * pow( x[8], 2 ) - 3 * x[0] * pow( ( x[8] - 1 ), 2 ) + 3 * x[2] * pow( ( x[8] - 1 ), 2 ) - 6 * x[4] * x[8] * ( x[8] - 1 ) + 3 * x[2] * x[8] * ( 2 * x[8] - 2 );
	dif_y = 3 * x[7] * pow( x[8], 2 ) - 3 * x[5] * pow( x[8], 2 ) - 3 * x[1] * pow( ( x[8] - 1 ), 2 ) + 3 * x[3] * pow( ( x[8] - 1 ), 2 ) - 6 * x[5] * x[8] * ( x[8] - 1 ) + 3 * x[3] * x[8] * ( 2 * x[8] - 2 );
	*output = atan2( dif_y, dif_x );
	return 0;
}

int GetcurvebyT( double t, const var__edge_t * curgraph, double *output ) {
	double x[9];

	if ( !curgraph || !output ) return -1;

	x[0] = curgraph->start_point_.x_;     x[1] = curgraph->start_point_.y_;
	x[2] = curgraph->control_point_1_.x_; x[3] = curgraph->control_point_1_.y_;
	x[4] = curgraph->control_point_2_.x_; x[5] = curgraph->control_point_2_.y_;
	x[6] = curgraph->end_point_.x_;       x[7] = curgraph->end_point_.y_;
	x[8] = t;

	// Cur
	*output = ( ( 6 * x[2] * x[8] - 12 * x[4] * x[8] + 6 * x[6] * x[8] - 6 * x[4] * ( x[8] - 1 ) - 3 * x[0] * ( 2 * x[8] - 2 ) + 6 * x[2] * ( 2 * x[8] - 2 ) )*( 3 * x[5] * pow( x[8], 2 ) - 3 * x[7] * pow( x[8], 2 ) + 3 * x[1] * pow( ( x[8] - 1 ), 2 ) - 3 * x[3] * pow( ( x[8] - 1 ), 2 ) + 6 * x[5] * x[8] * ( x[8] - 1 ) - 3 * x[3] * x[8] * ( 2 * x[8] - 2 ) ) - ( 6 * x[3] * x[8] - 12 * x[5] * x[8] + 6 * x[7] * x[8] - 6 * x[5] * ( x[8] - 1 ) - 3 * x[1] * ( 2 * x[8] - 2 ) + 6 * x[3] * ( 2 * x[8] - 2 ) )*( 3 * x[4] * pow( x[8], 2 ) - 3 * x[6] * pow( x[8], 2 ) + 3 * x[0] * pow( ( x[8] - 1 ), 2 ) - 3 * x[2] * pow( ( x[8] - 1 ), 2 ) + 6 * x[4] * x[8] * ( x[8] - 1 ) - 3 * x[2] * x[8] * ( 2 * x[8] - 2 ) ) ) / ( 27 * pow( ( pow( ( 3 * x[4] * pow( x[8], 2 ) - 3 * x[6] * pow( x[8], 2 ) + 3 * x[0] * pow( ( x[8] - 1 ), 2 ) - 3 * x[2] * pow( ( x[8] - 1 ), 2 ) + 6 * x[4] * x[8] * ( x[8] - 1 ) - 3 * x[2] * x[8] * ( 2 * x[8] - 2 ) ), 2 )*0.1111 + pow( ( 3 * x[5] * pow( x[8], 2 ) - 3 * x[7] * pow( x[8], 2 ) + 3 * x[1] * pow( ( x[8] - 1 ), 2 ) - 3 * x[3] * pow( ( x[8] - 1 ), 2 ) + 6 * x[5] * x[8] * ( x[8] - 1 ) - 3 * x[3] * x[8] * ( 2 * x[8] - 2 ) ), 2 )*0.1111 ), ( 3.0 / 2 ) ) );
	return 0;
}

int GetpercentbyT( double t, const var__edge_t * curgraph ,double* per) {
	int retval;
	double dist;
	position_t* dis_points;
	double dist_whole;
	double percent;

	if ( !curgraph ) return -1;

	retval = GetdistbyT( t, curgraph, &dist );
	if ( retval < 0 ) {
		return -1;
	}

	dis_points = ( position_t* ) curgraph->disperse_points_.data_;
	if ( !dis_points || curgraph->disperse_points_.count_ != 201 ) return -1;

	dist_whole = dis_points[curgraph->disperse_points_.count_ - 1].distance_;
	percent =  ( 100.0*dist / dist_whole );

	if ( percent < 0 ) {
		percent = 0;
	} else if ( percent>100 ) {
		percent = 100;
	}

	*per = percent;
	return 0;
}

int GetdistbyTOrg( double t, const var__edge_t * curgraph, double *output ) {
	double x[9], _t;

	if ( !curgraph || !output ) return -1;

	x[0] = curgraph->start_point_.x_;     x[1] = curgraph->start_point_.y_;
	x[2] = curgraph->control_point_1_.x_; x[3] = curgraph->control_point_1_.y_;
	x[4] = curgraph->control_point_2_.x_; x[5] = curgraph->control_point_2_.y_;
	x[6] = curgraph->end_point_.x_;       x[7] = curgraph->end_point_.y_;
	x[8] = t;
	_t = x[8] * 1000;
	*output = Getdis_bezeil( _t, x );
	return 0;
}

int GetPosAllInfobyT( double t, const var__edge_t * curgraph,track__pos_info_t *pos ) {
	double x[9], dif_x, dif_y;

	if ( !curgraph || !pos ) return -1;

	x[0] = curgraph->start_point_.x_;     x[1] = curgraph->start_point_.y_;
	x[2] = curgraph->control_point_1_.x_; x[3] = curgraph->control_point_1_.y_;
	x[4] = curgraph->control_point_2_.x_; x[5] = curgraph->control_point_2_.y_;
	x[6] = curgraph->end_point_.x_;       x[7] = curgraph->end_point_.y_;
	x[8] = t;

	{
		// x,y
		pos->pos_ang_.x_ = x[0] * pow( ( 1 - x[8] ), 3 ) + x[2] * 3 * pow( ( 1 - x[8] ), 2 )*x[8] + x[4] * 3 * ( 1 - x[8] )*pow( x[8], 2 ) + x[6] * pow( x[8], 3 ); // x
		pos->pos_ang_.y_ = x[1] * pow( ( 1 - x[8] ), 3 ) + x[3] * 3 * pow( ( 1 - x[8] ), 2 )*x[8] + x[5] * 3 * ( 1 - x[8] )*pow( x[8], 2 ) + x[7] * pow( x[8], 3 ); // y
		// angle		
		dif_x = 3 * x[6] * pow( x[8], 2 ) - 3 * x[4] * pow( x[8], 2 ) - 3 * x[0] * pow( ( x[8] - 1 ), 2 ) + 3 * x[2] * pow( ( x[8] - 1 ), 2 ) - 6 * x[4] * x[8] * ( x[8] - 1 ) + 3 * x[2] * x[8] * ( 2 * x[8] - 2 );
		dif_y = 3 * x[7] * pow( x[8], 2 ) - 3 * x[5] * pow( x[8], 2 ) - 3 * x[1] * pow( ( x[8] - 1 ), 2 ) + 3 * x[3] * pow( ( x[8] - 1 ), 2 ) - 6 * x[5] * x[8] * ( x[8] - 1 ) + 3 * x[3] * x[8] * ( 2 * x[8] - 2 );
		pos->pos_ang_.angle_ = atan2( dif_y, dif_x );
		// Cur
		pos->curve_ = ( ( 6 * x[2] * x[8] - 12 * x[4] * x[8] + 6 * x[6] * x[8] - 6 * x[4] * ( x[8] - 1 ) - 3 * x[0] * ( 2 * x[8] - 2 ) + 6 * x[2] * ( 2 * x[8] - 2 ) )*( 3 * x[5] * pow( x[8], 2 ) - 3 * x[7] * pow( x[8], 2 ) + 3 * x[1] * pow( ( x[8] - 1 ), 2 ) - 3 * x[3] * pow( ( x[8] - 1 ), 2 ) + 6 * x[5] * x[8] * ( x[8] - 1 ) - 3 * x[3] * x[8] * ( 2 * x[8] - 2 ) ) - ( 6 * x[3] * x[8] - 12 * x[5] * x[8] + 6 * x[7] * x[8] - 6 * x[5] * ( x[8] - 1 ) - 3 * x[1] * ( 2 * x[8] - 2 ) + 6 * x[3] * ( 2 * x[8] - 2 ) )*( 3 * x[4] * pow( x[8], 2 ) - 3 * x[6] * pow( x[8], 2 ) + 3 * x[0] * pow( ( x[8] - 1 ), 2 ) - 3 * x[2] * pow( ( x[8] - 1 ), 2 ) + 6 * x[4] * x[8] * ( x[8] - 1 ) - 3 * x[2] * x[8] * ( 2 * x[8] - 2 ) ) ) / ( 27 * pow( ( pow( ( 3 * x[4] * pow( x[8], 2 ) - 3 * x[6] * pow( x[8], 2 ) + 3 * x[0] * pow( ( x[8] - 1 ), 2 ) - 3 * x[2] * pow( ( x[8] - 1 ), 2 ) + 6 * x[4] * x[8] * ( x[8] - 1 ) - 3 * x[2] * x[8] * ( 2 * x[8] - 2 ) ), 2 )*0.1111 + pow( ( 3 * x[5] * pow( x[8], 2 ) - 3 * x[7] * pow( x[8], 2 ) + 3 * x[1] * pow( ( x[8] - 1 ), 2 ) - 3 * x[3] * pow( ( x[8] - 1 ), 2 ) + 6 * x[5] * x[8] * ( x[8] - 1 ) - 3 * x[3] * x[8] * ( 2 * x[8] - 2 ) ), 2 )*0.1111 ), ( 3.0 / 2 ) ) );
		// dis
		GetdistbyT( t, curgraph, &pos->dist_start_ );
		pos->percent_ = ( int ) ( 100 * pos->dist_start_ / curgraph->length_ );
	}

	return 0;
}

double NormalAngle( double sita ) {
	double result = sita;
	while ( result > Pi ) {
		result = result - 2 * Pi;
	}
	while ( result <= -Pi ) {
		result = result + 2 * Pi;
	}
	return result;
}
