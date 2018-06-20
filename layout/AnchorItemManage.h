#pragma once
#include "AnchorItem.h"

class MapScene;

class AnchorItemManage
{
public:
	AnchorItemManage(MapScene* pMapScene);
	~AnchorItemManage();
public:
	void AddAnchorItem(const QPointF& pos, const EDGEINFO& edgeInfo);
	AnchorItem* AddAnchorItem(const ANCHORINFO& anchorInfo, const EDGEINFO& edgeInfo);
	void SetAnchorInfo(const std::vector<ANCHORINFO>& vecAnchor);
	void RemoveAllTargetItem();
	bool RemoveAnchorItem(int anchor_id);
	//根据边id更新边上所有停靠点的信息，用于切割边以后停靠点所在的边变化的情况
	void UpdateAnchorInfo(const ANCHORINFO& anchorInfo);
	void GetAnchorInfoOnEdge(int edge_id,std::vector<ANCHORINFO>& vecAnchor);
private:
	int GetMaxAnchorId();
	void AddAnchorItemToVector(AnchorItem* pAnchorItem);
private:
	MapScene* m_mapScene = nullptr;
	std::vector<AnchorItem*> m_vecAnchorItem;
	
};

