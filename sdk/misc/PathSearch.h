#pragma once
#include<vector>

#ifdef _USRDLL
#define METH dllexport
#else
#define METH dllimport
#endif
#include <stdint.h>


#if !defined EXPS_FUNCTION
#if _WIN32
#if _USRDLL
#define EXPS_FUNCTION(__ret_type__) extern "C" __declspec ( dllexport ) __ret_type__ __stdcall
#else
#define EXPS_FUNCTION(__ret_type__) extern "C" __declspec ( dllimport ) __ret_type__ __stdcall
#endif
#else
#define EXPS_FUNCTION(type)  extern "C" type
#endif
#endif
enum PathSearch_Type
{
	PathSearch_FLOYD,
	PathSearch_DIJKSTRA,
};

enum PathSearch_Direction
{
	PathSearch_FORWARD,
	PathSearch_BACKWARD,
	//PathSearch_BOTH
};
enum PathSearch_AngleType //车在路线上维持角度的方式（攻角）
{
	PathSearch_CONST_ANGLE,            //与切矢保持恒定角度
	//VARIABLE_ANGLE,         //与切矢保持变角
	PathSearch_GLOBAL_CONST_ANGLE,     //保持全局恒定角
	//GLOBAL_VAR_ANGLE        //保持全局变角
};

struct PathSearch_Wop
{
	unsigned int           wop_id;
	unsigned int           edge_id;
	PathSearch_Direction   direction;
	PathSearch_AngleType   angle_type;
	double                 angle ;
	float                  distance;

	PathSearch_Wop()
	{

	}
	
	PathSearch_Wop( const PathSearch_Wop &lref )
	{
		
		wop_id = lref.wop_id;
			edge_id = lref.edge_id;
			direction = lref.direction;
			angle_type = lref.angle_type;
			angle = lref.angle;
			distance = lref.distance;
	}

	bool operator < ( const PathSearch_Wop& other ) const
	{
		if( edge_id < other.edge_id ) {
			return true;
		} else {
			if( wop_id < other.wop_id ) {
				return true;
			} else {
				if( distance < other.distance ) {
					return true;
				} else {
					if( angle < other.angle ) {
						return true;
					} else {
						if( angle_type < other.angle_type ) {
							return true;
						} else {
							if( direction < other.direction ) {
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
};

typedef struct PathSearch_LinkEdge
{
	int edge_id;
	int wop_id;
}PathSearch_LINKEDGE;

typedef struct PathSearch_Link
{
	int link_id;
	PathSearch_LinkEdge link_from;
	PathSearch_LinkEdge link_to;
	double convert_cost; //wop转换的代价，主要用于如果车可以在节点原地旋转的话，旋转的角度的时间代价，换算成每30度用1米表示
}PathSearch_LINK;

typedef struct PathSearch_Upl
{
	unsigned int wop_id;
	unsigned int edge_id;
	float fPercent ;
	float fAoa ;

	PathSearch_Upl& operator = (const PathSearch_Upl& other)
	{
		if (this != &other) {
			wop_id = other.wop_id;
			edge_id = other.edge_id;
			fPercent = other.fPercent;
			fAoa = other.fAoa;
		}
		return *this;
	}

}PathSearch_UPL;

typedef struct PathSearch_Position
{
	float x;
	float y;
	float angle;
}PathSearch_POSITION;

typedef struct PathSearch_Point
{
	double x = 0.0;
	double y = 0.0;
	double distance_to_start = 0.0;//此点到起始点的距离，此字段只在边的离散点上使用

	PathSearch_Point& operator = ( const PathSearch_Point& other )
	{
		if( this != &other ) {
			x = other.x;
			y = other.y;
			distance_to_start = other.distance_to_start;
		}
		return *this;
	}
}PathSearch_POSPOINT;

typedef struct PathSearch_EdgeSpeed
{
	double percent = 0.0;//所在线段的百分比
	double speed = 0.4;//所在路线位置的速度

	PathSearch_EdgeSpeed& operator = ( const PathSearch_EdgeSpeed& other )
	{
		if( this != &other ) {
			percent = other.percent;
			speed = other.speed;
		}
		return *this;
	}
}PathSearch_EDGESPEED;

typedef struct PathSearch_EdgeInfo
{
	int id = -1;
	int start_node_id = 0;
	int end_node_id = 0;
	int direction = PathSearch_Direction::PathSearch_FORWARD;//边的方向，0正向，1反向，2双向
	int backward = false;//是否允许倒车
	PathSearch_POSPOINT ctrl_point1;
	PathSearch_POSPOINT ctrl_point2;
	PathSearch_POSPOINT termstpos;
	PathSearch_POSPOINT termendpos;
	double terminal_start_percent = 0.0;
	double terminal_end_percent = 0.0;
	int priority = 0;

	//以下为新增加的属性
	int type = 0;  //类型为3阶贝塞尔还是其他算法
	double length = 0.0; //边的长度
	std::vector<PathSearch_EDGESPEED> speed;  //路线分段速度，最多10段
	std::vector<PathSearch_POSPOINT> disperse_point;//边的离散点，默认200个点
	std::vector<int> wop_list; //边所对应的所有WOP的id

	PathSearch_EdgeInfo& operator = ( const PathSearch_EdgeInfo& other )
	{
		if( this != &other ) {
			start_node_id = other.start_node_id;
			end_node_id = other.end_node_id;
			direction = other.direction;
			backward = other.backward;
			ctrl_point1 = other.ctrl_point1;
			ctrl_point2 = other.ctrl_point2;
			termstpos = other.termstpos;
			termendpos = other.termendpos;
			terminal_start_percent = other.terminal_start_percent;
			terminal_end_percent = other.terminal_end_percent;
			priority = other.priority;

			id = other.id;
			type = other.type;
			length = other.length;
			speed = other.speed;
			disperse_point = other.disperse_point;
			wop_list = other.wop_list;
		}
		return *this;
	}
}PathSearch_EDGEINFO;

typedef struct PathSearch_NodeInfo
{
	int node_id = 0;
	double pos_x = 0.0;
	double pos_y = 0.0;
	int spin = false;//是否允许旋转，0表示不允许，1表示允许
	int tshaped = false;//是否丁字路口，0不是，1是

	PathSearch_NodeInfo& operator = ( const PathSearch_NodeInfo& other )
	{
		if( this == &other ) {
			return *this;
		}
		node_id = other.node_id;
		pos_x = other.pos_x;
		pos_y = other.pos_y;
		spin = other.spin;
		tshaped = other.tshaped;

		return *this;
	}
}PathSearch_NODEINFO;

typedef struct PathSearch_LayoutData
{
	int type = 0;//路径类型，0贝塞尔曲线
	std::vector<PathSearch_NODEINFO> nodeList;
	std::vector<PathSearch_EDGEINFO> edgeList;
	std::vector<PathSearch_Wop> wopList;
	std::vector<PathSearch_LINK> linkList;

	PathSearch_LayoutData& operator = ( const PathSearch_LayoutData& other )
	{
		if( this != &other ) {
			type = other.type;
			nodeList = other.nodeList;
			edgeList = other.edgeList;
			wopList = other.wopList;
			linkList = other.linkList;
		}
		return *this;
	}
}PathSearch_LAYOUTDATA;

#define PathSearch_GetUserId PathSearch_GetUserId
#define PathSearch_SetType PathSearch_SetType
#define PathSearch_OpenMap PathSearch_OpenMap
#define PathSearch_LoadData PathSearch_LoadData
#define PathSearch_SetWops PathSearch_SetWops
#define PathSearch_SetLinks	PathSearch_SetLinks
#define PathSearch_SetEdges PathSearch_SetEdges
#define PathSearch_StartFolyd PathSearch_StartFolyd 
#define PathSearch_GetPath PathSearch_GetPath
#define PathSearch_GetPathByPos PathSearch_GetPathByPos
#define PathSearch_GetPathBySPosAndEUpl PathSearch_GetPathBySPosAndEUpl
#define PathSearch_GetPathByWop PathSearch_GetPathByWop
#define PathSearch_ReleasePath PathSearch_ReleasePath
#define PathSearch_GetPathAtWeight PathSearch_GetPathAtWeight
#define PathSearch_Release PathSearch_Release

/*加载新地图获取新的ID*/
EXPS_FUNCTION(int) PathSearch_GetUserId();
/*选择搜索类型，须选择floyd或者dijkstra*/	
EXPS_FUNCTION(void) PathSearch_SetType(const int iUserId, const PathSearch_Type type);
/* 打开地图，读取数据 */
EXPS_FUNCTION( int ) PathSearch_OpenMap(const char* pFilePath, PathSearch_LayoutData* pLayoutData );
/*调用此函数后，就不需要调用PathSearch_SetWops，PathSearch_SetLinks，PathSearch_SetEdges*/
EXPS_FUNCTION(int) PathSearch_LoadData(const int iUserId, const PathSearch_LayoutData* pLayoutData);
/*将所有边对应wop设置进去*/
EXPS_FUNCTION(int) PathSearch_SetWops(const int iUserId, const PathSearch_Wop* pWops, const unsigned int iLength);
EXPS_FUNCTION(int) PathSearch_SetLinks(const int iUserId, const PathSearch_Link* pLinks, const unsigned int iLength);
EXPS_FUNCTION(int) PathSearch_SetEdges(const int iUserId, const PathSearch_EdgeInfo* pEdges, const unsigned int iLength);
/*如果是Folyd，则调用回调计算进度，如果dijkstra则直接返回0；pFloydPath为生成的路径文件*/
EXPS_FUNCTION(int) PathSearch_StartFolyd(const int iUserId, const char* pFloydPath, int(*FinishLevelCB)(const unsigned int iPercent, const void *pUsr), void* pUsr);
/*不同形式的路径搜索*/
EXPS_FUNCTION(int) PathSearch_GetPath(const int iUserId, const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance);
EXPS_FUNCTION(int) PathSearch_GetPathByPos(const int iUserId, const PathSearch_Position stStartPos, const PathSearch_Position stEndPos, PathSearch_Wop** path, int* iLength, float* fDistance);
EXPS_FUNCTION(int) PathSearch_GetPathBySPosAndEUpl(const int iUserId, const PathSearch_Position stStartPos, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance);
EXPS_FUNCTION(int) PathSearch_GetPathByWop(const int iUserId, const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance);
EXPS_FUNCTION(void) PathSearch_ReleasePath(const int iUserId, PathSearch_Wop * path);
EXPS_FUNCTION(int) PathSearch_GetPathAtWeight(const int iUserId, const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance, const int* pWeightEdgesId, const unsigned int iWeightEdgesNum);
EXPS_FUNCTION(void) PathSearch_Release();