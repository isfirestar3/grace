#ifndef __NETDRIVER_OMRONPLC_H__
#define __NETDRIVER_OMRONPLC_H__

#include "network_handler.h"
#include <thread>
#include "var.h"
#include "memnetdev.h"
#include "os_util.hpp"
#include <condition_variable>
#include <mutex>
#include "netdriver_base.h"

class netdriver_omron_plc :public nsp::tcpip::obudp ,public netdriver_base
{
public:
	netdriver_omron_plc();
	~netdriver_omron_plc();
public:
    virtual int net_create(var__functional_object_t *object);
    virtual int net_close();
public:
	int listen(const nsp::tcpip::endpoint &ep);
	int set_plc_object_ptr(var__memory_netdev_t *object);
	void set_ep(const nsp::tcpip::endpoint &ep);
protected:
	virtual void on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep) override final;
	int send(char* bytes_stream, uint32_t len,std::string &buffer);
private:
	void check_write_blocks();
private:
	std::recursive_mutex mutex_flag;
	bool notify_flag_ = true;
	std::condition_variable con_var_;
	std::mutex  locker_;
	std::string buffer_;

	int host_ip_ = 70;
	int plc_ip_ = 2;
	nsp::tcpip::endpoint remote_ep_;

	/*std::thread * th_read_ = nullptr;
	std::thread * th_write_ = nullptr;
	nsp::os::waitable_handle waiter_read_;
	nsp::os::waitable_handle waiter_write_;*/
	std::thread * th_read_and_write_ = nullptr;
	nsp::os::waitable_handle waiter_read_and_write_;

	int  the_interval_of_read_ =1000;
	int  the_interval_of_write_ = 1000;

	std::recursive_mutex  mutex_read_;
	std::recursive_mutex  mutex_write_;
	void th_read();
	void th_write();
	void th_read_and_write();

	int post_read_request(unsigned int addr, unsigned int len_word,std::string &data);
	int post_write_request(unsigned int addr, char* data, unsigned int len);
	var__memory_netdev_t *ptr_object_;
	var__byte_orders_t byte_order_;
};



#endif
