#include "edgemutexcalculate.h"

#include "EMapScene.h"
#include "EMapView.h"
#include "EMapAgv.h"
#include "EMapAgvId.h"

#include "CoordinateSystem.hpp"

#include "math.h"
#include "fstream"
#include "iostream"


EdgeMutexCalculate::EdgeMutexCalculate(double agv_width, double agv_head, double agv_tail, QWidget *parent)
	: QMainWindow(parent)
{
	agv_item_ = new EMapAgv(1);
	agv_item_->SetAgvSize(agv_width, agv_head, agv_tail);
	agv_id_item_ = new EMapAgvId(1);

	other_agv_item_ = new EMapAgv(2);
	other_agv_item_->SetAgvSize(agv_width, agv_head, agv_tail);
	other_agv_id_item_ = new EMapAgvId(2);

	emap_scene_ = new EMapScene;
	emap_scene_->setSceneRect(-400000, -400000, 800000, 800000);
	emap_scene_->addItem(agv_item_);
	emap_scene_->addItem(agv_id_item_);
	emap_scene_->addItem(other_agv_item_);
	emap_scene_->addItem(other_agv_id_item_);

	agv_item_->setPos(0, 0);
	agv_item_->setRotation(0);

	other_agv_item_->setPos(0, 0);
	other_agv_item_->setRotation(0);

	emap_view_ = new EMapView(this);
	emap_view_->setScene(emap_scene_);
	emap_view_->SetEMapRect(emap_scene_->sceneRect());
	emap_view_->FitInView();
	emap_view_->scale(600, 600);
	emap_view_->centerOn(QPointF(1300, 1300));

	InitForm();
}

EdgeMutexCalculate::~EdgeMutexCalculate()
{

}

void EdgeMutexCalculate::LoadMapData(const std::string & file_name)
{
	int r = PathSearch_OpenMap(file_name.c_str(), &layout_data_);
	
	emap_scene_->LoadNodeEdgeData(layout_data_.nodeList, layout_data_.edgeList);

	this->LoadWopData();

	th_ = new std::thread(std::bind(&EdgeMutexCalculate::CalculateEdgeMutex, this));
}

void EdgeMutexCalculate::CalculateEdgeMutex()
{
	for (unsigned int i = 0; i < layout_wops_.size(); ++i) {

		PathSearch_Wop m_wop = layout_wops_[i];
		EdgeWopMutex m_edge_wop_mutex;
		m_edge_wop_mutex.edge_wop_id_ = std::pair<int, int>(m_wop.edge_id, m_wop.wop_id);

		for (unsigned int j = 0; j < layout_wops_.size(); ++j) {
			if (i == j) {
				continue;
			}
			PathSearch_Wop m_other_wop = layout_wops_[j];

			if (true == this->CheckEdgeMutex(m_wop, m_other_wop)) {
				m_edge_wop_mutex.mutex_edge_wops_.push_back(std::pair<int, int>(m_other_wop.edge_id, m_other_wop.wop_id));
			}

			std::cout << i << "%" << layout_wops_.size() << " " << j << "%" << layout_wops_.size() << std::endl;
		}
		base_mutex_.push_back(m_edge_wop_mutex);

	}

	std::ofstream ofstr;
	ofstr.open("AutoTrafficMutex.ini");
	for (unsigned int i = 0; i < base_mutex_.size(); ++i) {
		EdgeWopMutex m_edge_wop_mutex = base_mutex_[i];
		std::pair<int, int> m_wop = m_edge_wop_mutex.edge_wop_id_;
		for (unsigned int j = 0; j < m_edge_wop_mutex.mutex_edge_wops_.size(); ++j) {
			std::pair<int, int > m_other_wop = m_edge_wop_mutex.mutex_edge_wops_[j];
			ofstr << m_wop.first << " " << m_wop.second << " " << m_other_wop.first << " " << m_other_wop.second << std::endl;
		}
	}
	ofstr.close();

	std::cout << "Calculate Finish..." << std::endl;
}

void EdgeMutexCalculate::InitForm()
{
	QTabWidget *centre_basic_tab = new QTabWidget();
	this->setCentralWidget(centre_basic_tab);
	centre_basic_tab->addTab(emap_view_, tr("TrafficManage"));
	centre_basic_tab->setCurrentWidget(emap_view_);

	this->setDockOptions(AnimatedDocks | AllowNestedDocks | AllowTabbedDocks | ForceTabbedDocks | VerticalTabs);
}

void EdgeMutexCalculate::LoadWopData()
{
	for (unsigned int i = 0; i < layout_data_.nodeList.size(); ++i) {
		PathSearch_NodeInfo m_node = layout_data_.nodeList[i];
		layout_nodes_[m_node.node_id] = m_node;
	}

	std::map<unsigned int/*wop_id*/, PathSearch_Wop> wop_types;
	for (unsigned int i = 0; i < layout_data_.wopList.size(); ++i) {
		PathSearch_Wop m_wop = layout_data_.wopList[i];
		wop_types[m_wop.wop_id] = m_wop;
	}

	for (unsigned int i = 0; i < layout_data_.edgeList.size(); ++i) {
		PathSearch_EdgeInfo m_edge = layout_data_.edgeList[i];
		layout_edges_[m_edge.id] = m_edge;
		for (unsigned int j = 0; j < m_edge.wop_list.size(); ++j) {
			int m_wop_id = m_edge.wop_list[j];

			PathSearch_Wop wop;
			wop.wop_id = m_wop_id;
			wop.edge_id = m_edge.id;
			wop.angle_type = wop_types[m_wop_id].angle_type;
			wop.angle = wop_types[m_wop_id].angle;
			wop.direction = wop_types[m_wop_id].direction;
			wop.distance = m_edge.length;

			layout_wops_.push_back(wop);
		}
	}
}

bool EdgeMutexCalculate::CheckEdgeMutex(PathSearch_Wop m_wop, PathSearch_Wop m_other_wop)
{
	PathSearch_EdgeInfo m_edge, m_other_edge;
	if (false == this->FindEdgeInfo(m_wop.edge_id, m_edge) || false == this->FindEdgeInfo(m_other_wop.edge_id, m_other_edge)) {
		return false;
	}

	int m_edge_start_id, m_edge_end_id;
	if (m_wop.direction == PathSearch_FORWARD) {
		m_edge_start_id = m_edge.start_node_id;
		m_edge_end_id = m_edge.end_node_id;
	}
	else if (m_wop.direction == PathSearch_BACKWARD) {
		m_edge_start_id = m_edge.end_node_id;
		m_edge_end_id = m_edge.start_node_id;
	}
	else {
		return false;
	}

	int m_other_edge_start_id, m_other_edge_end_id;
	if (m_other_wop.direction == PathSearch_FORWARD) {
		m_other_edge_start_id = m_other_edge.start_node_id;
		m_other_edge_end_id = m_other_edge.end_node_id;
	}
	else if (m_other_wop.direction == PathSearch_BACKWARD) {
		m_other_edge_start_id = m_other_edge.end_node_id;
		m_other_edge_end_id = m_other_edge.start_node_id;
	}
	else {
		return false;
	}

	if (m_wop.wop_id == m_other_wop.wop_id && m_wop.edge_id == m_other_wop.edge_id) {
		return false;
	}

	// 注释后，仅仅使用边互斥
	//if (m_edge_end_id == m_other_edge_start_id && m_edge_start_id != m_other_edge_end_id) {
	//	return false;
	//}

	//if (m_other_edge_end_id == m_edge_start_id && m_other_edge_start_id != m_edge_end_id) {
	//	return false;
	//}

	//if (m_edge_start_id == m_other_edge_start_id && m_edge_end_id != m_other_edge_end_id) {
	//	return false;
	//}

	//if (m_edge_start_id != m_other_edge_start_id && m_edge_end_id == m_other_edge_end_id) {
	//	return false;
	//}

	PathSearch_NodeInfo m_edge_start_node, m_edge_end_node;
	if (false == this->FindNodeInfo(m_edge_start_id, m_edge_start_node) || false == this->FindNodeInfo(m_edge_end_id, m_edge_end_node)) {
		return false;
	}

	PathSearch_NodeInfo m_other_edge_start_node, m_other_edge_end_node;
	if (false == this->FindNodeInfo(m_other_edge_start_id, m_other_edge_start_node) || false == this->FindNodeInfo(m_other_edge_end_id, m_other_edge_end_node)) {
		return false;
	}

	if (this->CalculateDist(m_edge_start_node, m_other_edge_start_node) / 100.0 > 5 &&
		this->CalculateDist(m_edge_start_node, m_other_edge_end_node) / 100.0 > 5 &&
		this->CalculateDist(m_edge_end_node, m_other_edge_start_node) / 100.0 > 5 &&
		this->CalculateDist(m_edge_end_node, m_other_edge_end_node) / 100.0 > 5) {
		return false;
	}

	std::vector<PathSearch_POSPOINT> m_path = m_edge.disperse_point;
	std::vector<PathSearch_POSPOINT> m_other_path = m_other_edge.disperse_point;

	for (unsigned int i = 0; i < m_path.size(); i+=10) {
		for (unsigned int j = 0; j < m_other_path.size(); j+=10) {
			PathSearch_POSPOINT m_pos = m_path[i];
			PathSearch_Position agv_pos_1;
			agv_pos_1.x = m_pos.x / 100.0;
			agv_pos_1.y = m_pos.y / 100.0;

			double m_angle;
			if (i < m_path.size() / 2) {
				PathSearch_POSPOINT m_pos2 = m_path[i + 1];
				m_angle = this->CalculateDirection(m_pos, m_pos2);
			}
			else {
				PathSearch_POSPOINT m_pos2 = m_path[i - 1];
				m_angle = this->CalculateDirection(m_pos2, m_pos);
			}

			if (m_wop.angle_type == PathSearch_CONST_ANGLE) {
				m_angle += m_wop.angle;
			}
			else if (m_wop.angle_type == PathSearch_GLOBAL_CONST_ANGLE) {
				m_angle = m_wop.angle;
			}
			agv_pos_1.angle = m_angle;
			
			PathSearch_POSPOINT m_other_pos = m_other_path[j];
			PathSearch_Position agv_pos_2;
			agv_pos_2.x = m_other_pos.x / 100.0;
			agv_pos_2.y = m_other_pos.y / 100.0;

			double m_other_angle;
			if (j < m_other_path.size() / 2) {
				PathSearch_POSPOINT m_other_pos2 = m_other_path[j + 1];
				m_other_angle = this->CalculateDirection(m_other_pos, m_other_pos2);
			}
			else {
				PathSearch_POSPOINT m_other_pos2 = m_other_path[j - 1];
				m_other_angle = this->CalculateDirection(m_other_pos2, m_other_pos);
			}

			if (m_other_wop.angle_type == PathSearch_CONST_ANGLE) {
				m_other_angle += m_other_wop.angle;
			}
			else if (m_other_wop.angle_type == PathSearch_GLOBAL_CONST_ANGLE) {
				m_other_angle = m_other_wop.angle;
			}
			agv_pos_2.angle = m_other_angle;

			if (true == CheckAgvCollocation(agv_pos_1, agv_pos_2)) {
				std::cout << m_edge_start_id << " " << m_edge_end_id << " " << m_other_edge_start_id << " " << m_other_edge_end_id << std::endl;
				return true;
			}
		}
	}

	return false;
}

bool EdgeMutexCalculate::CheckAgvCollocation(PathSearch_Position agv_pos_1, PathSearch_Position agv_pos_2)
{
	if (agv_pos_1.x > -0.89739 && agv_pos_1.x < 47.1611
		&& agv_pos_1.y > -0.193664 && agv_pos_1.y < 20.5278) {
		agv_item_->SetAgvSize(0.88, 0.55, 1.07);
	}
	else {
		agv_item_->SetAgvSize(1.3, 1.575, 1.575);
	}
	agv_item_->setPos(CoordinateSystem::Map2Scene(agv_pos_1.x), CoordinateSystem::Map2Scene(agv_pos_1.y));
	agv_item_->setRotation(agv_pos_1.angle);
	agv_item_->update();

	agv_id_item_->setPos(CoordinateSystem::Map2Scene(agv_pos_1.x), CoordinateSystem::Map2Scene(agv_pos_1.y));
	agv_id_item_->update();

	
	if (agv_pos_2.x > -0.89739 && agv_pos_2.x < 47.1611
		&& agv_pos_2.y > -0.193664 && agv_pos_2.y < 20.5278) {
		other_agv_item_->SetAgvSize(0.88, 0.55, 1.07);
	}
	else {
		other_agv_item_->SetAgvSize(1.3, 1.575, 1.575);
	}
	other_agv_item_->setPos(CoordinateSystem::Map2Scene(agv_pos_2.x), CoordinateSystem::Map2Scene(agv_pos_2.y));
	other_agv_item_->setRotation(agv_pos_2.angle);
	other_agv_item_->update();

	other_agv_id_item_->setPos(CoordinateSystem::Map2Scene(agv_pos_2.x), CoordinateSystem::Map2Scene(agv_pos_2.y));
	other_agv_id_item_->update();

	if (agv_item_->collidesWithItem(other_agv_item_, Qt::IntersectsItemBoundingRect)) {
		return true;
	}
	else {
		return false;
	}
}

bool EdgeMutexCalculate::FindNodeInfo(int node_id, PathSearch_NodeInfo &node_info)
{
	std::map<int, PathSearch_NodeInfo>::iterator iter = layout_nodes_.find(node_id);
	if (iter != layout_nodes_.end()) {
		node_info = iter->second;
		return true;
	}
	return false;
}

bool EdgeMutexCalculate::FindEdgeInfo(int edge_id, PathSearch_EdgeInfo &edge_info)
{
	std::map<int, PathSearch_EdgeInfo>::iterator iter = layout_edges_.find(edge_id);
	if (iter != layout_edges_.end()) {
		edge_info = iter->second;
		return true;
	}
	return false;
}

double EdgeMutexCalculate::CalculateDist(PathSearch_NodeInfo agv_pos_1, PathSearch_NodeInfo agv_pos_2)
{
	return sqrt((agv_pos_1.pos_x - agv_pos_2.pos_x) * (agv_pos_1.pos_x - agv_pos_2.pos_x) + (agv_pos_1.pos_y - agv_pos_2.pos_y) * (agv_pos_1.pos_y - agv_pos_2.pos_y));
}

double EdgeMutexCalculate::CalculateDirection(PathSearch_POSPOINT agv_pos_1, PathSearch_POSPOINT agv_pos_2)
{
	return atan2(agv_pos_2.y - agv_pos_1.y, agv_pos_2.x - agv_pos_1.x) * 180 / 3.1415926;
}

