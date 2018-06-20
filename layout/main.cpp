#include <stdio.h>
#include <iostream>
#include <QtWidgets/qapplication.h>
#include <QtCore/qsettings.h>

#include "MainWnd.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	QSettings settings("language.ini", QSettings::IniFormat);
	app.setProperty("language", settings.value("language"));

	QSharedPointer<MainWnd> mw = QSharedPointer<MainWnd>::create();

	if (!mw.isNull())
	{
		mw->show();
	}

	return app.exec();
}