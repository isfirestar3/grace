#include "EMapScene.h"
#include "EMapNode.h"
#include "EMapEdge.h"
#include "EMapEdgeDirection.h"
#include "EMapAgv.h"
#include "EMapAgvId.h"
#include "CoordinateSystem.hpp"

EMapScene::EMapScene(QObject *parent)
	: QGraphicsScene(parent)
{
	robot_pos_update_ = false;
	allocate_agv_update_ = false;
	setParent(parent);
	setItemIndexMethod(QGraphicsScene::NoIndex);
	//setItemIndexMethod(QGraphicsScene::BspTreeIndex);
	//setSceneRect(-100000, -100000, 200000, 200000);
	//setSceneRect(-5000, -5000, 5000, 5000);
}

EMapScene::~EMapScene()
{

}

void EMapScene::SetAgvSize(double agv_width, double agv_head, double agv_tail)
{
	agv_width_ = agv_width;
	agv_head_ = agv_head;
	agv_tail_ = agv_tail;
	startTimer(200);
}

void EMapScene::LoadMap(std::string file_name, double pos_x, double pos_y )
{
	QImage m_image;
	bool b_load = m_image.load(file_name.c_str(), "bmp");
	if (!b_load) {
		return;
	}

	setSceneRect(pos_x * 10, pos_y * 10, m_image.size().width() * 10, m_image.size().height() * 10);

	m_image = m_image.scaled(m_image.size().width() * 10, m_image.size().height() * 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	QPixmap m_pixmap = QPixmap::fromImage(m_image);
	m_pixmap = m_pixmap.transformed(QMatrix(1.0, 0.0 , 0.0 , -1.0 , 0.0 , 0.0), Qt::SmoothTransformation);

	QGraphicsPixmapItem *pixmap_item = new QGraphicsPixmapItem(m_pixmap);
	pixmap_item->setZValue(-100);
	pixmap_item->setPos(pos_x * 10, pos_y * 10);
	this->addItem(pixmap_item);

	//setSceneRect(pos_x * 10 - 200, pos_y * 10 - 200, m_image.size().width() * 10 + 400, m_image.size().height() * 10 + 400);
}

void EMapScene::LoadNodeEdgeData(const std::vector<PathSearch_NodeInfo>& node_datas, const std::vector<PathSearch_EdgeInfo>& edge_datas, bool bShow)
{
	for (unsigned int i = 0; i < node_datas.size(); ++i) {
		PathSearch_NodeInfo node_data = node_datas[i];

		EMapNode *m_node = new EMapNode(node_data.node_id);
		m_node->setPos(QPointF(CoordinateSystem::Map2Scene(node_data.pos_x/100.0), CoordinateSystem::Map2Scene(node_data.pos_y/100.0)));
		m_node->SetShow(bShow);
		this->addItem(m_node);
		node_items_[node_data.node_id] = m_node;
	}

	for (unsigned int i = 0; i < edge_datas.size(); ++i) {
		PathSearch_EdgeInfo edge_data = edge_datas[i];

		PathSearch_NodeInfo start_node;
		if (false == this->FindNodeData(node_datas, edge_data.start_node_id, start_node)) {
			continue;
		}

		PathSearch_NodeInfo end_node;
		if (false == this->FindNodeData(node_datas, edge_data.end_node_id, end_node)) {
			continue;
		}

		EMapEdge *m_edge = new EMapEdge(edge_data.id, QPointF(CoordinateSystem::Map2Scene(start_node.pos_x / 100.0), CoordinateSystem::Map2Scene(start_node.pos_y / 100.0)),
			QPointF(CoordinateSystem::Map2Scene(edge_data.ctrl_point1.x / 100.0), CoordinateSystem::Map2Scene(edge_data.ctrl_point1.y / 100.0)),
			QPointF(CoordinateSystem::Map2Scene(edge_data.ctrl_point2.x / 100.0), CoordinateSystem::Map2Scene(edge_data.ctrl_point2.y / 100.0)),
			QPointF(CoordinateSystem::Map2Scene(end_node.pos_x / 100.0), CoordinateSystem::Map2Scene(end_node.pos_y / 100.0)),
										(EMapEdge::EdgeDirection)edge_data.direction, edge_data.backward);
		this->addItem(m_edge);
		edge_items_[edge_data.id] = m_edge;

		EMapEdgeDirection *m_edge_direction = new EMapEdgeDirection(QPointF(CoordinateSystem::Map2Scene(start_node.pos_x / 100.0), CoordinateSystem::Map2Scene(start_node.pos_y / 100.0)),
			QPointF(CoordinateSystem::Map2Scene(edge_data.ctrl_point1.x / 100.0), CoordinateSystem::Map2Scene(edge_data.ctrl_point1.y / 100.0)),
			QPointF(CoordinateSystem::Map2Scene(edge_data.ctrl_point2.x / 100.0), CoordinateSystem::Map2Scene(edge_data.ctrl_point2.y / 100.0)),
			QPointF(CoordinateSystem::Map2Scene(end_node.pos_x / 100.0), CoordinateSystem::Map2Scene(end_node.pos_y / 100.0)),
			(EMapEdgeDirection::EdgeDirection)edge_data.direction, edge_data.backward);


		this->addItem(m_edge_direction);
	}
}

void EMapScene::SetRobotPos(const std::map<int, PathSearch_Position>& robot_pos)
{
	robot_pos_mux_.lock();
	robot_pos_ = robot_pos;
	robot_pos_update_ = true;
	robot_pos_mux_.unlock();
}

void EMapScene::SetAllocateAgv(const std::vector<std::pair<int, int> > &node_allocate_agv, const std::vector<std::pair<std::pair<int, int>, int> > &segment_allocate_agv)
{
	allocate_agv_mux_.lock();
	node_allocate_agv_ = node_allocate_agv;
	segment_allocate_agv_ = segment_allocate_agv;
	allocate_agv_update_ = true;
	allocate_agv_mux_.unlock();
}

void EMapScene::timerEvent(QTimerEvent *event)
{
	this->UpdateRobotPos();
	this->UpdateAllovateAgv();
}

void EMapScene::UpdateRobotPos()
{
	if (false == robot_pos_update_) {
		return;
	}
	std::map<int, PathSearch_Position> robot_pos = this->GetRobotPos();
	for (std::map<int, PathSearch_Position>::iterator iter = robot_pos.begin();
		iter != robot_pos.end(); ++iter) {
		int m_car_id = iter->first;
		PathSearch_Position m_car_pos = iter->second;

		EMapAgv * emap_agv = NULL;
		emap_agv = this->FindAgvItem(m_car_id);
		if (NULL == emap_agv) {
			emap_agv = new EMapAgv(m_car_id);
			emap_agv->SetAgvSize(agv_width_, agv_head_, agv_tail_);
			agv_items_.insert(std::map<int, EMapAgv*>::value_type(m_car_id, emap_agv));
			this->addItem(emap_agv);
		}
		emap_agv->setPos(QPointF(CoordinateSystem::Map2Scene(m_car_pos.x), CoordinateSystem::Map2Scene(m_car_pos.y)));
		emap_agv->setRotation(m_car_pos.angle * 180.0 / 3.1415926);

		EMapAgvId * emap_agv_id = NULL;
		emap_agv_id = this->FindAgvIdItem(m_car_id);
		if (NULL == emap_agv_id) {
			emap_agv_id = new EMapAgvId(m_car_id);
			agv_id_items_.insert(std::map<int, EMapAgvId*>::value_type(m_car_id, emap_agv_id));
			this->addItem(emap_agv_id);
		}
		emap_agv_id->setPos(QPointF(CoordinateSystem::Map2Scene(m_car_pos.x), CoordinateSystem::Map2Scene(m_car_pos.y)));
	}
	//////////////////////////////////////////////////////////////////////////
	//¨¦?3y¡Á¡é?¨²¦Ì?¦Ì?AGV
	if (robot_pos.size() != agv_items_.size())
	{
		std::map<int, EMapAgv*> m_agvs = agv_items_;
		for (std::map<int, EMapAgv*>::iterator iter = m_agvs.begin();
			iter != m_agvs.end(); ++iter)
		{
			std::map<int, PathSearch_Position>::iterator p_iter = robot_pos.find(iter->first);
			if (p_iter == robot_pos.end())
			{
				///?¦Ì?¡Â2?3¦Ì¨°?¡Á¡é?¨²¡ê?¨¦?¦Ì?¦Ì?¡Á¨®¦Ì?¨ª??D¦Ì?2?3¦Ì
				std::map<int, EMapAgv *>::iterator agv_iter = agv_items_.find(iter->first);
				std::map<int, EMapAgvId * >::iterator id_iter = agv_id_items_.find(iter->first);
				this->removeItem(id_iter->second);
				agv_id_items_.erase(id_iter);
				this->removeItem(agv_iter->second);
				agv_items_.erase(agv_iter);
			}
		}

	}
	
	robot_pos_update_ = false;
}

void EMapScene::UpdateAllovateAgv()
{
	if (false == allocate_agv_update_) {
		return;
	}

	std::vector<std::pair<int, int> > node_allocate_agv = this->GetNodeAllocateAgv();
	for (unsigned int i = 0; i < node_allocate_agv.size(); ++i) {
		std::pair<int, int> m_node_allocate_agv = node_allocate_agv[i];
		this->SetNodeColor(m_node_allocate_agv.first, this->GetAgvColor(m_node_allocate_agv.second), m_node_allocate_agv.second == -1 ? false : true);
	}

	std::map<int, int> segment_allocate_agv;

	std::vector<std::pair<std::pair<int, int>, int> > segment_wop_allocate_agv = this->GetSegmentAllocateAgv();
	for (unsigned int i = 0; i < segment_wop_allocate_agv.size(); ++i) {
		std::pair<std::pair<int, int>, int> m_segment_wop_allocate_agv = segment_wop_allocate_agv[i];
		std::map<int, int>::iterator iter_find = segment_allocate_agv.find(m_segment_wop_allocate_agv.first.first);
		if (iter_find != segment_allocate_agv.end()) {
			if (iter_find->second == -1) {
				segment_allocate_agv[m_segment_wop_allocate_agv.first.first] = m_segment_wop_allocate_agv.second;
			}
		}
		else {
			segment_allocate_agv[m_segment_wop_allocate_agv.first.first] = m_segment_wop_allocate_agv.second;
		}
	}

	for (std::map<int, int>::iterator iter = segment_allocate_agv.begin(); iter != segment_allocate_agv.end(); ++iter) {

		this->SetEdgeColor(iter->first, this->GetAgvColor(iter->second), iter->second == -1 ? false : true);
	}
}

bool EMapScene::FindNodeData(const std::vector<PathSearch_NodeInfo>& node_datas, int node_id, PathSearch_NodeInfo &node_data)
{
	for (unsigned int i = 0; i < node_datas.size(); ++i) {
		PathSearch_NodeInfo m_node_data = node_datas[i];
		if (m_node_data.node_id == node_id) {
			node_data = m_node_data;
			return true;
		}
	}
	return false;
}

EMapAgv * EMapScene::FindAgvItem(int agv_id)
{
	std::map<int, EMapAgv*>::iterator iter = agv_items_.find(agv_id);
	if (iter != agv_items_.end()) {
		return iter->second;
	}
	else  {
		return NULL;
	}
}

EMapAgvId* EMapScene::FindAgvIdItem(int agv_id)
{
	std::map<int, EMapAgvId*>::iterator iter = agv_id_items_.find(agv_id);
	if (iter != agv_id_items_.end()) {
		return iter->second;
	}
	else  {
		return NULL;
	}
}

std::map<int, PathSearch_Position> EMapScene::GetRobotPos()
{
	robot_pos_mux_.lock();
	std::map<int, PathSearch_POSITION> m_robot_pos = robot_pos_;
	robot_pos_mux_.unlock();
	return m_robot_pos;
}

std::vector<std::pair<int, int> > EMapScene::GetNodeAllocateAgv()
{
	allocate_agv_mux_.lock();
	std::vector<std::pair<int, int> > node_allocate_agv = node_allocate_agv_;
	allocate_agv_mux_.unlock();
	return node_allocate_agv;
}

std::vector<std::pair<std::pair<int, int>, int> > EMapScene::GetSegmentAllocateAgv()
{
	allocate_agv_mux_.lock();
	std::vector<std::pair<std::pair<int, int>, int> > segment_allocate_agv = segment_allocate_agv_;
	allocate_agv_mux_.unlock();
	return segment_allocate_agv;
}

void EMapScene::SetEdgeColor(int edge_id, QColor & color, bool color_valid)
{
	std::map<int, EMapEdge *>::iterator itera = edge_items_.find(edge_id);
	if (itera != edge_items_.end())
	{
		itera->second->SetColor(color, color_valid);
	}
}

void EMapScene::SetNodeColor(int node_id, QColor & color, bool color_valid)
{
	std::map<int, EMapNode *>::iterator itera = node_items_.find(node_id);
	if (itera != node_items_.end())
	{
		itera->second->SetColor(color, color_valid);
	}
}

QColor EMapScene::GetAgvColor(int agv_id)
{
	if (-1 != agv_id) {
		int blue_value = (100 * agv_id) % 255;
		int green_temp = ((100 * agv_id) / 255) * 100;
		int green_value = green_temp % 255;
		int red_value = ((green_temp / 255) * 100) % 255;
		return QColor(red_value, green_value, blue_value);
	}
	else {
		return QColor(Qt::red);
	}
}

