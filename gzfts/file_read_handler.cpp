#include "file_read_handler.h"
#include "log.h"

#if _WIN32
#include <windows.h>
#include <Shlwapi.h>

#else
    #if defined _LARGEFILE64_SOURCE
    #undef _LARGEFILE64_SOURCE
    #define _LARGEFILE64_SOURCE 
    #else
    #define _LARGEFILE64_SOURCE 
    #endif
#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/dir.h>
#include <cstring>

#endif

#ifndef _WIN32
int get_time_zone()
{
    time_t t = time(NULL);
    struct tm local = *(localtime(&t));
    struct tm gm = *(gmtime(&t));
	
    int hour = local.tm_hour - gm.tm_hour;
    int zone = 0;
    if(hour > 0) {
        if(local.tm_mday == gm.tm_mday) {
            zone = hour;
		} else {
            zone = hour - 24;
		}
    } else {
        if(local.tm_mday == gm.tm_mday) {
            zone = hour;
        } else {
            zone = hour + 24;
        }
    }
	return zone;
}
#endif

//////////////////////////////// file_read_headler ////////////////////////////////

file_read_headler::file_read_headler(){
#ifndef _WIN32
    time_zone_ = get_time_zone();
#endif
}

file_read_headler::~file_read_headler(){

}


int file_read_headler::get_fts_listdir(const std::string& file_path, std::string & dir, const std::string& prefix){

#ifdef _WIN32
	_finddata_t f_info;
	std::string current_path = file_path + "/*.*"; //也可以用/*来匹配所有  
	int handle = _findfirst(current_path.c_str(), &f_info);
	//返回值为-1则查找失败  
	if (HANDLE_INVALID == handle)
	{
		return -EINVAL;
	}
	do
	{
		//判断是否子目录  
		if (f_info.attrib == _A_SUBDIR)
		{
			//递归遍历子目录  
			if (strcmp(f_info.name, "..") != 0 && strcmp(f_info.name, ".") != 0){
				dir += prefix + '/' + f_info.name;
				char dir_tmp[2] = { '\1', '\0' };
				dir.append(dir_tmp, 2);
				get_fts_listdir(file_path + '/' + f_info.name, dir, prefix + '/' + f_info.name);//再windows下可以用\\转义分隔符，不推荐
			}
		}
		else
		{
			//为快捷方式
			if (strcmp(PathFindExtensionA(f_info.name), ".lnk") == 0){
				std::string des_path;
				std::string src_path = file_path + "/" + f_info.name;
				shell_handler_.find_lnk_folder(src_path.c_str(), des_path);
				if (PathIsDirectoryA(des_path.c_str())){
					//如果快捷方式为目录，则取出该指向目录中所有文件，如果为文件，则直接过滤，不做处理;
					std::string lnk_dir;
					std::string prefix_tmp = prefix + "/" + f_info.name;
					get_fts_listdir(des_path, lnk_dir, prefix_tmp);
					dir.append(lnk_dir);
				}
			}
			//不再取文件列表
			else{
				//为普通文件
				dir += prefix + '/' + f_info.name;
				char dir_tmp[2] = { '\0', '\0' };
				dir.append(dir_tmp, 2);
			}
		}
	} while (!_findnext(handle, &f_info));  //返回0则遍历完  
	//关闭文件句柄  
	_findclose(handle);
	return 0;
#else
	struct dirent *ent;
    DIR *dirptr;
	struct stat st;
    char type_dir[2] = {'\1','\0'};
    char type_file[2] = {'\0','\0'};
    
    dirptr = opendir(file_path.c_str());
    if (!dirptr) {
        return -1;
    }

    while (NULL != (ent = readdir(dirptr))) {
        if (0 == strcmp(ent->d_name, ".") || 0 == strcmp(ent->d_name, "..")) {
            continue;
        }

        char filename[260];
        bzero(filename, sizeof(filename));
        sprintf(filename, "%s/%s", file_path.c_str(), ent->d_name);
		
		if (stat(filename, &st) >= 0) {
			uint32_t mode = st.st_mode;
            // 对不可传输类型进行过滤: 字符设备 接口设备 软链接文件和FIFO的映像文件
			if ((__S_IFCHR & mode) || (S_ISBLK(mode)) || (__S_IFIFO & mode)) {
				continue;
			}
			
			if (mode & __S_IFDIR) {
				dir.append(filename);
				dir.append(type_dir, 2);
                get_fts_listdir(filename, dir);
			} else {
				dir.append(filename);
                dir.append(type_file, 2);
            }
            
		} else {
            loerror("fts") << "\n\t errno:" << errno;
        }
    }

    closedir(dirptr);
    return 0;
#endif
}

void* file_read_headler::read_file_head(const std::string& current_path, uint64_t& file_size, 
	uint64_t& create_time, uint64_t& modify_time, uint64_t& access_time, int &error_code)
{
#ifdef _WIN32
	std::string file_path;
	//此处先判断传入的文件路径是否存在，如果不存在，则回滚上一层查看是否是lnk路径，如果是快捷方式路径，则进去快捷方式文件夹内取数据
	if (!PathFileExistsA(current_path.c_str())){
		shell_handler_.find_lnk_file(current_path, file_path);
	}
	else{
		file_path = current_path;
	}
	void *retptr;
	HANDLE hFile = CreateFileA(file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile) {
		error_code = GetLastError();
		return nullptr;
	}
	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size)) {
		error_code = GetLastError();
		CloseHandle(hFile);
		return nullptr;
	}
	FILETIME c_time, m_time, a_time;
	if (!GetFileTime(hFile, &c_time,&a_time,&m_time)){
		error_code = GetLastError();
		CloseHandle(hFile);
		return nullptr;
	}
	create_time = (create_time = c_time.dwHighDateTime) << 32 | c_time.dwLowDateTime;
	modify_time = (modify_time = m_time.dwHighDateTime) << 32 | m_time.dwLowDateTime;
	access_time = (access_time = a_time.dwHighDateTime) << 32 | a_time.dwLowDateTime;

	file_size = size.QuadPart;
	retptr = (void *)hFile;
	error_code = 0;
	return retptr;
#else
	//LINUX 
    struct stat st;
    if (stat(current_path.c_str(), &st) < 0) {
		loerror("fts") << "failed to get file stat for size";
		error_code = errno;
		return nullptr;
    }
	
	mode_t mode = st.st_mode;
	if ((__S_IFCHR & mode) || (S_ISBLK(mode)) || (__S_IFIFO & mode) || (__S_IFDIR & mode)) {
		error_code = errno;
		return nullptr;
	}
    //O_RDWR 如果有可执行文件会失败
	int fd;
	fd = open(current_path.c_str(), O_RDONLY, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if (fd < 0){
		error_code = errno;
		return nullptr;
	}
	
    //windows 是从16010101000000开始的秒数, 单位为100 ns, linux 是从19700101000000 开始的秒数, 相差 11644444800 s
    //取服务器时区返回给客户端
    create_time = (st.st_ctime + 11644444800 + time_zone_ * 60 * 60) * 10000000L; 
    modify_time = (st.st_mtime + 11644444800 + time_zone_ * 60 * 60) * 10000000L;
    access_time = (st.st_atime + 11644444800 + time_zone_ * 60 * 60) * 10000000L;
    
    file_size = st.st_size;
	error_code = 0;
	return (void *)fd;
#endif
}

int file_read_headler::read_file_id(const void* file_handler, uint64_t & f_id, int &error_code)
{
#ifdef _WIN32
	if (INVALID_HANDLE_VALUE == file_handler){
		return -1;
	}
	_FILE_ID_INFO fsi = { 0 };
	if (!::GetFileInformationByHandleEx((HANDLE)file_handler, FileIdInfo, &fsi, sizeof(_FILE_ID_INFO)))
	{
		error_code = GetLastError();
		return -1;
	}
	FILE_ID_128 id = fsi.FileId;
	memcpy_s(&f_id, sizeof(f_id), id.Identifier, sizeof(f_id));
	return 0;
#else

#endif
}

int file_read_headler::read_file_stream(const void* handler, const uint64_t offset, const uint32_t read_size, std::string& data){

#ifdef _WIN32
	char *segment_buffer = new char[read_size];
	DWORD read_bytes = 0;
	LARGE_INTEGER liDistanceToMove;
	liDistanceToMove.QuadPart = offset;
	if (!SetFilePointerEx((HANDLE)handler, liDistanceToMove, NULL, FILE_BEGIN)){
		if (segment_buffer) {
			delete[] segment_buffer;
			segment_buffer = nullptr;
		}
		//未能成功移动指针
		return -ESPIPE;
	}
	
	if (!ReadFile((HANDLE)handler, segment_buffer, read_size, &read_bytes, NULL)) {
		if (segment_buffer) {
			delete[]segment_buffer;
			segment_buffer = nullptr;
		}
		return -1;
	}
	data.assign(segment_buffer, read_size);
	if (segment_buffer) {
		delete[] segment_buffer;
		segment_buffer = nullptr;
	}
#else
	if (!handler) {
		return -EINVAL;
	}
	
	int fd = (long)handler & 0xffffffff;
	if (fd <= 0) {
		return -EINVAL;
	}
	
	char *pdata = nullptr;
	try {
		pdata = new char[read_size];
	}catch(...) {
		return -ENOMEM;
	}
	
	lseek64(fd, offset, SEEK_SET);
	
	uint64_t rdoff = 0;
	int retval;
	while (rdoff < read_size) {
		retval = read(fd, pdata + rdoff, read_size - rdoff);
		if (retval <= 0) {
			if (errno == EINTR) {
				continue;
			}
			break;
		}
		rdoff += retval;
	}
	
	data.assign(pdata, rdoff);
	delete []pdata;
#endif
	return 0;
}

void file_read_headler::close_handler(const void*file_handler){
#if _WIN32
	if (INVALID_HANDLE_VALUE != file_handler){
		CloseHandle((HANDLE)file_handler);
	}
#else
	int fd = (long)file_handler & 0xffffffff;
	if (fd > 0) {
		close(fd);
	}
#endif
}

int file_read_headler::read_folder_info(const std::string& current_path, uint64_t& create_time, uint64_t& modify_time, uint64_t& access_time,int& error_code){

#ifdef _WIN32
	WIN32_FILE_ATTRIBUTE_DATA wfd;
	if (!GetFileAttributesExA(current_path.c_str(),GetFileExInfoStandard,&wfd)){
		error_code = GetLastError();
		return -1;
	}
	create_time = (create_time = wfd.ftCreationTime.dwHighDateTime) << 32 | wfd.ftCreationTime.dwLowDateTime;
	modify_time = (modify_time = wfd.ftLastWriteTime.dwHighDateTime) << 32 | wfd.ftLastWriteTime.dwLowDateTime;
	access_time = (access_time = wfd.ftLastAccessTime.dwHighDateTime) << 32 | wfd.ftLastAccessTime.dwLowDateTime;
	error_code = 0;
	return 0;
#else
    struct stat st;
    if (stat(current_path.c_str(), &st) < 0) {
		loerror("fts") << "failed to get file stat for size";
		error_code = errno;
		return -1;
    }
	
    //windows 是从16010101000000开始的秒数, 单位为100 ns, linux 是从19700101000000 开始的秒数, 相差 11644444800 s
    //取服务器时区返回给客户端
    create_time = (st.st_ctime + 11644444800 + time_zone_ * 60 * 60) * 10000000L; 
    modify_time = (st.st_mtime + 11644444800 + time_zone_ * 60 * 60) * 10000000L;
    access_time = (st.st_atime + 11644444800 + time_zone_ * 60 * 60) * 10000000L;
    
    error_code = 0;
    return 0;
#endif
}

int file_read_headler::get_fts_current_catalog(const std::string& current_path, std::vector<nsp::file::file_info>& vct_f)
{
#ifdef _WIN32
	//_finddata_t f_info;
	WIN32_FIND_DATAA FindFileData;
	std::string c_path = current_path + "/*.*"; //也可以用/*来匹配所有  
	HANDLE handle = ::FindFirstFileA(c_path.c_str(), &FindFileData);// _findfirst(c_path.c_str(), &f_info);
	//返回值为-1则查找失败  
	if (INVALID_HANDLE_VALUE == handle)
	{
		return -EINVAL;
	}
	do
	{
		nsp::file::file_info info;
		info.file_name_ = FindFileData.cFileName;
		info.file_attri_ = FindFileData.dwFileAttributes;

		//计算crc32校验码,计算文件名称+文件最近修改时间组成的字符串的crc32
		info.file_access_time_ = (info.file_access_time_ = FindFileData.ftLastAccessTime.dwHighDateTime) << 32 | FindFileData.ftLastAccessTime.dwLowDateTime;
		info.file_create_time_ = (info.file_create_time_ = FindFileData.ftCreationTime.dwHighDateTime) << 32 | FindFileData.ftCreationTime.dwLowDateTime;
		info.file_modify_time_ = (info.file_modify_time_ = FindFileData.ftLastWriteTime.dwHighDateTime) << 32 | FindFileData.ftLastWriteTime.dwLowDateTime;
		info.file_size_ = (info.file_size_ = FindFileData.nFileSizeHigh) << 32 | FindFileData.nFileSizeLow;
		char file_crc[MAX_PATH];
		sprintf_s(file_crc, MAX_PATH, "%s%I64u", info.file_name_.c_str(), info.file_modify_time_);
		std::string tmp = file_crc;
		info.file_crc32_ = nsp::toolkit::crc32(info.file_crc32_, (unsigned char*)file_crc, tmp.size());
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (strcmp(PathFindExtensionA(FindFileData.cFileName), ".lnk") == 0))
		{
			if (strcmp(FindFileData.cFileName, "..") != 0 && strcmp(FindFileData.cFileName, ".") != 0){
				vct_f.push_back(info);
			}
		}
		else
		{
			vct_f.push_back(info);
		}

		////判断是否子目录或者为快捷方式
		//if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (strcmp(PathFindExtensionA(FindFileData.cFileName), ".lnk") == 0))
		//{
		//	//递归遍历子目录  
		//	if (strcmp(FindFileData.cFileName, "..") != 0 && strcmp(FindFileData.cFileName, ".") != 0){
		//		info.file_access_time_ = (info.file_access_time_ = FindFileData.ftLastAccessTime.dwHighDateTime) << 32 | FindFileData.ftLastAccessTime.dwLowDateTime;
		//		info.file_create_time_ = (info.file_create_time_ = FindFileData.ftCreationTime.dwHighDateTime) << 32 | FindFileData.ftCreationTime.dwLowDateTime;
		//		info.file_modify_time_ = (info.file_modify_time_ = FindFileData.ftLastWriteTime.dwHighDateTime) << 32 | FindFileData.ftLastWriteTime.dwLowDateTime;
		//		info.file_size_ = (info.file_size_ = FindFileData.nFileSizeHigh) << 32 | FindFileData.nFileSizeLow;
		//		/*std::string input_folder;
		//		if (current_path.at(current_path.size() - 1) != '/')input_folder = current_path + '/' + FindFileData.cFileName;
		//		else input_folder = current_path + FindFileData.cFileName;*/
		//		char file_crc[MAX_PATH];
		//		sprintf_s(file_crc, MAX_PATH, "%s%I64u", info.file_name_.c_str(), info.file_modify_time_);
		//		std::string tmp = file_crc;
		//		info.file_crc32_ = nsp::toolkit::crc32(info.file_crc32_, (unsigned char*)file_crc, tmp.size());//calculate_folder_crc32(input_folder);
		//		vct_f.push_back(info);
		//	}
		//}
		//else
		//{
		//	//为普通文件,计算crc32校验码,计算文件名称+文件最近修改时间组成的字符串的crc32
		//	/*int error_code;
		//	HANDLE file_handler = read_file_head(current_path + "/" + FindFileData.cFileName, info.file_size_,
		//		info.file_create_time_, info.file_modify_time_, info.file_access_time_, error_code);
		//	if (file_handler == nullptr)
		//	{
		//		loerror("fts") << "failed to read file head information:" << FindFileData.cFileName << " ,the error code is " << error_code;
		//		continue;
		//	}
		//	info.file_crc32_ = calculate_crc32(file_handler, info.file_size_, 0x10000);
		//	close_handler(file_handler);*/
		//	info.file_access_time_ = (info.file_access_time_ = FindFileData.ftLastAccessTime.dwHighDateTime) << 32 | FindFileData.ftLastAccessTime.dwLowDateTime;
		//	info.file_create_time_ = (info.file_create_time_ = FindFileData.ftCreationTime.dwHighDateTime) << 32 | FindFileData.ftCreationTime.dwLowDateTime;
		//	info.file_modify_time_ = (info.file_modify_time_ = FindFileData.ftLastWriteTime.dwHighDateTime) << 32 | FindFileData.ftLastWriteTime.dwLowDateTime;
		//	info.file_size_ = (info.file_size_ = FindFileData.nFileSizeHigh) << 32 | FindFileData.nFileSizeLow;
		//	char file_crc[MAX_PATH];
		//	sprintf_s(file_crc, MAX_PATH, "%s%I64u", info.file_name_.c_str(), info.file_modify_time_);
		//	std::string tmp = file_crc;
		//	info.file_crc32_ = nsp::toolkit::crc32(info.file_crc32_, (unsigned char*)file_crc, tmp.size());
		//	vct_f.push_back(info);
		//}
	} while (FindNextFileA(handle, &FindFileData));  //返回0则遍历完  
	//关闭文件句柄  
	FindClose(handle);
	return 0;
#else
	struct dirent *ent;
	DIR *dirptr;
	struct stat st;
	char filename[260];
	int file_handler;

	dirptr = opendir(current_path.c_str());
	if (!dirptr) {
		return -1;
	}

	while (NULL != (ent = readdir(dirptr))) {
		if (0 == strcmp(ent->d_name, ".") || 0 == strcmp(ent->d_name, "..")) {
			continue;
		}

		bzero(filename, sizeof(filename));
		sprintf(filename, "%s/%s", current_path.c_str(), ent->d_name);
		
		if (stat(filename, &st) >= 0) {
			uint32_t mode = st.st_mode;
			// 对不可传输类型进行过滤: 字符设备 接口设备 FIFO的映像文件
			if ((__S_IFCHR & mode) || (S_ISBLK(mode)) || (__S_IFIFO & mode)) {
				continue;
			}
			nsp::file::file_info info;

			info.file_name_ = ent->d_name;
			info.file_size_ = st.st_size;
			//windows 是从16010101000000开始的秒数, 单位为100 ns, linux 是从19700101000000 开始的秒数, 相差 11644444800 s
			info.file_access_time_ = (st.st_atime + 11644444800 + time_zone_ * 60 * 60) * 10000000L;
			info.file_create_time_ = (st.st_ctime + 11644444800 + time_zone_ * 60 * 60) * 10000000L;
			info.file_modify_time_ = (st.st_mtime + 11644444800 + time_zone_ * 60 * 60) * 10000000L;
			
			if (mode & __S_IFDIR) {
				info.file_attri_ = 0;
				info.file_crc32_ = 0;
			} else {
				info.file_attri_ = 1;
				
				char file_crc[MAX_PATH];
				sprintf(file_crc, "%s%llu", info.file_name_.c_str(), info.file_modify_time_);
				std::string tmp = file_crc;
				info.file_crc32_ = nsp::toolkit::crc32(info.file_crc32_, (unsigned char*)file_crc, tmp.size());
				/*file_handler = open(filename, O_RDONLY, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
				if (file_handler < 0){
					loerror("fts") << "\n\t open file:" << filename << ", errno:" << errno;
					continue;
				}
				info.file_crc32_ = calculate_crc32((void*)file_handler, info.file_size_, 0x10000);
				close_handler((void*)file_handler);*/
			}
			vct_f.push_back(info);
			
		} else {
			loerror("fts") << "\n\t errno:" << errno;
		}
	}

	closedir(dirptr);
	return 0;
#endif
}

uint64_t file_read_headler::calculate_crc32(const void* file_handler, uint64_t file_size, uint64_t file_block_size)
{
	uint64_t result = 0;
	uint64_t block_num = file_size / file_block_size;
	block_num = (file_size % file_block_size == 0) ? block_num : (block_num + 1);

	uint32_t crc = 0;
	for (size_t i = 0; i < block_num; i++) {
		uint32_t read_size;
		if (i == (block_num - 1)) {
			//计算最后一片数据大小
			read_size = file_size - i * file_block_size;
		}
		else {
			read_size = file_block_size;
		}
		std::string data;
		if (read_file_stream(file_handler, i*file_block_size, read_size, data) < 0) {
#ifdef _WIN32
			int error = GetLastError();
			loinfo("fts") << "get an error while calculate crc32, the error code is " << error;
#else
			loinfo("fts") << "get an error while calculate crc32, the error code is " << errno;
#endif
			
			return 0;
		}
		crc = nsp::toolkit::crc32(crc, (unsigned char*)data.c_str(), data.size());
	}
	result = crc;
	return result;
}

uint64_t file_read_headler::calculate_folder_crc32(const std::string& folder_path)
{
	uint64_t result = 0;
	std::string list;
	if (get_fts_listdir(folder_path, list, folder_path) < 0)
	{
		loinfo("fts") << "can not get file list :" << folder_path;
		return result;
	}
	size_t pos = list.find('\0');
	std::string file_name_all;
	while (pos != std::string::npos)
	{
		std::string tmp;
		uint64_t create_t, modify_t, access_t;
		int error_code = 0;
		//为文件夹
		if (list.at(pos - 1) == '\1')
		{
			pos++;
		}
		else
		{
			tmp = list.substr(0, pos);
			read_folder_info(list, create_t, modify_t, access_t, error_code);
			pos += 2;
		}
		if (tmp.size() > folder_path.size())//记录文件夹信息，使用该信息进行计算crc32值
		{
			file_name_all += tmp.substr(folder_path.size());
			char modify[32];
			sprintf(modify, "%I64u", modify_t);
			file_name_all += modify;
		}
		list = list.substr(pos);
		pos = list.find('\0');
	}
	uint32_t crc = 0;
	crc = nsp::toolkit::crc32(crc, (unsigned char*)file_name_all.c_str(), file_name_all.size());
	result = crc;
	return result;
}