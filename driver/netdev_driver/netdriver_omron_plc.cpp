#include "netdriver_omron_plc.h"
#include "memnetdev.h"
#include "var.h"
#include "log.h"
#include "toolkit.h"

#define PACKAGE_HEAD_LEN 18
#define PACKAGEACK_HEAD_LEN 14 

netdriver_omron_plc::netdriver_omron_plc() : /*waiter_read_(0), waiter_write_(0)*/waiter_read_and_write_(0){

}

netdriver_omron_plc::~netdriver_omron_plc() {
   
	waiter_read_and_write_.sig();
	if (th_read_and_write_) {
		if (th_read_and_write_->joinable()) {
			th_read_and_write_->join();
			delete th_read_and_write_;
			th_read_and_write_ = nullptr;
		}
	}
	waiter_read_and_write_.reset();
	
}

int netdriver_omron_plc::listen(const nsp::tcpip::endpoint &ep) {
    if (nsp::tcpip::obudp::create(ep, UDP_FLAG_BROADCAST) < 0) {
        return -1;
    }
    return 0;
}


void netdriver_omron_plc::on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep) {
    std::unique_lock<decltype(locker_) > guard(locker_);
    buffer_ = data;
    int len = buffer_.length();
    notify_flag_ = true;
    con_var_.notify_one();
}

void netdriver_omron_plc::th_read_and_write() {
	while (waiter_read_and_write_.wait(the_interval_of_write_) > 0) {
		th_read();
		th_write();
	}
}
void netdriver_omron_plc::th_read() {
	
        posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
        var__memory_data_block *p_plc_dup = new var__memory_data_block[sizeof (ptr_object_->readonly_blocks)];
#if _WIN32  
        memcpy_s(p_plc_dup, sizeof (ptr_object_->readonly_blocks), ptr_object_->readonly_blocks, sizeof (ptr_object_->readonly_blocks));
#else
        memcpy(p_plc_dup, ptr_object_->readonly_blocks, sizeof (ptr_object_->readonly_blocks));
#endif
        posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
        
        for (size_t i = 0; i < MEM_MAXIMUM_BLOCK; i++) {
            if (!p_plc_dup[i].actived_) continue;

            if (p_plc_dup[i].flush_) {
                std::string recv_data;
                if (post_read_request(p_plc_dup[i].start_address_, p_plc_dup[i].effective_count_in_word_, recv_data) < 0) {
                    continue;
                }
                
				if (recv_data.size() <= 0 || recv_data.size() > 2 * MEM_BLOCK_REGION_DATA_SIZE) {
				loinfo("omron") << "post_read_request, recv_data len > MEM_BLOCK_REGION_DATA_SIZE or recv len is 0";
                    continue;
                }
                
                posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
#if _WIN32		
				memcpy_s(ptr_object_->readonly_blocks[i].data_, recv_data.size(), recv_data.c_str(), recv_data.size());

#else
                memcpy(ptr_object_->readonly_blocks[i].data_, recv_data.c_str(), recv_data.length());
#endif
                posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
            }

            //check 是否周期性发送
            posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
            if (p_plc_dup[i].period_) {
                ptr_object_->readonly_blocks[i].flush_ = true;//////////
            } else {
                ptr_object_->readonly_blocks[i].flush_ = false;////////
            }
            posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
        }
        delete[] p_plc_dup;
//    }
}

void netdriver_omron_plc::th_write() {
 //   while (waiter_write_.wait(the_interval_of_write_) > 0) {
        
        var__memory_data_block *p_plc_dup = new var__memory_data_block[sizeof (ptr_object_->writeable_blocks_)];
        
        //指定对象拷贝
        posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
#if _WIN32	
        memcpy_s(p_plc_dup, sizeof (ptr_object_->writeable_blocks_), ptr_object_->writeable_blocks_, sizeof (ptr_object_->writeable_blocks_));
#else
        memcpy(p_plc_dup, ptr_object_->writeable_blocks_, sizeof (ptr_object_->writeable_blocks_));
#endif
        posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
        
        //根据制定参数，对有效blocks操作
        for (size_t i = 0; i < MEM_MAXIMUM_BLOCK; i++) {
            if (!p_plc_dup[i].actived_) continue;

            if (p_plc_dup[i].flush_) {
				
                if (post_write_request(p_plc_dup[i].start_address_, (char*) p_plc_dup[i].data_, p_plc_dup[i].effective_count_in_word_) < 0) {
                    continue;
                }
            }

            //chenck是否周期发送
            posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
            if (p_plc_dup[i].period_) {
                ptr_object_->writeable_blocks_[i].flush_ = true;
            } else {
                ptr_object_->writeable_blocks_[i].flush_ = false;
            }
            posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
        }
        delete[] p_plc_dup;
//    }
}

int netdriver_omron_plc::set_plc_object_ptr(var__memory_netdev_t *object) {
    if (!ptr_object_) {
        return -1;
    }

    ptr_object_ = object;

    byte_order_ = object->netdev_head_.byte_order_;

    std::string str_remote_ip = object->netdev_head_.remote_.ipv4_;
    std::string str_local_ip = object->netdev_head_.local_.ipv4_;
    int remote_pos = str_remote_ip.find_last_of(".");
    int local_pos = str_local_ip.find_last_of(".");
    plc_ip_ = atoi(str_remote_ip.substr(remote_pos + 1, str_remote_ip.length() - remote_pos).c_str());
    host_ip_ = atoi(str_local_ip.substr(local_pos + 1, str_local_ip.length() - local_pos).c_str());

    remote_ep_.ipv4(object->netdev_head_.remote_.ipv4_);
    remote_ep_.port(object->netdev_head_.remote_.port_);

    the_interval_of_read_ = object->interval_for_read_thread_;
    the_interval_of_write_ = object->interval_for_write_thread_;


    //对所有的有效write blocks 初始化
    check_write_blocks();

	
	if (!th_read_and_write_) {
		th_read_and_write_ = new std::thread(std::bind(&netdriver_omron_plc::th_read_and_write, this));
	}
    return 0;
}

int netdriver_omron_plc::post_read_request(unsigned int addr, unsigned int word_num, std::string &recv_data) {
    unsigned int package_len = PACKAGE_HEAD_LEN;
    char* pkt_data = new char[package_len];
    pkt_data[0] = (char) 0x80; //ICF
    pkt_data[1] = 0x00; //RSV
    pkt_data[2] = 0x02; //GCT
    pkt_data[3] = 0x00; //PLC网络号
    pkt_data[4] = (unsigned char) (plc_ip_ & 0x000000FF); //PLC节点号
    pkt_data[5] = 0x00; //PLC单元号
    pkt_data[6] = 0x00; //上位机网络号
    pkt_data[7] = (unsigned char) (host_ip_ & 0x000000FF); //上位机节点号
    pkt_data[8] = 0x00; //上位机节点号
    pkt_data[9] = (char) 0xed; //SID
    pkt_data[10] = 0x01; //command code
    pkt_data[11] = 0x01; //command code
    pkt_data[12] = (char) 0x82; //memory area code
    pkt_data[13] = (unsigned char) ((addr & 0x0000FF00) >> 8); //beginning addr, High 8 bits.
    pkt_data[14] = (unsigned char) (addr & 0x000000FF); //beginning addr, Low 8 bits.
    pkt_data[15] = 0x00; //beginning addr, always 0x00 for word reading.
    pkt_data[16] = (unsigned char) ((word_num & 0x0000FF00) >> 8); //number of items, High 8 bits.
    pkt_data[17] = (unsigned char) (word_num & 0x000000FF); //number of items, Low 8 bits.

    //发送请求,buffer回包内容
    std::string str_buffer;

    {
        std::lock_guard<decltype(mutex_flag) > guard(mutex_flag);
        if (notify_flag_) {
            if (send(pkt_data, package_len, str_buffer) < 0) {
                loinfo("omron") << "post_read_request fail to send package";
                delete[] pkt_data;
                return -1;
            }
        }
		//loinfo << "post_read:" << pkt_data[13] << " " << pkt_data[14] << " " << pkt_data[16] << " " << pkt_data[17] << " "
		//	<< pkt_data[18] << " " << pkt_data[19] << " ";// << pkt_data[20] << " " << pkt_data[21] << " ";
		//loinfo << "post_read_feedback:" << str_buffer << " ";

        delete[] pkt_data; 
    }

    //校验包的长度
    int len_buffer = str_buffer.size();
    if (len_buffer != 2 * word_num + PACKAGEACK_HEAD_LEN) {
        loinfo("omron") << "post_read_request recv len is wrong:" << len_buffer << " target len is " << 2 * word_num + PACKAGEACK_HEAD_LEN;
        return -1;
    }


    //校验数据正确性
    char * data = new char[len_buffer - PACKAGEACK_HEAD_LEN];
    int len_data = 0;
    const char* package_bytes = str_buffer.c_str();
    if (package_bytes[10] == 0x01 &&
            package_bytes[11] == 0x01 &&
            package_bytes[12] == 0x00 &&
            package_bytes[13] == 0x00) {
        for (int i = PACKAGEACK_HEAD_LEN; i < len_buffer; i++) {
            data[len_data] = str_buffer[i];
            len_data++;
        }
		
		if (kNetworkByteOrder_BigEndian == byte_order_) {
            short *target_data = new short[len_data / 2];
            char *prev = data; /////
            for (int i = 0; i < len_data / 2; i++) {
                short tans_data = *(short*) prev;
                tans_data = nsp::toolkit::change_byte_order(tans_data);
                target_data[i] = tans_data;
                prev += sizeof (short);
            }

            recv_data = std::string((char*) target_data, len_data);
            delete[] target_data;
        } else if (kNetworkByteOrder_LittleEndian == byte_order_) {
            recv_data = std::string(data, len_data);
        }
        delete[] data;
    } else {
        delete[] data;
        loinfo("omron") << "post_read_request byte is wrong " << "buf[10]:" << package_bytes[10] << ", buf[11] : " << package_bytes[11] << ", buf[12] : " << package_bytes[12] << ", buf[13] : " << package_bytes[13];
        return -1;
    }

    return 0;
}

int netdriver_omron_plc::post_write_request(unsigned int addr, char* data, unsigned int len) {
    //package_len 包头固定大小,len单位为字,需要转化为字节。
    unsigned int package_len = PACKAGE_HEAD_LEN + 2 * len;
    unsigned char* pkt_data = new unsigned char[package_len];
    pkt_data[0] = (unsigned char) 0x80; //ICF
    pkt_data[1] = 0x00; //RSV
    pkt_data[2] = 0x02; //GCT
    pkt_data[3] = 0x00; //PLC网络号
    pkt_data[4] = (unsigned char) ((unsigned char) plc_ip_ & 0x000000FF); //PLC节点号
    pkt_data[5] = 0x00; //PLC单元号
    pkt_data[6] = 0x00; //上位机网络号
    pkt_data[7] = (unsigned char) ((unsigned char) host_ip_ & 0x000000FF); //上位机节点号
    pkt_data[8] = 0x00; //上位机节点号
    pkt_data[9] = (char) 0xed; //SID
    pkt_data[10] = 0x01; //command code
    pkt_data[11] = 0x02; //command code
    pkt_data[12] = (char) 0x82; //memory area code
    pkt_data[13] = (unsigned char) ((unsigned char) ((addr & 0x0000FF00) >> 8)); //beginning addr, High 8 bits.
    pkt_data[14] = (unsigned char) (addr & 0x000000FF); //beginning addr, Low 8 bits.
    pkt_data[15] = 0x00; //beginning addr, always 0x00 for word writting.
    pkt_data[16] = (unsigned char) ((len & 0x0000FF00) >> 8); //number of items, Low 8 bits.
    pkt_data[17] = (unsigned char) (len & 0x000000FF); //number of items, Low 8 bits.

    if (kNetworkByteOrder_BigEndian == byte_order_) {
        for (unsigned int i = 0; i < len * 2; i += 2) {
            pkt_data[18 + i] = data[i + 1];
            pkt_data[19 + i] = data[i];
        }
    } else if (kNetworkByteOrder_LittleEndian == byte_order_) {
        for (unsigned int i = 0; i < len * 2; i += 2) {
            pkt_data[18 + i] = data[i];
            pkt_data[19 + i] = data[i + 1];
        }
    }


    std::string str_buffer;

    {
        std::lock_guard<decltype(mutex_flag) > guard(mutex_flag);
        if (notify_flag_) {
            if (send((char *) pkt_data, package_len, str_buffer) < 0) {
                delete[] pkt_data;
                return -1;
            }
        }
    }
	//loinfo << "post_write_:" << pkt_data[13] << " " << pkt_data[14] << " " << pkt_data[16] << " " << pkt_data[17] << " "
	//	<< pkt_data[18] << " " << pkt_data[19] << " ";// << pkt_data[20] << " " << pkt_data[21] << " ";
	//loinfo << "post_write_feedback:" << str_buffer << " ";

	delete[] pkt_data;

    //校验包的长度
    int len_buffer = str_buffer.size();
    const char* package_bytes = str_buffer.c_str();
    if (len_buffer != PACKAGEACK_HEAD_LEN) {
        loinfo("omron") << "post_write_request recv len is wrong:" << len_buffer << " len must be 14";
        return -1;
    } else if (package_bytes[10] == 0x01 &&
            package_bytes[11] == 0x02 &&
            package_bytes[12] == 0x00 &&
            package_bytes[13] == 0x00) {
        return 0;
    } else {
        loinfo("omron") << "post_write_request,byte is wrong " << "buf[10]:" << package_bytes[10] << ", buf[11] : " << package_bytes[11] << ", buf[12] : " << package_bytes[12] << ", buf[13] : " << package_bytes[13];
        return -1;
    }

    return 0;
}

int netdriver_omron_plc::send(char* bytes_stream, uint32_t len, std::string &buffer) {
    std::unique_lock<decltype(locker_) > guard(locker_);
    if (nsp::tcpip::obudp::sendto((char*) bytes_stream, len, remote_ep_) < 0) return -1;
    notify_flag_ = false;
    while (!notify_flag_) {
        if (std::cv_status::timeout == con_var_.wait_for(guard, std::chrono::seconds(1))) {
            loinfo("omron") << "time out";
            close();
            return -1;
        }
    }

    buffer = buffer_;//on_recvdata()
    return 0;
}

void netdriver_omron_plc::check_write_blocks() {
    var__memory_data_block *p_plc_dup = new var__memory_data_block[sizeof (ptr_object_->writeable_blocks_)];
     
    posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
#if _WIN32
    memcpy_s(p_plc_dup, sizeof (ptr_object_->writeable_blocks_), ptr_object_->writeable_blocks_, sizeof (ptr_object_->writeable_blocks_));
#else
    memcpy(p_plc_dup, ptr_object_->writeable_blocks_, sizeof (ptr_object_->writeable_blocks_));
#endif
    posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);

    for (size_t i = 0; i < MEM_MAXIMUM_BLOCK; i++) {
        if (p_plc_dup[i].actived_) {

                std::string recv_data;
                if (post_read_request(p_plc_dup[i].start_address_, p_plc_dup[i].effective_count_in_word_, recv_data) < 0) {
                    continue;
                }
                if (recv_data.length() <= 0) {
                    continue;
                }
                if (recv_data.length() > 2 * MEM_BLOCK_REGION_DATA_SIZE) {
                    loinfo("omron") << "check_write_blocks,recv_data len > MEM_BLOCK_REGION_DATA_SIZE";
                    continue;
                }
                
                posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
#if _WIN32
                memcpy_s(ptr_object_->writeable_blocks_[i].data_, recv_data.length(), recv_data.c_str(), recv_data.length());
#else
                memcpy(ptr_object_->writeable_blocks_[i].data_, recv_data.c_str(), recv_data.length());
#endif
                posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
                
        }
    }
}

int netdriver_omron_plc::net_create(var__functional_object_t *object)
{
    const nsp::tcpip::endpoint ep("0.0.0.0", 0);
    try {
        
        if (this->listen(ep) < 0) return -1;

        var__memory_netdev_t *plc_object;
        plc_object = var__object_body_ptr(var__memory_netdev_t, object);
        if (!plc_object) {
            return -1;
        }   

        return this->set_plc_object_ptr(plc_object);
    }
    catch (...) {
        return -1;
    }
}

int netdriver_omron_plc::net_close()
{
    close();
    return 0;
}
