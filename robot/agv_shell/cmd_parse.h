#pragma once 

#include <iostream>
#include <string>
#include <vector>

struct opt
{
	std::string  __opt_long_name;  // 选项名字
	char		 __opt_short_name;	// 选项短名
	std::vector<std::string>  __parameter_of_command;	// 该选项携带的参数
};

struct parsed_command
{
	std::string __command;		// 命令主体
	std::vector<std::string>  __parameter_of_command;	// 命令主体的直接参数
	std::vector<opt>		__all_opt;  //所有选项
};

int prepareCmdLineArgs(const char *command, parsed_command &output); //对外提供的分割函数
int start_process(const std::string& file_name, const std::string& cmd_line);

namespace cmd_parse
{
	//内联函数，用于去除字符串左空格
	inline std::string trim_left(const std::string& source, const std::string& t = " ")
	{
		std::string src = source;
		return src.erase(0, source.find_first_not_of(t));
	}

	//内联函数，用于去除字符串右空格
	inline std::string trim_right(const std::string& source, const std::string& t = " ")
	{
		std::string src = source;
		return src.erase(source.find_last_not_of(t) + 1);
	}

	bool split(std::string& src, const char* separator, std::vector<std::string>& dest);
	int is_cmmand_parament(const std::vector<std::string>& vec, parsed_command &output);
	bool option_handler(const std::string& option_str, parsed_command &output);
	bool is_short_opt_exist(const char* opt_short, const parsed_command &cmd_par);
	bool is_long_opt_exist(const std::string& opt_str, const parsed_command &cmd_par);
	bool is_command_valid(const std::string& command_str);
	bool short_opt_handler(const std::string& option_str, parsed_command &output);
	bool long_opt_handler(const std::string& option_str, parsed_command &output);

}