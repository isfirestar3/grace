#if !defined NAVMAP_H
#define NAVMAP_H

#include "vartypes.h"
#include "avltree.h"

#pragma pack(push ,1)

// 单位用 mm
typedef struct {
    int id_;
    position_t pos_;
} var__map_node_t;

typedef struct {
    double percent_; // 0-100
    double lim_speed_; // m/s
} var__speed_limit_t;

typedef struct {
    struct avltree_node_t index_;
    int id_;
    int start_node_id_;
    int end_node_id_;
    position_t control_point_1_;
    position_t control_point_2_;
    position_t start_point_;
    position_t end_point_;
    double length_; // 边的长度单位 m
    var__vector_t speed_limit_; // var__speed_limit_t[] 分段限速相关
    var__vector_t disperse_points_; // position_t [] 离散点定义， 实施软件计算完成后， 填入文件， 下发到标准平台
    var__vector_t wops_; // var__edge_wop_properties_t [] 集合
} var__edge_t;

typedef struct {
    struct avltree_node_t index_;
    int id_;
    int direction_;
    int angle_type_;
    double angle_; // 弧度
} var__way_of_pass_t;

typedef struct {
    int id_;
    int from_edge_;
    int from_wop_;
    int to_edge_;
    int to_wop_;
} var__edge_link_t;

typedef struct {
    var__vector_t nodes_; // var__map_node_t[]
    var__vector_t edges_; // var__edge_t []
    int map_type_; // 全局使用何种曲线？三阶贝塞尔？B样条 
    var__vector_t wops_; // var__way_of_pass_t[]; 所有的WOP合集
    var__vector_t links_; // var__edge_link_t[] 链接表
    struct avltree_node_t *wop_searcher_root_;
    struct avltree_node_t *edge_searcher_root_;
} var__map_layout_t;

#pragma pack(pop)

#if !defined __cplusplus

extern
int var__load_layout();

/* 获取地图布局的原始变量区指针, 使用完成后需手动调用 var__release_object_reference 显式释放
 */
extern
var__map_layout_t *var__get_layout();

extern
int var__load_layout_ffile();

/* var__search_edge 或 var__search_wop 调用后， 需手动调用 var__end_search 释放搜索资源 
 */
extern
const var__edge_t * const var__search_edge(int id);
extern
const var__way_of_pass_t * const var__search_wop(int id);
extern
void var__end_search();

#endif
#endif