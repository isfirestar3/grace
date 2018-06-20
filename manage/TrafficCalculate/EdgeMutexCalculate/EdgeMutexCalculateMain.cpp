#include <iostream>
#include <qapplication.h>
#include <Log4cppSuro.hpp>
#include "EdgeMutexCalculate.h"
#include "NormalSleep.hpp"
#include "ConfigFile.hpp"

int main(int argc,char** argv) 
{
	LOGS_PRIORITY_DEBUG("EdgeMutex");
	LOG.enableFileLog("EdgeMutex");

	LOGS_PRIORITY_DEBUG("EdgeMutexDebug");
	LOG.enableFileLog("EdgeMutexDebug");

	ConfigFile conf_file("EdgeMutexCalculate.ini");

	double agv_width = atof(conf_file.GetValue("agv_width", "AgvSize", "0.7").c_str());
	double agv_head = atof(conf_file.GetValue("agv_head", "AgvSize", "0.45").c_str());
	double agv_tail = atof(conf_file.GetValue("agv_tail", "AgvSize", "0.45").c_str());

	QApplication a(argc,argv);

	Q_INIT_RESOURCE(AgvEMap);
	EdgeMutexCalculate2 edge_mutex(agv_width, agv_head, agv_tail);
	edge_mutex.showMaximized();
	//SLEEP(1000);

	std::cout << "请输入数字，1：重新计算基本互斥边，2：直接载入基本互斥边" << std::endl;
	int m_out = 0;
	std::cin >> m_out;

	if (m_out == 1) {
		edge_mutex.LoadMapData("untitled1.agv");
	}
	else if (m_out == 2) {
		edge_mutex.LoadMapData("untitled2.agv", false);
	}
	

	return a.exec();
}
