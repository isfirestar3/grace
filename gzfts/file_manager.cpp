#include "file_manager.h"
#include "log.h"
#include "receiver_manager.h"
#include "sender_manager.h"
#ifdef _WIN32
#include <io.h>
#endif

file_manager::file_manager(){
	initlization();
}

file_manager::~file_manager(){

}

void file_manager::initlization(){
	//创建5个读线程,5个写线程
	for (int i = 0; i < 5; i++){
		std::shared_ptr<file_rw_task_thread> task = std::make_shared<file_rw_task_thread>();
		map_rw_task_.insert(std::make_pair(i, task));
	}
	return;
}

//创建文件
int file_manager::create_files(const uint64_t link, const nsp::file::file_head& f_head, int & error_code, bool is_long_lnk, const uint64_t file_id){
	if (f_head.path_remote.size() <= 0) return 0;

	std::string curr_folder_path;
	file_write_handler write_handler;
	//判断输入参数是否为文件夹参数，如果是则创建文件夹
	if (f_head.path_remote.at(f_head.path_remote.size() - 1) == '/'){
		curr_folder_path = f_head.path_remote;
		if (f_head.path_remote.find(".lnk") != std::string::npos){
			lnk_handler_.find_lnk_file(f_head.path_remote, curr_folder_path);
		}
		return create_directory(curr_folder_path, error_code);
	}

	//判断文件夹路径是否存在，如果不存在，则需要创建断层文件夹
	std::string folder_path = f_head.path_remote.substr(0, f_head.path_remote.find_last_of('/'));
	//判断输入路径中是否有lnk快捷方式符号
	if (folder_path.find(".lnk") != std::string::npos){
		//寻找lnk指向的真正路径
		if (!lnk_handler_.find_lnk_file(folder_path, curr_folder_path)){
			//未能处理lnk文件夹，则使用输入参数作为目标路径
			curr_folder_path = folder_path;
		}
	}
	else{
		curr_folder_path = folder_path;
	}

	int type = nsp::os::is_dir(curr_folder_path);
	
	if (type < 0){
		std::lock_guard<decltype(folder_mutex_)> lock(folder_mutex_);
		curr_folder_path += '/';
		if (recreate_file_path(curr_folder_path) < 0){//nsp::os::mkdir(curr_folder_path) < 0
#ifdef _WIN32
			error_code = GetLastError();
#else
            error_code = errno;
#endif
			loerror("fts") << "failed to create folder: " << curr_folder_path << " the error code is " << error_code;
			return -1;
		}
	}

	void* file_handler = nullptr;
	if ((file_handler = write_handler.create_file(f_head.path_remote, f_head.file_create_time_, f_head.file_modify_time_, f_head.file_access_time_, error_code)) == nullptr){
		loerror("fts") << "failed to create file " << f_head.path_remote << " the error code is " << error_code;
		return -1;
	}
	if (!is_long_lnk)
	{
		push_file_handler(link, file_handler);
	}
	else
	{
		push_fd_handler(link, file_id, file_handler);
	}
	return 0;
}

std::string file_manager::convert_positive(const std::string& str, const char preview_cr, const  char new_cr){
	std::string result = str;
	for (size_t i = 0; i < result.size(); i++){
		if (result.at(i) == preview_cr){
			result.at(i) = new_cr;
		}
	}
	return result;
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
int file_manager::read_files(const uint32_t link, const std::string& file_path, nsp::file::file_info& f_info, int&error_code){
	void* file_handler = nullptr;
	file_read_headler read_handler;
	//如果发现要求读取的是文件夹，则调用读取文件夹信息函数
	if (nsp::os::is_dir(file_path) > 0){
		return read_handler.read_folder_info(file_path, f_info.file_create_time_, f_info.file_modify_time_, f_info.file_access_time_, error_code);
	}
	
	if ((file_handler = read_handler.read_file_head(file_path, f_info.file_size_,
		f_info.file_create_time_, f_info.file_modify_time_, f_info.file_access_time_, error_code)) == nullptr){
		loerror("fts") << "failed read file:" << file_path << " ,the error code is " << error_code;
		return -1;
	}
	push_file_handler(link, file_handler);
	return 0;
}

//长连接情况下读取文件信息，在同一个lnk链接的情况下会有多个文件句柄的情况存在，故此时需要使用文件ID来区分
int file_manager::read_files_long_lnk(const uint32_t link, const std::string& file_path, nsp::file::file_info& f_info, int&error_code)
{
	void* file_handler = nullptr;
	file_read_headler read_handler;
	//如果发现要求读取的是文件夹，则调用读取文件夹信息函数
	if (nsp::os::is_dir(file_path) > 0){
		return read_handler.read_folder_info(file_path, f_info.file_create_time_, f_info.file_modify_time_, f_info.file_access_time_, error_code);
	}

	if ((file_handler = read_handler.read_file_head(file_path, f_info.file_size_,
		f_info.file_create_time_, f_info.file_modify_time_, f_info.file_access_time_, error_code)) == nullptr){
		loerror("fts") << "failed read file:" << file_path << " ,the error code is " << error_code;
		return -1;
	}
	/*if (read_handler.read_file_id(file_handler, f_info.file_id_, error_code) < 0)
	{
		loerror("fts") << "failed to get file ID ";
		return -1;
	}*/
	push_fd_handler(link,f_info.file_id_, file_handler);
	return 0;
}

//读取指定块数据流信息
int file_manager::read_file_block_stream(const uint32_t pkt_id, const uint32_t link, 
	const nsp::file::current_identify identify, const uint64_t offset, const uint32_t read_size){
	void* file_handler;
	{
		std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
		auto iter = map_file_handler_.find(link);
		if (iter == map_file_handler_.end()){
			loerror("fts") << "read_file_block_stream can not find link in the map_file_handler.";
			return -1;
		}
		file_handler = iter->second;
	}

	int index = ((long)file_handler & 0xffffffff) % 5;
	auto iter_thread = map_rw_task_.find(index);
	if (iter_thread == map_rw_task_.end()){
		loerror("fts") << "read_file_block_stream can not find read task in the map_rw_task_.";
		return -1;
	}

	iter_thread->second->add_read_task(file_handler, pkt_id, link, offset, read_size, identify);
	return 0;
}

//长连接读取数据流信息
int file_manager::read_file_block_long_lnk(const uint32_t pkt_id, const uint32_t link, const uint64_t file_id,
	const nsp::file::current_identify identify, const uint64_t offset, const uint32_t read_size)
{
	void* file_handler;
	{
		std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
		auto iter = map_fd_.find(link);
		if (iter == map_fd_.end()){
			loerror("fts") << "read_file_block_long_lnk can not find link in the map_file_identify.";
			return -1;
		}
		auto identify_iter = iter->second.find(file_id);
		if (identify_iter == iter->second.end())
		{
			loerror("fts") << "read_file_block_long_lnk can not find file id in the map_file_identify.";
			return -1;
		}
		file_handler = identify_iter->second;
	}

	int index = ((long)file_handler & 0xffffffff) % 5;
	auto iter_thread = map_rw_task_.find(index);
	if (iter_thread == map_rw_task_.end()){
		loerror("fts") << "read_file_block_stream can not find read task in the map_rw_task_.";
		return -1;
	}

	iter_thread->second->add_read_task(file_handler, pkt_id, link, offset, 
		read_size, identify, true, file_id);
	return 0;
}

//写文件流信息
int file_manager::write_file_block_stream(const uint32_t link, const nsp::file::current_identify identify, 
	const nsp::file::file_block& f_blcok, const bool is_long_lnk){
	void* file_handler;

	if (!is_long_lnk)
	{
		std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
		auto iter = map_file_handler_.find(link);
		if (iter == map_file_handler_.end()){
			loerror("fts") << "write_file_block_stream can not find link in the map_file_handler.";
			return -1;
		}
		file_handler = iter->second;
	}
	else
	{
		std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
		auto iter = map_fd_.find(link);
		if (iter == map_fd_.end())
		{
			loerror("fts") << "write_file_block_stream can not find link in the long link.";
			return -1;
		}
		auto fd_iter = iter->second.find(f_blcok.file_id);
		if (fd_iter == iter->second.end())
		{
			loerror("fts") << "write_file_block_stream can not find file handler in the long link.";
			return -1;
		}
		file_handler = fd_iter->second;
	}

	int index = ((long)file_handler & 0xffffffff) % 5;
	auto iter_thread = map_rw_task_.find(index);
	if (iter_thread == map_rw_task_.end()){
		loerror("fts") << "write_file_block_stream can not find write task in the map_rw_task_.";
		return -1;
	}
	if (!is_long_lnk)
	{
		iter_thread->second->add_write_task(file_handler, link, f_blcok, identify);
	}
	else
	{
		iter_thread->second->add_write_task(file_handler, link, f_blcok, identify, true, f_blcok.file_id);
	}
	return 0;
}

//删除指定文件接口
int file_manager::delete_file(const std::string& file_path){
	std::string handler_path;
	//判断该路径是否为lnk快捷方式
	if (file_path.find(".lnk") != std::string::npos){
		//寻找lnk指向的真正路径
		if (!lnk_handler_.find_lnk_file(file_path, handler_path)){
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

//将文件句柄与link关联管理
void file_manager::push_file_handler(const uint32_t link, void* handler){
	std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
	map_file_handler_.insert(std::make_pair(link, handler));
}

//保存关联的link，file id,文件句柄
void file_manager::push_fd_handler(const uint32_t link, const uint64_t file_id, void* handler)
{
	std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
	auto iter = map_fd_.find(link);
	if (iter == map_fd_.end())
	{
		std::map<uint64_t, void*> tmp;
		tmp.insert(std::make_pair(file_id, handler));
		map_fd_.insert(std::make_pair(link,tmp));
	}
	else
	{
		auto id_iter = iter->second.find(file_id);
		if (id_iter == iter->second.end())
		{
			iter->second.insert(std::make_pair(file_id, handler));
		}
		else
		{
			id_iter->second = handler;
		}
	}
}

//关闭长连接下某个文件句柄
bool file_manager::close_fts_file_handler(const uint32_t link, const uint64_t file_id)
{
	std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
	auto iter = map_fd_.find(link);
	if (iter == map_fd_.end())
	{
		return false;
	}
	auto iter_fd = iter->second.find(file_id);
	if (iter_fd == iter->second.end())
	{
		return false;
	}
	file_write_handler read_handler;
	bool result = read_handler.close_handler(iter_fd->second);
	iter->second.erase(iter_fd);
	return result;
}

//比较源文件大小与目标文件大小是否一致，如果不一致，则删除目标文件
void file_manager::is_file_complete(const std::string& file_path, const uint64_t src_file_size){
	//写文件是否存在且没写完
#ifdef _WIN32
	if (_access(file_path.c_str(), 0) != -1){
#else
	if (access(file_path.c_str(), F_OK) != -1){
#endif
		uint64_t  file_size, c_time, a_time, m_time;
		int error_code;
		file_read_headler read_handler;
		//取已经写了的文件的大小与源文件大小比较
		read_handler.close_handler(read_handler.read_file_head(file_path, file_size, c_time, m_time, a_time, error_code));
		if (src_file_size > file_size){
			loinfo("fts") << "the size of writiing file does not match the source file size.";
			file_write_handler write;
			write.delete_file(file_path);
			loinfo("fts") << "delete the file:" << file_path;
		}
	}
}

//删除读写文件句柄
void file_manager::remove_rw_file_handler(const uint32_t link){
	std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
	auto iter = map_file_handler_.find(link);
	if (iter == map_file_handler_.end()){
		return;
	}
	file_write_handler read_handler;
	read_handler.close_handler(iter->second);
	map_file_handler_.erase(iter);
}

//删除长连接读写文件句柄
void file_manager::remove_fw_long_file_handler(const uint32_t link)
{
	std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
	auto iter = map_fd_.find(link);
	if (iter == map_fd_.end()){
		return;
	}
	file_write_handler read_handler;
	for (const auto& iter_fd : iter->second)
	{
		read_handler.close_handler(iter_fd.second);
	}
	iter->second.clear();
	map_fd_.erase(iter);
}

//删除读写线程中读写队列中数据
void file_manager::clear_rw_task_deque(const uint32_t link){
	void* file_handler;
	{
		std::lock_guard<decltype(rw_mutex_)> lock(rw_mutex_);
		auto iter = map_file_handler_.find(link);
		if (iter == map_file_handler_.end()){
			return;
		}
		file_handler = iter->second;
	}

	int index = ((long)file_handler & 0xffffffff) % 5;
	auto iter_thread = map_rw_task_.find(index);
	if (iter_thread == map_rw_task_.end()){
		return;
	}
	iter_thread->second->clear_rw_deque(link);
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
	file_write_handler w_handler;
	return w_handler.reset_file_time(file_name, create_time, modify_time, access_time);
}