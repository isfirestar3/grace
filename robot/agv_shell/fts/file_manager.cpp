#include "file_manager.h"
#include "const.h"
#include "file_shell_handler.h"
#include "log.h"
#include "public.h"

#ifdef _WIN32
#include <io.h>
#endif

file_manager::file_manager() {
	initlization();
	file_dir = "./standard/";
}

file_manager::~file_manager(){
	auto iter_thread = map_rw_task_.begin();
	if (iter_thread != map_rw_task_.end()){
		iter_thread->second->stop_write_thread();
		iter_thread->second->stop_read_thread();
	}
	
	{
		std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
		map_file_head_.clear();
	}
}

void file_manager::initlization(){
	//创建FILE_OPERATOR_THREAD_SIZE个读和写线程
	for (int i = 0; i < FILE_OPERATOR_THREAD_SIZE; ++i){
		std::shared_ptr<file_rw_task_thread> task = std::make_shared<file_rw_task_thread>();
		map_rw_task_.insert(std::make_pair(i, task));
	}
}

//保存关联的link，file id,文件句柄
void file_manager::inert_head_t_map(const uint32_t link, const uint64_t file_id, file::st_file_head& f_head)
{
	std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
	auto iter = map_file_head_.find(link);
	if (iter == map_file_head_.end()) {
		std::map<uint64_t, file::st_file_head> tmp;
		tmp.insert(std::make_pair(file_id, f_head));
		map_file_head_.insert(std::make_pair(link, tmp));
	} else {
		auto id_iter = iter->second.find(file_id);
		if (id_iter == iter->second.end()) {
			iter->second.insert(std::make_pair(file_id, f_head));
		} else {
			id_iter->second = f_head;
		}
	}
}

//获取文件head 结构 
int file_manager::get_head_f_map(const uint32_t link, const uint64_t file_id, file::st_file_head** f_head)
{
	std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
	auto iter = map_file_head_.find(link);
	if (iter == map_file_head_.end()) {
		loerror("fts") << "cannot find link in map, lnk:" << link;
		return -1;
	}
	
	auto id_iter = iter->second.find(file_id);
	if (id_iter == iter->second.end()) {
		loerror("fts") << "cannot find file id in map, file id:" << file_id;
		return -1;
	} 
	
	file::st_file_head& f_h = id_iter->second;
    *f_head = &f_h;
	return 0;
	
}

//创建文件
int file_manager::create_files(const uint64_t link, file::st_file_head& f_head, int& error_code){
	if (f_head.file_path.size() <= 0) return 0;
    
    if(f_head.file_type == file_type::type_upgrade) {
		if(is_upgrade_) {
			loerror("fts")<< "exist upgrade packet, cannot push evnymore, flag:" << is_upgrade_;
			error_code = -1;
			return -1;
		}
		set_upgrade();
	}

	file_shell_handler lnk_handler;
	std::string curr_folder_path;
	//判断输入参数是否为文件夹参数，如果是则创建文件夹
	if (f_head.file_path.at(f_head.file_path.size() - 1) == '/'){
		curr_folder_path = f_head.file_path;
		if (f_head.file_path.find(".lnk") != std::string::npos){
			lnk_handler.find_lnk_file(f_head.file_path, curr_folder_path);
		}
		return create_directory(curr_folder_path, error_code);
	}

	f_head.file_name = get_process_name(f_head.file_path);
	f_head.file_path = this->file_dir + f_head.file_name;
	//判断文件夹路径是否存在，如果不存在，则需要创建断层文件夹
	std::string folder_path = f_head.file_path.substr(0, f_head.file_path.find_last_of('/'));
	//判断输入路径中是否有lnk快捷方式符号
	if (folder_path.find(".lnk") != std::string::npos){
		//寻找lnk指向的真正路径
		if (0 != lnk_handler.find_lnk_file(folder_path, curr_folder_path)){
			//未能处理lnk文件夹，则使用输入参数作为目标路径
			curr_folder_path = folder_path;
		}
	} else {
		curr_folder_path = folder_path;
	}

	int type = nsp::os::is_dir(curr_folder_path);
	
	if (type > 0){
		std::lock_guard<decltype(folder_mutex_)> lock(folder_mutex_);
		curr_folder_path += '/';
		if (recreate_file_path(curr_folder_path) < 0){//nsp::os::mkdir(curr_folder_path) < 0
#ifdef _WIN32
			error_code = GetLastError();
#else
            error_code = errno;
#endif
			loerror("fts") << "failed to create folder: " << curr_folder_path << " the error code is " << error_code;
			if(f_head.file_type == file_type::type_upgrade) {
				release_upgrade();
			}
			return -1;
		}
	}

	void* file_handler = nullptr;
	if ((file_handler = write_handle_.create_file(f_head.file_path, f_head.file_create_time, f_head.file_modify_time, f_head.file_access_time, error_code)) == nullptr){
		if(f_head.file_type == file_type::type_upgrade) { 
			release_upgrade();
		}
		loerror("fts") << "failed to create file " << f_head.file_path << " the error code is " << error_code;
		return -1;
	}
	f_head.hdl_fd = file_handler;
	f_head.oper_time = nsp::os::gettick();
	f_head.oper_type = oper_type::oper_type_push;
	inert_head_t_map(link, f_head.file_id, f_head);
	
	return 0;
}

int file_manager::recreate_file_path(const std::string& file_name)
{
	std::string tmp_file = convert_positive(file_name, '\\', '/');

	size_t pos = tmp_file.find("/");
	if (pos == std::string::npos)return -1;
	std::string temp_path = tmp_file.substr(pos + 1, tmp_file.length() - pos - 1);//去掉盘符如（"E:\\")
	std::string root_path = tmp_file.substr(0, pos + 1);
	pos = temp_path.find("/");
	if (pos == std::string::npos)return -1;
	std::string path;
	do{
		path = root_path + temp_path.substr(0, pos);
#ifdef _WIN32
		if (nsp::os::mkdir(path) < 0){
			loerror("fts") << "recreate_file_path mkdir path error:" << path;
			return -1;
		}
#else
        struct stat st;
        if (lstat(path.c_str(), &st) < 0) {
        	loerror("fts") << "failed to get file stat for create, folder name:" << path;
        } else {
            char real_folder_path[128] = {0};
            if ( S_ISLNK(st.st_mode) ) {
            	int folder_len = readlink(path.c_str(), real_folder_path, 127);
            	if ( folder_len >= 0 ) {
            		real_folder_path[folder_len] = '\0';
            		path = real_folder_path;
            	} else {
            		loinfo("fts") << "readlink file_len:" << folder_len << ", errno:" << errno;
            		//return -1;
            	}
            }
        }
#endif
		root_path = path + "/";
		temp_path = temp_path.substr(pos + 1, temp_path.length() - pos - 1);
		pos = temp_path.find("/");
	} while (pos != std::string::npos);
#ifndef _WIN32
	if (nsp::os::is_dir(path) < 0) {
    	if (nsp::os::mkdir_s(path) < 0) {
    		loerror("fts") << "recreate_file_path mkdir error: " << errno;
    		return -1;
    	}
    }
#endif
	return 0;
}

//读取文件信息
int file_manager::read_file_info(const uint32_t link, file::st_file_head& f_head, int& error_code) {
	if(f_head.file_path.empty()) return 0;
	
	std::string file_path = f_head.file_path;
	void* file_handler = nullptr;
	//如果发现要求读取的是文件夹，则调用读取文件夹信息函数
	if (nsp::os::is_dir(file_path) > 0){
		return read_handler_.read_folder_info(file_path, f_head.file_create_time, f_head.file_modify_time, f_head.file_access_time, error_code);
	}

	if ((file_handler = read_handler_.read_file_head(file_path, f_head.total_size,
		f_head.file_create_time, f_head.file_modify_time, f_head.file_access_time, error_code)) == nullptr){
		loerror("fts") << "failed read file:" << file_path << " ,the error code is " << error_code;
		return -1;
	}
	
	f_head.hdl_fd = file_handler;
	f_head.file_name = get_process_name(f_head.file_path);
	f_head.oper_time = nsp::os::gettick();
	f_head.oper_type = oper_type::oper_type_pull;
	inert_head_t_map(link, f_head.file_id, f_head);
	return 0;
}

//读取指定块数据流信息
int file_manager::read_file_block_stream(const uint32_t link, const int pkt_id, const uint64_t file_id, 
		const uint32_t block_num, const uint64_t offset, const uint32_t read_size) {
	file::st_file_head* f_info = nullptr;
	
	if(get_head_f_map(link, file_id, &f_info) < 0 ) {
		loerror("fts") << "read_file_block_stream get file head info failure.";
		return -1;
	}
	
	if( !f_info->hdl_fd ) {
		loerror("fts") << "read_file_block_stream file is not open, file:" << f_info->file_path;
		return -1;
	}

	int index = ((long)f_info->hdl_fd & 0xffffffff) % FILE_OPERATOR_THREAD_SIZE;
	auto iter_thread = map_rw_task_.find(index);
	if (iter_thread == map_rw_task_.end()){
		loerror("fts") << "read_file_block_stream can not find read task in the map_rw_task_.";
		return -1;
	}

	iter_thread->second->add_read_task(link, pkt_id, block_num, f_info, offset, read_size);
	return 0;
}

//写文件流信息
int file_manager::write_file_block_stream(const uint32_t link, const int pkt_id, const uint64_t file_id, 
		const uint32_t block_num, const uint64_t offset, const std::string& stream){
	file::st_file_head* f_info = nullptr;
	
	if(get_head_f_map(link, file_id, &f_info) < 0 ) {
		loerror("fts") << "write_file_block_stream get file head info failure.";
		return -1;
	}
	
	if( !f_info->hdl_fd ) {
		loerror("fts") << "write_file_block_stream file is not open, file:" << f_info->file_path;
		return -1;
	}

	int index = ((long)f_info->hdl_fd & 0xffffffff) % FILE_OPERATOR_THREAD_SIZE;
	auto iter_thread = map_rw_task_.find(index);
	if (iter_thread == map_rw_task_.end()){
		loerror("fts") << "write_file_block_stream can not find write task in the map_rw_task_.";
		return -1;
	}
	
	iter_thread->second->add_write_task(link, pkt_id, block_num, f_info, offset, stream);
	return 0;
}

//删除指定文件接口
int file_manager::delete_file(const std::string& file_path){
	file_shell_handler lnk_handler;
	std::string handler_path;
	//判断该路径是否为lnk快捷方式
	if (file_path.find(".lnk") != std::string::npos){
		//寻找lnk指向的真正路径
		if (0 != lnk_handler.find_lnk_file(file_path, handler_path)){
			//未能处理lnk文件夹，则使用输入参数作为目标路径
			handler_path = file_path;
		}
	}
	else{
		handler_path = file_path;
#ifndef _WIN32
    struct stat st;
    if (lstat(file_path.c_str(), &st) < 0) {
    	loerror("fts") << "failed to get file stat for delete, file name:" << file_path;
    	return -1;
    }
    
    char real_file[128] = {0};
    if ( S_ISLNK(st.st_mode) ) {
    	int file_len = readlink(file_path.c_str(), real_file, 127);
    	if ( file_len >= 0 ) {
    		real_file[file_len] = '\0';
    		handler_path = real_file;
    	} else {
    		loinfo("fts") << "readlink file_len:" << file_len << ", errno:" << errno;
    	}
    }
#endif
	}
	int res = nsp::os::is_dir(handler_path);
	if (res > 0){
		//目录
		return (nsp::os::rmdir_s(handler_path) >= 0 ? 0 : -1);//rmdir_s == 0 删除成功；other 删除失败
	}
	else if (res == 0){
		//文件
		return (nsp::os::rmfile(handler_path) >= 0 ? 0 : -1);//rmfile == 0删除成功，other：删除失败
	}
	else{
		//错误
		loerror("fts") << "can not find or delete file:" << handler_path;
		return -1;
	}
}

//比较源文件大小与目标文件大小是否一致，如果不一致，则删除目标文件
int file_manager::file_complete(const uint32_t link, const uint64_t file_id) {
	file::st_file_head* f_info = nullptr;
	int ret = -1;
	
	if(get_head_f_map(link, file_id, &f_info) < 0 ) {
		loerror("fts") << "file_complete get file head info failure.";
		return ret;
	}
	
	do {
		if( !f_info->hdl_fd ) {
			loerror("fts") << "file_complete file is not open, file:" << f_info->file_path;
			break;
		}
		
		read_handler_.close_handler(f_info->hdl_fd);
		f_info->hdl_fd = nullptr;
		
		ret = 0;
		loinfo("fts")<< "oper_type:" << f_info->oper_type << ", file_type:" << f_info->file_type;
		if( oper_type_push == f_info->oper_type ) {
			reset_file_time(f_info->file_path, f_info->file_create_time, f_info->file_modify_time, f_info->file_access_time);
			if(f_info->file_type == file_type::type_upgrade) {
				release_upgrade();
			}
			
			//写文件是否存在且没写完
#ifdef _WIN32
			if (_access(f_info->file_path.c_str(), 0) != -1){
#else
			if (access(f_info->file_path.c_str(), F_OK) != -1){
#endif
				uint64_t  file_size, c_time, a_time, m_time;
				int error_code;
				//取已经写了的文件的大小与源文件大小比较
				read_handler_.close_handler(read_handler_.read_file_head(f_info->file_path, file_size, c_time, m_time, a_time, error_code));
				if (f_info->total_size != file_size){
					loerror("fts") << "the size of writing file:" << file_size << " does not match the source file size:" << f_info->total_size;
					write_handle_.delete_file(f_info->file_path);
					loerror("fts") << "delete the file:" << f_info->file_path;
					ret = -2;
				} else {
					ret = 0;
				}
			}
		}
		
		
	} while (0);
	
	close_rw_file_handler(link, f_info->file_id);
	return ret;
}

//关闭长连接下某个文件句柄
int file_manager::close_rw_file_handler(const uint32_t link, const uint64_t file_id)
{
	std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
	auto iter = map_file_head_.find(link);
	if (iter == map_file_head_.end())
	{
		return -1;
	}
	auto iter_fd = iter->second.find(file_id);
	if (iter_fd == iter->second.end())
	{
		return -1;
	}
	loinfo("fts") << "close file:" << iter_fd->second.file_path;
	int result = iter_fd->second.hdl_fd ? write_handle_.close_handler(iter_fd->second.hdl_fd) : 0;
	iter->second.erase(iter_fd);
	return result;
}

//删除连接下所有读写文件句柄
void file_manager::close_rw_file_handler(const uint32_t link)
{
	std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
	auto iter = map_file_head_.find(link);
	if (iter == map_file_head_.end()){
		return;
	}
	loinfo("fts") << "close all link:" << link;
	for (const auto& iter_fd : iter->second)
	{
		loinfo("fts") << "close file:" << iter_fd.second.file_path;
		if (iter_fd.second.file_type == file_type::type_upgrade) {
			release_upgrade();
		}
		if(iter_fd.second.hdl_fd) write_handle_.close_handler(iter_fd.second.hdl_fd);
	}
	iter->second.clear();
	map_file_head_.erase(iter);
}

//删除读写线程中读写队列中数据
void file_manager::clear_rw_task_deque(const uint32_t link){
	auto iter_thread = map_rw_task_.begin();
	if (iter_thread != map_rw_task_.end()){
		iter_thread->second->clear_rw_deque(link);
	}
}

int file_manager::create_directory(const std::string& dir, int & error_code){
#ifdef _WIN32
	if (!PathFileExistsA(dir.c_str())){
		if (recreate_file_path(dir) < 0){
			error_code = GetLastError();
			loerror("fts") << "failed to create directory: " << dir << " the error code is " << error_code;
			return -1;
		}
	}
	return 0; 
#else
    if (recreate_file_path(dir) < 0){
        error_code = errno;
    	loerror("fts") << "failed to create directory: " << dir << " the error code is " << error_code;
    	return -1;
    }
    return 0;
#endif
}

int file_manager::reset_file_time(const std::string& file_name, const uint64_t& create_time, const uint64_t& modify_time, const uint64_t& access_time)
{
	return write_handle_.reset_file_time(file_name, create_time, modify_time, access_time);
}

void file_manager::check_file_timeout() {
	uint64_t now = nsp::os::gettick();
	uint64_t diff_time = 0;
	{
		std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
		auto iter = map_file_head_.begin();
		while(iter != map_file_head_.end()) {
			auto it = iter->second.begin();
			while(it != iter->second.end()) {
				diff_time = now - it->second.oper_time;
				if( diff_time > TIMEOUT_TRANSFER) {
					loerror("fts") << "file operator overtime:" << diff_time << ",name:" << it->second.file_path << ",file_type:" << it->second.file_type;
					
					read_handler_.close_handler(it->second.hdl_fd);
					it->second.hdl_fd = nullptr;
					if(file_type::type_upgrade == it->second.file_type) {
						release_upgrade();
					}
					write_handle_.delete_file(it->second.file_path);
					
					it = iter->second.erase(it);
				} else {
					++it;
				}
			}
			if( 0 == iter->second.size()) {
				loerror("fts") << "delete data of link:" << iter->first;
				iter = map_file_head_.erase(iter);
			} else {
				++iter;
			}
		}
	}
}

void file_manager::set_upgrade() {
	is_upgrade_++;
	loinfo("fts") << "upgrade flag set to:" << is_upgrade_;
}

void file_manager::release_upgrade() {
	is_upgrade_--;
	loinfo("fts") << "upgrade flag set to:" << is_upgrade_;
}
