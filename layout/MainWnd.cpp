#include "MainWnd.h"
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qstatusbar.h>
#include "DataManage.h"
#include "rw_layout_data.h"
#include "WopsEditWnd.h"
#include "ComputeLinks.hpp"
#include "point_algorithm.hpp"
#include "net_impls.h"
#include "log.h"
#include "rw_config_file.h"
#include "search_path.h"
#include "ConnectVehicleWnd.h"
#include "mntypes.h"
#include "navigation.h"
//#include "net_interface.h"
#include "OperateCtrlWnd.h"
#include "polygonitem.h"
#include "BasicDefine.h"
#include "watcherwaiter.hpp"
#include <functional>
#include <QtWidgets/qapplication.h>
#include "OneKeyAdjustDialog.h"
#include "generateemapdialog.h"
#include <vector>

namespace
{
	static const int F5_COUNT = 5;
	static const int EMAP_CONVERT_FACTOR = 60;
	static const int GENERATE_EMAP_TIMEOUT = 500;
}

MainWnd::MainWnd()
:m_bTaskSuspend(false)
, m_bManualLocation(false),
m_generateEmapTimer{ QSharedPointer<QTimer>::create() },
m_updateNavStateTimer{ QSharedPointer<QTimer>::create() }
{

	m_en = qApp->property("language").toInt() == 1;
	InitWindow();
	InitCtrl();
	InitFileToolBar();
	InitViewToolBar();
	InitOperaToolBar();
	InitStatusBar();
	InitSlot();
	InitData();
	m_timerLocation = startTimer(200);
	m_timeTaskStatus = startTimer(100);
	//agv_net_ = std::make_shared<net_interface>();
	//net_interface::Instance()->connect_net();
	m_generateEmapTimer->setSingleShot(true);
	m_updateNavStateTimer->start(500);

}


MainWnd::~MainWnd()
{
	if (m_bConnected)
	{
		mn::reset_net();
	}
}

void MainWnd::InitWindow()
{
	m_pDrawMapWid = new DrawMapWid;
	setCentralWidget(m_pDrawMapWid);
	showMaximized();
}

void MainWnd::InitCtrl()
{
	m_pActionOpenFile = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/open.png")), m_en ? "Open File" : QStringLiteral("打开数据文件"), this);
	m_pActionSaveFile = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/save.png")), m_en ? "Save" : QStringLiteral("保存数据"), this);
	m_pActionHand = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/cursor_hand.png")), m_en ? "Move" : QStringLiteral("移动视图"), this);
	m_pActionNode = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/node.png")), m_en ? "Add Node" : QStringLiteral("添加节点"), this);
	m_pActionEdge = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/curve.png")), m_en ? "Add edge" : QStringLiteral("添加边"), this);
	m_pActionPolygon = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/polygon.png")), m_en ? "Add Polygon" : QStringLiteral("添加多边形"), this);
	m_pActionPointLine = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/line.png")), m_en ? "Line" : QStringLiteral("点点成线"), this);
	m_pActionMoveItem = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/move.png")), m_en ? "Drag" : QStringLiteral("拖动元素"), this);
	m_pActionVehicle = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/vehicle_small.png")), m_en ? "Add Vehicle" : QStringLiteral("添加车辆"), this);
	m_pActionEditWop = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/edit.png")), m_en ? "Edit Wop" : QStringLiteral("编辑wop"), this);
	m_pActionComputeLinks = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/finish.png")), m_en ? "Generate Link" : QStringLiteral("生成link表"), this);
	m_pActionStartNav = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/start.png")), m_en ? "Start Navigation(Ctrl+Q)" : QStringLiteral("开始导航（Ctrl+Q）"), this);
	m_pActionEnterNavMode = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/navigation.png")), m_en ? "EnterNavMode" : QStringLiteral("进入导航模式"), this);
	m_pActionFormatBrush = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/clear.png")), m_en ? "Formatting Ddge Property" : QStringLiteral("格式化边属性"), this);
	m_pActionPreviewPath = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/preview.png")), m_en ? "View Navigation" : QStringLiteral("预览导航路径"), this);
	m_pActionStopNav = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/stop.png")), m_en ? "Stop Navigation(Ctrl+E)" : QStringLiteral("停止导航（Ctrl+E）"), this);
	m_pActionSuspendNav = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/suspend.png")), m_en ? "Pause Navigation(Ctrl+W)" : QStringLiteral("暂停导航（Ctrl+W）"), this);
	m_pActionAnchor = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/anchor.png")), m_en ? "Add Anchor Node" : QStringLiteral("添加停靠点"), this);
	m_pActionConnect = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/offline.png")), m_en ? "Connect" : QStringLiteral("连接车辆"), this);
	m_pActionLocAnchor = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/location.png")), m_en ? "LocAnchor(Ctrl+Z)" : QStringLiteral("定位停靠点（Ctrl+Z）"), this);
	m_pActionOperate = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/operate.png")), m_en ? "Operate" : QStringLiteral("操作控制"), this);
	m_pActionBezierLink = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/link_curve.png")), m_en ? "BezierLink" : QStringLiteral("生成曲线"), this);
	m_pActionLocNode = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/locnode.png")), m_en ? "LocNode" : QStringLiteral("定位节点"), this);
	m_pActionConvertFile = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/spin.png")), m_en ? "ConvertFile" : QStringLiteral("旋转文件"), this);
	m_pActionShowEdgeList = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/show.png")), m_en ? "ShowEdgeList" : QStringLiteral("边查看器"), this);
	m_pActionOneKeyAdjust = QSharedPointer<QAction>(new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/OneKeyAdjust.png")), m_en ? "One key adjust all edge property" : QStringLiteral("一键调整所有边属性"), this));
	m_pActionDeleteMore = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/deleteMore.png")), m_en ? "deleteMore" : QStringLiteral("批量删除"), this);
	m_pActionEditNodes = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/editNodes.png")), m_en ? "editNodes" : QStringLiteral("批量编辑节点"), this);
	m_pActionEditEdges = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/editEdges.png")), m_en ? "editEdges" : QStringLiteral("批量编辑边"), this);
	m_pActionFormatItems = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/editClerr.png")), m_en ? "FormatItems" : QStringLiteral("批量格式刷"), this);
	m_pActionQRcodeSave = new QAction(QIcon(QString::fromUtf8(":/newPrefix/images/saveas.png")), m_en ? "QRcodeSave" : QStringLiteral("保存二维码"), this);

	if (nullptr != m_pActionLocAnchor)
	{
		m_pActionLocAnchor->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
	}

	if (nullptr != m_pActionStartNav)
	{
		m_pActionStartNav->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	}

	if (nullptr != m_pActionStopNav)
	{
		m_pActionStopNav->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	}

	if (nullptr != m_pActionSuspendNav)
	{
		m_pActionSuspendNav->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	}

	if (m_pActionHand != NULL)
	{
		m_pActionHand->setCheckable(true);//这个设置是必须的不然没有toggled特性，也不会触发槽函数
	}
	if (m_pActionNode != NULL)
	{
		m_pActionNode->setCheckable(true);
	}
	if (m_pActionEdge != NULL)
	{
		m_pActionEdge->setCheckable(true);
	}
	if (m_pActionPointLine != NULL)
	{
		m_pActionPointLine->setCheckable(true);
	}
	if (m_pActionMoveItem)
	{
		m_pActionMoveItem->setCheckable(true);
	}
	if (m_pActionVehicle)
	{
		m_pActionVehicle->setCheckable(true);
	}
	if (m_pActionEnterNavMode)
	{
		m_pActionEnterNavMode->setCheckable(true);
	}
	if (m_pActionFormatBrush)
	{
		m_pActionFormatBrush->setCheckable(true);
	}
	if (m_pActionAnchor)
	{
		m_pActionAnchor->setCheckable(true);
	}
	if (m_pActionBezierLink)
	{
		m_pActionBezierLink->setCheckable(true);
	}
	if (m_pActionShowEdgeList){
		m_pActionShowEdgeList->setCheckable(true);
	}
	m_pEdgeListWnd = new EdgeListShowWnd;
}

void MainWnd::InitFileToolBar()
{
	QToolBar* pMainToolBar = this->addToolBar(QStringLiteral("file"));
	if (pMainToolBar == NULL)
	{
		return;
	}
	pMainToolBar->addAction(m_pActionOpenFile);
	pMainToolBar->addAction(m_pActionConvertFile);
	pMainToolBar->addAction(m_pActionSaveFile);
}

void MainWnd::InitViewToolBar()
{
	QToolBar* pViewToolBar = this->addToolBar(QStringLiteral("view"));
	if (pViewToolBar == NULL)
	{
		return;
	}
	pViewToolBar->addAction(m_pActionHand);
	pViewToolBar->addAction(m_pActionNode);
	pViewToolBar->addAction(m_pActionLocNode);
	pViewToolBar->addAction(m_pActionEdge);
	pViewToolBar->addAction(m_pActionPolygon);
	//pViewToolBar->addAction(m_pActionPointLine);
	pViewToolBar->addAction(m_pActionAnchor);
	pViewToolBar->addAction(m_pActionLocAnchor);
	pViewToolBar->addAction(m_pActionVehicle);
	pViewToolBar->addAction(m_pActionMoveItem);
	pViewToolBar->addAction(m_pActionFormatBrush);
	pViewToolBar->addAction(m_pActionBezierLink);
	pViewToolBar->addAction(m_pActionDeleteMore);
	pViewToolBar->addAction(m_pActionEditNodes);
	pViewToolBar->addAction(m_pActionEditEdges);
	pViewToolBar->addAction(m_pActionFormatItems);
	pViewToolBar->addAction(m_pActionQRcodeSave);
}							

void MainWnd::InitOperaToolBar()
{
	QToolBar* pOperaToolBar = addToolBar(QStringLiteral("opera"));
	if (!pOperaToolBar)
	{
		return;
	}
	pOperaToolBar->addAction(m_pActionEditWop);
	pOperaToolBar->addAction(m_pActionComputeLinks);
	pOperaToolBar->addAction(m_pActionEnterNavMode);
	pOperaToolBar->addAction(m_pActionPreviewPath);
	pOperaToolBar->addAction(m_pActionStartNav);
	pOperaToolBar->addAction(m_pActionStopNav);
	pOperaToolBar->addAction(m_pActionSuspendNav);
	pOperaToolBar->addAction(m_pActionOperate);
	pOperaToolBar->addAction(m_pActionConnect);
	pOperaToolBar->addAction(m_pActionShowEdgeList);
	pOperaToolBar->addAction(m_pActionOneKeyAdjust.data());
}

void MainWnd::InitStatusBar()
{
	m_pLabelMousePos = new QLabel(m_en ? "X,Y:" : QStringLiteral("鼠标坐标："));
	if (m_pLabelMousePos)
	{
		m_pLabelMousePos->setMinimumSize(m_pLabelMousePos->sizeHint());
		m_pLabelMousePos->setAlignment(Qt::AlignVCenter);
		m_pLabelMousePos->setFixedWidth(210 * 1.5);
		statusBar()->addWidget(m_pLabelMousePos);
		if (m_pDrawMapWid)
		{
			m_pDrawMapWid->SetMousePosCallback(std::bind(&MainWnd::CallbackUpdateMousePos, this, std::placeholders::_1));
		}
	}
	m_pLabelVehiclePos = new QLabel(m_en ? "VehiclePos:" : QStringLiteral("当前车位置："));
	if (m_pLabelVehiclePos)
	{
		m_pLabelVehiclePos->setMinimumSize(m_pLabelVehiclePos->sizeHint());
		m_pLabelVehiclePos->setAlignment(Qt::AlignVCenter);
		m_pLabelVehiclePos->setFixedWidth(480);
		statusBar()->addWidget(m_pLabelVehiclePos);
	}
	m_pLabelTaskStatus = new QLabel(m_en ? "TaskStatus:" : QStringLiteral("任务状态："));
	if (m_pLabelTaskStatus){
		m_pLabelTaskStatus->setMinimumSize(m_pLabelTaskStatus->sizeHint());
		m_pLabelTaskStatus->setAlignment(Qt::AlignVCenter);
		m_pLabelTaskStatus->setFixedWidth(200);
		statusBar()->addWidget(m_pLabelTaskStatus);
	}
	m_pLabelNodeId = new QLabel(QStringLiteral("Node ID："));
	if (m_pLabelNodeId){

		m_pDrawMapWid->GetScene()->SetNodeIdLabel(m_pLabelNodeId);
		m_pLabelNodeId->setMinimumSize(m_pLabelNodeId->sizeHint());
		m_pLabelNodeId->setAlignment(Qt::AlignVCenter);
		m_pLabelNodeId->setFixedWidth(200);
		statusBar()->addWidget(m_pLabelNodeId);
	}
}

void MainWnd::InitSlot()
{
	connect(m_pActionPointLine, &QAction::toggled, this, &MainWnd::SlotActionPointLine);
	connect(m_pActionNode, &QAction::toggled, this, &MainWnd::SlotActionAddNode);
	connect(m_pActionEdge, &QAction::toggled, this, &MainWnd::SlotActionAddEdge);
	connect(m_pActionPolygon, &QAction::triggered, this, &MainWnd::SlotActionAddPolygon);

	connect(m_pActionHand, &QAction::toggled, this, &MainWnd::SlotActionDragView);
	connect(m_pActionSaveFile, &QAction::triggered, this, &MainWnd::SlotActionSaveFile);
	connect(m_pActionOpenFile, &QAction::triggered, this, &MainWnd::SlotActionOpenFile);
	connect(m_pActionMoveItem, &QAction::toggled, this, &MainWnd::SlotActionMoveItem);
	connect(m_pActionVehicle, &QAction::toggled, this, &MainWnd::SlotActionAddVehicle);
	connect(m_pActionEditWop, &QAction::triggered, this, &MainWnd::SlotActionEditWop);
	connect(m_pActionComputeLinks, &QAction::triggered, this, &MainWnd::SlotActionComputeLinks);
	connect(m_pActionStartNav, &QAction::triggered, this, &MainWnd::SlotStartNavigation);
	connect(m_pActionEnterNavMode, &QAction::toggled, this, &MainWnd::SlotActionEnterNavMode);
	connect(m_pActionFormatBrush, &QAction::toggled, this, &MainWnd::SlotActionFormatEdge);
	connect(m_pActionPreviewPath, &QAction::triggered, this, &MainWnd::SlotActionPreviewPath);
	connect(m_pActionStopNav, &QAction::triggered, this, &MainWnd::SlotActionStopNav);
	connect(m_pActionSuspendNav, &QAction::triggered, this, &MainWnd::SlotActionSuspendNav);
	connect(m_pActionAnchor, &QAction::toggled, this, &MainWnd::SlotActionAddAnchor);
	connect(m_pActionConnect, &QAction::triggered, this, &MainWnd::SlotActionConnectNet);

	connect(m_pActionOperate, &QAction::triggered, this, &MainWnd::SlotActionOperate);
	connect( m_pActionLocAnchor, &QAction::triggered, this, &MainWnd::SlotActionLocAnchor );
	connect(m_pActionBezierLink, &QAction::toggled, this, &MainWnd::SlotActionBezierLink);
	connect(m_pActionLocNode, &QAction::triggered, this, &MainWnd::SlotActionLocNode);
	connect(m_pActionConvertFile, &QAction::triggered, this, &MainWnd::SlotActionConvertFile);
	connect(m_pActionShowEdgeList, &QAction::toggled, this, &MainWnd::SlotActionShowEdgeList);
	connect(m_pActionOneKeyAdjust.data(), &QAction::triggered, this, &MainWnd::SlotActionOneKeyAdjust);

	connect(m_generateEmapTimer.data(), &QTimer::timeout, this, &MainWnd::OnGenerateEmapTimeout);
	connect(m_updateNavStateTimer.data(), &QTimer::timeout, this, &MainWnd::OnUpdateNavState);

	connect(m_pActionDeleteMore, &QAction::triggered, this, &MainWnd::SlotActionDeleteMore);
	connect(m_pActionEditNodes, &QAction::triggered, this, &MainWnd::SlotActionEditNodes);
	connect(m_pActionEditEdges, &QAction::triggered, this, &MainWnd::SlotActionEditEdges);
	connect(m_pActionFormatItems,&QAction::triggered,this,&MainWnd::SlotActionFormatItems);
	connect(m_pActionQRcodeSave, &QAction::triggered, this, &MainWnd::SlotActionQRcodeSave);
}

void MainWnd::InitData()
{
	CONFIGDATA configData;
	LoadConfigFile(configData);
	DATAMNG->SetConfigData(configData);

	DATAMNG->SetDefaultWop( );
}

void MainWnd::SetActionState(bool bState, ActionType actionSelect)
{
	if (m_pActionNode && actionSelect == ActionType::ACTION_ADD_NODE)
	{
		m_pActionNode->setChecked(bState);
		m_pActionEdge->setChecked(!bState);
		m_pActionHand->setChecked(!bState);
		m_pActionMoveItem->setChecked(!bState);
		//m_pActionVehicle->setChecked(!bState);
		m_pActionEnterNavMode->setChecked(!bState);
		m_pActionFormatBrush->setChecked(!bState);
		m_pActionAnchor->setChecked(!bState);
		m_pActionBezierLink->setChecked(!bState);
	}
	if (m_pActionEdge && actionSelect == ActionType::ACTION_ADD_EDGE)
	{
		m_pActionEdge->setChecked(bState);
		m_pActionNode->setChecked(!bState);
		m_pActionHand->setChecked(!bState);
		m_pActionMoveItem->setChecked(!bState);
		//m_pActionVehicle->setChecked(!bState);
		m_pActionEnterNavMode->setChecked(!bState);
		m_pActionFormatBrush->setChecked(!bState);
		m_pActionAnchor->setChecked(!bState);
		m_pActionBezierLink->setChecked(!bState);
	}
	if (m_pActionHand && actionSelect == ActionType::ACTION_HAND_DRAG)
	{
		m_pActionHand->setChecked(bState);
		m_pActionNode->setChecked(!bState);
		m_pActionEdge->setChecked(!bState);
		m_pActionMoveItem->setChecked(!bState);
		//m_pActionVehicle->setChecked(!bState);
		m_pActionEnterNavMode->setChecked(!bState);
		m_pActionFormatBrush->setChecked(!bState);
		m_pActionAnchor->setChecked(!bState);
		m_pActionBezierLink->setChecked(!bState);
	}
	if (m_pActionMoveItem && actionSelect == ActionType::ACTION_MOVE_ITEM)
	{
		m_pActionMoveItem->setChecked(bState);
		m_pActionHand->setChecked(!bState);
		m_pActionNode->setChecked(!bState);
		m_pActionEdge->setChecked(!bState);
		//m_pActionVehicle->setChecked(!bState);
		m_pActionEnterNavMode->setChecked(!bState);
		m_pActionFormatBrush->setChecked(!bState);
		m_pActionAnchor->setChecked(!bState);
		m_pActionBezierLink->setChecked(!bState);
	}
	if (m_pActionVehicle && actionSelect == ActionType::ACTION_ADD_VEHICLE)
	{
		m_pActionVehicle->setChecked(bState);
		m_pActionNode->setChecked(!bState);
		m_pActionEdge->setChecked(!bState);
		m_pActionHand->setChecked(!bState);
		m_pActionMoveItem->setChecked(!bState);
		m_pActionEnterNavMode->setChecked(!bState);
		m_pActionFormatBrush->setChecked(!bState);
		m_pActionAnchor->setChecked(!bState);
		m_pActionBezierLink->setChecked(!bState);
	}
	if (m_pActionEnterNavMode && actionSelect == ActionType::ACTION_ENTER_NAV)
	{
		m_pActionEnterNavMode->setChecked(bState);
		//m_pActionVehicle->setChecked(!bState);
		m_pActionNode->setChecked(!bState);
		m_pActionEdge->setChecked(!bState);
		m_pActionHand->setChecked(!bState);
		m_pActionMoveItem->setChecked(!bState);
		m_pActionFormatBrush->setChecked(!bState);
		m_pActionAnchor->setChecked(!bState);
		m_pActionBezierLink->setChecked(!bState);
	}
	if (m_pActionFormatBrush && actionSelect == ActionType::ACTION_FORMAT)
	{
		m_pActionFormatBrush->setChecked(bState);
		m_pActionEnterNavMode->setChecked(!bState);
		//m_pActionVehicle->setChecked(!bState);
		m_pActionNode->setChecked(!bState);
		m_pActionEdge->setChecked(!bState);
		m_pActionHand->setChecked(!bState);
		m_pActionMoveItem->setChecked(!bState);
		m_pActionAnchor->setChecked(!bState);
		m_pActionBezierLink->setChecked(!bState);
	}
	if (m_pActionAnchor && actionSelect == ActionType::ACTION_ADD_ANCHOR)
	{
		m_pActionAnchor->setChecked(bState);
		m_pActionFormatBrush->setChecked(!bState);
		m_pActionEnterNavMode->setChecked(!bState);
		//m_pActionVehicle->setChecked(!bState);
		m_pActionNode->setChecked(!bState);
		m_pActionEdge->setChecked(!bState);
		m_pActionHand->setChecked(!bState);
		m_pActionMoveItem->setChecked(!bState);
		m_pActionBezierLink->setChecked(!bState);
	}
	if (m_pActionBezierLink && actionSelect == ActionType::ACTION_LINK_CURVE)
	{
		m_pActionBezierLink->setChecked(bState);
		m_pActionAnchor->setChecked(!bState);
		m_pActionFormatBrush->setChecked(!bState);
		m_pActionEnterNavMode->setChecked(!bState);
		//m_pActionVehicle->setChecked(!bState);
		m_pActionNode->setChecked(!bState);
		m_pActionEdge->setChecked(!bState);
		m_pActionHand->setChecked(!bState);
		m_pActionMoveItem->setChecked(!bState);
	}

}

void MainWnd::SlotActionAddPolygon() {
	if (nullptr == m_pDrawMapWid)
	{
		return;
	}

	m_pDrawMapWid->dddPolygonVertex();
	m_pDrawMapWid->SetOperaMode(ACTION_ADD_POLYGON);
}

void MainWnd::SlotActionPointLine(bool bChecked)
{

}

void MainWnd::SlotActionAddNode(bool bChecked)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	if (bChecked)
	{
		SetActionState(true, ActionType::ACTION_ADD_NODE);
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_ADD_NODE);
	}
	else
	{
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_SELECT);
	}
}

void MainWnd::SlotActionAddEdge(bool bChecked)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	if (bChecked)
	{
		SetActionState(true, ActionType::ACTION_ADD_EDGE);
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_ADD_EDGE);
	}
	else
	{
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_SELECT);
	}
}

void MainWnd::SlotActionDragView(bool bChecked)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	if (bChecked)
	{
		SetActionState(true, ActionType::ACTION_HAND_DRAG);
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_HAND_DRAG);
	}
	else
	{
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_SELECT);
	}
}

void MainWnd::SlotActionSaveFile()
{
	if (m_strLayoutFilePath.empty())
	{
		QString qsFileName = QFileDialog::getSaveFileName(this, m_en ? "Save As" : QStringLiteral("另存为"), "XML/LayoutData.xml", QStringLiteral("XML(*.xml)"));
		if (qsFileName.isEmpty())
		{
			return;
		}
		m_strLayoutFilePath = qsFileName.toLocal8Bit();
	}
	

	LayoutData layoutData;
	DATAMNG->DispersePoint();
	DATAMNG->GetLayoutData(layoutData);


	QList<PolygonItem*> pitems = m_pDrawMapWid->GetScene()->polygonItems; // add by fengq
	QListIterator<PolygonItem*> i(pitems);
	while (i.hasNext()) {

		PolygonItem* item = i.next();
		QPolygonF const& onePolygon = item->polygon();
		QVectorIterator<QPointF> pi(onePolygon);
		POLYGON polygon_;
		polygon_.id = item->id;
		while (pi.hasNext()) {

			QPointF const& point = pi.next();
			PolygonVertex vertex;
			vertex.x = point.x();
			vertex.y = point.y();
			polygon_.vertexes.push_back(vertex);
		}
		layoutData.polygonList.push_back(polygon_);
	}
																		 // 

	if (RWXml::write_layout_data(m_strLayoutFilePath, layoutData) == SUCCESS)
	{
		QMessageBox::warning(this, m_en ? "information" : QStringLiteral("提示"), m_en ? "Save Successed!" : QStringLiteral("文件保存成功！").arg(QString::fromStdString(m_strLayoutFilePath)));
	}
	else
	{
		QMessageBox::warning(this, m_en ? "information" : QStringLiteral("提示"), m_en ? "Save Failed!" : QStringLiteral("文件保存失败！").arg(QString::fromStdString(m_strLayoutFilePath)));
	}
	std::string strTaskFilePath;
	GetDocksFilePath(strTaskFilePath);
	DOCKDATA dockData;
	DATAMNG->GetOperData(dockData);
	NavSearch::Instance()->set_docks_data(dockData);
	RWXml::write_docks_data(strTaskFilePath, dockData);
}

void MainWnd::SlotActionOpenFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Data"), "XML/", QStringLiteral("XML(*.xml)"));
	if (fileName.isEmpty())
	{
		return;
	}

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this, m_en ? "Open File Error" : QStringLiteral("打开文件错误"), m_en ? file.errorString() : QStringLiteral("未能成功打开文件").arg(fileName));
		return;
	}
	//LAYOUTDATA layoutData;
	//int ret = RWXml::get_layout_data(fileName.toLocal8Bit().toStdString(), layoutData);
	//if (ret != SUCCESS && ret != SUCCESS NO_POLYGON)
	//{
	//	QMessageBox::warning(this, m_en ? "Error！" : QStringLiteral("错误提示！"), m_en ? "Read File Error！" : QStringLiteral("读取文件数据出错！").arg(fileName));
	//	return;
	//}

	bool(MainWnd::*func)(const QString &, LAYOUTDATA &) = &MainWnd::ParseLayoutXml;
	LAYOUTDATA layoutData;
	const std::function<void(bool)> &retCallback = [&](bool ret)->void{
		if (ret)
		{
			if (layoutData.wopList.size() == 0) {
				std::vector<WOP> vecWopInfo;
				DATAMNG->GetAllWopInfo(vecWopInfo);
				layoutData.wopList = vecWopInfo;

				for (auto wopType : layoutData.wopList) {
					if (wopType.direction == FORWARD && wopType.angle == 0) {
						for (size_t i = 0; i < layoutData.edgeList.size(); i++) {
							layoutData.edgeList[i].wop_list.clear();
							layoutData.edgeList[i].wop_list.push_back(wopType.id);
						}
						break;
					}
				}
			}

			m_strLayoutFilePath = fileName.toLocal8Bit();
			DATAMNG->SetLayoutData(layoutData);

			DOCKDATA dockData;
			LoadDocksFile(dockData);//读取TaskOper.xml数据
			DATAMNG->SetOperData(dockData);
			if (m_pDrawMapWid)
			{
				m_pDrawMapWid->DrawAllMapItem(layoutData, dockData);
			}
			CONFIGDATA configData;
			DATAMNG->GetConfigData(configData);
			NavSearch::Instance()->get_user_id();
			NavSearch::Instance()->InitNavigationData(layoutData, configData.search_type,
				m_strLayoutFilePath.c_str(), CallbackFinishProcess, this);
			SlotActionDragView(true);
		}
		else
		{
			QMessageBox::warning(this, m_en ? "Error！" : QStringLiteral("错误提示！"), m_en ? "Read File Error！" : QStringLiteral("读取文件数据出错！").arg(fileName));
		}
	};

	nsp::toolkit::singleton<shr::gui::WatcherDialog>::instance()->run(this, func, fileName, std::ref(layoutData), retCallback, shr::gui::getTopWidget());
}

void MainWnd::SlotActionConvertFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Data"), "XML/", QStringLiteral("XML(*.xml)"));
	if (fileName.isEmpty())
	{
		return;
	}

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this, m_en ? "Open File Error" : QStringLiteral("打开文件错误"), m_en ? file.errorString() : QStringLiteral("未能成功打开文件").arg(fileName));
		return;
	}
	LAYOUTDATA layoutData;
	int ret = RWXml::get_layout_data(fileName.toLocal8Bit().toStdString(), layoutData);
	if (ret != SUCCESS && ret != SUCCESS NO_POLYGON)
	{
		QMessageBox::warning(this, m_en ? "Error!" : QStringLiteral("错误提示！"), m_en ? "Read File Error!" : QStringLiteral("读取文件数据出错！").arg(fileName));
		return;
	}
	ConvertPos(layoutData);

	if (layoutData.wopList.size() == 0) {
		std::vector<WOP> vecWopInfo;
		DATAMNG->GetAllWopInfo(vecWopInfo);
		layoutData.wopList = vecWopInfo;

		for (auto wopType : layoutData.wopList) {
			if (wopType.direction == FORWARD && wopType.angle == 0) {
				for (size_t i = 0; i < layoutData.edgeList.size(); i++) {
					layoutData.edgeList[i].wop_list.clear();
					layoutData.edgeList[i].wop_list.push_back(wopType.id);
				}
				break;
			}
		}
	}

	m_strLayoutFilePath = fileName.toLocal8Bit();
	DATAMNG->SetLayoutData(layoutData);

	DOCKDATA dockData;
	LoadDocksFile(dockData);//读取TaskOper.xml数据
	ConvertDockPos(dockData);
	DATAMNG->SetOperData(dockData);
	if (m_pDrawMapWid)
	{
		m_pDrawMapWid->DrawAllMapItem(layoutData, dockData);
	}
	CONFIGDATA configData;
	DATAMNG->GetConfigData(configData);
	NavSearch::Instance()->get_user_id();
	NavSearch::Instance()->InitNavigationData(layoutData, configData.search_type,
		m_strLayoutFilePath.c_str(), CallbackFinishProcess, this);
}

void MainWnd::ConvertPos(LAYOUTDATA& layoutData)
{
	double temp = 0.0;
	for (auto &iter : layoutData.nodeList){
		temp = iter.pos_x;
		iter.pos_x = iter.pos_y;
		iter.pos_y = -temp;
	}
	for (auto &iter : layoutData.edgeList){
		temp = iter.ctrl_point1.x;
		iter.ctrl_point1.x = iter.ctrl_point1.y;
		iter.ctrl_point1.y = -temp;

		temp = iter.ctrl_point2.x;
		iter.ctrl_point2.x = iter.ctrl_point2.y;
		iter.ctrl_point2.y = -temp;

		temp = iter.termstpos.x;
		iter.termstpos.x = iter.termstpos.y;
		iter.termstpos.y = -temp;

		temp = iter.termendpos.x;
		iter.termendpos.x = iter.termendpos.y;
		iter.termendpos.y = -temp;

		for (auto &pos : iter.disperse_point){
			temp = pos.x;
			pos.x = pos.y;
			pos.y = -temp;
		}
	}
}

void MainWnd::ConvertDockPos(DOCKDATA& dockData)
{
	double temp = 0.0;
	for (auto &dock : dockData.anchorList){
		temp = dock.pos_x;
		dock.pos_x = dock.pos_y;
		dock.pos_y = -temp;
		dock.angle = dock.angle - PI / 2;
	}
}

void MainWnd::LoadConfigFile(CONFIGDATA& configData)
{
	GetConfigFilePath(m_strConfigFilePath);
	if (RWXml::get_config_data(m_strConfigFilePath, configData) != SUCCESS)
	{
		QMessageBox::warning(this, m_en ? "Error" : QStringLiteral("错误提示！"), m_en ? "Read LayoutDesigner.xml Error" : QStringLiteral("读取LayoutDesigner.xml文件数据出错！"));
		return;
	}
}

void MainWnd::LoadDocksFile(DOCKDATA& dockData)
{
	std::string strTaskPath;
	GetDocksFilePath(strTaskPath);
	if (RWXml::get_docks_data(strTaskPath, dockData) != SUCCESS)
	{
		QMessageBox::warning(this, m_en ? "Error" : QStringLiteral("错误提示！"), m_en ? "Read docks.xml Error" : QStringLiteral("读取docks.xml文件数据出错！"));
		return;
	}
}

void MainWnd::GetConfigFilePath(std::string& strConfig)
{
	QString strPath = QDir::currentPath();
	strConfig = strPath.toLocal8Bit() + "/LayoutDesigner.xml";
}

void MainWnd::GetDocksFilePath(std::string& strTask)
{
	QString strPath = QDir::currentPath();
	strTask = strPath.toLocal8Bit() + "/docks.xml";
}

void MainWnd::SlotActionMoveItem(bool bChecked)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	if (bChecked)
	{
		SetActionState(true, ActionType::ACTION_MOVE_ITEM);
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_MOVE_ITEM);
	}
	else
	{
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_SELECT);
	}
}

void MainWnd::SlotActionAddVehicle(bool bChecked)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	if (bChecked)
	{
		SetActionState(true, ActionType::ACTION_ADD_VEHICLE);
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_ADD_VEHICLE);
		m_bManualLocation = true;
	}
	else
	{
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_SELECT);
		m_bManualLocation = false;
	}
}

int MainWnd::CallbackFinishProcess(const unsigned int iPercent, const void *pUsr)
{

	return 0;
}

void MainWnd::SlotActionEditWop()
{
	WopsEditWnd wopEditWnd;
	wopEditWnd.SetLayoutFilePath(m_strLayoutFilePath);
	wopEditWnd.UpdateTableWidget();
	wopEditWnd.exec();
}

void MainWnd::SlotActionComputeLinks()
{
	if (m_strLayoutFilePath.empty())
	{
		QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "Load Xml Pathfile First" : QStringLiteral("请先载入xml路线文件"), QMessageBox::Ok);
		return;
	}
	std::vector<EDGEINFO> vecEdgeInfo;
	DATAMNG->GetAllEdgeInfo(vecEdgeInfo);

	std::vector<NODEINFO> vecNodeInfo;
	DATAMNG->GetAllNodeInfo(vecNodeInfo);
	std::vector<LINK> vecLinkInfo;
	Links::instance()->SetNodeList(vecNodeInfo);//先传入节点信息
	Links::instance()->ComputeLinks(vecEdgeInfo, true, vecLinkInfo);

	DATAMNG->SetAllLinkInfo(vecLinkInfo);
	LAYOUTDATA layoutData;
	CONFIGDATA configData;
	DATAMNG->GetLayoutData(layoutData);
	DATAMNG->GetConfigData(configData);
	NavSearch::Instance()->InitNavigationData(layoutData, configData.search_type, 
		m_strLayoutFilePath.c_str(), CallbackFinishProcess, this);
	SlotActionSaveFile();
}

void MainWnd::SlotStartNavigation()
{
	SearchNavPath();
	PostNavTask();
}

void MainWnd::SlotActionEnterNavMode(bool bChecked)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	if (bChecked)
	{
		SetActionState(true, ActionType::ACTION_ENTER_NAV);
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_ENTER_NAV);
	}
	else
	{
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_SELECT);
	}
}

void MainWnd::SlotActionFormatEdge(bool bChecked)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	if (bChecked)
	{
		SetActionState(true, ActionType::ACTION_FORMAT);
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_FORMAT);
	}
	else
	{
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_SELECT);
	}
}

void MainWnd::SlotActionPreviewPath()
{
	SearchNavPath();
}

void MainWnd::SearchNavPath()
{
	m_vecPathUpl.clear();//先清空路径列表
	std::vector<TARGETINFO> vecTargetInfo;
	DATAMNG->GetAllTargetInfo(vecTargetInfo);

	position_t start_pos;
	GetStartPos(start_pos);

	double distance_total = 0.0;
	for (size_t i = 0; i < vecTargetInfo.size(); ++i)
	{
		m_destPos.x = vecTargetInfo[i].pos_x;
		m_destPos.y = vecTargetInfo[i].pos_y;
		position_t target_pos;
		target_pos.x_ = vecTargetInfo[i].pos_x;
		target_pos.y_ = vecTargetInfo[i].pos_y;
		target_pos.angle_ = vecTargetInfo[i].angle;

		if (vecTargetInfo[i].wop_id > 0)//目标点的wop已经选定的情况
		{
			UPL map_upl;
			PT_ALG::point_mapping_upl(target_pos, map_upl);
			m_destUpl.edge_id = vecTargetInfo[i].edge_id;
			m_destUpl.wop_id = vecTargetInfo[i].wop_id;
			m_destUpl.fPercent = map_upl.percent;

			distance_total += NavSearch::Instance()->SearchNavPath(start_pos, m_destUpl, m_vecPathUpl);
		}
		else
		{
			DOCKDATA dockData;
			LoadDocksFile(dockData);
			for (auto&iter : dockData.anchorList){

				if (abs(target_pos.x_ - iter.pos_x)<0.05&&abs(target_pos.y_ - iter.pos_y)<0.05){
					target_pos.angle_ = iter.angle;
				}
			}
			NavSearch::Instance()->set_docks_data(dockData);
			distance_total += NavSearch::Instance()->SearchNavPath(start_pos, target_pos, m_vecPathUpl, m_destUpl);
		}
		GetStartPosByTarget(vecTargetInfo[i],start_pos);//用当前目标点作为新起点，继续搜路
	}

	nspinfo << "edge_num:" << m_vecPathUpl.size() << " distance:" << distance_total;
	for (auto upl : m_vecPathUpl)
	{
		nspinfo << "edge_id:" << upl.edge_id << " wop_id:" << upl.wop_id << " percent:" << upl.percent;
	}
	if (m_pDrawMapWid)
	{
		m_pDrawMapWid->ShowNavPath(m_vecPathUpl);
	}
	if (m_vecPathUpl.empty())
	{
		QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "SearchNav Failed" : QStringLiteral("路径搜索失败！"), QMessageBox::Ok);
	}
}

void MainWnd::GetStartPos(position_t& pos_t)
{
	if (m_connectedVehicle.vehicle_id <= 0)//如果没有连车，那么直接从地图上获取车的位置
	{
		if (m_pDrawMapWid)
		{
			VEHICLEINFO vehicleInfo;
			m_pDrawMapWid->GetVehicleInfo(1, vehicleInfo);//默认获取地图上1号车的位置
			pos_t.x_ = vehicleInfo.pos_x;
			pos_t.y_ = vehicleInfo.pos_y;
			pos_t.angle_ = vehicleInfo.angle;
		}
	}
	else
	{
		if (m_pDrawMapWid)//如果连接了车，还是从地图上获取车的位置
		{
			VEHICLEINFO vehicleInfo;
			m_pDrawMapWid->GetVehicleInfo(m_connectedVehicle.vehicle_id, vehicleInfo);
			pos_t.x_ = vehicleInfo.pos_x;
			pos_t.y_ = vehicleInfo.pos_y;
			pos_t.angle_ = vehicleInfo.angle;
			NavSearch::Instance()->set_start_edge(vehicleInfo.edge_id);
		}
	}
}

void MainWnd::GetStartPosByTarget(const TARGETINFO& targetInfo, position_t& startPos)
{
	startPos.x_ = targetInfo.pos_x;
	startPos.y_ = targetInfo.pos_y;
	startPos.angle_ = targetInfo.angle;
}

void MainWnd::PostNavTask()
{
	upl_t destUpl;
	destUpl.edge_id_ = m_destUpl.edge_id;
	destUpl.wop_id_ = m_destUpl.wop_id;
	destUpl.percentage_ = m_destUpl.fPercent;

	position_t destPos;
	destPos.x_ = DataConvert::map2world(m_destPos.x);
	destPos.y_ = DataConvert::map2world(m_destPos.y);
	//destPos.angle_ = DataConvert::map2world( m_destPos. );

	std::vector<trail_t> pathUpl;
	for (auto upl : m_vecPathUpl)
	{
		trail_t path;
		path.edge_id_ = upl.edge_id;
		path.wop_id_ = upl.wop_id;

		pathUpl.push_back(path);
	}
	//int iRet = nsp::toolkit::singleton<net_interface>::instance()->post_nav_task(destUpl, destPos, pathUpl);

	int (net_interface::*func)(const upl_t &, const position_t &, const std::vector<trail_t> &) = &net_interface::post_nav_task;
	const std::function<void(int)> &retCb = [&](int iRet)->void{
		nspinfo << "post_navigation_task ,result:" << iRet;

		if (iRet != QUERY_SUCCESS)
		{
			QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "PostNavTask Failed" : QStringLiteral("发送导航任务失败！"), QMessageBox::Ok);
			return;
		}

		DATAMNG->SetTaskStatus(kStatusDescribe_Startup);
	};

	nsp::toolkit::singleton<shr::gui::WatcherDialog>::instance()->run(nsp::toolkit::singleton<net_interface>::instance(), func, destUpl, destPos, pathUpl, retCb, this);
}

void MainWnd::SlotActionStopNav()
{
	/*if (net_interface::Instance()->post_cancel_nav_task() != QUERY_SUCCESS)*/
		//if (agv_net_->post_cancel_nav_task() != QUERY_SUCCESS)
	//if (nsp::toolkit::singleton<net_interface>::instance()->post_cancel_nav_task() != QUERY_SUCCESS)
	//{
	//	
	//	QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "StopNav Failed" : QStringLiteral("取消导航任务失败！"), QMessageBox::Ok);
	//}

	int(net_interface::*func)() = &net_interface::post_cancel_nav_task;
	const std::function<void(int)> &retCb = [&](int ret)->void{
		if (QUERY_SUCCESS != ret)
		{
			QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "StopNav Failed" : QStringLiteral("取消导航任务失败！"), QMessageBox::Ok);
		}
	};

	nsp::toolkit::singleton<shr::gui::WatcherDialog>::instance()->run(nsp::toolkit::singleton<net_interface>::instance(), func, retCb, this);
}

void MainWnd::SlotActionSuspendNav()
{
	if (!m_bTaskSuspend)
	{		
		/*if (net_interface::Instance()->post_suspend_nav_task() != QUERY_SUCCESS)*/
		//if (nsp::toolkit::singleton<net_interface>::instance()->post_suspend_nav_task() != QUERY_SUCCESS)
		//{
		//	QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "SuspendNav Failed" : QStringLiteral("暂停导航任务失败！"), QMessageBox::Ok);
		//	return;
		//}

		//m_bTaskSuspend = true;
		//m_pActionSuspendNav->setText(m_en ? "Resume Navigation(Ctrl+W)" : QStringLiteral("恢复导航（Ctrl+W）"));

		int(net_interface::*func)() = &net_interface::post_suspend_nav_task;
		const std::function<void(int)> &retCb = [&](int ret)->void{
			if (QUERY_SUCCESS != ret)
			{
				QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "SuspendNav Failed" : QStringLiteral("暂停导航任务失败！"), QMessageBox::Ok);
				return;
			}

			m_bTaskSuspend = true;
			m_pActionSuspendNav->setIcon(QIcon(":/newPrefix/images/resume.png"));
			m_pActionSuspendNav->setText(m_en ? "Resume Navigation(Ctrl+W)" : QStringLiteral("恢复导航（Ctrl+W）"));
		};

		nsp::toolkit::singleton<shr::gui::WatcherDialog>::instance()->run(nsp::toolkit::singleton<net_interface>::instance(), func, retCb, this);
	}
	else
	{
		/*if (net_interface::Instance()->post_resume_nav_task() != QUERY_SUCCESS)*/
		//if (nsp::toolkit::singleton<net_interface>::instance()->post_resume_nav_task() != QUERY_SUCCESS)
		//{
		//	QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "ResumeNavTask Failed" : QStringLiteral("恢复导航任务失败！"), QMessageBox::Ok);
		//	return;
		//}

		//m_bTaskSuspend = false;
		//m_pActionSuspendNav->setText(m_en ? "Pause Navigation(Ctrl+W)" : QStringLiteral("暂停导航（Ctrl+W）"));

		int(net_interface::*func)() = &net_interface::post_resume_nav_task;
		const std::function<void(int)> &retCb = [&](int ret)->void{
			if (QUERY_SUCCESS != ret)
			{
				QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "ResumeNavTask Failed" : QStringLiteral("恢复导航任务失败！"), QMessageBox::Ok);
				return;
			}

			m_bTaskSuspend = false;
			m_pActionSuspendNav->setIcon(QIcon(":/newPrefix/images/suspend.png"));
			m_pActionSuspendNav->setText(m_en ? "Pause Navigation(Ctrl+W)" : QStringLiteral("暂停导航（Ctrl+W）"));
		};

		nsp::toolkit::singleton<shr::gui::WatcherDialog>::instance()->run(nsp::toolkit::singleton<net_interface>::instance(), func, retCb, this);
	}
}

void MainWnd::OnGenerateEmap()
{
	GenerateEmapDialog geDlg(this);

	if (QDialog::Accepted == geDlg.exec())
	{
		int rowCount = geDlg.getRowCount();
		int columnCount = geDlg.getColumnCount();
		GenerateEmap(rowCount, columnCount);
	}
}

void MainWnd::GenerateEmap(int rowCount, int columnCount)
{
	if (nullptr == m_pDrawMapWid)
	{
		return;
	}

	LAYOUTDATA layoutData;
	DATAMNG->GetLayoutData(layoutData);
	shr::gui::std_vector_free(layoutData.nodeList);
	shr::gui::std_vector_free(layoutData.edgeList);
	shr::gui::std_vector_free(layoutData.linkList);
	shr::gui::std_vector_free(layoutData.polygonList);
	m_pDrawMapWid->Clear();
	GenerateNodeItems(rowCount, columnCount);
	GenerateEdgeItems(rowCount, columnCount);
	MapView *pMapView = m_pDrawMapWid->GetView();

	if (nullptr == pMapView)
	{
		pMapView->LocationCenterItem();
	}

	SlotActionDragView(true);
}

void MainWnd::GenerateNodeItems(int rowCount, int columnCount)
{
	if (nullptr == m_pDrawMapWid)
	{
		return;
	}

	LAYOUTDATA layoutData;
	DATAMNG->GetLayoutData(layoutData);
	NODEINFO nodeInfo;

	for (int i = 0; i < rowCount; ++i)
	{
		for (int j = 0; j < columnCount; ++j)
		{
			nodeInfo.node_id = i * columnCount + j;
			nodeInfo.pos_x = i * EMAP_CONVERT_FACTOR;
			nodeInfo.pos_y = j * EMAP_CONVERT_FACTOR;
			nodeInfo.spin = 1;
			m_pDrawMapWid->AddNodeItem(nodeInfo);
			layoutData.nodeList.push_back(nodeInfo);
		}
	}

	DATAMNG->SetLayoutData(layoutData);
}

void MainWnd::GenerateEdgeItems(int rowCount, int columnCount)
{
	if (nullptr == m_pDrawMapWid)
	{
		return;
	}

	LAYOUTDATA layoutData;
	DATAMNG->GetLayoutData(layoutData);
	int edgeId = 0;
	int startNodeId = -1;
	int endNodeId = -1;
	double nodeLength = 0;
	POSPOINT ctrlPoint1;
	POSPOINT ctrlPoint2;
	POSPOINT edgeStartPoint;
	POSPOINT edgeEndPoint;
	QPointF nodeStartPoint;
	QPointF nodeEndPoint;
	NodeItem *pStartNodeItem = nullptr;
	NodeItem *pEndNodeItem = nullptr;
	EDGEINFO edgeInfo;

	for (int i = 0; i < rowCount; ++i)
	{
		for (int j = 0; j < columnCount; ++j)
		{
			if (j == columnCount - 1)
			{
				continue;
			}

			startNodeId = i * columnCount + j;
			endNodeId = startNodeId + 1;
			pStartNodeItem = m_pDrawMapWid->GetNodeItem(startNodeId);
			pEndNodeItem = m_pDrawMapWid->GetNodeItem(endNodeId);

			if (nullptr == pStartNodeItem || nullptr == pEndNodeItem)
			{
				continue;
			}

			nodeStartPoint = pStartNodeItem->scenePos();
			nodeEndPoint = pEndNodeItem->scenePos();
			nodeLength = nodeEndPoint.y() - nodeStartPoint.y();
			ctrlPoint1.x = nodeStartPoint.x();
			ctrlPoint1.y = nodeStartPoint.y() + nodeLength / 3.0;
			ctrlPoint2.x = nodeStartPoint.x();
			ctrlPoint2.y = nodeStartPoint.y() + nodeLength * 2.0 / 3.0;
			edgeStartPoint.x = nodeStartPoint.x();
			edgeStartPoint.y = nodeStartPoint.y();
			edgeEndPoint.x = nodeEndPoint.x();
			edgeEndPoint.y = nodeEndPoint.y();
			edgeInfo.id = ++edgeId;
			edgeInfo.start_node_id = startNodeId;
			edgeInfo.end_node_id = endNodeId;
			edgeInfo.direction = BOTH;
			edgeInfo.ctrl_point1 = ctrlPoint1;
			edgeInfo.ctrl_point2 = ctrlPoint2;
			edgeInfo.termstpos = edgeStartPoint;
			edgeInfo.termendpos = edgeEndPoint;
			edgeInfo.length = nodeLength;

			{
				std::vector<WOP> vecWopInfo;
				DATAMNG->GetAllWopInfo(vecWopInfo);
				std::vector<int> vecWopId;

				for (auto wopInfo : vecWopInfo)
				{
					vecWopId.push_back(wopInfo.id);
				}

				edgeInfo.wop_list = vecWopId;
			}

			m_pDrawMapWid->AddEdgeItem(pStartNodeItem, pEndNodeItem, edgeInfo);
			layoutData.edgeList.push_back(edgeInfo);
		}
	}

	for (int i = 0; i < rowCount; ++i)
	{
		for (int j = 0; j < columnCount; ++j)
		{
			startNodeId = i * columnCount + j;
			endNodeId = startNodeId + columnCount;
			pStartNodeItem = m_pDrawMapWid->GetNodeItem(startNodeId);
			pEndNodeItem = m_pDrawMapWid->GetNodeItem(endNodeId);

			if (nullptr == pStartNodeItem || nullptr == pEndNodeItem)
			{
				continue;
			}

			nodeStartPoint = pStartNodeItem->scenePos();
			nodeEndPoint = pEndNodeItem->scenePos();
			nodeLength = nodeEndPoint.x() - nodeStartPoint.x();
			ctrlPoint1.x = nodeStartPoint.x() + nodeLength / 3.0;
			ctrlPoint1.y = nodeStartPoint.y();
			ctrlPoint2.x = nodeStartPoint.x() + nodeLength * 2.0 / 3.0;
			ctrlPoint2.y = nodeStartPoint.y();
			edgeStartPoint.x = nodeStartPoint.x();
			edgeStartPoint.y = nodeStartPoint.y();
			edgeEndPoint.x = nodeEndPoint.x();
			edgeEndPoint.y = nodeEndPoint.y();
			edgeInfo.id = ++edgeId;
			edgeInfo.start_node_id = startNodeId;
			edgeInfo.end_node_id = endNodeId;
			edgeInfo.direction = BOTH;
			edgeInfo.ctrl_point1 = ctrlPoint1;
			edgeInfo.ctrl_point2 = ctrlPoint2;
			edgeInfo.termstpos = edgeStartPoint;
			edgeInfo.termendpos = edgeEndPoint;
			edgeInfo.length = nodeLength;

			{
				std::vector<WOP> vecWopInfo;
				DATAMNG->GetAllWopInfo(vecWopInfo);
				std::vector<int> vecWopId;

				for (auto wopInfo : vecWopInfo)
				{
					vecWopId.push_back(wopInfo.id);
				}

				edgeInfo.wop_list = vecWopId;
			}

			m_pDrawMapWid->AddEdgeItem(pStartNodeItem, pEndNodeItem, edgeInfo);
			layoutData.edgeList.push_back(edgeInfo);
		}
	}

	DATAMNG->SetLayoutData(layoutData);
}

void MainWnd::OnGenerateEmapTimeout()
{
	m_f5Count = 0;
}

void MainWnd::OnUpdateNavState()
{
	if (nullptr == m_pActionStartNav || nullptr == m_pActionStopNav || nullptr == m_pActionSuspendNav)
	{
		return;
	}

	var__navigation_t nav_t_;
	int(net_interface::*func)(var__navigation_t &) = &net_interface::get_nav_info;
	const std::function<void(int)> &retCb = [&](int ret)->void{
		if (ret >= 0)
		{
			status_describe_t status_ = nav_t_.track_status_.response_;

			if (status_ <= kStatusDescribe_Unknown || status_ > kStatusDescribe_Error)
			{
				m_pActionStartNav->setEnabled(false);
				m_pActionStopNav->setEnabled(false);
				m_pActionSuspendNav->setEnabled(false);
			}
			else
			{
				bool canStartNav = (status_ < kStatusDescribe_PendingFunction || status_ > kStatusDescribe_FinalFunction);
				m_pActionStartNav->setEnabled(canStartNav);
				m_pActionStopNav->setEnabled(!canStartNav);
				m_pActionSuspendNav->setEnabled(!canStartNav);
			}
		}
		else
		{
			m_pActionStartNav->setEnabled(false);
			m_pActionStopNav->setEnabled(false);
			m_pActionSuspendNav->setEnabled(false);
		}
	};

	nsp::toolkit::singleton<shr::gui::WatcherWaiter>::instance()->run(nsp::toolkit::singleton<net_interface>::instance(), func, std::ref(nav_t_), retCb);
}

void MainWnd::SlotActionDeleteMore()
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	m_pDrawMapWid->deleteMoreItem();
	//m_pDrawMapWid->SetOperaMode(ACTION_SELECT_MORE);
	
}

void MainWnd::SlotActionEditNodes()
{
	if (!m_pDrawMapWid){
		return;
	}
	m_pDrawMapWid->editMoreNodes();
}
void MainWnd::SlotActionEditEdges()
{
	if (!m_pDrawMapWid){
		return;
	}
	m_pDrawMapWid->editMoreEdges();
}

void MainWnd::SlotActionFormatItems()
{
	if (!m_pDrawMapWid){
		return;
	}
	m_pDrawMapWid->formatMoreItems();
}

void MainWnd::SlotActionQRcodeSave()
{
	if (!m_pDrawMapWid){
		return;
	}
	std::vector<CODEPOSXYA>vecCodeInfo;
	DATAMNG->GetAllCodeInfo(vecCodeInfo);

	QString qsFileName = QFileDialog::getSaveFileName(this, m_en ? "Save As" : QStringLiteral("另存为"), "XML/QRcodeData.xml", QStringLiteral("XML(*.xml)"));
	if (qsFileName.isEmpty())
	{
		return;
	}

	if (RWXml::write_QRcode_data(qsFileName.toLocal8Bit().toStdString(), vecCodeInfo) < 0){
		QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "failed to write_QRcode_data" : QStringLiteral("写二维码文件失败!"), QMessageBox::Ok);
	}
}

void MainWnd::timerEvent(QTimerEvent *event)
{
	if (!event)
	{
		return;
	}
	if (event->timerId() == m_timerLocation)
	{
		if (!m_bManualLocation)//如果没有开启手动定位，才实时更新车的位置
		{
			/*if (net_interface::Instance()->get_vehicle_pos_asyn() < 0)*/
			if (nsp::toolkit::singleton<net_interface>::instance()->get_vehicle_pos_asyn() < 0)
			{
				return;
			}
			POSXYA vehiclePos;
			if (!DATAMNG->GetVehiclePos(m_connectedVehicle.vehicle_ip, vehiclePos))
			{
				return;
			}
			if (m_pDrawMapWid)
			{
				m_pDrawMapWid->SetVehiclePos(m_connectedVehicle.vehicle_id, vehiclePos);
			}
			UpdateStatusBarVehiclePos(vehiclePos);
		}
	}
	if (event->timerId() == m_timeTaskStatus){
		QString text = m_en ? "TaskStatus:" : QStringLiteral("任务状态：");
		if (DATAMNG->GetTaskStatus() == kStatusDescribe_Completed){
			text = text + (m_en ? "Finished" : QStringLiteral("已完成"));
		}
		if (DATAMNG->GetTaskStatus() == kStatusDescribe_Startup){
			text = text + (m_en ? "Startup" : QStringLiteral("任务开始"));
		}
		if (m_pLabelTaskStatus){
			m_pLabelTaskStatus->setText(text);
		}
	}
}

void MainWnd::CallbackVehicleData(void *data, int type)
{
	
}

void MainWnd::CallbackUpdateMousePos(const QPointF& pos)
{
	if (m_pLabelMousePos != NULL)
	{
		QString strx = QString::number(DataConvert::map2world(pos.x()));
		QString stry = QString::number(DataConvert::map2world(pos.y()));
		QString str = (m_en ? "CursorPos:" : QStringLiteral("鼠标坐标：")) + strx + " m , " + stry + " m";
		m_pLabelMousePos->setText(str);
	}
}

void MainWnd::UpdateStatusBarVehiclePos(const POSXYA& pos)
{
	if (m_pLabelVehiclePos != NULL)
	{
		QString strx = QString::number(DataConvert::map2world(pos.pos_x));
		QString stry = QString::number(DataConvert::map2world(pos.pos_y));
		QString angle = QString::number(pos.angle);
		QString str = (m_en ? "VehiclePos:" : QStringLiteral("当前车位置：")) + strx + " m , " + stry + " m , " + angle + QStringLiteral(" 度");
		m_pLabelVehiclePos->setText(str);
	}
}

void MainWnd::SlotActionAddAnchor(bool bChecked)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	if (bChecked)
	{
		SetActionState(true, ActionType::ACTION_ADD_ANCHOR);
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_ADD_ANCHOR);
	}
	else
	{
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_SELECT);
	}
}

void MainWnd::SlotActionConnectNet()
{
	ConnectVehicleWnd connWnd;
	std::vector<VEHICLEINFO> vecVehicle;
	DATAMNG->GetAllVehicleInfo(vecVehicle);
	connWnd.SetVehicleInfo(vecVehicle);
	{
		std::lock_guard<decltype(m_mutexVehicleInfo)> guard(m_mutexVehicleInfo);
		connWnd.SetConnectVehicle(m_connectedVehicle);
	}
	
	if (connWnd.exec() == QDialog::Accepted)
	{
		VEHICLEINFO vehicleInfo;
		if (!connWnd.GetSelectVehicle(vehicleInfo))
		{
			QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "Connect Fail, Please Choice a Vehicle" : QStringLiteral("连接网络失败，请选择指定车辆!"), QMessageBox::Ok);
			return;
		}
		//int ret = net_interface::Instance()->login(vehicleInfo.vehicle_ip, vehicleInfo.vehicle_port);//初始化连接网络
		//int ret = nsp::toolkit::singleton<net_interface>::instance()->net_login(vehicleInfo.vehicle_ip, vehicleInfo.vehicle_port);//初始化连接网络
		int(net_interface::*func)(const std::string &, int) = &net_interface::net_login;
		const std::function<void(int)> &retCb = [&](int ret)->void{
			if (ret < 0)
			{
				QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "net_login Failed" : QStringLiteral("登录失败!"), QMessageBox::Ok);
				return;
			}

			m_bConnected = true;

			{
				std::lock_guard<decltype(m_mutexVehicleInfo)> guard(m_mutexVehicleInfo);
				m_connectedVehicle = vehicleInfo;
			}
			//net_interface::Instance()->set_callback_notify();//设置接收数据的回调函数
			nsp::toolkit::singleton<net_interface>::instance()->set_callback_notify();
		};
		nsp::toolkit::singleton<shr::gui::WatcherDialog>::instance()->run(nsp::toolkit::singleton<net_interface>::instance(), func, vehicleInfo.vehicle_ip, vehicleInfo.vehicle_port, retCb, this);

		//if (ret < 0)
		//{
		//	QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示"), m_en ? "net_login Failed" : QStringLiteral("登录失败!"), QMessageBox::Ok);
		//	return;
		//}
		//m_bConnected = true;
		//{
		//	std::lock_guard<decltype(m_mutexVehicleInfo)> guard(m_mutexVehicleInfo);
		//	m_connectedVehicle = vehicleInfo;
		//}
		////net_interface::Instance()->set_callback_notify();//设置接收数据的回调函数
		//nsp::toolkit::singleton<net_interface>::instance()->set_callback_notify();
	}
	
}

void MainWnd::SlotActionLocAnchor()
{
	POSXYA posXya;
	if( RequestVehiclePos(posXya) < 0 ) {
		return;
	}

	m_pDrawMapWid->AddAnchor( posXya );
}

int MainWnd::RequestVehiclePos(POSXYA& posXya)
{
	/*int ret = net_interface::Instance()->get_vehicle_pos( posXya );
	if( ret == QUERY_SUCCESS ) {
		return 0;
	}*/

	if (nullptr == m_pDrawMapWid)
	{
		return -1;
	}

	VehicleItem *viPtr = m_pDrawMapWid->GetScene()->GetCurrentVehicleItem();

	if (nullptr == viPtr)
	{
		return -1;
	}

	VEHICLEINFO vehicleInfo;

	if (!m_pDrawMapWid->GetVehicleInfo(viPtr->GetVehicleId(), vehicleInfo)) {
		return -1;
	}

	posXya.pos_x = vehicleInfo.pos_x;
	posXya.pos_y = vehicleInfo.pos_y;
	posXya.angle = vehicleInfo.angle;

	return 0;

}

void MainWnd::SlotActionOperate()
{
	OperateCtrlWnd opt_ctrl_wnd;
	if (opt_ctrl_wnd.exec() == QDialog::Accepted)
	{

	}
}

void MainWnd::SlotActionBezierLink(bool bChecked)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	if (bChecked)
	{
		SetActionState(true, ActionType::ACTION_LINK_CURVE);
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_LINK_CURVE);
	}
	else
	{
		m_pDrawMapWid->SetOperaMode(ActionType::ACTION_SELECT);
	}
}

void MainWnd::SlotActionLocNode()
{
	POSXYA posXya;
	if (RequestVehiclePos(posXya) < 0) {
		return;
	}
	m_pDrawMapWid->AddNode(posXya);
}

void MainWnd::keyPressEvent(QKeyEvent* keyEvent)
{
	if (!keyEvent){
		return;
	}
	switch (keyEvent->key()){
	case Qt::Key_Delete:
		if (QMessageBox::warning(this, m_en ? "Warning" : QStringLiteral("提示！"), m_en ? "Delete?" : QStringLiteral("确定删除所选元素？"),
			QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes){
			return;
		}
		if (m_pDrawMapWid){
			m_pDrawMapWid->DeleteSelectedItems();
		}
		break;
	case Qt::Key_F5:
	{
		if (m_generateEmapTimer.isNull())
		{
			return;
		}

		if (!m_generateEmapTimer->isActive())
		{
			m_generateEmapTimer->start(GENERATE_EMAP_TIMEOUT);
		}
		
		++m_f5Count;

		if (F5_COUNT == m_f5Count)
		{
			OnGenerateEmap();
			m_f5Count = 0;
		}
	}
		break;
	default:
		break;
	}
	QMainWindow::keyPressEvent(keyEvent);
}

void MainWnd::SlotActionShowEdgeList()
{
	if (!m_pActionShowEdgeList){
		return;
	}
	if (m_pActionShowEdgeList->isChecked()){
		ShowEdgeListShowWnd(true);
	}
	else{
		ShowEdgeListShowWnd(false);
	}
}

void MainWnd::SlotActionOneKeyAdjust()
{
	OneKeyAdjustDialog onekeyAdjustDlg(this);
	
	if (QDialog::Accepted == onekeyAdjustDlg.exec())
	{
		const std::vector<int>& vecWopId = onekeyAdjustDlg.GetWopIdVector();

		if (nullptr != m_pDrawMapWid)
		{
			m_pDrawMapWid->GetScene()->AdjustAllEdgeProperty(vecWopId);
		}
	}
}

void MainWnd::ShowEdgeListShowWnd(bool bShow)
{
	if (!m_pDockEdgeListShow){
		m_pDockEdgeListShow = new QDockWidget(m_en ? "EdgeList" : QStringLiteral("边查看器"), this);
		m_pDockEdgeListShow->setAllowedAreas(Qt::RightDockWidgetArea);
		m_pDockEdgeListShow->setWidget(m_pEdgeListWnd);
		m_pDockEdgeListShow->setMinimumWidth(600);
		addDockWidget(Qt::RightDockWidgetArea, m_pDockEdgeListShow);
		//std::vector<EDGEINFO> vecEdgeInfo;
		//DATAMNG->GetAllEdgeInfo(vecEdgeInfo);
		m_pEdgeListWnd->SetCallback(std::bind(&MainWnd::CallbackEdgeListClicked, this, std::placeholders::_1));
		//m_pEdgeListWnd->SetAllEdgesInfo(vecEdgeInfo);
	}
	std::vector<EDGEINFO> vecEdgeInfo;
	DATAMNG->GetAllEdgeInfo(vecEdgeInfo);
	m_pEdgeListWnd->SetAllEdgesInfo(vecEdgeInfo);

	if (m_pDockEdgeListShow){
		if (bShow){
			m_pDockEdgeListShow->show();
		}
		else{
			m_pDockEdgeListShow->hide();
		}
	}
}

bool MainWnd::ParseLayoutXml(const QString &filename, LAYOUTDATA &layoutData)
{
	int ret = RWXml::get_layout_data(filename.toLocal8Bit().toStdString(), layoutData);

	if (SUCCESS != ret && NO_POLYGON != ret)
	{
		return false;
	}

	return true;
}

void MainWnd::CallbackEdgeListClicked(int edge_id)
{
	if (!m_pDrawMapWid)
	{
		return;
	}
	SetActionState(true, ActionType::ACTION_SELECT);
	m_pDrawMapWid->SetEdgeId(edge_id);
}
