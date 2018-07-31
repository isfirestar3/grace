#pragma once
#include "NodeItem.h"

class MapScene;

class NodeItemManage
{
public:
	NodeItemManage(MapScene* mapScene);
	~NodeItemManage();
public:
	void SetNodeInfo(const std::vector<NODEINFO>& vecNodeInfo);
	NodeItem* AddNodeItem(const QPointF& pos);
	NodeItem* AddNodeItem(const NodeInfo& nodeInfo);
	void AddNodeToVector(NodeItem* nodeItem);
	void GetAllNodeItem(std::vector<NodeItem*>& vecNodeItem);
	NodeItem* GetNodeItem(int iNodeId);
	int GetMaxNodeId();
	void SetItemMoveable(bool bMoveable);
	bool RemoveNodeItem(int node_id);
	bool UpdateNodeItem(int node_id,bool spin,double x,double y);//批量修改用
private:
	MapScene* m_mapScene;

	std::vector<NodeItem*> m_vecNodeItem;
};

