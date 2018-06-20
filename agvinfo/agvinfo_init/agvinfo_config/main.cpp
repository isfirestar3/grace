#include "agvinfo_config.h"
#include <QtWidgets/QApplication>

agvinfo_config *agv_ptr;

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);
	agvinfo_config w;
	agv_ptr = &w;
	w.show();
	return a.exec();
}
