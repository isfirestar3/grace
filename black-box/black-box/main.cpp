#include "mainMind.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	log_select w;
	w.show();
	return a.exec();
}
