#include "libagvinfo.h"
#include "log.h"
#include "endpoint.h"
#include "os_util.hpp"

#define CONVERT_STATUS_STRING(status) ((status) == (1) ? ("online") : ("offline"))

void view_agvs() {
    struct agv_info *agvs;
    if (load_agvinfo(&agvs, LAM_Server) >= 0) {
        loinfo("agvinfo_test") << "agv info changed.";
        struct agv_info *cursor = agvs;
        while (cursor) {
            loinfo("agvinfo_test") << "vehicle:[" << cursor->vhid << "," << CONVERT_STATUS_STRING(cursor->status) << "] IP is:" << cursor->inet << " MAC is:" << cursor->hwaddr;
            cursor = cursor->next;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        loerror("agvinfo_test") << "Syntax: agvinfo_test arget-host-endpoint";
        return 1;
    }

    nsp::tcpip::endpoint ep;
    if (nsp::tcpip::endpoint::build(argv[1], ep) < 0) {
        loerror("agvinfo_test") << "Syntax: agvinfo_test target-host-endpoint";
        return 1;
    }

    if (connect_agvinfo_server(ep.ipv4(), ep.port()) < 0) {
        loerror("agvinfo_test") << "failed connect agvinfo server " << ep.to_string();
        return 1;
    }

    about_something_changed([](){
        view_agvs();
    });

    view_agvs();
    nsp::os::pshang();
    return 0;
}
