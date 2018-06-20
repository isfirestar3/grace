/********************************************************************
* Copyright (c) 2015 浙江国自机器人技术有限公司 
* GZRobot All rights reserved. 
*
* 文 件 名: StringTool.hpp
* 创 建 者: zhanghuihui
* 创建日期: 2016:1:22 14:30
* 
* 摘    要: 封装一个字符串常用函数的类
*********************************************************************/
#ifndef STRING_TOOL_ZHH_20160122_H_
#define STRING_TOOL_ZHH_20160122_H_

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

class StringTool
{
public:
    StringTool(){}
public:
    static std::string Trim(std::string strText,const std::string& strTrim = " ")
    {
        if(strText.empty())
        {
            return "";
        }
        strText.erase(0,strText.find_first_not_of(" "));  
        strText.erase(strText.find_last_not_of(" ") + 1);  
        return strText;
    }
    static void SplitString(std::string strText,const std::string& strSplit,std::vector<std::string>& vecStrArray)
    {
        std::string::size_type pos;
        strText += strSplit;//扩展字符串，以便能读取分隔符后面最后一个字段
        int iSize = static_cast<int>(strText.size());
        for(int i = 0;i < iSize; ++i)
        {
            pos = strText.find(strSplit,i);
            if(pos != std::string::npos)
            {
                std::string strTemp = strText.substr(i,pos - i);
                vecStrArray.push_back(Trim(strTemp));
                i = pos + strSplit.size() - 1;
            }
        }
    }
	static char* Ansi2Utf8(const char* chText)
	{
		//先转为Unicode
		int size = MultiByteToWideChar(CP_ACP, 0, chText, -1, NULL, 0);
		wchar_t *strUnicode = new wchar_t[size + 1];
		MultiByteToWideChar(CP_ACP, 0, chText, -1, strUnicode, size);
		strUnicode[size] = 0;

		//Unicode转换成UTF-8;
		int len = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
		char *strUtf8 = new char[len + 1];
		WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUtf8, len, NULL, NULL);
		strUtf8[len] = 0;
		return strUtf8;
	}

	static std::string Wstring2Utf8(std::wstring wstr)
	{
		std::string str(wstr.length() * 3 + 1, ' ');
		size_t len = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), &str[0], str.length(), NULL, NULL);
		str.resize(len);
		return str;
	}

	static std::string String2Utf8(std::string str)
	{
		std::wstring wszStr;
		int nLength = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, NULL);
		wszStr.resize(nLength);

		LPWSTR lpwszStr = new wchar_t[nLength];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, lpwszStr, nLength);
		wszStr = lpwszStr;

		delete[] lpwszStr;

		return Wstring2Utf8(wszStr);
	}
	//测试通过，但是使用此函数必须注意其字节不能超过256，且此函数会多次构造字符对象，影响效率
	static std::string StringFormat(const char* _format, ...)
	{
		char chText[256] = { 0 };

		va_list marker = NULL;
		va_start(marker, _format);
		size_t num_of_chars = _vscprintf(_format, marker);

		if (num_of_chars > 256)
		{
			return "";
		}
		vsprintf_s(chText, 256, _format, marker);
		std::cout << chText << std::endl;

		va_end(marker);
		return std::string(chText);
	}
};

#endif