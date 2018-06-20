#pragma once
#include "BasicDefine.h"
#include <mutex>
#include <map>
#include <atomic>
#include "PosandTrack.h"
#include "AlgorithmDefine.hpp"

class DataManage
{
private:
	DataManage();
	~DataManage();
public:
	static DataManage* GetInstance()
	{
		static DataManage dataMngIns;
		return &dataMngIns;
	}
public:
	void AddNodeInfo(const NODEINFO& nodeInfo);
	void AddEdgeInfo(EDGEINFO& edgeInfo);
	void AddQRcodeInfo(CODEPOSXYA&posInfo);
	bool UpdateNodeInfo(const NODEINFO& nodeInfo);
	bool UpdateEdgeInfo(EDGEINFO& edgeInfo);
	void GetAllNodeInfo(std::vector<NODEINFO>& vecNodeInfo);
	void GetAllEdgeInfo(std::vector<EDGEINFO>& vecEdgeInfo);
	void GetAllCodeInfo(std::vector<CODEPOSXYA>&vecCodeInfo);
	void SetAllNodeInfo(const std::vector<NODEINFO>& vecNodeInfo);
	void SetAllEdgeInfo(const std::vector<EDGEINFO>& vecEdgeInfo);
	bool DeleteEdgeInfo(int id);
	bool DeleteNodeInfo(int id);
	bool DeletePosInfo(int id);

	void GetLayoutData(LayoutData& layoutData);
	void SetLayoutData(const LayoutData& layoutData);
	void SetOperData(const DOCKDATA& dockData);
	void GetOperData(DOCKDATA& dockData);
	void SetConfigData(const CONFIGDATA& configData);
	void GetConfigData(CONFIGDATA& configData);
	bool IsWopExist(const WOP& wop);
	void GetAllWopInfo(std::vector<WOP>& vecWopInfo);
	void SetAllWopInfo(const std::vector<WOP>& vecWopInfo);
	bool GetWopById(int id, WOP& wop);
	void SetAllLinkInfo(const std::vector<LINK>& vecLinkInfo);
	void GetAllLinkInfo(std::vector<LINK>& vecLinkInfo);
	bool GetEdgeById(int id, EDGEINFO& edgeInfo);
	void DispersePoint();
	void DispersePoint(EDGEINFO& edge_info);
	void ConvertEdgeInfo(const EDGEINFO& edgeInfo, var__edge_t& edge_t);

	void SetAllTargetInfo(const std::vector<TARGETINFO>& vecTargetInfo);
	void GetAllTargetInfo(std::vector<TARGETINFO>& vecTargetInfo);
	void AddTargetInfo(const TARGETINFO& targetInfo);
	bool GetTargetInfo(int id, TARGETINFO& targetInfo);
	bool DeleteTargetInfo(int id);
	void DeleteAllTargetInfo();
	int GetTargetSize();
	void AddAnchorInfo(const ANCHORINFO& anchorInfo);
	bool DeleteAnchorInfo(int anchor_id);
	void GetAllAnchorInfo(std::vector<ANCHORINFO>& vecAnchor);
	void SetAllAnchorInfo(const std::vector<ANCHORINFO>& vecAnchor);
	bool ChangeAnchorToTarget(int anchorId);

	void SetAllVehicleInfo(const std::vector<VEHICLEINFO>& vecVehicle);
	void GetAllVehicleInfo(std::vector<VEHICLEINFO>& vecVehicle);
	bool GetVehicleInfo(int vehicle_id, VEHICLEINFO& vehicleInfo);
	void AddVehicleInfo(const VEHICLEINFO& vehicleInfo);

	bool SetVehiclePos(const std::string& vehicleIp, const POSXYA& pos);
	bool GetVehiclePos(const std::string& vehicleIp, POSXYA& pos);
	bool SetVehicleUpl(const std::string& vehicleIp, const UPL& upl);
	bool GetVehicleUpl(const std::string& vehicleIp, UPL& upl);
	void SetTaskStatus(int status);
	int  GetTaskStatus();

	void SetDefaultWop();

public:
	std::recursive_mutex m_mutexNodeInfo;
	std::recursive_mutex m_mutexEdgeInfo;
	std::recursive_mutex m_mutexWopInfo;
	std::recursive_mutex m_mutexLinkInfo;
	std::recursive_mutex m_mutexTargetInfo;
	std::recursive_mutex m_mutexAnchorInfo;
	std::recursive_mutex m_mutexPosInfo;

	std::vector<NODEINFO> m_vecNodeInfo;
	std::vector<EDGEINFO> m_vecEdgeInfo;
	std::vector<WOP> m_vecWopInfo;
	std::vector<LINK> m_vecLinkInfo;
	std::vector<TARGETINFO> m_vecTargetInfo;
	std::vector<ANCHORINFO> m_vecAnchorInfo;
	std::vector<CODEPOSXYA>m_vctCodeInfo;

	//一些网络交互的实时数据
	std::recursive_mutex m_mutexVehicle;
	std::vector<VEHICLEINFO> m_vecVehicleInfo;//存放所有车信息的列表
	std::recursive_mutex m_mutexVehiclePos;
	std::map<std::string, POSXYA> m_vehicle_pos;//存放所有车的当前位置
	std::recursive_mutex m_mutexVehicleUpl;
	std::map<std::string, UPL> m_vecVehicleUpl; //存放所有车的当前UPL

	std::atomic<int> m_search_type = 0;
	std::atomic<int> m_task_status = 0;
};

#define DATAMNG DataManage::GetInstance() 

