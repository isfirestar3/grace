#ifndef __NETDRIVER_MODBUS_H__
#define __NETDRIVER_MODBUS_H__
#include "netdriver_base.h"
#include <string>
#include "application_network_framwork.hpp"
#include <condition_variable>
#include <memory>
#include "memnetdev.h"
#include "os_util.hpp"
#include <thread>

class netdriver_modbus :public netdriver_base, public nsp::tcpip::tcp_application_client<nsp::proto::nspnull::protocol>
{
public:
    netdriver_modbus();
    ~netdriver_modbus();

public: //dirver
    virtual int net_create(var__functional_object_t *object);
    virtual int net_close();

public://net
    virtual void on_recvdata(const std::string &pkt) override;
    virtual void on_disconnected(const HTCPLINK previous);
private:
    int send_with_reply(std::string s_buf, std::string& recv_buf);
    void check_write_blocks();
    void th_read();
    void th_write();
private:
    int read_register(int addr, int word_num_want, char* register_data, int& word_num_get);
    int write_register(int addr, char* register_data, int word_num);   
private:
    std::string host_ip_;
    int host_port_;

    std::recursive_mutex mutex_flag;
    bool notify_flag_ = true;
    std::condition_variable con_var_;
    std::mutex  locker_;
    std::string buffer_;

    std::thread * th_read_ = nullptr;
    std::thread * th_write_ = nullptr;
    nsp::os::waitable_handle waiter_read_;
    nsp::os::waitable_handle waiter_write_;

    int  the_interval_of_read_ = 1000;
    int  the_interval_of_write_ = 1000;

    std::recursive_mutex  mutex_read_;
    std::recursive_mutex  mutex_write_;

    var__memory_netdev_t *ptr_object_;
    var__byte_orders_t byte_order_;
};

#endif


