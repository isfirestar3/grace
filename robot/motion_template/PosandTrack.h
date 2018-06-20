#if !defined _POS_AND_TRACK_2016_11_22_PANG_H_
#define _POS_AND_TRACK_2016_11_22_PANG_H_

#include "map.h"
#include "navigation.h"

#if !defined Pi
#define Pi  3.1415926
#endif
#if !defined D2R
#define D2R  0.0174533
#endif 
#if !defined R2D
#define R2D  57.29578
#endif 


#if !defined NECESSARY_DISPERSE_POINT_COUNT
#define NECESSARY_DISPERSE_POINT_COUNT   (201)
#endif

#pragma pack(push, 1)

enum Rot_Tran_t {
    ROTFIRST = 0,
    TRANFIRST,
};

typedef struct {
    position_t pos_ang_; 
    double curve_; 
    double dist_start_; 
    double percent_; 
} track__pos_info_t;

typedef struct {
    position_t pos_ang_; 
    double h_; 
} track__pos_and_h_t;

typedef struct {
    double L_;
    double angle_;
    double h_;
} track__pos_fork_t_;
#pragma pack(pop)

extern
int Sgn(double x);

extern
int GetTClosestOnLine(position_t current_pos, int edge_id, double *output);
extern
int GetTClosestOnLineBycurg(position_t current_pos, const var__edge_t * curgraph, double *output);
extern
int GetTnewbyDistandTcur(double t_cur, double dist, const var__edge_t * curgraph, double *output);
extern
int GetTbydist(double dist, const var__edge_t * curgraph, double *output);
extern
int GetposbyT(double t, const var__edge_t * curgraph, position_t *pos);
extern
int GetPosInfobyTdistExt(double t, double dist, const var__edge_t *curgragh, track__pos_info_t *pos);
extern
int GetAnglebyT(double t, const var__edge_t * curgraph, double *output);
extern
int GetcurvebyT(double t, const var__edge_t * curgraph, double *output);
extern
int GetdistbyT(double t, const var__edge_t * curgraph, double *output);
extern
int GetdistbyTOrg(double t, const var__edge_t * curgraph, double *output);
extern
int GetpercentbyT(double t, const var__edge_t * curgraph, double* per);
extern
int GetPosAllInfobyT(double t, const var__edge_t * curgraph, track__pos_info_t *track_pos);
extern
double NormalAngle(double sita); 
extern

int GetPosTran(double x_inB, double y_inB, double ang_in_B,
        double x_BinA, double y_BinA, double ang_BinA,
        double *x_inA, double *y_inA, double *ang_inA, enum Rot_Tran_t seq);

extern
int GetPosBoxInGlobal(position_t posveh_in_global, track__pos_and_h_t posh_forkflat_in_veh,
        track__pos_fork_t_ posfork_std_in_forkflat, position_t posbox_in_forkflat, double angline_in_global,
        track__pos_and_h_t *posbox_in_global);

extern
int GetVehtoBoxInGlobal(track__pos_and_h_t posbox_in_global, position_t posbox_in_forkflat,
        track__pos_and_h_t posh_forkflat_in_veh, position_t posveh_in_global_init, double *dist_stop, double *ang_veh_in_global);

extern
int GetForkInForkFlat(track__pos_and_h_t posbox_in_global, position_t posbox_in_forkflat,
        track__pos_and_h_t posh_forkflat_in_veh, position_t posveh_in_global, track__pos_fork_t_ *posfork_std_in_forkflat);

extern
int GetVertiPos(double x_line, double y_line, double ang_line, double x_out, double y_out, double *x_verti, double *y_verti, double *ang_out_verti);
#endif
