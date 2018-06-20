#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>

#include "BasicDefine.h"

class QCheckBox;
class NodeInfoWnd : public QDialog
{
public:
	NodeInfoWnd();
	~NodeInfoWnd();
public:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
public:
	void SetNodeInfo(const NodeInfo& nodeInfo);
	void GetNodeInfo(NodeInfo& nodeInfo);
private:
	void SlotBtnComfirm();
	void SlotBtnCancel();
private:
	QLineEdit* m_pEditeNodeId = nullptr;
	QLineEdit* m_pEditNodePosx = nullptr;
	QLineEdit* m_pEditNodePosy = nullptr;
	QPushButton* m_pBtnComfirm = nullptr;
	QPushButton* m_pBtnCancel = nullptr;
	QCheckBox* m_pSpin = nullptr;

	NodeInfo m_nodeInfo;
	bool en;
};

