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
