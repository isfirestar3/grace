#include "EdgeItem.h"
#include <QtCore/qmath.h>
#include <QtGui/QKeyEvent>
#include "Geometry.h"
#include "DataManage.h"
#include "MapScene.h"
#include "EdgeInfoWnd.h"

#define STROKER_WIDTH 10
EdgeItem::EdgeItem(NodeItem* pStartNode, NodeItem* pEndNode)
: m_pStartNode(pStartNode)
, m_pEndNode(pEndNode)
, m_pStartTermPt(NULL)
, m_pEndTermPt(NULL)
, m_mouse_hover(false)
{
	SetFeature();
	InitMembers();
	Adjust();
}

EdgeItem::~EdgeItem()
{
}

void EdgeItem::SetFeature()
{
	setOpacity(0.5);
	this->setFlag(ItemIsSelectable, true);
	setFlag(ItemIsFocusable);
	this->setAcceptedMouseButtons(Qt::LeftButton);
	this->setAcceptHoverEvents(true);
	setZValue(-10);
}

void EdgeItem::InitMembers()
{
	if (m_pStartNode == NULL || m_pEndNode == NULL)
	{
		return;
	}
	m_edgeInfo.start_node_id = m_pStartNode->GetNodeId();
	m_edgeInfo.end_node_id = m_pEndNode->GetNodeId();
	QPointF ctrlPos1 = InitCtrlPoint1Pos(m_pStartNode, m_pEndNode);
	QPointF ctrlPos2 = InitCtrlPoint2Pos(m_pStartNode, m_pEndNode);

	InitCtrlPoint(ctrlPos1, ctrlPos2);
	InitTerminalPoint(m_pStartNode->pos(), m_pEndNode->pos());

	m_pStartNode->SetEdgeItem(this);
	m_pEndNode->SetEdgeItem(this);
}

void EdgeItem::SetEdgeInfo(const EDGEINFO& edgeInfo)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexEdgeInfo);
	m_edgeInfo = edgeInfo;
	UpdateEdgeDirection(m_edgeInfo); 
}

void EdgeItem::GetEdgeInfo(EDGEINFO& edgeInfo)
{
	std::lock_guard<std::recursive_mutex> guard(m_mutexEdgeInfo);
	edgeInfo = m_edgeInfo;
}

void EdgeItem::Adjust()
{
	if (m_pStartNode == NULL || m_pEndNode == NULL)
	{
		return;
	}

	QLineF line(mapFromItem(m_pStartNode, 0, 0), mapFromItem(m_pEndNode, 0, 0));
	qreal length = line.length();

	prepareGeometryChange();
	QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);

	//if (length > qreal(20)) 
	//{
		m_startNodePos = mapFromItem(m_pStartNode, 0, 0);
		m_endNodePos = mapFromItem(m_pEndNode, 0, 0);
		m_ctrlPt1Pos = mapFromItem(m_pCtrlPoint1, 0, 0);
		m_ctrlPt2Pos = mapFromItem(m_pCtrlPoint2, QPointF(0,0));

		m_edgeInfo.ctrl_point1.x = mapToScene(m_ctrlPt1Pos).x();
		m_edgeInfo.ctrl_point1.y = mapToScene(m_ctrlPt1Pos).y();
		m_edgeInfo.ctrl_point2.x = mapToScene(m_ctrlPt2Pos).x();
		m_edgeInfo.ctrl_point2.y = mapToScene(m_ctrlPt2Pos).y();

		m_edgeInfo.termstpos.x = mapToScene(m_startNodePos).x();
		m_edgeInfo.termstpos.y = mapToScene(m_startNodePos).y();
		m_edgeInfo.termendpos.x = mapToScene(m_endNodePos).x();
		m_edgeInfo.termendpos.y = mapToScene(m_endNodePos).y();
	//}
	/*else 
	{
		m_startNodePos = m_endNodePos = line.p1();
	}*/

	SetTangentLine();

	m_edgeInfo.length = GetPath().length();
	DATAMNG->UpdateEdgeInfo(m_edgeInfo);
}

QPointF EdgeItem::InitCtrlPoint1Pos(NodeItem* pStartNode, NodeItem* pEndNode)
{
	if (pStartNode == NULL || pEndNode == NULL)
	{
		return QPointF(0.0,0.0);
	}
	QPointF startPos = pStartNode->scenePos();
	QPointF endPos = pEndNode->scenePos();
	return ((endPos - startPos) / 3 + startPos);
}

QPointF EdgeItem::InitCtrlPoint2Pos(NodeItem* pStartNode, NodeItem* pEndNode)
{
	if (pStartNode == NULL || pEndNode == NULL)
	{
		return QPointF(0.0, 0.0);
	}
	QPointF startPos = pStartNode->scenePos();
	QPointF endPos = pEndNode->scenePos();
	return ((endPos - startPos) / 3 * 2 + startPos);
}

void EdgeItem::InitCtrlPoint(QPointF ctrlPos1, QPointF ctrlPos2)
{
	m_pCtrlPoint1 = new CtrlPointItem(m_pStartNode, this);
	m_pCtrlPoint2 = new CtrlPointItem(m_pEndNode, this);
	if (m_pCtrlPoint1 == NULL || m_pCtrlPoint2 == NULL)
	{
		return;
	}
	m_pCtrlPoint1->setVisible(false);
	m_pCtrlPoint2->setVisible(false);

	m_pCtrlPoint1->setPos(ctrlPos1);
	m_pCtrlPoint2->setPos(ctrlPos2);

	m_edgeInfo.ctrl_point1.x = ctrlPos1.x();
	m_edgeInfo.ctrl_point1.y = ctrlPos1.y();
	m_edgeInfo.ctrl_point2.x = ctrlPos2.x();
	m_edgeInfo.ctrl_point2.y = ctrlPos2.y();
}

void EdgeItem::InitTerminalPoint(QPointF startPos, QPointF endPos)
{
	if (m_pStartNode == NULL || m_pEndNode == NULL)
	{
		return;
	}
	m_pStartTermPt = new TerminalPointItem(m_pStartNode, this);
	m_pEndTermPt = new TerminalPointItem(m_pEndNode, this);
	if (m_pStartTermPt == NULL || m_pEndTermPt == NULL)
	{
		return;
	}

	m_edgeInfo.termstpos.x = startPos.x();
	m_edgeInfo.termstpos.y = startPos.y();
	m_edgeInfo.termendpos.x = endPos.x();
	m_edgeInfo.termendpos.y = endPos.y();

	if (m_pStartNode->GetTshaped() == TRUE && m_edgeInfo.direction != Direction::FORWARD)
	{
		m_pStartTermPt->show();
	}
	else
	{
		m_pStartTermPt->hide();
	}

	if (m_pEndNode->GetTshaped() == TRUE && m_edgeInfo.direction != Direction::BACKWARD)
	{
		m_pEndTermPt->show();
	}
	else
	{
		m_pEndTermPt->hide();
	}
}

void EdgeItem::SetTangentLine()
{
	QPainterPath edge_path_ = GetPath();
	double length = edge_path_.length();
	double fst_tangent_angle_ = 0;
	double sec_tangent_angle_ = 0;

	fst_tangent_angle_ = edge_path_.angleAtPercent(0) + 180;
	if (fst_tangent_angle_ >= 360)
	{
		fst_tangent_angle_ = fst_tangent_angle_ - 360;
	}	

	sec_tangent_angle_ = edge_path_.angleAtPercent(1);

	if (m_lineTangent1.isNull())
	{
		m_lineTangent1 = QLineF(m_pStartNode->pos(), m_pEndNode->pos());
	}	

	m_lineTangent1.setP1(m_pStartNode->pos());
	m_lineTangent1.setAngle(fst_tangent_angle_);
	m_lineTangent1.setLength(length / 2);

	if (m_lineTangent2.isNull())
	{
		m_lineTangent2 = QLineF(m_pEndNode->pos(), m_pStartNode->pos());
	}

	m_lineTangent2.setP1(m_pEndNode->pos());
	m_lineTangent2.setAngle(sec_tangent_angle_);
	m_lineTangent2.setLength(length / 2);

	QPainterPath fstpath;
	fstpath.moveTo(m_lineTangent1.p1());
	fstpath.lineTo(m_lineTangent1.p2());
	m_pathTangent1 = fstpath;

	QPainterPath secpath;
	secpath.moveTo(m_lineTangent2.p1());
	secpath.lineTo(m_lineTangent2.p2());
	m_pathTangent2 = secpath;
}

void EdgeItem::UpdateCtrlPoint(QPointF ctrlPos1, QPointF ctrlPos2)
{
	if (m_pCtrlPoint1 == NULL || m_pCtrlPoint2 == NULL)
	{
		return;
	}
	m_pCtrlPoint1->setPos(ctrlPos1);
	m_pCtrlPoint2->setPos(ctrlPos2);
}

QPainterPath EdgeItem::GetPath()
{
	QPainterPath path;
	path.moveTo(m_startNodePos);
	path.cubicTo(m_ctrlPt1Pos, m_ctrlPt2Pos, m_endNodePos);
	return path;
}

void EdgeItem::AdjustTerminalPoints()
{
	if (m_edgeInfo.terminal_start_percent < 0)
	{
		m_pStartTermPt->setPos(m_pathTangent1.pointAtPercent(-m_edgeInfo.terminal_start_percent));
	}
	else
	{
		m_pStartTermPt->setPos(GetPath().pointAtPercent(m_edgeInfo.terminal_start_percent));
	}
	if (m_edgeInfo.terminal_end_percent < 0)
	{
		m_pEndTermPt->setPos(m_pathTangent2.pointAtPercent(-m_edgeInfo.terminal_end_percent));
	}
	else
	{
		m_pEndTermPt->setPos(GetPath().pointAtPercent(m_edgeInfo.terminal_end_percent));
	}
}

QPainterPath EdgeItem::shape() const
{
	if (isSelected())
	{
		QPainterPath path;
		path.moveTo(m_startNodePos);
		path.cubicTo(m_ctrlPt1Pos, m_ctrlPt2Pos, m_endNodePos);

		path.addPath(m_pCtrlPoint1->mapToScene(m_pCtrlPoint1->shape()));
		path.addPath(m_pCtrlPoint2->mapToScene(m_pCtrlPoint2->shape()));

		QPainterPathStroker stroker;
		stroker.setWidth(STROKER_WIDTH);
		if (m_mouse_hover)
		{
			stroker.setWidth(1.5 * STROKER_WIDTH);
		}
		return stroker.createStroke(path);
	}
	else
	{
		QPainterPath path;
		path.moveTo(m_startNodePos);
		path.cubicTo(m_ctrlPt1Pos, m_ctrlPt2Pos, m_endNodePos);

		QPainterPathStroker stroker;
		stroker.setWidth(STROKER_WIDTH);
		if (m_mouse_hover)
		{
			stroker.setWidth(1.5 * STROKER_WIDTH);
		}
		return stroker.createStroke(path);
	}
}

QRectF EdgeItem::boundingRect() const
{
	if (m_pStartNode == NULL || m_pEndNode == NULL)
	{
		return QRectF();
	}

	QPainterPath path;
	path.moveTo(m_startNodePos);
	path.cubicTo(m_ctrlPt1Pos, m_ctrlPt2Pos, m_endNodePos);
	QPainterPathStroker stroker;
	stroker.setWidth(2.5 * STROKER_WIDTH * 2);
	path = stroker.createStroke(path);

	QRectF fstlinerect = m_pathTangent1.boundingRect();
	QRectF seclinerect = m_pathTangent2.boundingRect();

	QRectF linesrect = fstlinerect.united(seclinerect);

	return (path.controlPointRect() | childrenBoundingRect()).united(linesrect);
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	DrawEdge(painter);
}

void EdgeItem::DrawEdge(QPainter* painter)
{
	if (painter == NULL || m_pStartNode == NULL || m_pEndNode == NULL)
	{
		return;
	}

	QLineF line(m_startNodePos, m_endNodePos);//判断这条线的长度，是否画线
	if (qFuzzyCompare(line.length(), qreal(0.)))
	{
		return;
	}
	QPainterPath path;
	path.moveTo(m_startNodePos);
	path.cubicTo(m_ctrlPt1Pos, m_ctrlPt2Pos, m_endNodePos);

	QPainterPathStroker stroker;
	stroker.setWidth(STROKER_WIDTH);
	if (m_mouse_hover)
	{
		stroker.setWidth(1.5 * STROKER_WIDTH);
	}

	stroker.setCapStyle(Qt::RoundCap);
	stroker.setJoinStyle(Qt::RoundJoin);
	path = stroker.createStroke(path);


//	painter->setPen(Qt::NoPen);

	if (isSelected()) 
	{
		//画线
		if (Direction::BOTH == m_edgeInfo.direction)
		{
			judge_line_ctrl_point() ? painter->setBrush(QColor(Qt::green)) : painter->setBrush(QColor(255, 0, 255, 255));

		}
		else
		{
			judge_line_ctrl_point() ? painter->setBrush(QColor(Qt::green)) : painter->setBrush(QColor(0, 128, 192, 255));
		}
	}
	else
	{
		//画线
		if (Direction::BOTH == m_edgeInfo.direction)
		{
			judge_line_ctrl_point() ? painter->setBrush(QColor(Qt::green)) : painter->setBrush(QColor(255, 128, 255, 255));
		}
		else
		{
			judge_line_ctrl_point() ? painter->setBrush(QColor(Qt::green)) : painter->setBrush(QColor(0, 255, 255, 255));
		}
	}
	if (m_color_type == NAV_PATH_TYPE)
	{
		painter->setBrush(QColor(10, 194, 37, 255));
	}
	painter->setPen(QPen(painter->brush().color(), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->drawPath(path);
	DrawEdgeText(painter);
	DrawEdgeArrow(painter);
	DrawTangentLine(painter);
}

void EdgeItem::DrawEdgeText(QPainter* painter)
{
	QPainterPath path;
	path.moveTo(m_startNodePos);
	path.cubicTo(m_ctrlPt1Pos, m_ctrlPt2Pos, m_endNodePos);

	QPointF textPoint = path.pointAtPercent(0.5);
	painter->setFont(QFont("Times", 12, QFont::Normal));
	QPen pen = QPen(Qt::black, 1);
	painter->setPen(pen);
	QString strText;
	strText = QString::number(m_edgeInfo.id);
	painter->save();
	QTransform transform = painter->transform();
	transform.translate(textPoint.x(), textPoint.y());
	transform.scale(1, -1);
	painter->setTransform(transform);
	painter->drawText(pos(), strText);
	painter->restore();
}

void EdgeItem::DrawEdgeArrow(QPainter *painter)
{
	QPainterPath path;
	path.moveTo(m_startNodePos);
	path.cubicTo(m_ctrlPt1Pos, m_ctrlPt2Pos, m_endNodePos);
	int length = path.length() / 40;

	switch (m_edgeInfo.direction)
	{
	case Direction::FORWARD:
		{
			   QPointF arrowendnear = path.pointAtPercent(0.95);
			   QPointF arrowend = path.pointAtPercent(1);
			   QPolygonF polygon;
			   CalculateArrow(polygon, arrowendnear, arrowend);
			   painter->setBrush(QColor(Qt::red));
			   painter->drawPolygon(polygon);
			   break;
		}
	case Direction::BACKWARD:
		{
			   QPointF arrowendnear = path.pointAtPercent(0.05);
			   QPointF arrowend = path.pointAtPercent(0);
			   QPolygonF polygon;
			   CalculateArrow(polygon, arrowendnear, arrowend);
			   painter->setBrush(QColor(Qt::blue));
			   painter->drawPolygon(polygon);
			   break;
		}
	case Direction::BOTH:
		{
			   QPointF arrowendnear = path.pointAtPercent(0.95);
			   QPointF arrowend = path.pointAtPercent(1);
			   QPolygonF polygon;
			   CalculateArrow(polygon, arrowendnear, arrowend);
			   painter->setBrush(QColor(Qt::red));
			   painter->drawPolygon(polygon);

			   QPolygonF polygon1;
			   arrowendnear = path.pointAtPercent(0.05);
			   arrowend = path.pointAtPercent(0);
			   CalculateArrow(polygon1, arrowendnear, arrowend);
			   painter->setBrush(QColor(Qt::blue));
			   painter->drawPolygon(polygon1);
			   break;
		}
	default:
		break;
	}
	/*if (judge_line_ctrl_point()){
		painter->setBrush(QColor(Qt::green));
	}*/
}
//判断起点终点在一条直线上时，控制点的位置是否正确，确保一条直线是没有重叠的部分
bool EdgeItem::judge_line_ctrl_point()
{
	//xy坐标相等的情况
	//if ((fabs(m_edgeInfo.termstpos.x - m_edgeInfo.termendpos.x) < 3) && //x坐标相等
	//	(fabs(m_edgeInfo.ctrl_point1.x - m_edgeInfo.ctrl_point2.x) < 3) &&
	//	(fabs(m_edgeInfo.ctrl_point1.x - m_edgeInfo.termstpos.x) < 3))
	//{
	//	double c1 = fabs(m_edgeInfo.termstpos.y - m_edgeInfo.ctrl_point1.y);
	//	double c2 = fabs(m_edgeInfo.termendpos.y - m_edgeInfo.ctrl_point2.y);
	//	double c12 = fabs(m_edgeInfo.ctrl_point1.y - m_edgeInfo.ctrl_point2.y);
	//	double c = fabs(m_edgeInfo.termstpos.y - m_edgeInfo.termendpos.y) / 3;
	//	if ((fabs(c1 - c) > 10) || (fabs(c2 - c) > 10) || (fabs(c12 - c) > 10)){
	//		return true;
	//	}
	//}
	//else if ((fabs(m_edgeInfo.termstpos.y - m_edgeInfo.termendpos.y) < 3) && //y坐标相等
	//	(fabs(m_edgeInfo.ctrl_point1.y - m_edgeInfo.ctrl_point2.y) < 3) &&
	//	(fabs(m_edgeInfo.ctrl_point1.y - m_edgeInfo.termstpos.y) < 3))
	//{
	//	double c1 = fabs(m_edgeInfo.termstpos.x - m_edgeInfo.ctrl_point1.x);
	//	double c2 = fabs(m_edgeInfo.termendpos.x - m_edgeInfo.ctrl_point2.x);
	//	double c12 = fabs(m_edgeInfo.ctrl_point1.x - m_edgeInfo.ctrl_point2.x);
	//	double c = fabs(m_edgeInfo.termstpos.x - m_edgeInfo.termendpos.x) / 3;
	//	if ((fabs(c1 - c) > 10) || (fabs(c2 - c) > 10) || (fabs(c12-c) > 10)){
	//		return true;
	//	}
	//}

	//xy坐标不一定相等，根据斜率计算，此功能只用于直线，曲线不管
	double angle1 = atan2(fabs(m_edgeInfo.termstpos.y - m_edgeInfo.ctrl_point1.y), fabs(m_edgeInfo.termstpos.x - m_edgeInfo.ctrl_point1.x));
	double angle2 = atan2(fabs(m_edgeInfo.termendpos.y - m_edgeInfo.ctrl_point2.y), fabs(m_edgeInfo.termendpos.x - m_edgeInfo.ctrl_point2.x));
	double angle0 = atan2(fabs(m_edgeInfo.termendpos.y - m_edgeInfo.termstpos.y), fabs(m_edgeInfo.termendpos.x - m_edgeInfo.termstpos.x));
	if ((fabs(angle1 - angle2) < 0.5) && (fabs(angle1 - angle0) < 0.5) && (fabs(angle0 - angle2) < 0.5)){
		double length01 = sqrt(pow(m_edgeInfo.termstpos.x - m_edgeInfo.ctrl_point1.x, 2) + pow(m_edgeInfo.termstpos.y - m_edgeInfo.ctrl_point1.y, 2));
		double length32 = sqrt(pow(m_edgeInfo.termendpos.x - m_edgeInfo.ctrl_point2.x, 2) + pow(m_edgeInfo.termendpos.y - m_edgeInfo.ctrl_point2.y, 2));
		double length12 = sqrt(pow(m_edgeInfo.ctrl_point1.x - m_edgeInfo.ctrl_point2.x, 2) + pow(m_edgeInfo.ctrl_point1.y - m_edgeInfo.ctrl_point2.y, 2));
		double length03 = sqrt(pow(m_edgeInfo.termstpos.x - m_edgeInfo.termendpos.x, 2) + pow(m_edgeInfo.termstpos.y - m_edgeInfo.termendpos.y, 2)) / 3;
		if ((fabs(length01 - length03) > 10) || (fabs(length32 - length03) > 10) || (fabs(length12 - length03) > 10)){
				return true;
			}
	}
	return false;
}
void EdgeItem::CalculateArrow(QPolygonF &polygon, QPointF &arrowendnear, QPointF &arrowend)
{
	double pathlength = STROKER_WIDTH * 1.5;
	if (m_mouse_hover)
	{
		pathlength = 1.5 * pathlength;
	}

	QLineF dir_line(arrowendnear, arrowend);
	double dir_angle = (360 - dir_line.angle())* M_PI / 180;

	double k = tan(dir_angle);
	double nk = -1 / k;
	double normal_agl = atan(nk);

#if 1
	QPointF ar0 = arrowend - QPointF(pathlength * cos(dir_angle), pathlength * sin(dir_angle));
	QPointF ar1 = ar0 + QPointF(pathlength * cos(normal_agl), pathlength * sin(normal_agl));
	QPointF ar2 = ar0 - QPointF(pathlength * cos(normal_agl), pathlength * sin(normal_agl));
	polygon << arrowend << ar1 << ar2;

#else
	QPointF ar0 = arrowend - 1.5 *QPointF(pathlength * cos(dir_angle), pathlength * sin(dir_angle));
	QPointF ar1 = ar0 + QPointF(pathlength * cos(normal_agl), pathlength * sin(normal_agl));
	QPointF ar2 = ar0 - QPointF(pathlength * cos(normal_agl), pathlength * sin(normal_agl));
	QPointF ar3 = arrowend - QPointF(pathlength * cos(dir_angle), pathlength * sin(dir_angle));
	polygon << arrowend << ar1 << ar3 << ar2;
#endif
}

void EdgeItem::DrawTangentLine(QPainter *painter)
{
	if (isSelected())
	{
		painter->setPen(QPen(Qt::darkGreen, 6, Qt::DashLine));
	}
	else
	{
		painter->setPen(QPen(Qt::green, 3, Qt::DashLine));
	}
	if (m_pStartNode == NULL || m_pEndNode == NULL)
	{
		return;
	}
	painter->setPen(Qt::DotLine);
	if (m_pStartNode->GetTshaped() == TRUE && m_edgeInfo.direction != Direction::FORWARD)
	{
		painter->drawLine(m_lineTangent1);
	}
	if (m_pEndNode->GetTshaped() == TRUE && m_edgeInfo.direction != Direction::BACKWARD)
	{
		painter->drawLine(m_lineTangent2);
	}
}

NodeItem* EdgeItem::StartNode() const
{
	return m_pStartNode;
}

NodeItem* EdgeItem::EndNode() const
{
	return m_pEndNode;
}

void EdgeItem::SetItemMoveable(bool bMoveable)
{
	if (m_pCtrlPoint1)
	{
		m_pCtrlPoint1->SetMoveable(bMoveable);
	}
	if (m_pCtrlPoint2)
	{
		m_pCtrlPoint2->SetMoveable(bMoveable);
	}
	if (m_pStartTermPt)
	{
		m_pStartTermPt->SetMoveable(bMoveable);
	}
	if (m_pEndTermPt)
	{
		m_pEndTermPt->SetMoveable(bMoveable);
	}
}

void EdgeItem::AdjustMoveTermPt(NodeItem* node, const QPointF& newpos)
{
	QLineF newline(node->scenePos(), newpos);
	double moveline_angle = newline.angle();
	double dangle = 0;
	NearPoint newnearpos;
	QPointF setpos;
	QPainterPath path;
	if (node == m_pStartNode)
	{
		dangle = VecPosition::normalizeAngle(newline.angle() - m_lineTangent1.angle());

		if ((dangle <= 90) && (dangle >= -90))
		{
			path = m_pathTangent1;
			newnearpos = FindNearPos(path, newpos, 0, 1);
			m_edgeInfo.terminal_start_percent = -newnearpos.percent_near;
		}
		else
		{
			path = GetPath();
			newnearpos = FindNearPos(path, newpos, 0, 0.5);
			m_edgeInfo.terminal_start_percent = newnearpos.percent_near;
		}
		m_pStartTermPt->setPos(newnearpos.point_near);
	}
	else if (node == m_pEndNode)
	{
		dangle = VecPosition::normalizeAngle(newline.angle() - m_lineTangent2.angle());

		if ((dangle <= 90) && (dangle >= -90)){
			path = m_pathTangent2;
			newnearpos = FindNearPos(path, newpos, 0, 1);
			m_edgeInfo.terminal_end_percent = -newnearpos.percent_near;
		}
		else {
			path = GetPath();
			newnearpos = FindNearPos(path, newpos, 0.5, 1);
			m_edgeInfo.terminal_end_percent = newnearpos.percent_near;
		}
		m_pEndTermPt->setPos(newnearpos.point_near);
	}
	DATAMNG->UpdateEdgeInfo(m_edgeInfo);
}

NearPoint EdgeItem::FindNearPos(QPainterPath path, QPointF newpos, double stper, double endper)
{
	NearPoint nearpos;

	nearpos.min_distance = 100000000;

	int start = (int)(200 * stper);
	int end = (int)(200 * endper);
	for (int i = start; i < end; i++) 
	{
		double percent = i * 0.005;
		QPointF pos = path.pointAtPercent(percent);

		double dis = GeometryCompute::CalDisTowPoint(newpos, pos);
		if (nearpos.min_distance > dis) 
		{
			nearpos.min_distance = dis;
			nearpos.percent_near = percent;
			nearpos.point_near = pos;
		}

	}
	return nearpos;
}

void EdgeItem::UpdateWhenTermPtChanged()
{
	if (m_pStartNode != NULL)
	{
		m_termPt1Pos = mapFromItem((QGraphicsItem *)m_pStartNode, 0, 0);
		m_edgeInfo.termstpos.x = mapToScene(m_termPt1Pos).x();
		m_edgeInfo.termstpos.y = mapToScene(m_termPt1Pos).y();
	}
	if (m_pEndNode != NULL)
	{
		m_termPt2Pos = mapFromItem((QGraphicsItem *)m_pEndNode, 0, 0);
		m_edgeInfo.termendpos.x = mapToScene(m_termPt2Pos).x();
		m_edgeInfo.termendpos.y = mapToScene(m_termPt2Pos).y();
	}
	DATAMNG->UpdateEdgeInfo(m_edgeInfo);
}

void EdgeItem::ShowControlPoint(bool bShow)
{
	if (m_pCtrlPoint1)
	{
		m_pCtrlPoint1->setVisible(bShow);
	}
	if (m_pCtrlPoint2)
	{
		m_pCtrlPoint2->setVisible(bShow);
	}
}

void EdgeItem::keyPressEvent(QKeyEvent *event) {

	if (event->modifiers() & Qt::ControlModifier) {
		
		bool changed = true;
		double dx = 0.0;
		double dy = 0.0;

		switch (event->key()) {

			case Qt::Key_Left:

				dx = -1.0;
				break;
			case Qt::Key_Right:
			
				dx = 1.0;
				break;
			case Qt::Key_Up:
			
				dy = 1.0;
				break;
			case Qt::Key_Down:
			
				dy = -1.0;
				break;
			default:
				changed = false;
		}
		if (changed) {
		
			QPointF pos_ = m_pStartNode->pos();
			pos_.rx() += dx;
			pos_.ry() += dy;
			m_pStartNode->setPos(pos_);
			
			pos_ = m_pEndNode->pos();
			pos_.rx() += dx;
			pos_.ry() += dy;
			m_pEndNode->setPos(pos_);

			pos_ = m_pCtrlPoint1->pos();
			pos_.rx() += dx;
			pos_.ry() += dy;
			m_pCtrlPoint1->setPos(pos_);

			pos_ = m_pCtrlPoint2->pos();
			pos_.rx() += dx;
			pos_.ry() += dy;
			m_pCtrlPoint2->setPos(pos_);
		}
	}
}

void EdgeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (static_cast<MapScene*>(scene())->GetActionType() == ActionType::ACTION_SELECT ||
		static_cast<MapScene*>(scene())->GetActionType() == ActionType::ACTION_HAND_DRAG)
	{
		EdgeInfoWnd edgeInfoWnd;
		edgeInfoWnd.SetEdgeInfo(m_edgeInfo);
		if (edgeInfoWnd.exec() == QDialog::Accepted)
		{
			edgeInfoWnd.GetEdgeInfo(m_edgeInfo);
			UpdateEdgeDirection(m_edgeInfo);
			DATAMNG->UpdateEdgeInfo(m_edgeInfo);
			update();
		}
	}
	QGraphicsItem::mouseDoubleClickEvent(event);
}

void EdgeItem::ChangeEdgeColor(double percent, Direction direction, int type)
{
	m_color_type = type;
	m_start_percent = percent;
	m_direction = direction;
	update();
}

void EdgeItem::UpdateEdgeDirection(EDGEINFO& edgeInfo)
{
	Direction dirTemp = FORWARD;
	for (size_t i = 0; i < edgeInfo.wop_list.size(); ++i)
	{
		WOP wop;
		if (DATAMNG->GetWopById(edgeInfo.wop_list[i], wop))
		{
			if (i == 0)
			{
				dirTemp = wop.direction;
			}
			else
			{
				if (dirTemp != wop.direction)
				{
					dirTemp = BOTH;
				}
			}
		}
	}
	edgeInfo.direction = dirTemp;
	if (dirTemp == BACKWARD || dirTemp == BOTH)
	{
		edgeInfo.backward = TRUE;
	}
	else
	{
		edgeInfo.backward = FALSE;
	}
}

void EdgeItem::GetEdgeWopList(std::vector<int>& vecWopId)
{
	std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
	vecWopId = m_edgeInfo.wop_list;
}

void EdgeItem::SetEdgeWopList(const std::vector<int>& vecWopId)
{
	{ 
		std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
		m_edgeInfo.wop_list = vecWopId; 
	}
	UpdateEdgeDirection(m_edgeInfo);
	DATAMNG->UpdateEdgeInfo(m_edgeInfo);
	update();
}

void EdgeItem::SetEdgeSpeed(const std::vector<EDGESPEED>& speed)
{
	{
		std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
		m_edgeInfo.speed = speed;
	}
	DATAMNG->UpdateEdgeInfo(m_edgeInfo);
}
void EdgeItem::SetEdgeRealDistance(const double length)
{
	{
		std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
		m_edgeInfo.reLength = length;
	}
	DATAMNG->UpdateEdgeInfo(m_edgeInfo);
}
void EdgeItem::SetEdgeWopAttr(const std::vector<WOPATTR>& wopAttrList)
{
	{
		std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
		m_edgeInfo.wop_attr_list = wopAttrList;
	}
	DATAMNG->UpdateEdgeInfo(m_edgeInfo);
}

void EdgeItem::RemoveFromNode()
{
	if (m_pStartNode)
	{
		m_pStartNode->RemoveEdge(this);
	}
	if (m_pEndNode)
	{
		m_pEndNode->RemoveEdge(this);
	}
}

void EdgeItem::UpdateEdgeAttr(const EDGEINFO& edgeInfo)
{
	SetEdgeSpeed(edgeInfo.speed);
	SetEdgeWopList(edgeInfo.wop_list);
	SetEdgeWopAttr(edgeInfo.wop_attr_list);
	SetEdgeRealDistance(edgeInfo.reLength);
}

void  EdgeItem::UpdateEdgeInfo(EDGEINFO& edgeInfo)
{
	{
		std::lock_guard<decltype(m_mutexEdgeInfo)> guard(m_mutexEdgeInfo);
		m_edgeInfo.wop_list = edgeInfo.wop_list;
		m_edgeInfo.wop_attr_list = edgeInfo.wop_attr_list;
		m_edgeInfo.reLength = edgeInfo.reLength;
		m_edgeInfo.speed = edgeInfo.speed;
		Direction dirTemp = FORWARD;
		for (size_t i = 0; i < edgeInfo.wop_list.size(); ++i)
		{
			WOP wop;
			if (DATAMNG->GetWopById(edgeInfo.wop_list[i], wop))
			{
				if (i == 0)
				{
					dirTemp = wop.direction;
				}
				else
				{
					if (dirTemp != wop.direction)
					{
						dirTemp = BOTH;
					}
				}
			}
		}
		m_edgeInfo.direction = dirTemp;
		if (dirTemp == BACKWARD || dirTemp == BOTH)
		{
			m_edgeInfo.backward = TRUE;
		}
		else
		{
			m_edgeInfo.backward = FALSE;
		}
	}
	DATAMNG->UpdateEdgeInfo(m_edgeInfo);
	update();
}
