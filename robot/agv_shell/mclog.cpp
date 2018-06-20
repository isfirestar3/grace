#include "compiler.h"
#include "posix_string.h"
#include "logger.h"
#include "nisdef.h"
#include "nis.h"


#if !defined DEFAULT_MCLOG_PORT
#define DEFAULT_MCLOG_PORT  (0x5001)
#endif // !DEFAULT_MCLOG_PORT

#if !defined DEFAULT_SYSLOG_PORT
#define DEFAULT_SYSLOG_PORT  (514)
#endif // !DEFAULT_SYSLOG_PORT

HUDPLINK __mclog_link = INVALID_HUDPLINK;
HUDPLINK __syslog_link = INVALID_HUDPLINK;

int mclog__recvdata(HUDPLINK link, const char *data, int cb) {
    char *logstr = (char*) malloc(cb + 1);
    if (!logstr) {
        return -ENOMEM;
    }

    memcpy(logstr, data, cb);
    logstr[cb] = 0;
    log__save("mcmesg", kLogLevel_Info, kLogTarget_Filesystem, "\n%s", logstr);
    free(logstr);
    return 0;
}

void STD_CALL mclog__callback(const nis_event_t *naio_event, const void *param2) {
    udp_data_t *udp_data = (udp_data_t *) param2;
    switch (naio_event->Event) {
        case EVT_RECEIVEDATA:
            mclog__recvdata(naio_event->Ln.Udp.Link, udp_data->e.Packet.Data, udp_data->e.Packet.Size);
            break;
        case EVT_CLOSED:
            if (INVALID_HUDPLINK != __mclog_link) {
                __mclog_link = INVALID_HUDPLINK;
            }
            break;
        default:
            break;
    }
}

void mclog__uninit() {
    if (INVALID_HUDPLINK != __mclog_link) {
        udp_destroy(__mclog_link);
        __mclog_link = INVALID_HUDPLINK;
    }
}

int mclog__init(const char *ip, uint16_t port) {
    uint16_t mclog_port = ((0 == port) ? (DEFAULT_MCLOG_PORT) : (port));
    char mclog_ipstr[16];

    mclog__uninit();

    if (!ip) {
        posix__strcpy(mclog_ipstr, cchof(mclog_ipstr), "0.0.0.0");
    } else {
        if (0 == strlen(ip)) {
            posix__strcpy(mclog_ipstr, cchof(mclog_ipstr), "0.0.0.0");
        } else {
            posix__strcpy(mclog_ipstr, cchof(mclog_ipstr), ip);
        }
    }

    __mclog_link = udp_create(&mclog__callback, mclog_ipstr, mclog_port, UDP_FLAG_UNITCAST);
    if (INVALID_HUDPLINK == __mclog_link) {
        return -1;
    }
    return 0;
}

int syslog__recvdata(HUDPLINK link, const char *data, int cb) {
    char *logstr = (char*) malloc(cb + 1);
    if (!logstr) {
        return -ENOMEM;
    }

    memcpy(logstr, data, cb);
    logstr[cb] = 0;
    log__save("vcusysmesg", kLogLevel_Info, kLogTarget_Filesystem, "\n%s", logstr);
    free(logstr);
    return 0;
}

void STD_CALL syslog__callback(const nis_event_t *naio_event, const void *param2) {
    udp_data_t *udp_data = (udp_data_t *) param2;
    switch (naio_event->Event) {
        case EVT_RECEIVEDATA:
            syslog__recvdata(naio_event->Ln.Udp.Link, udp_data->e.Packet.Data, udp_data->e.Packet.Size);
            break;
        case EVT_CLOSED:
            if (INVALID_HUDPLINK != __syslog_link) {
                __syslog_link = INVALID_HUDPLINK;
            }
            break;
        default:
            break;
    }
}

void syslog__uninit() {
    if (INVALID_HUDPLINK != __syslog_link) {
        udp_destroy(__syslog_link);
        __syslog_link = INVALID_HUDPLINK;
    }
}

int syslog__init(const char *ip, uint16_t port) {
    uint16_t syslog_port = ((0 == port) ? (DEFAULT_SYSLOG_PORT) : (port));
    char syslog_ipstr[16];

    syslog__uninit();

    if (!ip) {
        posix__strcpy(syslog_ipstr, cchof(syslog_ipstr), "0.0.0.0");
    } else {
        if (0 == strlen(ip)) {
            posix__strcpy(syslog_ipstr, cchof(syslog_ipstr), "0.0.0.0");
        } else {
            posix__strcpy(syslog_ipstr, cchof(syslog_ipstr), ip);
        }
    }

    __syslog_link = udp_create(&syslog__callback, syslog_ipstr, syslog_port, UDP_FLAG_UNITCAST);
    if (INVALID_HUDPLINK == __syslog_link) {
        return -1;
    }
    return 0;
}
