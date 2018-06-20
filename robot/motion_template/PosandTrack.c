#include "PosandTrack.h"
#include "var.h"
#include <math.h>
/*
#if !defined NECESSARY_DISPERSE_POINT_COUNT
#define NECESSARY_DISPERSE_POINT_COUNT   (201)
#endif
*/
static
double Getdistsqure(const position_t *p1, const position_t *p2) {
    return ( p2->x_ - p1->x_)*(p2->x_ - p1->x_) + (p2->y_ - p1->y_)*(p2->y_ - p1->y_);
}

static
double GetdistbyPos(const position_t *p1, const position_t *p2) {
    return sqrt(Getdistsqure(p1, p2));
}

int Sgn(double x) {
    return ( x >= 0 ? 1 : -1);
}

double Getdis_bezeil(double _t, double x[9]) {
    double t_temp = 0;
    double sum_dis = 0;
    int i = 0;
    for (i = 0; i <= _t; i++) {
        if (i > 0) {
            double t = i * 0.001;
            double x_1 = x[0] * pow((1 - t), 3) + x[2] * 3 * pow((1 - t), 2) * t + x[4] * 3 * (1 - t) * pow(t, 2) + x[6] * pow(t, 3); // x
            double y_1 = x[1] * pow((1 - t), 3) + x[3] * 3 * pow((1 - t), 2) * t + x[5] * 3 * (1 - t) * pow(t, 2) + x[7] * pow(t, 3); // y 
            double x_2 = x[0] * pow((1 - t_temp), 3) + x[2] * 3 * pow((1 - t_temp), 2) * t_temp + x[4] * 3 * (1 - t_temp) * pow(t_temp, 2) + x[6] * pow(t_temp, 3); // x
            double y_2 = x[1] * pow((1 - t_temp), 3) + x[3] * 3 * pow((1 - t_temp), 2) * t_temp + x[5] * 3 * (1 - t_temp) * pow(t_temp, 2) + x[7] * pow(t_temp, 3); // y 
            sum_dis = sum_dis + sqrt(pow((x_2 - x_1), 2) + pow((y_2 - y_1), 2));
        }
        t_temp = i * 0.001;
    }
    return sum_dis;
}

int GetTClosestOnLine(position_t current_pos, int edge_id, double *output) {
    double t_result = 0.0;
    var__edge_t *p_thisedge;
    position_t* disperse_points;
    int index_mindist_sq, i;
    double thisdist_sq, min_dist_sq, ts, te, tm, dts, dte, dtm;
	double space_t = 1/(double)(NECESSARY_DISPERSE_POINT_COUNT - 1);
    double eplon = 0.001;
    position_t pos;
    thisdist_sq = min_dist_sq = ts = te = tm = dts = dte = dtm = 0.0;

    if (!output || edge_id <= 0) {
        return -1;
    }

    p_thisedge = (var__edge_t*) var__search_edge(edge_id);
    if (!p_thisedge) {
        return -1;
    }

    if (!p_thisedge->disperse_points_.data_ || NECESSARY_DISPERSE_POINT_COUNT != p_thisedge->disperse_points_.count_) {
        var__end_search();
        return -1;
    }
    disperse_points = (position_t*) p_thisedge->disperse_points_.data_;

    index_mindist_sq = 0;
    thisdist_sq = 0;
    min_dist_sq = Getdistsqure(&disperse_points[index_mindist_sq], &current_pos);
    for (i = 1; i < NECESSARY_DISPERSE_POINT_COUNT; i++) {
        thisdist_sq = Getdistsqure(&disperse_points[i], &current_pos);
        if (thisdist_sq < min_dist_sq) {
            min_dist_sq = thisdist_sq;
            index_mindist_sq = i;
        }
    }

    if (index_mindist_sq == 0) {
        ts = 0;
        te = (index_mindist_sq + 1) * space_t;
	}
	else if (index_mindist_sq == NECESSARY_DISPERSE_POINT_COUNT - 1) {
        ts = (index_mindist_sq - 1) * space_t;
        te = index_mindist_sq*space_t;
    } else {
        ts = (index_mindist_sq - 1) * space_t;
        te = (index_mindist_sq + 1) * space_t;
    }

    for (i = 0;; i++) {
        if (GetposbyT(ts, p_thisedge, &pos) >= 0) {
            dts = GetdistbyPos(&current_pos, &pos);
        }
        if (GetposbyT(te, p_thisedge, &pos) >= 0) {
            dte = GetdistbyPos(&current_pos, &pos);
        }

		if (fabs(dts - dte) < eplon || i > 10) {
			t_result = (ts + te) / 2.0;
			break;
		}
		else {
			tm = (ts + te) / 2.0;
			if (dts < dte){
				te = tm;
			}
			else{
				ts = tm;
			}
		}
    }

    var__end_search();
    *output = t_result;
    return 0;
}

int GetTClosestOnLineBycurg(position_t current_pos, const var__edge_t * curgraph, double *output) {
    double t_result;
    position_t* disperse_points;
    int index_mindist_sq;
    double thisdist_sq;
    double min_dist_sq;
    double ts, te, tm;
	double space_t = 1/(double)(NECESSARY_DISPERSE_POINT_COUNT - 1);
    int i;
    double eplon = 0.0001;
    double dts, dte, dtm;
    position_t pos;

    dts = dte = dtm = 0.0;
    if (!curgraph || !output) {
        return -1;
    }

    disperse_points = (position_t*) curgraph->disperse_points_.data_;
    if (!disperse_points || NECESSARY_DISPERSE_POINT_COUNT != curgraph->disperse_points_.count_) {
        return -1;
    }

    index_mindist_sq = 0;
    thisdist_sq = 0;
    min_dist_sq = Getdistsqure(&disperse_points[index_mindist_sq], &current_pos);
    for (i = 1; i < NECESSARY_DISPERSE_POINT_COUNT; i++) {
        thisdist_sq = Getdistsqure(&disperse_points[i], &current_pos);
        if (thisdist_sq < min_dist_sq) {
            min_dist_sq = thisdist_sq;
            index_mindist_sq = i;
        }
    }

    if (index_mindist_sq == 0) {
        ts = 0;
        te = (index_mindist_sq + 1) * space_t;
	}
	else if (index_mindist_sq == NECESSARY_DISPERSE_POINT_COUNT-1) {
        ts = (index_mindist_sq - 1) * space_t;
        te = index_mindist_sq*space_t;
    } else {
        ts = (index_mindist_sq - 1) * space_t;
        te = (index_mindist_sq + 1) * space_t;
    }

    for (i = 0;; i++) {
        if (GetposbyT(ts, curgraph, &pos) >= 0) {
            dts = GetdistbyPos(&current_pos, &pos);
        }
        if (GetposbyT(te, curgraph, &pos) >= 0) {
            dte = GetdistbyPos(&current_pos, &pos);
        }
        if (fabs(dts - dte) < eplon || i > 10) {
			t_result = (ts + te) / 2.0;
            break;
        } else {
            tm = (ts + te) / 2.0;       
			if (dts < dte){
				te = tm;
			}
			else{
				ts = tm;
			}
        }
    }

    *output = t_result;
    return 0;
}

int GetTnewbyDistandTcur(double t_cur, double dist, const var__edge_t * curgraph, double *output) {
    double dist_cur;
    double dist_new;
    int retval;

    if (!curgraph || !output) return -1;

    retval = GetdistbyT(t_cur, curgraph, &dist_cur);
    if (retval >= 0) {
        dist_new = dist_cur + dist;
        return GetTbydist(dist_new, curgraph, output);
    }
    return -1;
}

int GetTbydist(double dist, const var__edge_t * curgraph, double *output) {
    position_t* disperse_points;
    int is = 0;
	int ie = NECESSARY_DISPERSE_POINT_COUNT-1;
    int im = 0;
    double ts, te;
    int i;

    if (!curgraph || !output) return -1;

    if (dist <= 0) {
        *output = 0;
        return 0;
    }

    disperse_points = (position_t*) curgraph->disperse_points_.data_;
    if (!disperse_points || NECESSARY_DISPERSE_POINT_COUNT != curgraph->disperse_points_.count_) {
        return -1;
    }

	if (dist >= disperse_points[NECESSARY_DISPERSE_POINT_COUNT-1].distance_) {
        *output = 1.0;
        return 0;
    }

    for (i = 0;; i++) {
        if (is - ie >= -1) {
            break;
        }
        im = (is + ie) / 2;
        if (dist <= disperse_points[im].distance_) {
            ie = im;
        } else {
            is = im;
        }
    }
	ts = is / (double)(NECESSARY_DISPERSE_POINT_COUNT - 1);
	te = ie / (double)(NECESSARY_DISPERSE_POINT_COUNT - 1);

    *output = (ts * (disperse_points[ie].distance_ - dist) + te * (dist - disperse_points[is].distance_))
            / (disperse_points[ie].distance_ - disperse_points[is].distance_);
    return 0;
}

int GetdistbyT(double t, const var__edge_t * curgraph, double *output) {
    double dist_result;
    position_t* disperse_points;
    int is, ie;
	double k = 1 / (double)(NECESSARY_DISPERSE_POINT_COUNT - 1);

    if (!output || !curgraph) {
        return -1;
    }

    disperse_points = (position_t*) curgraph->disperse_points_.data_;
    if (!disperse_points || NECESSARY_DISPERSE_POINT_COUNT != curgraph->disperse_points_.count_) {
        return -1;
    }

    if (t <= 0) {
        *output = 0;
        return 0;
    } else if (t >= 1) {
		*output = disperse_points[NECESSARY_DISPERSE_POINT_COUNT-1].distance_;
        return 0;
    }

	is = (int)(t * (double)(NECESSARY_DISPERSE_POINT_COUNT - 1));
	if (is >= (NECESSARY_DISPERSE_POINT_COUNT - 1)) {
		*output = disperse_points[NECESSARY_DISPERSE_POINT_COUNT-1].distance_;
        return 0;
    }

    ie = is + 1;
	dist_result = ((ie * k - t) * disperse_points[is].distance_ + (t - is * k) * disperse_points[ie].distance_)*(double)(NECESSARY_DISPERSE_POINT_COUNT - 1);
    *output = dist_result;
    return 0;
}

int GetposbyT(double t, const var__edge_t * curgraph, position_t *pos) {
    double x[9], dif_x, dif_y;

    if (!curgraph || !pos) return -1;

    x[0] = curgraph->start_point_.x_;
    x[1] = curgraph->start_point_.y_;
    x[2] = curgraph->control_point_1_.x_;
    x[3] = curgraph->control_point_1_.y_;
    x[4] = curgraph->control_point_2_.x_;
    x[5] = curgraph->control_point_2_.y_;
    x[6] = curgraph->end_point_.x_;
    x[7] = curgraph->end_point_.y_;
    x[8] = t;
    pos->x_ = x[0] * pow((1 - x[8]), 3) + x[2] * 3 * pow((1 - x[8]), 2) * x[8] + x[4] * 3 * (1 - x[8]) * pow(x[8], 2) + x[6] * pow(x[8], 3); // x
    pos->y_ = x[1] * pow((1 - x[8]), 3) + x[3] * 3 * pow((1 - x[8]), 2) * x[8] + x[5] * 3 * (1 - x[8]) * pow(x[8], 2) + x[7] * pow(x[8], 3); // y
    dif_x = 3 * x[6] * pow(x[8], 2) - 3 * x[4] * pow(x[8], 2) - 3 * x[0] * pow((x[8] - 1), 2) + 3 * x[2] * pow((x[8] - 1), 2) - 6 * x[4] * x[8] * (x[8] - 1) + 3 * x[2] * x[8] * (2 * x[8] - 2);
    dif_y = 3 * x[7] * pow(x[8], 2) - 3 * x[5] * pow(x[8], 2) - 3 * x[1] * pow((x[8] - 1), 2) + 3 * x[3] * pow((x[8] - 1), 2) - 6 * x[5] * x[8] * (x[8] - 1) + 3 * x[3] * x[8] * (2 * x[8] - 2);
    pos->angle_ = atan2(dif_y, dif_x);
    return 0;
}

int GetPosInfobyTdistExt(double t, double dist, const var__edge_t *curgragh, track__pos_info_t *pos) {
    double dist_now;
    double dist_new;
    position_t pos_start, pos_end;
    double a_ext, dist_ext, t_new;
    int retval;

    if (!curgragh || !pos) return -1;

    retval = GetdistbyT(t, curgragh, &dist_now);
    if (retval < 0) {
        return -1;
    }
    dist_new = dist_now + dist;

    if (dist_new < 0) {
        retval = GetposbyT(0, curgragh, &pos_start);
        if (retval < 0) {
            return -1;
        }

        a_ext = pos_start.angle_ + Pi;
        dist_ext = -dist_new;
        pos->pos_ang_.x_ = pos_start.x_ + dist_ext * cos(a_ext);
        pos->pos_ang_.y_ = pos_start.y_ + dist_ext * sin(a_ext);
        pos->pos_ang_.angle_ = NormalAngle(a_ext);
        pos->curve_ = 0;
        pos->dist_start_ = dist_new;
        pos->percent_ = 0;
    } else if (dist_new > curgragh->length_) {
        retval = GetposbyT(1, curgragh, &pos_end);
        if (retval < 0) {
            return -1;
        }

        a_ext = pos_end.angle_;
        dist_ext = dist_new - curgragh->length_;
        pos->pos_ang_.x_ = pos_end.x_ + dist_ext * cos(a_ext);
        pos->pos_ang_.y_ = pos_end.y_ + dist_ext * sin(a_ext);
        pos->pos_ang_.angle_ = a_ext;
        pos->curve_ = 0;
        pos->dist_start_ = dist_new;
        pos->percent_ = 0;
    } else {
        retval = GetTbydist(dist_new, curgragh, &t_new);
        if (retval < 0) {
            return -1;
        }

        retval = GetPosAllInfobyT(t_new, curgragh, pos);
    }

    return retval;
}

int GetAnglebyT(double t, const var__edge_t * curgraph, double *output) {
    double x[9], dif_x, dif_y;

    if (!curgraph || !output) return -1;

    x[0] = curgraph->start_point_.x_;
    x[1] = curgraph->start_point_.y_;
    x[2] = curgraph->control_point_1_.x_;
    x[3] = curgraph->control_point_1_.y_;
    x[4] = curgraph->control_point_2_.x_;
    x[5] = curgraph->control_point_2_.y_;
    x[6] = curgraph->end_point_.x_;
    x[7] = curgraph->end_point_.y_;
    x[8] = t;
    dif_x = 3 * x[6] * pow(x[8], 2) - 3 * x[4] * pow(x[8], 2) - 3 * x[0] * pow((x[8] - 1), 2) + 3 * x[2] * pow((x[8] - 1), 2) - 6 * x[4] * x[8] * (x[8] - 1) + 3 * x[2] * x[8] * (2 * x[8] - 2);
    dif_y = 3 * x[7] * pow(x[8], 2) - 3 * x[5] * pow(x[8], 2) - 3 * x[1] * pow((x[8] - 1), 2) + 3 * x[3] * pow((x[8] - 1), 2) - 6 * x[5] * x[8] * (x[8] - 1) + 3 * x[3] * x[8] * (2 * x[8] - 2);
    *output = atan2(dif_y, dif_x);
    return 0;
}

int GetcurvebyT(double t, const var__edge_t * curgraph, double *output) {
    double x[9];

    if (!curgraph || !output) return -1;

    x[0] = curgraph->start_point_.x_;
    x[1] = curgraph->start_point_.y_;
    x[2] = curgraph->control_point_1_.x_;
    x[3] = curgraph->control_point_1_.y_;
    x[4] = curgraph->control_point_2_.x_;
    x[5] = curgraph->control_point_2_.y_;
    x[6] = curgraph->end_point_.x_;
    x[7] = curgraph->end_point_.y_;
    x[8] = t;

    *output = ((6 * x[2] * x[8] - 12 * x[4] * x[8] + 6 * x[6] * x[8] - 6 * x[4] * (x[8] - 1) - 3 * x[0] * (2 * x[8] - 2) + 6 * x[2] * (2 * x[8] - 2))*(3 * x[5] * pow(x[8], 2) - 3 * x[7] * pow(x[8], 2) + 3 * x[1] * pow((x[8] - 1), 2) - 3 * x[3] * pow((x[8] - 1), 2) + 6 * x[5] * x[8] * (x[8] - 1) - 3 * x[3] * x[8] * (2 * x[8] - 2)) - (6 * x[3] * x[8] - 12 * x[5] * x[8] + 6 * x[7] * x[8] - 6 * x[5] * (x[8] - 1) - 3 * x[1] * (2 * x[8] - 2) + 6 * x[3] * (2 * x[8] - 2))*(3 * x[4] * pow(x[8], 2) - 3 * x[6] * pow(x[8], 2) + 3 * x[0] * pow((x[8] - 1), 2) - 3 * x[2] * pow((x[8] - 1), 2) + 6 * x[4] * x[8] * (x[8] - 1) - 3 * x[2] * x[8] * (2 * x[8] - 2))) / (27 * pow((pow((3 * x[4] * pow(x[8], 2) - 3 * x[6] * pow(x[8], 2) + 3 * x[0] * pow((x[8] - 1), 2) - 3 * x[2] * pow((x[8] - 1), 2) + 6 * x[4] * x[8] * (x[8] - 1) - 3 * x[2] * x[8] * (2 * x[8] - 2)), 2)*0.1111 + pow((3 * x[5] * pow(x[8], 2) - 3 * x[7] * pow(x[8], 2) + 3 * x[1] * pow((x[8] - 1), 2) - 3 * x[3] * pow((x[8] - 1), 2) + 6 * x[5] * x[8] * (x[8] - 1) - 3 * x[3] * x[8] * (2 * x[8] - 2)), 2)*0.1111), (3.0 / 2)));
    return 0;
}

int GetpercentbyT(double t, const var__edge_t * curgraph, double* per) {
    int retval;
    double dist;
    position_t* dis_points;
    double dist_whole;
    double percent;

    if (!curgraph) return -1;

    retval = GetdistbyT(t, curgraph, &dist);
    if (retval < 0) {
        return -1;
    }

    dis_points = (position_t*) curgraph->disperse_points_.data_;
    if (!dis_points || NECESSARY_DISPERSE_POINT_COUNT != curgraph->disperse_points_.count_) return -1;

    dist_whole = dis_points[curgraph->disperse_points_.count_ - 1].distance_;
    percent = (100.0 * dist / dist_whole);

    if (percent < 0) {
        percent = 0;
    } else if (percent > 100) {
        percent = 100;
    }

    *per = percent;
    return 0;
}

int GetdistbyTOrg(double t, const var__edge_t * curgraph, double *output) {
    double x[9], _t;

    if (!curgraph || !output) return -1;

    x[0] = curgraph->start_point_.x_;
    x[1] = curgraph->start_point_.y_;
    x[2] = curgraph->control_point_1_.x_;
    x[3] = curgraph->control_point_1_.y_;
    x[4] = curgraph->control_point_2_.x_;
    x[5] = curgraph->control_point_2_.y_;
    x[6] = curgraph->end_point_.x_;
    x[7] = curgraph->end_point_.y_;
    x[8] = t;
    _t = x[8] * 1000;
    *output = Getdis_bezeil(_t, x);
    return 0;
}

int GetPosAllInfobyT(double t, const var__edge_t * curgraph, track__pos_info_t *pos) {
    double x[9], dif_x, dif_y;

    if (!curgraph || !pos) return -1;

    x[0] = curgraph->start_point_.x_;
    x[1] = curgraph->start_point_.y_;
    x[2] = curgraph->control_point_1_.x_;
    x[3] = curgraph->control_point_1_.y_;
    x[4] = curgraph->control_point_2_.x_;
    x[5] = curgraph->control_point_2_.y_;
    x[6] = curgraph->end_point_.x_;
    x[7] = curgraph->end_point_.y_;
    x[8] = t;

    {
        // x,y
        pos->pos_ang_.x_ = x[0] * pow((1 - x[8]), 3) + x[2] * 3 * pow((1 - x[8]), 2) * x[8] + x[4] * 3 * (1 - x[8]) * pow(x[8], 2) + x[6] * pow(x[8], 3); // x
        pos->pos_ang_.y_ = x[1] * pow((1 - x[8]), 3) + x[3] * 3 * pow((1 - x[8]), 2) * x[8] + x[5] * 3 * (1 - x[8]) * pow(x[8], 2) + x[7] * pow(x[8], 3); // y
        // angle		
        dif_x = 3 * x[6] * pow(x[8], 2) - 3 * x[4] * pow(x[8], 2) - 3 * x[0] * pow((x[8] - 1), 2) + 3 * x[2] * pow((x[8] - 1), 2) - 6 * x[4] * x[8] * (x[8] - 1) + 3 * x[2] * x[8] * (2 * x[8] - 2);
        dif_y = 3 * x[7] * pow(x[8], 2) - 3 * x[5] * pow(x[8], 2) - 3 * x[1] * pow((x[8] - 1), 2) + 3 * x[3] * pow((x[8] - 1), 2) - 6 * x[5] * x[8] * (x[8] - 1) + 3 * x[3] * x[8] * (2 * x[8] - 2);
        pos->pos_ang_.angle_ = atan2(dif_y, dif_x);
        // Cur
        pos->curve_ = ((6 * x[2] * x[8] - 12 * x[4] * x[8] + 6 * x[6] * x[8] - 6 * x[4] * (x[8] - 1) - 3 * x[0] * (2 * x[8] - 2) + 6 * x[2] * (2 * x[8] - 2))*(3 * x[5] * pow(x[8], 2) - 3 * x[7] * pow(x[8], 2) + 3 * x[1] * pow((x[8] - 1), 2) - 3 * x[3] * pow((x[8] - 1), 2) + 6 * x[5] * x[8] * (x[8] - 1) - 3 * x[3] * x[8] * (2 * x[8] - 2)) - (6 * x[3] * x[8] - 12 * x[5] * x[8] + 6 * x[7] * x[8] - 6 * x[5] * (x[8] - 1) - 3 * x[1] * (2 * x[8] - 2) + 6 * x[3] * (2 * x[8] - 2))*(3 * x[4] * pow(x[8], 2) - 3 * x[6] * pow(x[8], 2) + 3 * x[0] * pow((x[8] - 1), 2) - 3 * x[2] * pow((x[8] - 1), 2) + 6 * x[4] * x[8] * (x[8] - 1) - 3 * x[2] * x[8] * (2 * x[8] - 2))) / (27 * pow((pow((3 * x[4] * pow(x[8], 2) - 3 * x[6] * pow(x[8], 2) + 3 * x[0] * pow((x[8] - 1), 2) - 3 * x[2] * pow((x[8] - 1), 2) + 6 * x[4] * x[8] * (x[8] - 1) - 3 * x[2] * x[8] * (2 * x[8] - 2)), 2)*0.1111 + pow((3 * x[5] * pow(x[8], 2) - 3 * x[7] * pow(x[8], 2) + 3 * x[1] * pow((x[8] - 1), 2) - 3 * x[3] * pow((x[8] - 1), 2) + 6 * x[5] * x[8] * (x[8] - 1) - 3 * x[3] * x[8] * (2 * x[8] - 2)), 2)*0.1111), (3.0 / 2)));
        // dis
        GetdistbyT(t, curgraph, &pos->dist_start_);
        pos->percent_ = (100.0 * pos->dist_start_ / curgraph->length_);
    }

    return 0;
}

double NormalAngle(double sita) {
	double angle_result = sita;
	if (angle_result > Pi)
	{
		int tp1 = (int)(angle_result / Pi);
		if (tp1 % 2 == 0)
		{
			angle_result -= tp1*Pi;
		}
		else if (tp1 % 2 == 1)
		{
			angle_result -= (tp1 + 1)*Pi;
		}
	}
	if (angle_result < -Pi)
	{
		int tp1 = (int)(angle_result / -Pi);
		if (tp1 % 2 == 0)
		{
			angle_result += tp1*Pi;
		}
		else if (tp1 % 2 == 1)
		{
			angle_result += (tp1 + 1)*Pi;
		}
	}
	return angle_result;
}

int GetPosTran(double x_inB, double y_inB, double ang_in_B,
        double x_BinA, double y_BinA, double ang_BinA,
        double *x_inA, double *y_inA, double *ang_inA, enum Rot_Tran_t seq) {
    if (seq == TRANFIRST) {
        *x_inA = x_inB * cos(ang_BinA) - y_inB * sin(ang_BinA) + x_BinA;
        *y_inA = x_inB * sin(ang_BinA) + y_inB * cos(ang_BinA) + y_BinA;
        *ang_inA = ang_in_B + ang_BinA;
        return 0;
    }
    if (seq == ROTFIRST) {
        *x_inA = (x_inB + x_BinA) * cos(ang_BinA) - (y_inB + y_BinA) * sin(ang_BinA);
        *y_inA = (x_inB + x_BinA) * sin(ang_BinA) + (y_inB + y_BinA) * cos(ang_BinA);
        *ang_inA = ang_in_B + ang_BinA;
        return 0;
    }
    return -1;
}

int GetPosBoxInGlobal(position_t posveh_in_global, track__pos_and_h_t posh_forkflat_in_veh,
        track__pos_fork_t_ posfork_std_in_forkflat, position_t pos_boxorg_in_forkflat, double angline_in_global,
        track__pos_and_h_t *posbox_in_global) {
    track__pos_and_h_t pos_forkflat_in_global, pos_boxstd_in_forkflat;
    position_t pos_boxstd_in_forkstd;
    double x_temp, y_temp, ang_temp;
    int i;

    GetPosTran(posh_forkflat_in_veh.pos_ang_.x_, posh_forkflat_in_veh.pos_ang_.y_, posh_forkflat_in_veh.pos_ang_.angle_,
            posveh_in_global.x_, posveh_in_global.y_, posveh_in_global.angle_,
            &x_temp, &y_temp, &ang_temp, TRANFIRST);
    pos_forkflat_in_global.pos_ang_.x_ = x_temp;
    pos_forkflat_in_global.pos_ang_.y_ = y_temp;
    pos_forkflat_in_global.pos_ang_.angle_ = ang_temp;
    pos_forkflat_in_global.h_ = posh_forkflat_in_veh.h_;

    pos_boxstd_in_forkstd = pos_boxorg_in_forkflat;
    //step3
    GetPosTran(pos_boxstd_in_forkstd.x_, pos_boxstd_in_forkstd.y_, pos_boxstd_in_forkstd.angle_,
            posfork_std_in_forkflat.L_, 0, posfork_std_in_forkflat.angle_, &x_temp, &y_temp, &ang_temp, ROTFIRST);
    pos_boxstd_in_forkflat.pos_ang_.x_ = x_temp;
    pos_boxstd_in_forkflat.pos_ang_.y_ = y_temp;
    pos_boxstd_in_forkflat.pos_ang_.angle_ = ang_temp;
    pos_boxstd_in_forkflat.h_ = posfork_std_in_forkflat.h_;
    //step4: ????posbox_in_global
    GetPosTran(pos_boxstd_in_forkflat.pos_ang_.x_, pos_boxstd_in_forkflat.pos_ang_.y_, pos_boxstd_in_forkflat.pos_ang_.angle_,
            pos_forkflat_in_global.pos_ang_.x_, pos_forkflat_in_global.pos_ang_.y_, pos_forkflat_in_global.pos_ang_.angle_,
            &x_temp, &y_temp, &ang_temp, TRANFIRST);
    posbox_in_global->pos_ang_.x_ = x_temp;
    posbox_in_global->pos_ang_.y_ = y_temp;

    posbox_in_global->pos_ang_.angle_ = 0;
    for (i = 0; i < 4; i++) {
        if (fabs(NormalAngle(ang_temp - i * 0.5 * Pi)) < 45.0 * D2R) {
            posbox_in_global->pos_ang_.angle_ = NormalAngle(i * 0.5 * Pi);
            break;
        }
    }
    posbox_in_global->h_ = pos_boxstd_in_forkflat.h_ + posh_forkflat_in_veh.h_;
    return 0;
}

int GetVehtoBoxInGlobal(track__pos_and_h_t posbox_in_global, position_t posbox_in_forkflat,
        track__pos_and_h_t posh_forkflat_in_veh, position_t posveh_in_global_init, double *dist_stop, double *ang_veh_in_global) {
    //step1
    double x_vehtemp_in_global, y_vehtemp_in_global, ang_box_verti, ang_forkrot_in_forkflat,
            ang_temp, ang_vehtoflat_in_flat, ang_vehtoflat_in_veh, ang_vehtotarget_in_global;
    double x_boxrot_ver, y_boxrot_ver, dist_boxrot_vehtemp;
    int i, dir;
    double dang_veh;
    position_t pos_boxrot_in_forkrot;
    double x_forkrot_in_forkflat, y_forkrot_in_forkflat;
    double x_boxrot_in_forkflat, y_boxrot_in_forkflat;
    double x_boxrot_in_veh, y_boxrot_in_veh;
    double x_boxrot_in_global, y_boxrot_in_global;
    for (i = 0; i < 4; i++) {
        if (fabs(NormalAngle(posveh_in_global_init.angle_ - (i * 0.5 * Pi + posbox_in_global.pos_ang_.angle_))) < 45.0 * D2R) {
            *ang_veh_in_global = NormalAngle(i * 0.5 * Pi + posbox_in_global.pos_ang_.angle_);
            break;
        }
    }
    if (GetVertiPos(posveh_in_global_init.x_, posveh_in_global_init.y_, *ang_veh_in_global, posbox_in_global.pos_ang_.x_,
            posbox_in_global.pos_ang_.y_, &x_vehtemp_in_global, &y_vehtemp_in_global, &ang_box_verti) < 0) {
        return -1;
    }
    //step2
    //2.1
    ang_forkrot_in_forkflat = NormalAngle(ang_box_verti + Pi - *ang_veh_in_global - posh_forkflat_in_veh.pos_ang_.angle_);
    //2.2 Pos_Boxrot_in_forkflat
    pos_boxrot_in_forkrot = posbox_in_forkflat;
    x_forkrot_in_forkflat = 0;
    y_forkrot_in_forkflat = 0;
    GetPosTran(pos_boxrot_in_forkrot.x_, pos_boxrot_in_forkrot.y_, pos_boxrot_in_forkrot.angle_,
            x_forkrot_in_forkflat, y_forkrot_in_forkflat, ang_forkrot_in_forkflat, &x_boxrot_in_forkflat, &y_boxrot_in_forkflat, &ang_temp, ROTFIRST);
    //2.3 pos_boxrot_in_veh
    GetPosTran(x_boxrot_in_forkflat, y_boxrot_in_forkflat, ang_temp, posh_forkflat_in_veh.pos_ang_.x_, posh_forkflat_in_veh.pos_ang_.y_,
            posh_forkflat_in_veh.pos_ang_.angle_, &x_boxrot_in_veh, &y_boxrot_in_veh, &ang_temp, TRANFIRST);
    //2.4 pos_boxrot_in_global
    GetPosTran(x_boxrot_in_veh, y_boxrot_in_veh, ang_temp,
            x_vehtemp_in_global, y_vehtemp_in_global, *ang_veh_in_global, &x_boxrot_in_global, &y_boxrot_in_global, &ang_temp, TRANFIRST);
    //step3
    GetVertiPos(x_vehtemp_in_global, y_vehtemp_in_global, *ang_veh_in_global,
            x_boxrot_in_global, y_boxrot_in_global, &x_boxrot_ver, &y_boxrot_ver, &ang_temp);
    //step4
    dist_boxrot_vehtemp = sqrt(pow(x_vehtemp_in_global - x_boxrot_ver, 2) + pow(y_vehtemp_in_global - y_boxrot_ver, 2));

    ang_vehtoflat_in_flat = atan2(posh_forkflat_in_veh.pos_ang_.y_, posh_forkflat_in_veh.pos_ang_.x_);
    ang_vehtoflat_in_veh = ang_vehtoflat_in_flat + posh_forkflat_in_veh.pos_ang_.angle_;
    for (i = 0; i < 4; i++) {
        if (fabs(NormalAngle(ang_vehtoflat_in_veh - i * 0.5 * Pi)) < 45.0 * D2R) {
            ang_vehtoflat_in_veh = NormalAngle(i * 0.5 * Pi);
            break;
        }
    }
    x_vehtemp_in_global = x_vehtemp_in_global + dist_boxrot_vehtemp * cos(ang_vehtoflat_in_veh + Pi + *ang_veh_in_global);
    y_vehtemp_in_global = y_vehtemp_in_global + dist_boxrot_vehtemp * sin(ang_vehtoflat_in_veh + Pi + *ang_veh_in_global);

    //step5
    ang_vehtotarget_in_global = atan2(y_vehtemp_in_global - posveh_in_global_init.y_, x_vehtemp_in_global - posveh_in_global_init.x_);
    dang_veh = ang_vehtotarget_in_global - posveh_in_global_init.angle_;
    //dang_veh =( *ang_veh_in_global) - posveh_in_global_init.angle_;
    dir = (cos(dang_veh) >= 0 ? 1 : -1);
    *dist_stop = sqrt(pow(x_vehtemp_in_global - posveh_in_global_init.x_, 2) + pow(y_vehtemp_in_global - posveh_in_global_init.y_, 2)) * dir;
    return 0;
}

int GetForkInForkFlat(track__pos_and_h_t posbox_in_global, position_t posbox_in_forkflat,
        track__pos_and_h_t posh_forkflat_in_veh, position_t posveh_in_global, track__pos_fork_t_ *posfork_std_in_forkflat) {
    //step1: ??posfork_std_in_forkflat.angle
    //1.1
    double angdes_box_global, ang_boxto_verti, ang_forkdes_in_global;
    double x_verti, y_verti;
    int i;
    position_t pos_boxrot_in_forkrot;
    double ang_temp, dist_temp;
    double x_forkrot_in_forkflat, y_forkrot_in_forkflat;
    double x_boxrot_in_forkflat, y_boxrot_in_forkflat;
    double x_boxrot_in_veh, y_boxrot_in_veh;
    double x_boxrot_in_global, y_boxrot_in_global;

    angdes_box_global = ang_boxto_verti = ang_forkdes_in_global = 0.0;

    for (i = 0; i < 4; i++) {
        if (fabs(NormalAngle(posveh_in_global.angle_ - (i * 0.5 * Pi + posbox_in_global.pos_ang_.angle_))) < 45.0 * D2R) {
            angdes_box_global = NormalAngle(i * 0.5 * Pi + posbox_in_global.pos_ang_.angle_);
            break;
        }
    }
    //1.2
    GetVertiPos(posveh_in_global.x_, posveh_in_global.y_, angdes_box_global, posbox_in_global.pos_ang_.x_, posbox_in_global.pos_ang_.y_,
            &x_verti, &y_verti, &ang_boxto_verti);
    //1.3
    ang_forkdes_in_global = ang_boxto_verti + Pi;
    //1.4 ??posfork_std_in_forkflat.angle
    posfork_std_in_forkflat->angle_ = NormalAngle(ang_forkdes_in_global - posveh_in_global.angle_ - posh_forkflat_in_veh.pos_ang_.angle_);
    //step2: ??posfork_std_in_forkflat.L
    //2.1 ??pos_boxrot_in_global
    //2.1.1 ?? Pos_Boxrot_in_forkflat
    pos_boxrot_in_forkrot = posbox_in_forkflat;
    x_forkrot_in_forkflat = 0;
    y_forkrot_in_forkflat = 0;
    GetPosTran(pos_boxrot_in_forkrot.x_, pos_boxrot_in_forkrot.y_, pos_boxrot_in_forkrot.angle_,
            x_forkrot_in_forkflat, y_forkrot_in_forkflat, posfork_std_in_forkflat->angle_, &x_boxrot_in_forkflat, &y_boxrot_in_forkflat, &ang_temp, ROTFIRST);
    //2.1.2 ??pos_boxrot_in_veh
    GetPosTran(x_boxrot_in_forkflat, y_boxrot_in_forkflat, ang_temp, posh_forkflat_in_veh.pos_ang_.x_, posh_forkflat_in_veh.pos_ang_.y_,
            posh_forkflat_in_veh.pos_ang_.angle_, &x_boxrot_in_veh, &y_boxrot_in_veh, &ang_temp, TRANFIRST);
    //2.1.3 ??pos_boxrot_in_global
    GetPosTran(x_boxrot_in_veh, y_boxrot_in_veh, ang_temp,
            posveh_in_global.x_, posveh_in_global.y_, posveh_in_global.angle_, &x_boxrot_in_global, &y_boxrot_in_global, &ang_temp, TRANFIRST);
    //2.2 
    GetVertiPos(posbox_in_global.pos_ang_.x_, posbox_in_global.pos_ang_.y_, angdes_box_global, x_boxrot_in_global, y_boxrot_in_global,
            &x_verti, &y_verti, &ang_temp);
    //2.3 
    dist_temp = sqrt(pow(x_boxrot_in_global - x_verti, 2) + pow(y_boxrot_in_global - y_verti, 2));
    posfork_std_in_forkflat->L_ = dist_temp;
    //step 3:
    posfork_std_in_forkflat->h_ = posbox_in_global.h_ - posh_forkflat_in_veh.h_;
    return 0;
}

int GetVertiPos(double x_line, double y_line, double ang_line, double x_out, double y_out,
        double *x_verti, double *y_verti, double *ang_out_verti) {
    double ang_posline_out, dist_pos_verti, dist_pos_out, dir_rot;
    
    ang_posline_out = dist_pos_verti = dist_pos_out = dir_rot = 0.0;
    
    dist_pos_out = sqrt(pow(x_line - x_out, 2) + pow(y_line - y_out, 2));
    ang_posline_out = atan2(y_out - y_line, x_out - x_line);
    dist_pos_verti = dist_pos_out * cos(ang_line - ang_posline_out);
    *x_verti = x_line + dist_pos_verti * cos(ang_line);
    *y_verti = y_line + dist_pos_verti * sin(ang_line);
    dir_rot = sin(ang_line - ang_posline_out) >= 0 ? 1.0 : -1.0;
    *ang_out_verti = NormalAngle(ang_line + dir_rot * 0.5 * Pi);
    return 0;
}