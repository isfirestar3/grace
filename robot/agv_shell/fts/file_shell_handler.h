#pragma once

/*****************************************************************
*file name:file_shell_handler.h
*function：本例用于处理windows文件夹中lnk快捷方式，寻找最内层lnk文件真正绝对路径
*author:Yeoman
*create time:20170915 13:46:00
*****************************************************************/

#include <string>

class file_shell_handler{
public:
	file_shell_handler();
	~file_shell_handler();

public:
	/*****************************************************************************
		功能：找出lnk文件夹所指向的目标路径
		@input_folder:源lnk文件夹所在路径;
		@output_folder:目标lnk文件夹真正指向的文件夹
		@return value: true = success find,
					   false = can not find it.
	*****************************************************************************/
	int find_lnk_folder(const std::string& input_folder, std::string& output_folder);

	/****************************************************************************
		功能：找出lnk文件所在的目标文件夹（含文件名）
		@input_file:源lnk文件路径；
		@output_file:目标lnk文件所在的文件夹（含文件名）
		@return value: true = success find,
					   false = can not find it.
	****************************************************************************/
	int find_lnk_file(const std::string& input_file, std::string& output_file);
};