#include <QtWidgets\qapplication.h>
#include "framwork_wid.h"
#include "log.h"

int main(int argc, char *argv[])
{
	//char ip_port_local[64];
	//sprintf_s(ip_port_local, sizeof(ip_port_local), "%s:%d", "0.0.0.0", 0);
	//std::shared_ptr<network_session> net_session_;
	//for (int nIndex = 0; nIndex < 100; nIndex++)
	//{
	//	try{
	//		net_session_ = std::make_shared<network_session>();
	//	}
	//	catch (...){
	//		return -1;
	//	}
	//	nsp::tcpip::endpoint ep_local;
	//	nsp::tcpip::endpoint::build(ip_port_local, ep_local);		//不指定本地的IP 和端口

	//	if (net_session_){
	//		if (net_session_->create(ep_local) < 0){
	//			loinfo("create")<<"error";
	//		}
	//		//map_sessions_.insert(std::make_pair(ep.ipv4(), net_session));
	//	}
	//}

	//return 0;
	QApplication a(argc, argv);
	framwork_wid wid;
	wid.show();
	return a.exec();
}
