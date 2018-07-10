#pragma once

#include <functional>
#include <memory>
#include <errno.h>

#include "os_util.hpp"
#include "log.h"
#include "mntypes.h"

// 缺省的， 应答时间如果超过100ms， asio 即认定为超时(clock 单位100ns)
#if !defined MN_IO_TIMEDOUT 
#define MN_IO_TIMEDOUT          ((uint64_t)1000000)
#endif

#if !defined mnlog_info
#define mnlog_info  loinfo("motion_net") << "[" << robot_id_ << "] "
#endif

#if !defined mnlog_warn
#define mnlog_warn  lowarn("motion_net") << "[" << robot_id_ << "] "
#endif

#if !defined mnlog_error
#define mnlog_error  loerror("motion_net") << "[" << robot_id_ << "] "
#endif

namespace mn {
    class asio_partnet {
		std::function<void( uint32_t, const void *) > notify_fun_ = nullptr;
        uint64_t tick_;
        uint32_t robot_id_;
	public:
        asio_partnet() = delete;

        asio_partnet(uint32_t robot_id, const std::function<void( uint32_t, const void *) > &callback) {
            robot_id_ = robot_id;
            notify_fun_ = callback;
            refresh();
        };

		asio_partnet( const asio_partnet &lref ) {
			robot_id_ = lref.robot_id_;
			tick_ = lref.tick_;
			notify_fun_ = lref.notify_fun_;
		}

		asio_partnet &operator=( const asio_partnet &lref ) {
			robot_id_ = lref.robot_id_;
			tick_ = lref.tick_;
			notify_fun_ = lref.notify_fun_;
			return *this;
		}

        ~asio_partnet() {
            ;
        }

		std::function<void(uint32_t, const void *) > &get_notify_fun() {
			return notify_fun_;
		};

        void refresh() {
            tick_ = nsp::os::clock_gettime();
        };

        posix__boolean_t is_timedout(int pktid, uint64_t current_clock) const {
            do {
                if (current_clock > tick_) {
                    if ((current_clock - tick_) >= MN_IO_TIMEDOUT) {
                        break;
                    }
                }
                return posix__false;
            } while (0);

            // these logs means packet has been marked to timedout by checking thread.
            mnlog_warn << "asio timed check fatal,id=" << pktid << " current=" << current_clock << " record=" << tick_;
            return posix__true;
        }

        void commit_asio_error(int err) const {
            asio_t asio;
			asio.err_ = err;
            if (this->notify_fun_) {
                notify_fun_(this->robot_id_, (const void *) &asio);
            }
        }

        void exec(const char* byte_stream) const {
            if (this->notify_fun_) {
                this->notify_fun_(this->robot_id_, byte_stream);
            }
        }
    };
}