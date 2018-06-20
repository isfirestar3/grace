
#include "canio.h"
#include <stdio.h>
#include "log.h"
#include "os_util.hpp"
#include "toolkit.h"
#include "canbus_kvaser.h"
canbus_kvaser::canbus_kvaser(int can_id, int can_type, int baud)
: canbus_base(can_id, can_type, baud) {

}

canbus_kvaser::~canbus_kvaser() {
}

int canbus_kvaser::open_can_interface() {
    if (canbus_base::get_can_type() == kCanDrvType_Kvaser) {
        if (canbus_base::interface_is_opened_) {
            return 0;
        }
        memset(can_name, 0, 20);
        //nsp::toolkit::posix_vsnprintf<char>(can_name, 19, "KVASER%d", canbus_base::get_can_id());
        sprintf(can_name, "KVASER%d", canbus_base::get_can_id());
        kvaser_can_ = new KvaserCAN(can_name);

        kvaser_can_->SetBaud(canbus_base::get_can_baud());

        const Error* err = kvaser_can_->Open();
        if (err) {
            nsperror << "kvaser_can_ Open " << canbus_base::get_can_id() << " Fail!" << canbus_base::get_can_id() << " Error: " << err->toString();
            return err->GetID();
        }

        canbus_base::interface_is_opened_ = true;
        canbus_base::can_interface_ = kvaser_can_;
    } else {
        return -2;
    }

    return 0;
}


