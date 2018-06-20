#ifndef BASIC_DEFINE_ZHH_20161011
#define BASIC_DEFINE_ZHH_20161011

#include <vector>
#include <memory>

#define FALSE 0
#define TRUE 1

#define BIG_SIZE 1
#define SHAPE_R (10 * BIG_SIZE)
#define PI 3.14159
#define OPT_PARAM_NUM 20
#define OPT_CTRL_NUM 10

enum ItemType
{
	ITEM_NULL = -1,
	ITEM_NODE,
	ITEM_EDGE,
	ITEM_VEHICLE,
	ITEM_TARGET,
	ITEM_ANCHOR
};

enum ActionType
{
	ACTION_SELECT,
	ACTION_HAND_DRAG,
	ACTION_ADD_NODE,
	ACTION_ADD_EDGE,
	ACTION_POINT_LINE,
	ACTION_ADD_VEHICLE,
	ACTION_MOVE_ITEM,
	ACTION_ENTER_NAV,
	ACTION_FORMAT,
	ACTION_ADD_ANCHOR,
	ACTION_LINK_CURVE
};

enum Direction
{
	FORWARD,
	BACKWARD,
	BOTH
};
enum AngleType //车在路线上维持角度的方式（攻角）
{
	CONST_ANGLE,            //与切矢保持恒定角度
	GLOBAL_CONST_ANGLE,     //保持全局恒定角
	VARIABLE_ANGLE,         //与切矢保持变角
	GLOBAL_VAR_ANGLE        //保持全局变角
};

typedef struct NodeInfo
{
	int node_id = 0;
	double pos_x = 0.0;
	double pos_y = 0.0;
	int spin = FALSE;//是否允许旋转，0表示不允许，1表示允许
	int tshaped = FALSE;//是否丁字路口，0不是，1是

	NodeInfo& operator = (const NodeInfo& other)
	{
		if (this == &other)
		{
			return *this;
		}
		node_id = other.node_id;
		pos_x = other.pos_x;
		pos_y = other.pos_y;
		spin = other.spin;
		tshaped = other.tshaped;

		return *this;
	}
}NODEINFO;

typedef struct EdgeSpeed
{
	double percent = 1.0;//所在线段的百分比
	double speed = 0.4;//所在路线位置的速度

	EdgeSpeed& operator = (const EdgeSpeed& other)
	{
		if (this != &other)
		{
			percent = other.percent;
			speed = other.speed;
		}
		return *this;
	}
}EDGESPEED;

typedef struct Point
{
	double x = 0.0;
	double y = 0.0;
	double distance_to_start = 0.0;//此点到起始点的距离，此字段只在边的离散点上使用

	Point& operator = (const Point& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
			distance_to_start = other.distance_to_start;
		}
		return *this;
	}
}POSPOINT;

typedef struct Wop
{
	int id = -1;
	Direction direction = FORWARD;
	AngleType angle_type = CONST_ANGLE;
	double angle = 0.0;

	Wop& operator = (const Wop& other)
	{
		if (this != &other)
		{
			id = other.id;
			direction = other.direction;
			angle_type = other.angle_type;
			angle = other.angle;
		}
		return *this;
	}
}WOP;
typedef struct EdgeInfo
{
	int id = -1;
	int start_node_id = 0;
	int end_node_id = 0;
	int direction = Direction::FORWARD;//边的方向，0正向，1反向，2双向
	int backward = FALSE;//是否允许倒车
	POSPOINT ctrl_point1;
	POSPOINT ctrl_point2;
	POSPOINT termstpos;
	POSPOINT termendpos;
	double terminal_start_percent = 0.0;
	double terminal_end_percent = 0.0;
	int priority = 0;

	//以下为新增加的属性
	int type = 0;  //类型为3阶贝塞尔还是其他算法
	double length = 0.0; //边的长度
	std::vector<EDGESPEED> speed;  //路线分段速度，最多10段
	std::vector<POSPOINT> disperse_point;//边的离散点，默认200个点
	std::vector<int> wop_list; //边所对应的所有WOP的id
	EdgeInfo()
	{
		EDGESPEED edgeSpeed;
		speed.push_back(edgeSpeed);//给线路添加一个默认速度
	}

	EdgeInfo& operator = (const EdgeInfo& other)
	{
		if (this != &other)
		{
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
}EDGEINFO;

typedef struct LinkEdge
{
	int edge_id = 0;
	int wop_id = 0;

	LinkEdge& operator = (const LinkEdge& other)
	{
		if (this != &other)
		{
			edge_id = other.edge_id;
			wop_id = other.wop_id;
		}
		return *this;
	}
}LINKEDGE;

typedef struct Link
{
	int id;
	LINKEDGE link_from;
	LINKEDGE link_to;
	double convert_cost = 0.0; //wop转换的代价，主要用于如果车可以在节点原地旋转的话，旋转的角度的时间代价，换算成每30度用1米表示

	Link& operator = (const Link& other)
	{
		if (this != &other)
		{
			link_from = other.link_from;
			link_to = other.link_to;
			convert_cost = other.convert_cost;
		}
		return *this;
	}

}LINK;

typedef struct PosXya
{
	double pos_x = 0.0;
	double pos_y = 0.0;
	double angle = 0.0;
	PosXya& operator = (const PosXya& other)
	{
		if (this == &other)
		{
			return *this;
		}
		pos_x = other.pos_x;
		pos_y = other.pos_y;
		angle = other.angle;
		return *this;
	}
}POSXYA;
typedef struct VehicleInfo
{
	int vehicle_id = -1;
	std::string vehicle_name = "";
	std::string vehicle_ip = "127.0.0.1";
	int vehicle_port = 409;
	double pos_x = 0.0;
	double pos_y = 0.0;
	double angle = 90.0;
	double speed = 0.4;//当前车速
	double speed_min = 0.1;//最小车速
	double speed_max = 0.8;//最大车速
	double wspeed = 0.2;//角速度
	int status = 0; //0: 空闲状态，1：任务执行中，2：任务暂停中，3：不在线
	int edge_id = 0; //车所在边的ID
	VehicleInfo& operator = (const VehicleInfo& other)
	{
		if (this == &other)
		{
			return *this;
		}
		vehicle_id = other.vehicle_id;
		angle = other.angle;
		speed = other.speed;
		speed_min = other.speed_min;
		speed_max = other.speed_max;
		wspeed = other.wspeed;
		status = other.status;
		pos_x = other.pos_x;
		pos_y = other.pos_y;
		vehicle_name = other.vehicle_name;
		vehicle_ip = other.vehicle_ip;
		vehicle_port = other.vehicle_port;
		edge_id = other.edge_id;
		return *this;
	}
}VEHICLEINFO;

typedef struct TargetInfo
{
	int id = 0;
	int edge_id = 0;
	int wop_id = 0;
	double pos_x = 0.0;
	double pos_y = 0.0;
	double angle = 0.0;
	TargetInfo& operator = (const TargetInfo& other)
	{
		if (this != &other)
		{
			id = other.id;
			edge_id = other.edge_id;
			wop_id = other.wop_id;
			pos_x = other.pos_x;
			pos_y = other.pos_y;
			angle = other.angle;
		}
		return *this;
	}
}TARGETINFO;

typedef struct OptInfo
{
	int id = 0;
	int type = 0;
	uint64_t params[OPT_PARAM_NUM];
	OptInfo()
	{
		for (int i = 0; i < OPT_PARAM_NUM; ++i)
		{
			params[i] = 0;
		}
	}
	OptInfo& operator = (const OptInfo& other)
	{
		if (this != &other)
		{
			id = other.id;
			type = other.type;
			for (int i = 0; i < OPT_PARAM_NUM; ++i)
			{
				params[i] = other.params[i];
			}
		}
		return *this;
	}
	void clear_param()
	{
		for (int i = 0; i < OPT_PARAM_NUM; ++i)
		{
			params[i] = 0;
		}
	}
}OPTINFO;

typedef struct OptCtrlInfo
{
	int code = 0;
	int mask = 0;
	uint64_t params[OPT_CTRL_NUM];
	OptCtrlInfo()
	{
		for (int i = 0; i < OPT_CTRL_NUM; ++i)
		{
			params[i] = 0;
		}
	}
	OptCtrlInfo& operator = (const OptCtrlInfo& other)
	{
		if (this != &other)
		{
			code = other.code;
			mask = other.mask;
			for (int i = 0; i < OPT_CTRL_NUM; ++i)
			{
				params[i] = other.params[i];
			}
		}
		return *this;
	}
}OPTCTRLINFO;

typedef struct AnchorInfo
{
	int id = 0;
	int edge_id = 0;
	int wop_id = 0;
	double pos_x = 0.0;
	double pos_y = 0.0;
	double angle = 0.0;
	double percent = 0.0;
	double aoa = 0.0; //upl的攻角
	std::vector<OPTINFO> opts;
	AnchorInfo& operator = (const AnchorInfo& other)
	{
		if (this != &other)
		{
			id = other.id;
			edge_id = other.edge_id;
			wop_id = other.wop_id;
			pos_x = other.pos_x;
			pos_y = other.pos_y;
			angle = other.angle;
			percent = other.percent;
			aoa = other.aoa;
			opts = other.opts;
		}
		return *this;
	}
}ANCHORINFO;

typedef struct LayoutData
{
	int type = 0;//路径类型，0贝塞尔曲线
	int search_type = 0;
	std::vector<NODEINFO> nodeList;
	std::vector<EDGEINFO> edgeList;
	std::vector<WOP> wopList;
	std::vector<LINK> linkList;

	LayoutData& operator = (const LayoutData& other)
	{
		if (this != &other)
		{
			type = other.type;
			search_type = other.search_type;
			nodeList = other.nodeList;
			edgeList = other.edgeList;
			wopList = other.wopList;
			linkList = other.linkList;
		}
		return *this;
	}
}LAYOUTDATA;

typedef struct ConfigData
{
	int search_type = 1; //路径搜索算法类型,0:floyd，1:dijkstra
	std::vector<VEHICLEINFO> vehicleList; //获取配置文件中的车辆
	ConfigData& operator = (const ConfigData& other)
	{
		if (this != &other)
		{
			search_type = other.search_type;
			vehicleList = other.vehicleList;
		}
		return *this;
	}
}CONFIGDATA;

typedef struct DockData
{
	std::vector<ANCHORINFO> anchorList;
	DockData& operator = (const DockData& other)
	{
		if (this != &other)
		{
			anchorList = other.anchorList;
		}
		return *this;
	}
}DOCKDATA;

#endif