#include "file_read.h"
#include "log.h"
#include "toolkit.h"
#include <Shlwapi.h>

file_read::file_read(){

}

file_read::~file_read(){

}

HANDLE file_read::read_file_head_info(const std::string& file_path, file_head_info& f_info){
	HANDLE hFile = CreateFileA(file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile) {
		loerror("operation_net") << "can not open file : "<< file_path<<" in local,then can not read file total size or file time.The error code is "<< (int)GetLastError();
		return nullptr;
	}
	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size)) {
		loerror("operation_net") << "can not get file " << file_path << " size.The error code is "<< (int)GetLastError();
		CloseHandle(hFile);
		return nullptr;
	}

	f_info.total_size_ = size.QuadPart;
	f_info.total_block_num_ = f_info.total_size_ / FILE_BLOCK_SIZE;
	f_info.total_block_num_ = (f_info.total_size_ % FILE_BLOCK_SIZE == 0) ? f_info.total_block_num_ : (f_info.total_block_num_ + 1);

	//获取文件时间
	FILETIME lpCreationTime, lpLastAccessTime, lpLastWriteTime;
	if (!GetFileTime(hFile, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime)) {
		loerror("operation_net") << "can not get file " << file_path << " time information.The error code is " << (int)GetLastError();
		CloseHandle(hFile);
		return nullptr;
	}

	//转换为系统时间
	SYSTEMTIME lpSystemTime;
	char date_time[32];
	FileTimeToSystemTime(&lpCreationTime, &lpSystemTime);
	sprintf_s(date_time, "%4d/%02d/%02d %02d:%02d:%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
	f_info.create_time_ = date_time;

	FileTimeToSystemTime(&lpLastAccessTime,& lpSystemTime);
	sprintf_s(date_time, "%4d/%02d/%02d %02d:%02d:%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
	f_info.last_access_time_ = date_time;

	FileTimeToSystemTime(&lpLastWriteTime,&lpSystemTime);
	sprintf_s(date_time, "%4d/%02d/%02d %02d:%02d:%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
	f_info.last_write_time_ = date_time;

	return hFile;
}

int file_read::read_file_stream(const HANDLE file_handler, const uint64_t offset, const uint32_t read_size, std::string& data){
	if (INVALID_HANDLE_VALUE == file_handler) {
		return -1;
	}
	char *segment_buffer = new char[read_size];
	DWORD read_bytes = 0;
	LARGE_INTEGER liDistanceToMove;
	liDistanceToMove.QuadPart = offset;
	if (!SetFilePointerEx(file_handler, liDistanceToMove, NULL, FILE_BEGIN)){
		if (segment_buffer) {
			delete[] segment_buffer;
			segment_buffer = nullptr;
		}
		//未能成功移动指针
		return -ESPIPE;
	}

	if (!ReadFile(file_handler, segment_buffer, read_size, &read_bytes, NULL)) {
		if (segment_buffer) {
			free(segment_buffer);
			segment_buffer = nullptr;
		}
		return -1;
	}

	data.assign(segment_buffer, read_size);
	if (segment_buffer) {
		delete[] segment_buffer;
		segment_buffer = nullptr;
	}
	return 0;
}

void file_read::close_file_handler(const HANDLE file_handler){
	if (INVALID_HANDLE_VALUE != file_handler) {
		CloseHandle(file_handler);
	}
}

int file_read::get_floder_files(const std::string& input_folder, std::vector<std::string>& vct_files){
	_finddata_t f_info;
	std::string current_path = input_folder + "/*.*"; //也可以用/*来匹配所有  
	int handle = _findfirst(current_path.c_str(), &f_info);
	//返回值为-1则查找失败  
	if (HANDLE_INVALID == handle)
	{
		return -1;
	}
	do
	{
		//判断是否子目录  
		if (f_info.attrib == _A_SUBDIR)
		{
			//递归遍历子目录  
			if (strcmp(f_info.name, "..") != 0 && strcmp(f_info.name, ".") != 0){
				vct_files.push_back(input_folder + '/' + f_info.name + '/');
				get_floder_files(input_folder + '/' + f_info.name, vct_files);//再windows下可以用\\转义分隔符，不推荐
			}
		}
		else
		{
			vct_files.push_back(input_folder + '/' + f_info.name);
		}
	} while (!_findnext(handle, &f_info));  //返回0则遍历完
	//关闭文件句柄  
	_findclose(handle);
	return 0;
}

void file_read::file_copy_handler(const std::string& src_file, const std::string& des_file) {
	//如果为路径，直接返回
	if (PathIsDirectoryA(src_file.c_str()))return;
	if (!CopyFileExA(src_file.c_str(), des_file.c_str(), NULL, NULL, NULL, COPY_FILE_OPEN_SOURCE_FOR_WRITE)) {
		int err = GetLastError();
		loerror("operation_net") << "failed to copy file:" << src_file << " to destination:" << des_file << " ,the error code is " << err;
	}
}

int file_read::read_folder_info(const std::string& folder_str, file_head_info& f_info) {
	if (PathIsDirectoryA(folder_str.c_str())) {
		WIN32_FILE_ATTRIBUTE_DATA wfd;
		if (!GetFileAttributesExA(folder_str.c_str(), GetFileExInfoStandard, &wfd)) {
			return -1;
		}
		FILETIME lpCreationTime, lpLastAccessTime, lpLastWriteTime;
		lpCreationTime.dwHighDateTime = wfd.ftCreationTime.dwHighDateTime;
		lpCreationTime.dwLowDateTime = wfd.ftCreationTime.dwLowDateTime;

		lpLastWriteTime.dwHighDateTime = wfd.ftLastWriteTime.dwHighDateTime;
		lpLastWriteTime.dwLowDateTime = wfd.ftLastWriteTime.dwLowDateTime;

		lpLastAccessTime.dwHighDateTime = wfd.ftLastAccessTime.dwHighDateTime;
		lpLastAccessTime.dwLowDateTime = wfd.ftLastAccessTime.dwLowDateTime;

		//转换为系统时间
		SYSTEMTIME lpSystemTime;
		char date_time[32];
		FileTimeToSystemTime(&lpCreationTime, &lpSystemTime);
		sprintf_s(date_time, "%4d/%02d/%02d %02d:%02d:%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
		f_info.create_time_ = date_time;

		FileTimeToSystemTime(&lpLastAccessTime, &lpSystemTime);
		sprintf_s(date_time, "%4d/%02d/%02d %02d:%02d:%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
		f_info.last_access_time_ = date_time;

		FileTimeToSystemTime(&lpLastWriteTime, &lpSystemTime);
		sprintf_s(date_time, "%4d/%02d/%02d %02d:%02d:%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
		f_info.last_write_time_ = date_time;
	}
	return 0;
}

void file_read::cala_crc(const char* input_file, unsigned int& cra) {
	std::string file_name = input_file;
	size_t pos = file_name.find_last_of('/');
	if (pos == std::string::npos || pos == 0 || pos + 1 == 0 || pos + 1 == std::string::npos) {
		return;
	}
	file_name = file_name.substr(pos + 1);
	WIN32_FILE_ATTRIBUTE_DATA wfd;
	if (!GetFileAttributesExA(input_file, GetFileExInfoStandard, &wfd)) {
		return;
	}
	uint64_t modify_time;
	modify_time = (modify_time = wfd.ftLastWriteTime.dwHighDateTime) << 32 | wfd.ftLastWriteTime.dwLowDateTime;
	char modify[MAX_PATH];
	sprintf_s(modify, sizeof(modify), "%s%I64u", file_name.c_str(), modify_time);
	std::string crc_str = modify;
	uint32_t crc = 0;
	crc = nsp::toolkit::crc32(crc, (unsigned char*)crc_str.c_str(), crc_str.size());
	cra = crc;

}