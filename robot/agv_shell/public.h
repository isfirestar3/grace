#ifndef __AGV_SHELL_PUBLIC_H__
#define __AGV_SHELL_PUBLIC_H__

#include <string>
#include <vector>
#include <functional>

std::string get_process_name(std::string& path);
std::string get_process_path(std::string& path);

std::size_t split_symbol_string(const std::basic_string<char> &source, const char symbol, std::vector<std::basic_string<char>> &vct_substr);

std::string convert_positive(const std::string& str, const char preview_cr, const  char new_cr);

#ifndef _WIN32
//return @compressed_file_name 
int compress_files(const std::vector<std::string>& files, std::string& compressed_file_name);
#endif
//get system zone
int get_time_zone();

int reboot_os();
int shutdown_os();


#endif //__AGV_SHELL_PUBLIC_H__
