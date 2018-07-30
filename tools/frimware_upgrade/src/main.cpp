#include <QtWidgets\qapplication.h>
#include "framwork_wid.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	framwork_wid wid;
	wid.show();
	return a.exec();
}
