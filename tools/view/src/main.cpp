#include <QtCore/QCoreApplication>
#include <QtCore/QTextCodec>
#include "view_net.h"
#include "frm_main_wid.h"
#include "application.h"

void set_qss_style(){
	QFile qss_file(":/wid_qss/qss_setting");
	qss_file.open(QFile::ReadOnly);
	qApp->setStyleSheet(qss_file.readAll());
	qss_file.close();
}

agv_api agv_interface_single_;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("GzRobot");
	nsp::toolkit::singleton<Application>::instance()->SetArgc(argc);
	nsp::toolkit::singleton<Application>::instance()->SetArgv(argv);
	QCoreApplication::addLibraryPath("./plugins");
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));

	set_qss_style();

	//frm_main_wid ww(nullptr);
	//ww.show();

	QSharedPointer<frm_main_wid> fmw{QSharedPointer<frm_main_wid>::create()};
	fmw->show();

	return a.exec();
	return 0;
}
