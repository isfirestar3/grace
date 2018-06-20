#pragma once
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qdockwidget.h>
#include "DrawMapWid.h"
#include "PathSearch.h"
#include "BasicDefine.h"
#include <atomic>
#include <mutex>
#include "mntypes.h"
#include "EdgeListShowWnd.h"
#include <QtCore/QSharedPointer>
#include "net_interface.h"

class MainWnd : public QMainWindow
{
public:
	MainWnd();
	~MainWnd();
private:
	void InitWindow();
	void InitFileToolBar();
	void InitViewToolBar();
	void InitOperaToolBar();
	void InitCtrl();
	void InitSlot();
	void InitStatusBar();
	void InitData();
	void SetActionState(bool bState,ActionType actionSelect);
	void SearchNavPath();
	void PostNavTask();
	void GetStartPos(position_t& pos_t);
	void GetStartPosByTarget(const TARGETINFO& targetInfo, position_t& startPos);
	void UpdateStatusBarVehiclePos(const POSXYA& pos);
	void LoadConfigFile(CONFIGDATA& configData);
	void LoadDocksFile(DOCKDATA& dockData);
	void GetConfigFilePath(std::string& strConfig);
	void GetDocksFilePath(std::string& strTask);
	void ConvertPos(LAYOUTDATA& layoutData);
	void ConvertDockPos(DOCKDATA& dockData);
	void ShowEdgeListShowWnd(bool bShow);
	bool ParseLayoutXml(const QString &filename, LAYOUTDATA &layoutData);
private:
	int RequestVehiclePos(POSXYA& posXya);
private:
	static int CallbackFinishProcess(const unsigned int iPercent, const void *pUsr);
	void CallbackVehicleData(void *data, int type);
	void CallbackUpdateMousePos(const QPointF& pos);
	void CallbackEdgeListClicked(int edge_id);
private:
	void SlotActionAddPolygon();
	void SlotActionPointLine(bool bChecked);
	void SlotActionAddNode(bool bChecked);
	void SlotActionAddEdge(bool bChecked);
	void SlotActionDragView(bool bChecked);
	void SlotActionSaveFile();
	void SlotActionOpenFile();
	void SlotActionMoveItem(bool bChecked);
	void SlotActionAddVehicle(bool bChecked);
	void SlotActionEditWop();
	void SlotActionComputeLinks();
	void SlotActionEnterNavMode(bool bChecked);
	void SlotStartNavigation();
	void SlotActionFormatEdge(bool bChecked);
	void SlotActionPreviewPath();
	void SlotActionStopNav();
	void SlotActionSuspendNav();
	void SlotActionAddAnchor(bool bChecked);
	void SlotActionConnectNet();
	void SlotActionLocAnchor();
	void SlotActionOperate();
	void SlotActionBezierLink(bool bChecked);
	void SlotActionLocNode();
	void SlotActionConvertFile();
	void SlotActionShowEdgeList();
	void SlotActionOneKeyAdjust();
	void OnGenerateEmap();
	void GenerateEmap(int rowCount, int columnCount);
	void GenerateNodeItems(int rowCount, int columnCount);
	void GenerateEdgeItems(int rowCount, int columnCount);
	void OnGenerateEmapTimeout();
	void OnUpdateNavState();
	void SlotActionDeleteMore();
	void SlotActionEditNodes();
	void SlotActionEditEdges();
	void SlotActionFormatItems();
	void SlotActionQRcodeSave();
protected:
	void timerEvent(QTimerEvent *event);
	void keyPressEvent(QKeyEvent* keyEvent);
private:
	DrawMapWid* m_pDrawMapWid;

	QAction* m_pActionOpenFile;
	QAction* m_pActionSaveFile;
	QAction* m_pActionHand;
	QAction* m_pActionNode;
	QAction* m_pActionEdge;
	QAction* m_pActionPolygon;
	QAction* m_pActionPointLine;
	QAction* m_pActionMoveItem;
	QAction* m_pActionVehicle;
	QAction* m_pActionEditWop;
	QAction* m_pActionComputeLinks;
	QAction* m_pActionStartNav;
	QAction* m_pActionEnterNavMode;
	QAction* m_pActionFormatBrush;
	QAction* m_pActionPreviewPath;
	QAction* m_pActionStopNav;
	QAction* m_pActionSuspendNav;
	QAction* m_pActionAnchor;
	QAction* m_pActionConnect;
	QAction* m_pActionLocAnchor;
	QAction* m_pActionOperate;
	QAction* m_pActionBezierLink;
	QAction* m_pActionLocNode;
	QAction* m_pActionConvertFile;
	QAction* m_pActionShowEdgeList = nullptr;
	QSharedPointer<QAction> m_pActionOneKeyAdjust;//一键调整所有边的属性
	QAction* m_pActionDeleteMore;
	QAction* m_pActionEditNodes;
	QAction* m_pActionEditEdges;
	QAction* m_pActionFormatItems;
	QAction* m_pActionQRcodeSave;

	std::string m_strLayoutFilePath = "";
	std::string m_strConfigFilePath = "";
	std::vector<UPL> m_vecPathUpl;
	PathSearch_UPL m_destUpl;
	POSPOINT m_destPos;

	int m_timerLocation;
	int m_timeTaskStatus;
	bool m_bTaskSuspend; //是否有任务暂停中
	std::atomic<bool> m_bManualLocation = false;
	std::atomic<bool> m_bConnected = false; //是否已经连接网络
	std::recursive_mutex m_mutexVehicleInfo;
	VEHICLEINFO m_connectedVehicle; //当前已连接车的信息

	QLabel* m_pLabelMousePos;
	QLabel* m_pLabelVehiclePos;
	QLabel* m_pLabelTaskStatus;
	QLabel* m_pLabelNodeId;
	QDockWidget* m_pDockEdgeListShow = nullptr;
	EdgeListShowWnd* m_pEdgeListWnd = nullptr;
	bool m_en;
	std::shared_ptr<net_interface> agv_net_;
	unsigned char m_f5Count{0};
	QSharedPointer<QTimer> m_generateEmapTimer;
	QSharedPointer<QTimer> m_updateNavStateTimer;
};

