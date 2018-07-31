#include "cmd_parse.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int start_process(const std::string& file_name, const std::string& cmd_line)
{
#ifdef _WIN32
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;//指定wShowWindow成员有效
	si.wShowWindow = TRUE;            //此成员设为TRUE的话则显示新建进程的主窗口
	BOOL bRet = CreateProcessA(
		file_name.c_str(),			//不在此指定可执行文件的文件名
		(LPTSTR)cmd_line.c_str(),	//命令行参数
		NULL,						//默认进程安全性
		NULL,						//默认进程安全性
		FALSE,						//指定当前进程内句柄不可以被子进程继承
		CREATE_NEW_CONSOLE,			//为新进程创建一个新的控制台窗口
		NULL,						//使用本进程的环境变量
		NULL,						//使用本进程的驱动器和目录
		&si,
		&pi);
	if (bRet)
	{
		//关闭句柄
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		printf("the new process id is :%d\n", pi.dwProcessId);
		printf("the main thread id of new process is：%d\n", pi.dwThreadId);
		return 0;
	}
	return -1;
#else
	return 0;
#endif
}

/***********************************************************************************
* 输入command命令行需遵循以下规则：
* [命令主体] [命令主体参数] [-option短选项] [短选项参数] [--option长选项] [长选项参数]
* eg:copy	 d:\1.txt e:\2.txt	-c						--normal		
* 命令主体：必须为单词或者单个字母，不允许出现特殊字符，数字；
* 短选项、长选项：必须以'-'开头，只能为字母，不允许存在特殊字符，数字；
* 短选项、长选项：需进行去重处理，不允许存在相同的选项，如找到相同的选项，则丢弃当前选项；
* 参数：命令主体参数与option选项参数不需要进行去重，不进行去除特殊字符、数字处理
* 参数：跟随前一个option，如果前option已经存在，则当前参数被舍弃
************************************************************************************/
int prepareCmdLineArgs(const char *command, parsed_command &output)
{
	if (NULL == command)
	{
		return -1;
	}
	std::vector<std::string> rec;
	std::string com_str = command;
	cmd_parse::split(com_str, " ", rec);
	int opt_start_index = 0;
	if (rec.size() == 0)// || ! cmd_parse::is_command_valid(rec[0]))//rec[0].at(0) == '-')
	{
		std::cout << "Missing command main." << std::endl;
		return -1;
	}
	output.__command = rec[0];//命令主体；
	opt_start_index = cmd_parse::is_cmmand_parament(rec, output);//判断命令主体之后是否存在命令参数

	//读取opt命令
	for (auto opt_index = opt_start_index; opt_index < rec.size();){
		std::string opt_string = rec[opt_index++];
		//第一层检查
		if ('-' == opt_string.at(0) ){
			bool bSuccess = false;
			if (cmd_parse::option_handler(opt_string,output)){
				bSuccess = true;
			}
			//未能向output里插入option，做参数舍弃 eg: -abbddcc 123 234 -ef --norma2l wer
			//cc选项重复，其后面的123，234参数舍弃，norma2l选项不合法，其后面参数舍弃
			while (opt_index < rec.size() && rec[opt_index].at(0) != '-'){
				if (bSuccess){
					if (output.__all_opt.size() > 0){
						output.__all_opt[output.__all_opt.size()- 1].__parameter_of_command.push_back(rec[opt_index]);
					}
				}
				opt_index++;
			}
		}
	}
	return 0;
}

//判断command命令主体是否有效,也可用于long option长选项有效性检查
bool cmd_parse::is_command_valid(const std::string& command_str)
{
	for (size_t i = 0; i < command_str.size(); i++)
	{
		char temp = command_str[i];
		if ((temp >= 'A' && temp <= 'Z') ||
			(temp >= 'a' && temp <= 'z'))
		{
		}
		else
		{
			return false;
		}
	}
	return true;
}

//判断long option是否已经存在，true=已经存在，false=不存在
bool cmd_parse::is_long_opt_exist(const std::string& opt_str, const parsed_command &cmd_par)
{
	bool result = false;
	for (size_t i = 0; i < cmd_par.__all_opt.size(); i++)
	{
		if (opt_str == cmd_par.__all_opt[i].__opt_long_name)
		{
			result = true;
			break;
		}
	}
	return result;
}

//判断short option是否已经存在，true=已经存在，false=不存在
bool cmd_parse::is_short_opt_exist(const char* opt_short, const parsed_command &cmd_par)
{
	if (NULL == opt_short)
		return false;
	bool result = false;
	for (size_t i = 0; i < cmd_par.__all_opt.size(); i++)
	{
		if (*opt_short == cmd_par.__all_opt[i].__opt_short_name)
		{
			result = true;
			break;
		}
	}
	return result;
}

//处理option选项
bool cmd_parse::option_handler(const std::string& option_str, parsed_command &output)
{
	if (option_str.size() < 2)//option字符串长度必须大于等于2
		return false;
	//第二层检查
	if (option_str.at(1) == '-')
	{
		return long_opt_handler(option_str, output);
	}
	else
	{
		return short_opt_handler(option_str, output);
	}
}

bool cmd_parse::long_opt_handler(const std::string& option_str, parsed_command &output)
{
	//长选项 long option
	if (option_str.size() < 3) //长选项字符串长度必须大于等于3才合法
	{
		return false;
	}
	std::string temp = option_str.substr(2, option_str.size() - 2);
	//是否为单词检查
	if (!is_command_valid(temp))
	{
		return false;
	}
	//去重性检查
	if (!is_long_opt_exist(temp, output))
	{
		opt opt_struct;
		opt_struct.__opt_long_name = temp;
		output.__all_opt.push_back(opt_struct);
		return true;
	}
	else
	{
		return false;
	}
}

bool cmd_parse::short_opt_handler(const std::string& option_str, parsed_command &output)
{
	bool result = false;
	//短选项 short option
	for (size_t opt_index = 1; opt_index < option_str.size(); opt_index++)
	{
		char opt_short = option_str.at(opt_index);
		if ((opt_short >= 'A' && opt_short <= 'Z') ||
			(opt_short >= 'a' && opt_short <= 'z'))
		{
			//去重性检查
			if (!is_short_opt_exist(&opt_short, output))
			{
				opt opt_struct;
				opt_struct.__opt_short_name = opt_short;
				output.__all_opt.push_back(opt_struct);
				result = true;
			}
			else
			{
				result = false;
			}
		}
	}
	return result;
}

int cmd_parse::is_cmmand_parament(const std::vector<std::string>& vec, parsed_command &output)
{
	int index = 1;
	for (size_t i = 1; i < vec.size(); i++)
	{
		if (vec[i].at(0) != '-')
		{
			output.__parameter_of_command.push_back(vec[i]); //往结构体中添加命令主体参数
			index = i + 1;
		}
		else
		{
			break;
		}
	}
	return index;
}

bool cmd_parse::split(std::string& src, const char* separator, std::vector<std::string>& dest)
{
	if (NULL == separator)
		return false;
	//先去除左右空格
	std::string str_temp = trim_left(trim_right(src));
	char last_char = str_temp.at(str_temp.length() - 1);
	if (last_char != *separator)
	{
		str_temp += separator;
	}

	std::string::size_type index = str_temp.find_first_of(separator, 0);
	size_t start = 0;
	while (index != str_temp.npos)
	{
		std::string temp = str_temp.substr(start, index - start);
		start = index + 1;
		index = str_temp.find_first_of(separator, start);
		if (temp.size() != 0)
		{
			dest.push_back(temp);
		}
	}
	return true;
}