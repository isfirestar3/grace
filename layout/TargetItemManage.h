#pragma once
#include "TargetItem.h"

class MapScene;

class TargetItemManage
{
public:
	TargetItemManage(MapScene* mapScene);
	~TargetItemManage();
public:
	void AddTargetItem(const QPointF& pos, const EDGEINFO& edgeInfo);
	TargetItem* AddTargetItem(const TARGETINFO& targetInfo, const EDGEINFO& edgeInfo);
	void AddTargetToVector(TargetItem* pTargetItem);
	int GetMaxTargetId();
	bool GetTargetInfo(int iTargetId, TARGETINFO& targetInfo);
	void DeleteAllTarget();
	void SetTargetInfo(const std::vector<TARGETINFO>& vecTarget);
private:
	void ResetTargetItem(TargetItem* pTargetItem, const QPointF& pos, const EDGEINFO& edgeInfo);
private:
	MapScene* m_mapScene;

	std::vector<TargetItem*> m_vecTargetItem;
};