#if !defined VDECOM_H_
#define VDECOM_H_

#include "wheel.h"
#include "drive_unit.h"
#include "vehicle.h"
#include "var.h"
#include "navigation.h"
#include "clist.h"

enum TRIM_TYPE {
    NON = 0,
    DEC_DSTOP,
    ONLY_ACC,
    VEHICLE_LIMIT,
};

typedef struct _v_wheel {
    enum drive_unit_type_t _w_type;
    int index_drive;
    char left_or_right; //true表示left
    double _x; //轮子在整车车体坐标系中的位置
    double _y;
    double _v_x; //初步分解出的速度
    double _v_y;
    double _ae; //分解后得到的该轮期望角度
    double _af; //获得的该轮子的实际角度
    double _vp; //初步分解出的速度在实际角度方向上的投影值
    double _vo; //优化后该轮的速度
    double _roll_wt; //滚动系数
    double _slide_wt; //滑动系数。注：对于SWheel来说，这两个系数为零，计算出的速度也不下发，在Fscore中并不体现S轮的速度
    double _gauge;
    double _min_angle;
    double _max_angle;
    uint64_t time_stamp;
    uint64_t s_time_stamp;
    double max_speed;
	double _zero_angle;
    var__functional_object_t *_swheel_obj;
    var__functional_object_t *_dwheel_1_obj;
    var__functional_object_t *_dwheel_2_obj;
    var__functional_object_t *_sddwheel_obj;
} v_wheel;

//速度合成

typedef struct _v_chassis {
    double V_x;
    double V_y;
    double W;
    uint64_t time_stamp;
} v_chassis;

typedef struct v_wheel_with_time_ {

    union {
        double v_w;
        double angle;
    };
    uint64_t time_stamp;
} v_wheel_with_time;

void data_w_log(v_wheel * v_wheels, int num_wheel);
int v_composition(v_wheel * v_wheels, int num_wheel, int ref_enc, v_chassis *out); //速度合成
void sdd_v_composition(v_wheel * v_wheels, int num_wheel, int num_dri_unit, int ref_enc, v_chassis *out);
void first_v_decomposite(v_wheel *v_w, const struct list_head *drive_unit, var__vehicle_t *veh);
void DoOptimize(double *Xresult, int N_wh, v_wheel* vwheels, double* Xinit, char *pval);
int GetOpti_va(int N_wh, v_wheel *vw, double* va_init);
void WriteToGlobal(int N_wh, v_wheel * vwheels);
void v_wheel_limited(int N_wh, v_wheel * vwheels);
double trim_solo_only_acc(double v_x, double v_x_last, double v_acc);
extern
int VwResolution(v_chassis *V_com, var__vehicle_t *veh, const struct list_head *drive_unit);
extern
void trim(double v_x, double v_y, double w, var__navigation_t *nav, var__vehicle_t *veh, double deta_time, int trim_type, velocity_t *result);
extern
void trim2(double v, var__navigation_t *nav, var__vehicle_t *veh, double deta_time, int trim_type, double *result);
extern
void EnableDrive_unit(const struct list_head *drive_unit, int aa);

#endif
