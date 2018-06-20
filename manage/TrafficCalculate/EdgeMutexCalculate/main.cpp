#include "edgemutexcalculate.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//Q_INIT_RESOURCE(AgvEMap);

	double agv_width = 0.88;
	double agv_head = 0.55;
	double agv_tail = 1.07;

	EdgeMutexCalculate w(agv_width, agv_head, agv_tail);
	w.LoadMapData("layout.xml");
	w.showMaximized();

	return a.exec();
}
