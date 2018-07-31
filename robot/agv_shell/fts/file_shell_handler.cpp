#include "file_shell_handler.h"
#include "const.h"

#if _WIN32
#include <windows.h>
#include <shlobj.h>
#include <Shlwapi.h>
#endif

file_shell_handler::file_shell_handler(){

}

file_shell_handler::~file_shell_handler(){

}

int file_shell_handler::find_lnk_folder(const std::string& input_folder, std::string& output_folder){

	int blret = -1;
#ifdef _WIN32
	::CoInitialize(NULL);//初始化COM接口
	IShellLinkA *psl = NULL;
	//创建COM接口，IShellLink对象
	HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (LPVOID *)&psl);
	if (SUCCEEDED(hr)){
		//从IShellLink中获取IPersistFile对象
		IPersistFile *ppf;
		hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
		if (SUCCEEDED(hr)){
			WCHAR wsz[MAX_DIR_PATH_LEN] = { 0 };
			MultiByteToWideChar(CP_ACP, 0, input_folder.c_str(), -1, wsz, MAX_DIR_PATH_LEN); //转下宽字节
			hr = ppf->Load(wsz, STGM_READ);

			if (SUCCEEDED(hr))
			{
				WIN32_FIND_DATAA wfd;
				char shell_path[MAX_DIR_PATH_LEN];
				psl->GetPath(shell_path, MAX_DIR_PATH_LEN, &wfd, SLGP_SHORTPATH);
				output_folder = shell_path;
				//获取目标路径
				blret = 0;

			}
			ppf->Release();
		}
		psl->Release();
	}
	::CoUninitialize(); //释放COM接口
#endif
	return blret;
}

int file_shell_handler::find_lnk_file(const std::string& input_file, std::string& output_file){

#ifdef _WIN32

	size_t pos = input_file.find(".lnk");
	if (pos == 0 || pos == std::string::npos) return 0;

	std::string prefix_path = input_file.substr(0, pos) + ".lnk";
	std::string file_name = input_file.substr(pos + 4);//挪至 .lnk  后截取
	std::string output_path;
	if (find_lnk_folder(prefix_path.c_str(), output_path)){
		output_file = output_path + file_name;
		//如果遇到lnk快捷方式文件夹内有快捷方式，则迭代调用，直至查找出最后结果
		return PathFileExistsA(output_file.c_str()) ?  0 :  find_lnk_file(output_file, output_file);
	}
#endif
	return -1;

}