#pragma once
#include "QRcodeItem.h"

class MapScene;

class QRcodeItemManage
{
public:
	QRcodeItemManage(MapScene* mapScene);
	~QRcodeItemManage();
public:
	void SetCodeInfo(const std::vector<CODEPOSXYA>& vecNodeInfo);
	QRcodeItem* AddCodeItem(const QPointF& pos);
	QRcodeItem* AddCodeItem(const CODEPOSXYA& nodeInfo);
	void AddCodeToVector(QRcodeItem* nodeItem);
	void GetAllCodeItem(std::vector<QRcodeItem*>& vecNodeItem);
	QRcodeItem* GetCodeItem(int iNodeId);
	int GetMaxCodeId();
	void SetItemMoveable(bool bMoveable);
	bool RemoveCodeItem(int node_id);
private:
	MapScene* m_mapScene;

	std::vector<QRcodeItem*> m_vecCodeItem;
};

