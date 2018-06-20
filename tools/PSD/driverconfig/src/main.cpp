#include "driverconfig.h"
#include <QtWidgets/QApplication>
#include <QSharedPointer>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QSharedPointer<driverconfig> dc = QSharedPointer<driverconfig>::create();

	if (!dc.isNull())
	{
		dc->show();
	}

	return a.exec();
}
