#include "netdriver_modbus.h"


netdriver_modbus::netdriver_modbus()
{
}


netdriver_modbus::~netdriver_modbus()
{
}

int netdriver_modbus::net_create(var__functional_object_t *object)
{
    ptr_object_ = var__object_body_ptr(var__memory_netdev_t, object);
    if (!ptr_object_) {
        return -1;
    }
	the_interval_of_read_ = ptr_object_->interval_for_read_thread_;
	the_interval_of_write_ = ptr_object_->interval_for_write_thread_;
    byte_order_ = ptr_object_->netdev_head_.byte_order_;
    host_ip_ = ptr_object_->netdev_head_.remote_.ipv4_;
    host_port_ = ptr_object_->netdev_head_.remote_.port_;
    nsp::tcpip::endpoint ep;
    ep.ipv4(host_ip_);
    ep.port(host_port_);
    this->create();
    if (this->connect(ep) < 0) {
        return -1;
    }

    //对所有的有效write blocks 初始化
    check_write_blocks();


    if (!th_read_) {
        th_read_ = new std::thread(std::bind(&netdriver_modbus::th_read, this));
    }


//     if (!th_write_) {
//         th_write_ = new std::thread(std::bind(&netdriver_modbus::th_write, this));
//     }
    return 0;
}

int netdriver_modbus::net_close()
{
    waiter_read_.sig();
    if (th_read_) {
        if (th_read_->joinable()) {
            th_read_->join();
            delete th_read_;
            th_read_ = nullptr;
        }
    }
    waiter_read_.reset();

    waiter_write_.sig();
    if (th_write_) {
        if (th_write_->joinable()) {
            th_write_->join();
            delete th_write_;
            th_write_ = nullptr;
        }
    }
    waiter_write_.reset();
    return 0;
}

void netdriver_modbus::on_recvdata(const std::string &pkt)
{
    std::unique_lock<decltype(locker_) > guard(locker_);
    buffer_ = pkt;
    int len = buffer_.length();
    notify_flag_ = true;
    con_var_.notify_one();
}


int netdriver_modbus::read_register(int register_no, int word_num_want, char* register_data, int& word_num_get)
{
    if (register_no < 1)
    {
        loerror("modbus")<<"modbus "<< "err:addr < 1";
        return -1;
    }

    std::string recv_buf;
    //地址从0开始   寄存器号从1开始 实际地址为 （寄存器号-1）
    int addr = (register_no - 1);
    //组发送报文
    {
        char buf[12] = { 0 };
        //事务标识
        buf[0] = 0x00;
        buf[1] = 0x00;
        //协议标识
        buf[2] = 0x00;
        buf[3] = 0x00;
        //长度
        buf[4] = 0x00;
        buf[5] = 0x06;
        //单元标识
        buf[6] = 0x01;
        //功能码
        buf[7] = 0x03;//读寄存器
        //起始寄存器地址
        buf[9] = addr & 0xFF;
        buf[8] = (addr >> 8) & 0xFF;
        //寄存器个数
        buf[11] = word_num_want & 0xFF;
        buf[10] = (word_num_want >> 8) & 0xFF;


        send_with_reply(std::string(buf,12),recv_buf);
    }

    //接收结果
    {
        do
        {
            if (recv_buf.size() < 9)
            {
                loerror("modbus") << "modbus "  << "ReadData：：socket read_some failed ,len < 9";
                break;
            }
            else if ((recv_buf[7] == char(0x03 + 0x80)) && (recv_buf.size() == 9))
            {
                loerror("modbus") << "modbus " << "ReadData：：MODBUS error! error code = " << int(recv_buf[8]);
                break;
            }
            else if (recv_buf[7] != 0x03)
            {
                loerror("modbus") << "modbus " << "ReadData：：MODBUS error! fn code != 0x03 ,fn=" << (int)recv_buf[7];
                break;
            }

            word_num_get = recv_buf[8] / 2;

            if (kNetworkByteOrder_BigEndian == byte_order_) {
                short *target_data = new short[word_num_get];
                char *prev = &recv_buf[9];
                for (int i = 0; i < word_num_get; i++) {
                    short tans_data = *(short*)prev;
                    tans_data = nsp::toolkit::change_byte_order(tans_data);
                    target_data[i] = tans_data;
                    prev += sizeof (short);
                }

                memcpy(register_data, (char*)target_data, word_num_get * 2);
                delete[] target_data;

            }
            else if (kNetworkByteOrder_LittleEndian == byte_order_)
            {
                memcpy(register_data, &recv_buf[9], word_num_get * 2);
            }
            
            return 0;
        } while (0);
    }

    return -1;
}

int netdriver_modbus::write_register(int register_no, char* register_data, int word_num)
{
    if (register_no < 1)
    {
        loerror("modbus") << "modbus " << "err:addr < 1";
        return -1;
    }

    //地址从0开始   寄存器号从1开始 实际地址为 （寄存器号-1
    int addr = (register_no - 1);

    std::string recv_buf;
    //组报文
    {
        bool ret = false;
        char* buf = new (std::nothrow) char[word_num * 2 + 13];
        do
        {
            if (NULL == buf)
            {
                loerror("modbus") << "modbus " << "WriteData：：new memory failed!";
                break;
            }
            memset(buf, 0, word_num * 2 + 11);
            //事务标识
            buf[0] = 0x00;
            buf[1] = 0x10;
            //协议标识
            buf[2] = 0x00;
            buf[3] = 0x00;
            //长度
            int length = word_num * 2 + 7;
            buf[4] = (length >> 8) & 0xFF;
            buf[5] = length & 0xFF;
            //单元标识
            buf[6] = 0x01;
            //功能码
            buf[7] = 0x10;//写寄存器
            //起始寄存器地址
            buf[9] = addr & 0xFF;
            buf[8] = (addr >> 8) & 0xFF;
            //寄存器个数
            buf[11] = word_num & 0xFF;
            buf[10] = (word_num >> 8) & 0xFF;
            //数据长度
            buf[12] = word_num * 2;

            if (kNetworkByteOrder_BigEndian == byte_order_) {
                for ( int i = 0; i < word_num * 2; i += 2) {
                    buf[13 + i] = register_data[i + 1];
                    buf[14 + i] = register_data[i];
                }
            }
            else if (kNetworkByteOrder_LittleEndian == byte_order_) {
                for ( int i = 0; i < word_num * 2; i += 2) {
                    buf[13 + i] = register_data[i];
                    buf[14 + i] = register_data[i + 1];
                }
            }

            //发送
            send_with_reply(std::string(buf, word_num * 2 + 13), recv_buf);

        } while (0);
        delete[] buf;
    }

    //接收结果
    {

        if (recv_buf.size() < 9)
        {
            loerror("modbus") << "modbus " << "WriteData：：socket read_some failed ,len < 9";
            return -1;
        }
        else if ((recv_buf[7] == char(0x10 + 0x80)) && (recv_buf.size() == 9))
        {
            loerror("modbus") << "modbus " << "WriteData：：MODBUS error! error code = " << int(recv_buf[8]);
            return -1;
        }

        if (recv_buf.size() == 12)
        {
            return 0;
        }
    }
    return 0;
}

int netdriver_modbus::send_with_reply(std::string s_buf, std::string& recv_buf)
{
    std::unique_lock<decltype(locker_) > guard(locker_);
    if (this->send(s_buf) < 0) return -1;
    notify_flag_ = false;
    while (!notify_flag_) {
        if (std::cv_status::timeout == con_var_.wait_for(guard, std::chrono::seconds(1))) {
            loerror("modbus") << "modbus " << "time out";
            close();
            return -1;
        }
    }

    recv_buf = buffer_;
    return 0;
}

void netdriver_modbus::on_disconnected(const HTCPLINK previous)
{
    loerror("modbus") << "modbus " << "begin reconnect";
    nsp::tcpip::endpoint ep;
    ep.ipv4(host_ip_);
    ep.port(host_port_);
    this->create();
    this->connect(ep);
}

void netdriver_modbus::check_write_blocks()
{
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
                char* recv_data = new char[p_plc_dup[i].effective_count_in_word_*2];
                if (!recv_data)
                {
                    loerror("modbus") << "new mem failed!";
                    continue;
                }

                do 
                {
                    int word_get = 0;
                    if (read_register(p_plc_dup[i].start_address_, p_plc_dup[i].effective_count_in_word_, recv_data, word_get) < 0) {
                        break;
                    }

                    if (word_get >  MEM_BLOCK_REGION_DATA_SIZE / 2) {
                        loinfo("modbus") << "check_write_blocks,recv_data len > MEM_BLOCK_REGION_DATA_SIZE";
                        break;
                    }
                    posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
                    memcpy(ptr_object_->writeable_blocks_[i].data_, recv_data, word_get * 2);
                    posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
                } while (0);

                if (recv_data)
                {
                    delete[]recv_data;
                }
        }
    }
}

void netdriver_modbus::th_read()
{
    while (waiter_read_.wait(the_interval_of_read_) > 0) {

        {//read
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
                    char* recv_data = new char[p_plc_dup[i].effective_count_in_word_ * 2];
                    if (!recv_data)
                    {
                        loerror("modbus") << "new mem failed!";
                        continue;
                    }

                    do
                    {
                        int word_get = 0;
                        if (read_register(p_plc_dup[i].start_address_, p_plc_dup[i].effective_count_in_word_, recv_data, word_get) < 0) {
                            break;
                        }

                        if (word_get >  MEM_BLOCK_REGION_DATA_SIZE / 2) {
                            loinfo("modbus") << "check_write_blocks,recv_data len > MEM_BLOCK_REGION_DATA_SIZE";
                            break;
                        }
                        posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
                        memcpy(ptr_object_->readonly_blocks[i].data_, recv_data, word_get * 2);
                        posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
                    } while (0);

                    if (recv_data)
                    {
                        delete[]recv_data;
                    }
                }

                //check 是否周期性发送
                posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
                if (p_plc_dup[i].period_) {
                    ptr_object_->readonly_blocks[i].flush_ = true;
                }
                else {
                    ptr_object_->readonly_blocks[i].flush_ = false;
                }
                posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
            }
            delete[] p_plc_dup;
        }



        {    //write
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
                if (write_register(p_plc_dup[i].start_address_, (char*)p_plc_dup[i].data_, p_plc_dup[i].effective_count_in_word_) < 0) {
                    continue;
                }
            }

            //chenck是否周期发送
            posix__pthread_mutex_lock(&var__object_head_ptr(ptr_object_)->locker_);
            if (p_plc_dup[i].period_) {
                ptr_object_->writeable_blocks_[i].flush_ = true;
            }
            else {
                ptr_object_->writeable_blocks_[i].flush_ = false;
            }
            posix__pthread_mutex_unlock(&var__object_head_ptr(ptr_object_)->locker_);
        }
        delete[] p_plc_dup;

    }
    }
}

void netdriver_modbus::th_write()
{
    while (waiter_write_.wait(the_interval_of_write_) > 0) {

    }
}
