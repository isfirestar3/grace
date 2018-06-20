#pragma once
#include "DataManage.h"
#include "PosandTrack.h"
#include "AlgorithmDefine.hpp"

class Links
{
private:
	Links(){}
	~Links(){}
public:
	static Links* instance()
	{
		static Links link_instance;
		return &link_instance;
	}
private:
	std::vector<NODEINFO> _node_list;
public:
	void SetNodeList(const std::vector<NODEINFO> node_list)
	{
		_node_list = node_list;
	}
	void ComputeLinks(const std::vector<EDGEINFO>& vecEdgeInfo, bool bSpin, std::vector<LINK>& vecLink)
	{
		for (auto &edgeCur : vecEdgeInfo)
		{
			for (auto &edgeLink : vecEdgeInfo)
			{
				//当前边的终点是邻接边的起点,或者当前边的终点也是邻接边的终点
				if (edgeCur.end_node_id == edgeLink.start_node_id ||
					(edgeCur.end_node_id == edgeLink.end_node_id /*&& edgeCur.start_node_id != edgeLink.start_node_id*/))
				{
					//如果整体允许旋转，那么判断两条边之间的节点是否可旋转,否则，直接整体都不允许旋转
					bool nodeSpin = bSpin;
					if (nodeSpin){
						nodeSpin = GetNodeSpin(edgeCur.end_node_id);
					}
					ComputeLinksForward(edgeCur, edgeLink, nodeSpin, vecLink);
				}
				//当前边的起点是邻接边的终点,或者当前边的起点也是邻接边的起点
				if (edgeCur.start_node_id == edgeLink.end_node_id ||
					(edgeCur.start_node_id == edgeLink.start_node_id/* && edgeCur.end_node_id != edgeLink.end_node_id*/))
				{
					//如果整体允许旋转，那么判断两条边之间的节点是否可旋转,否则，直接整体都不允许旋转
					bool nodeSpin = bSpin;
					if (nodeSpin){
						nodeSpin = GetNodeSpin(edgeCur.start_node_id);
					}
					ComputeLinksBackward(edgeCur, edgeLink, nodeSpin, vecLink);
				}
			}
		}
	}
private:
	bool GetNodeSpin(int nodeId)
	{
		int spin = 1;
		for (auto nodeInfo : _node_list)
		{
			if (nodeInfo.node_id == nodeId)
			{
				spin = nodeInfo.spin;
				break;
			}
		}
		return spin;
	}
	double PointAngle(const EDGEINFO& edgeInfo, double t)
	{
		var__edge_t edge_t;
		PT_C::ED(edgeInfo,edge_t);
		double angle;
		GetAnglebyT(t, &edge_t, &angle);
		angle = angle * 180 / 3.14159;//此函数返回的是弧度单位，需要换算成角度
		if (angle < 0.0)
		{
			angle += 360;
		}
		return angle;
	}

	double cost(const EDGEINFO& edgeCur, const EDGEINFO& edgeLink, const WOP& wopCur, const WOP& wopLink)
	{
		double edgeCurAngle = PointAngle(edgeCur, 1.0);
		if (wopCur.direction == BACKWARD)
		{
			edgeCurAngle = PointAngle(edgeCur, 0.0);
		}
		double edgeLinkAngle = PointAngle(edgeLink, 0.0);
		if (wopLink.direction == BACKWARD)
		{
			edgeLinkAngle = PointAngle(edgeLink, 1.0);
		}
		double costAngle = 0.0;
		if (wopCur.angle_type == AngleType::CONST_ANGLE && wopLink.angle_type == AngleType::CONST_ANGLE)
		{
			costAngle = (edgeLinkAngle + wopLink.angle) - (edgeCurAngle + wopCur.angle);
		}
		if (wopCur.angle_type == AngleType::GLOBAL_CONST_ANGLE && wopLink.angle_type == AngleType::CONST_ANGLE)
		{
			costAngle = (edgeLinkAngle + wopLink.angle) - wopCur.angle;
		}
		if (wopCur.angle_type == AngleType::CONST_ANGLE && wopLink.angle_type == AngleType::GLOBAL_CONST_ANGLE)
		{
			costAngle = wopLink.angle - (edgeCurAngle + wopCur.angle);
		}
		if (wopCur.angle_type == GLOBAL_CONST_ANGLE && wopLink.angle_type == GLOBAL_CONST_ANGLE)
		{
			costAngle = wopLink.angle - wopCur.angle;
		}
		costAngle = fabs(costAngle);//取角度差的正值
		while (costAngle > 180.0)  //将角度转换为0-180以内的值
		{
			costAngle = fabs(360.0 - costAngle);
		}
		double cost = costAngle / 30.0;//每30度换算成1米（m）
		return (cost * 100.0);
	}

	void AddLink(const EDGEINFO& edgeCur, const EDGEINFO& edgeLink, const WOP& wopCur, const WOP& wopLink, bool bSpin, std::vector<LINK>& vecLink)
	{
		LINK link;
		link.id = static_cast<int>(vecLink.size()) + 1;
		link.link_from.edge_id = edgeCur.id;
		link.link_from.wop_id = wopCur.id;
		link.link_to.edge_id = edgeLink.id;
		link.link_to.wop_id = wopLink.id;
		if (bSpin)
		{
			link.convert_cost = cost(edgeCur, edgeLink, wopCur, wopLink);
		}
		vecLink.push_back(link);
	}

	void AddLinkWithDirection(const EDGEINFO& edgeCur, const EDGEINFO& edgeLink, const WOP& wopCur, const WOP& wopLink, bool bSpin,
		std::vector<LINK>& vecLink)
	{
		if (bSpin) //如果车可以进行原地旋转，则两条边可以link
		{
			AddLink(edgeCur, edgeLink, wopCur, wopLink, true, vecLink);
		}
		else
		{
			//如果两条wop的角度类型如何，只要转换角度小于30度，都可以link
			double edgeCurAngle = PointAngle(edgeCur, 1.0);
			if (wopCur.direction == BACKWARD)
			{
				edgeCurAngle = PointAngle(edgeCur, 0.0);
			}
			double edgeLinkAngle = PointAngle(edgeLink, 0.0);
			if (wopLink.direction == BACKWARD)
			{
				edgeLinkAngle = PointAngle(edgeLink, 1.0);
			}
			double costAngle = 0.0;
			costAngle = fabs((edgeLinkAngle + wopLink.angle) - (edgeCurAngle + wopCur.angle));
			while (costAngle > 360.0)  //将角度转换为0-360以内的值
			{
				costAngle = fabs(360.0 - costAngle);
			}
			if (costAngle < 30.0 ||
				costAngle > 330.0)//角度转换小于30度
			{
				AddLink(edgeCur, edgeLink, wopCur, wopLink, false, vecLink);
			}
		}
	}

	void ComputeLinksForward(const EDGEINFO& edgeCur, const EDGEINFO& edgeLink, bool bSpin, std::vector<LINK>& vecLink)
	{
		for (auto &wopCurId : edgeCur.wop_list)
		{
			WOP wopCur;
			DATAMNG->GetWopById(wopCurId, wopCur);
			for (auto &wopLinkId : edgeLink.wop_list)
			{
				WOP wopLink;
				DATAMNG->GetWopById(wopLinkId, wopLink);
				//如果当前边是正向走，邻接边也是正向走，那么当前边的终点必须是邻接边的起点，否则不能link
				//如果当前边是正向走，邻接边是反向走，那么当前边的终点必须是邻接边的终点，否则不能link
				if (wopCur.direction == FORWARD &&  wopLink.direction == FORWARD && edgeCur.end_node_id == edgeLink.start_node_id ||
					wopCur.direction == FORWARD && wopLink.direction == BACKWARD && edgeCur.end_node_id == edgeLink.end_node_id)
				{
					AddLinkWithDirection(edgeCur, edgeLink, wopCur, wopLink, bSpin, vecLink);
				}
			}
		}
	}
	void ComputeLinksBackward(const EDGEINFO& edgeCur, const EDGEINFO& edgeLink, bool bSpin, std::vector<LINK>& vecLink)
	{
		for (auto &wopCurId : edgeCur.wop_list)
		{
			WOP wopCur;
			DATAMNG->GetWopById(wopCurId, wopCur);
			for (auto &wopLinkId : edgeLink.wop_list)
			{
				WOP wopLink;
				DATAMNG->GetWopById(wopLinkId, wopLink);
				//如果当前边是反向走，邻接边是正向走，那么当前边的起点必须是邻接边的起点，否则不能link
				//如果当前边是反向走，邻接边也是反向走，那么当前边的起点必须是邻接边的终点，否则不能link
				if (wopCur.direction == BACKWARD &&  wopLink.direction == FORWARD && edgeCur.start_node_id == edgeLink.start_node_id ||
					wopCur.direction == BACKWARD && wopLink.direction == BACKWARD && edgeCur.start_node_id == edgeLink.end_node_id)
				{
					AddLinkWithDirection(edgeCur, edgeLink, wopCur, wopLink, bSpin, vecLink);
				}
			}
		}
	}

};