#include "agv_manager.h"
#include "os_util.hpp"
#include "memory"
#include "network.h"
#include "xml_node.h"
#include <algorithm>
//#include "agvinfo_proto.hpp"

//#define  debug_test_disconnect

agv_manager::agv_manager()
{

}

agv_manager::~agv_manager()
{
	disconnect();
}

int agv_manager::connect_server(std::string ip, uint16_t port)
{
	nsp::tcpip::endpoint ep;
	if (nsp::tcpip::endpoint::build(ip.c_str(), port, ep) < 0){
		return -1;
	}

	std::shared_ptr<agv_session> session = nullptr;
	{
		std::lock_guard<decltype(mutex_)> locker(mutex_);
		if (agv_session_){
			if (!ep.to_string().compare(agv_session_->remote().to_string().c_str())){
				return 0;
			}
		}

		try{
			session = std::make_shared<agv_session>(std::bind(&agv_manager::on_disconnect, this, std::placeholders::_1));
		}
		catch (...){
			loerror("agv_info") << "failed to make share ptr of session";
			return -2;
		}

		if ( session->create() < 0){
			loerror("agv_info") << "session create error";
			return -1;
		}

		if (session->connect(ep)< 0 ){
			loerror("agv_info") << "connect failed";
			return -1;
		}

		ipstr_ = session->remote().ipv4();
		port_ = session->remote().port();
		std::swap(agv_session_, session);
	}

	if (session){							//连接成功后再剔除已连接的session
		session->close();
	}

	if (!thread_)
	{
		try{
			thread_ = new std::thread(std::bind(&agv_manager::check_connect, this));
		}
		catch (...){
			nsperror << "create thread error";
			return -1;
		}
	}

	return 0;
}

void agv_manager::disconnect()
{
	ipstr_ = "";
	std::shared_ptr<agv_session> session = nullptr;
	{
		std::lock_guard<decltype(mutex_)> locker(mutex_);
		std::swap(session, agv_session_);
	}

	if (session){
		session->close();
	}
	session_condition_.notify_one();
	if (thread_){
		if (thread_->joinable()){
			thread_->join();
		}
		delete thread_;
		thread_ = nullptr;
	}
}

void agv_manager::on_disconnect( std::string ipstr )
{
	std::lock_guard<decltype(mutex_)> locker(mutex_);
	if (agv_session_ && !agv_session_->remote().to_string().compare(ipstr.c_str())){
		agv_session_ = nullptr;
		session_condition_.notify_one();
	}
}

int agv_manager::check_connect(){
	const static uint64_t time_out_tick = 60000;
	while (true){
		{
			std::shared_ptr<agv_session> session = nullptr;
			{
				std::unique_lock<decltype(mutex_)> locker(mutex_);
				if (agv_session_){
					uint64_t tick = agv_session_->get_tick();
					uint64_t current_tick = nsp::os::gettick();
					if (current_tick - tick > time_out_tick){
#ifdef debug_test_disconnect
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
						std::swap(session, agv_session_);
#ifdef debug_test_disconnect
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
					}
					else{
						if (agv_session_->keepalive() < 0){
							std::swap(session, agv_session_);
						}
					}
					
					if (agv_session_){
						session_condition_.wait_for(locker, std::chrono::seconds(5));
					}
				}
			}

			if (session){
				session->close();
			}
		}

		if (!ipstr_.compare("")){
			break;
		}

		if (!agv_session_){
			if (connect_server(ipstr_, port_)){
				std::this_thread::sleep_for(std::chrono::seconds(1));				//防止断链后，此时路由不存在（拔网线）导致的CPU占用过高
			}
		}
	}
	return 0;
}

agv_info * agv_manager::new_agvinfo(const agv_info_inner &agvinfo)
{
	agv_info * temp = nullptr;
	try{
		temp = new agv_info;
	}
	catch (...){
		return nullptr;
	}
	memset(temp, 0, sizeof(agv_info));

	temp->vhid = agvinfo.id_;
	temp->vhtype = agvinfo.type_;
	temp->mtport = agvinfo.mtport_;
	temp->shport = agvinfo.shport_;
	temp->status = agvinfo.status_;
	temp->ftsport = agvinfo.ftsport_;
	temp->attrs = nullptr;
	
	nsp::toolkit::posix_strncpy(temp->inet, sizeof(temp->inet), agvinfo.inet_.c_str(), sizeof(temp->inet)-1);
	nsp::toolkit::posix_strncpy(temp->hwaddr, sizeof(temp->hwaddr), agvinfo.hwaddr_.c_str(), sizeof(temp->hwaddr) - 1);
	temp->count_of_attrs = agvinfo.vec_attrs_.size();
	if (build_attr(agvinfo.vec_attrs_, temp->attrs)){
		delete temp;
		return nullptr;
	}
	temp->next = nullptr;

	return temp;
}

int agv_manager::build_attr(const std::vector<agv_attribute_inner> &vec_attr, struct agv_attribute *&attr)
{
	int length = vec_attr.size();
	if (length > 0){
		agv_attribute * arr_attr = nullptr;
		try{
			arr_attr = new agv_attribute[length];
		}
		catch (...){
			return -1;
		}

		attr = arr_attr;
		for (auto iter : vec_attr){
			nsp::toolkit::posix_strncpy<char>(arr_attr->name, sizeof(arr_attr->name), iter.name_.c_str(), sizeof(arr_attr->name) - 1);
			nsp::toolkit::posix_strncpy<char>(arr_attr->describe, sizeof(arr_attr->describe), iter.describe_.c_str(), sizeof(arr_attr->describe) - 1);
			arr_attr++;
		}
	}

	return 0;
}

int agv_manager::build_agvinfo(const std::vector<agv_info_inner> &agvinfo, agv_info **agvs)
{
	agv_info * pre_node = nullptr;
	for (auto iter : agvinfo){
		agv_info* pNew = new_agvinfo(iter);
		if (!pNew){
			release_agvinfo(*agvs);
			return -1;
		}
		
		if (!pre_node){
			pre_node = pNew;
			*agvs = pNew;
		}
		else{
			pre_node->next = pNew;
			pre_node = pNew;
		}
	}
	return 0;
}

int agv_manager::build_agvdetail(uint32_t vhid, const std::vector<agv_attribute_inner> &vec_attr, struct agv_detail **detail)
{
	if (!detail){
		return -1;
	}

	try{
		*detail = new agv_detail;
	}
	catch (...){
		return -1;
	}

	(*detail)->vhid = vhid;
	(*detail)->count_of_attrs = vec_attr.size();
	(*detail)->attrs = nullptr;
	if (build_attr(vec_attr, (*detail)->attrs)){
		delete *detail;
		return -1;
	}

	return 0;
}

void agv_manager::build_inner_agvinfo(const struct agv_info *agvs, std::vector<agv_info_inner> &vec_agvinfo)
{
	while (agvs){
		agv_info_inner inner_agvinfo;
		inner_agvinfo.id_ = agvs->vhid;
		inner_agvinfo.type_ = agvs->vhtype;
		inner_agvinfo.inet_ = agvs->inet;
		inner_agvinfo.mtport_ = agvs->mtport;
		inner_agvinfo.shport_ = agvs->shport;
		inner_agvinfo.ftsport_ = agvs->ftsport;
		inner_agvinfo.hwaddr_ = agvs->hwaddr;
		inner_agvinfo.status_ = agvs->status;

		if (agvs->count_of_attrs > 0 && agvs->attrs != nullptr){
			for (int index = 0; index < agvs->count_of_attrs; index++){
				inner_agvinfo.vec_attrs_.push_back(agv_attribute_inner(agvs->attrs[index].name, agvs->attrs[index].describe));
			}
		}
		vec_agvinfo.push_back(inner_agvinfo);
		agvs = agvs->next;
	}
}

int agv_manager::load_server_agvinfo(enum load_agvinfo_method method, std::vector<agv_info_inner>& vec_agvinfo)
{
	nsp::os::waitable_handle wait(0);
	agv_motion::motion_data motion_back;
	int errcode = 0;
	std::shared_ptr<agv_session> session = agv_session_;
#ifdef debug_test_disconnect
	std::this_thread::sleep_for(std::chrono::milliseconds(60));
	if (!agv_session_ && session){
		return -2;
	}
#endif
	if (!session){
		return -1;
	}

	errcode = session->load_agvinfo(std::make_shared<agv_motion::motion_ack>([&](const void * data){
		if (data){
			motion_back = *(agv_motion::motion_data *)data;
		}
		wait.sig();
	}), method);
	
	if(errcode){
		return errcode;
	};

	wait.wait();
	wait.reset();

	if (-1 == motion_back.error_){
		return -2;
	}

	int cb = motion_back.packet_.length();
	proto_req_agvinfo agvinfo;
	if (!agvinfo.build((unsigned char *)(motion_back.packet_.c_str()), cb)){
		return -3;
	}

	if (kAgvInfoProto_OK != agvinfo.head.err_){
		return -1;
	}

	for (auto iter : agvinfo.info){
		agv_info_inner inner_agvinfo;
		inner_agvinfo.id_ = iter.vhid;
		inner_agvinfo.type_ = iter.vhtype;
		inner_agvinfo.inet_ = iter.inet;
		inner_agvinfo.mtport_ = iter.mtport;
		inner_agvinfo.shport_ = iter.shport;
		inner_agvinfo.ftsport_ = iter.ftsport;
		inner_agvinfo.hwaddr_ = iter.hwaddr;
		inner_agvinfo.status_ = iter.status;
		for (auto attr_node : iter.attrs){
			inner_agvinfo.vec_attrs_.push_back(agv_attribute_inner(attr_node.name, attr_node.describe));
		}
		vec_agvinfo.push_back(inner_agvinfo);
	}
	return 0;
}

int agv_manager::load_agvinfo(struct agv_info **agvs, enum load_agvinfo_method method)
{
	if (!agvs){
		return -1;
	}

	std::vector<agv_info_inner> agvinfo;
	if (LAM_Local == method){
		if (nsp::toolkit::singleton<xml_node>::instance()->load_agvinfo(agvinfo)){
			return -1;
		}
	}
	else{
		if (load_server_agvinfo(method, agvinfo)){
			return -1;
		}
	}

	if (agvinfo.size() > 1){
		std::sort(agvinfo.begin(), agvinfo.end(), [&](const agv_info_inner & first, const agv_info_inner & secend)->bool{
			return first.id_ < secend.id_;
		});
	}
	if (build_agvinfo(agvinfo, agvs)){
		return -4;
	}

	return 0;
}

int agv_manager::conver_server_agvinfo(std::vector<agv_info_inner>& vec_agvinfo, enum load_agvinfo_method method)
{
	nsp::os::waitable_handle wait(0);
	agv_motion::motion_data motion_back;
	int errcode = 0;
	std::shared_ptr<agv_session> session = agv_session_;
	if (!session){
		return -1;
	}
	errcode = session->set_agvinfo(std::make_shared<agv_motion::motion_ack>([&](const void * data){
		if (data){
			motion_back = *(agv_motion::motion_data *)data;
		}
		wait.sig();
	}), method, vec_agvinfo);

	if (errcode){
		return errcode;
	};

	wait.wait();
	wait.reset();

	if (-1 == motion_back.error_){
		return -2;
	}
	proto_req_agvinfo agv_data;
	int cb = motion_back.packet_.length();
	if (!agv_data.build((unsigned char *)(motion_back.packet_.c_str()), cb)){
		return -3;
	}

	if (agv_data.head.err_ != kAgvInfoProto_OK){
		return -3;
	}

	return 0;
}

int agv_manager::cover_agvinfo(const struct agv_info *agvs, enum load_agvinfo_method method)
{
	std::vector<agv_info_inner> vec_agvinfo;
	build_inner_agvinfo(agvs, vec_agvinfo);
	if (LAM_Local == method){
		return nsp::toolkit::singleton<xml_node>::instance()->conver_agvinfo(vec_agvinfo);
	}
	else if (LAM_Server == method){
		return conver_server_agvinfo(vec_agvinfo, method);
	}

	return -1;
}

void agv_manager::about_something_changed(void(*notify)())
{
	nsp::toolkit::singleton<network>::instance()->reg_notify_callback(notify);
}

int agv_manager::get_server_detail(uint32_t vhid, enum load_agvinfo_method method, std::vector<agv_attribute_inner> &vec_attr)
{
	std::shared_ptr<agv_session> session = agv_session_;
	if (!session){
		return -1;
	}
	nsp::os::waitable_handle wait(0);
	agv_motion::motion_data motion_back;
	int errcode = session->get_agvdetail(std::make_shared<agv_motion::motion_ack>([&](const void * data){
		if (data){
			motion_back = *(agv_motion::motion_data *)data;
		}
		wait.sig();
	}), vhid, method);

	if (errcode){
		return errcode;
	};

	wait.wait();
	wait.reset();

	if (-1 == motion_back.error_){
		return -2;
	}
	proto_req_agvdetail detail;
	int length = motion_back.packet_.length();
	if (!detail.build((unsigned char*)motion_back.packet_.c_str(), length)){
		return -1;
	}

	if (kAgvInfoProto_OK != detail.head.err_){
		return -1;
	}

	for (auto iter : detail.detail.attrs){
		vec_attr.push_back(agv_attribute_inner(iter.name, iter.describe));
	}
	return 0;
}

int agv_manager::get_agvdetail(uint32_t vhid, struct agv_detail **detail, enum load_agvinfo_method method)
{
	std::vector<agv_attribute_inner> vec_attr;
	if (LAM_Local == method){
		if (nsp::toolkit::singleton<xml_node>::instance()->get_detail(vhid, vec_attr)){
			return -1;
		}
	}
	else{
		if (get_server_detail(vhid, method, vec_attr)){
			return -1;
		}
	}
	return build_agvdetail(vhid, vec_attr, detail);
}

int agv_manager::set_agvdetail(uint32_t vhid, const std::vector<agv_attribute_inner> &attribute )
{
	std::shared_ptr<agv_session> session = agv_session_;					//有可能出现 调用时发生断链
	if (!session){
		return -1;
	}

	nsp::os::waitable_handle wait(0);
	agv_motion::motion_data motion_back;
	int errcode = session->set_agvdetail(std::make_shared<agv_motion::motion_ack>([&](const void * data){
		if (data){
			motion_back = *(agv_motion::motion_data *)data;
		}
		wait.sig();
	}), vhid, attribute);

	if (errcode){
		return errcode;
	};

	wait.wait();
	wait.reset();

	if (-1 == motion_back.error_){
		return -2;
	}
	proto_req_agvdetail agv_update_detail;
	int length = motion_back.packet_.length();
	if (!agv_update_detail.build((unsigned char*)motion_back.packet_.c_str(), length)){
		return -1;
	}

	if (kAgvInfoProto_OK != agv_update_detail.head.err_){
		return -1;
	}
	return 0;
}

int agv_manager::set_agvdetail(uint32_t vhid, const struct agv_detail *detail, enum load_agvinfo_method method)
{
	if (!detail){
		return -1;
	}

	std::vector<agv_attribute_inner> vec_attr;
	for (int index = 0; index < detail->count_of_attrs; index++){
		vec_attr.push_back(agv_attribute_inner(detail->attrs[index].name, detail->attrs[index].describe));
	}

	if (LAM_Local == method){
		return nsp::toolkit::singleton<xml_node>::instance()->set_detail(vhid, vec_attr);
	}
	else if (LAM_Server == method){
		return set_agvdetail(vhid, vec_attr);
	}
	return 0;
}

void agv_manager::release_agvinfo(struct agv_info *agvs)
{
	while (agvs){
		struct agv_info * phead = agvs;
		if (phead->count_of_attrs > 0 ){
			release_attribute(phead->attrs);
		}
		agvs = phead->next;
		delete phead;
	}
}

void agv_manager::release_agvdetail(struct agv_detail *detail)
{
	if (detail){
		struct agv_attribute * pattr = detail->attrs;
		int count = detail->count_of_attrs;
		delete detail;
		if (count > 0){
			release_attribute(pattr);
		}
	}
}

void agv_manager::release_attribute(struct agv_attribute * attr)
{
	if (attr){
		delete [] attr;
	}
}