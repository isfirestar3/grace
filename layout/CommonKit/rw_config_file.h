/********************************************************************
* Copyright (c) 2015 浙江国自机器人技术有限公司 
* GZRobot All rights reserved. 
*
* 文 件 名: RWConfigFile.h
* 创 建 者: zhanghuihui
* 创建日期: 2015:6:12 10:18
* 
* 摘    要: 一个通用的读写配置文件的类
*********************************************************************/
#ifndef RW_CONFIG_FILE_ZHH_20150612_H_
#define RW_CONFIG_FILE_ZHH_20150612_H_

//#include <Singleton.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#define FILE_SIZE 1024*1024
#define FILE_PATH_LEN 256
#define LINE_MAX_STRING_LEN 1024
#define SECTION_NAME_LEN 32
#define KEY_LEN 32
#define VALUE_LEN 1024
#define VALUE_ERROR -65535
#define Min(a,b) (((a) > (b)) ? (b) : (a))
#define SafeDeleteArray(p)  if ((p)) {delete [] (p); (p) = NULL; }

enum ErrorType
{
    NoError,
    CanNotOpenFile,
    FormatError,
    ParamError,
    NotFound,                  //没有找到相关键值
    MemoryError              //内存错误
};
enum LineType
{
    TypeBlank,        //空行或者注释行
    TypeSection,     //区域行
    TypeKey,           //关键字行
    TypeValue,        //如果此行既不是区域、关键字也不是注释，那么认为是上一个关键字后面的键值，只是放在了另一行
    TypeError
};
typedef struct tag_KeyValue //关键字及键值结构体
{
    LineType Keytype;         //标记下关键字这一行是什么类型，也有可能是空行和注释行，以及键值的令一行
    char chInfo[LINE_MAX_STRING_LEN];//如果此行不是关键字行，那么用此字段保存此行信息
    char chKey[KEY_LEN];    //键
    char chValue[VALUE_LEN];  //键值
    tag_KeyValue()
    {
        Keytype = TypeKey;
        memset(chInfo,0,sizeof(char)*LINE_MAX_STRING_LEN);
        memset(chKey,0,sizeof(char)*KEY_LEN);
        memset(chValue,0,sizeof(char)*VALUE_LEN);
    }
}KEY_VALUE;

typedef struct tag_SectionInfo //行字符串信息结构体
{
    char chInfo[LINE_MAX_STRING_LEN];  //用此字段保存此行信息，如果是区域行，则记录的是区域名
    std::vector<KEY_VALUE> vecKeyValue;  //用于保存键信息
    tag_SectionInfo()
    {
        memset(chInfo,0,sizeof(char)*LINE_MAX_STRING_LEN);
        vecKeyValue.clear();
    }
}SECTIONINFO;
class CConfigFile
{
private:
    CConfigFile();
	~CConfigFile();
public:
    static CConfigFile* GetInstance()
    {
        static CConfigFile configFile;
        return &configFile;
    }

public:
	
	bool ReadFile(char* chFileName);

    /********************************************************************
    * 函数功能: 获取最近一次出错的错误类型
    * 输入参数: 无
    * 输出参数: 无
    * 返 回 值: 错误类型枚举值
    * 注意事项: 
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:16 19:29
    * ********************************************************************/
    ErrorType CF_GetLastError() const;
	std::string GetValue(const std::string& strSecKeyName,const std::string& strDefVal);
	std::wstring GetValueW(const std::string& strSecKeyName,const std::string& strDefVal);
	std::string GetValueT(const std::string& strSecKeyName,const std::string& strDefVal);
	int GetValueInt(const std::string& strSecKeyName,int nDefVal);
	double GetValueDouble(const std::string& strSecKeyName,double dDefVal);

    /********************************************************************
    * 函数功能: 设置关键字的键值，以下三个函数为重载函数，根据设置的类型来选择使用
    * 输入参数: strSecKeyName:区域加关键，字格式为:“/VideoIp/yaan_ip”,strValue:设置的键值
    * 输出参数: 无
    * 返 回 值: true:设置成功，false:设置失败
    * 注意事项: 
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:8:21 13:30
    * ********************************************************************/
    bool SetValue(const std::string& strSecKeyName,std::string strValue);
    bool SetValue(const std::string& strSecKeyName,int iValue);
    bool SetValue(const std::string& strSecKeyName,double dValue);
    bool WriteFile();//写文件接口由外部调用，不再修改完参数后立即内部调用写入

private:  //public:
    bool BuildString(char*& pStrBuf);

    /********************************************************************
    * 函数功能: 根据关键字获取键值的字符串信息，关键字需要严格按照如下格式:“/VideoIp/yaan_ip”前面字段为区域名，后面字段为关键字名
    * 输入参数: strSecKeyName:关键字
    * 输出参数: 无
    * 返 回 值: 键值字符串
    * 注意事项: 
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:16 19:30
    * ********************************************************************/
    std::string GetValue(const std::string& strSecKeyName);

    /********************************************************************
    * 函数功能: 根据关键字获取键值的字符串(宽字符)，由于键值可能有中文，而文件以UTF-8编码，需要转换成宽字符UTF8避免乱码
    * 输入参数: strSecKeyName:关键字
    * 输出参数: 无
    * 返 回 值: 键值字符串(宽字符)
    * 注意事项: 如果键值中含有中文，且返回值希望获取wstring则使用此接口获取键值
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:16 19:30
    * ********************************************************************/
    std::wstring GetValueW(const std::string& strSecKeyName);

    /********************************************************************
    * 函数功能: 根据关键字获取键值的字符串(多字节)，将char字符转换成UTF-8编码，再转到ANSI格式，可以避免乱码
    * 输入参数: strSecKeyName:关键字
    * 输出参数: 无
    * 返 回 值: 键值字符串ANSI多字节
    * 注意事项: 如果键值中含有中文，且返回值希望获取string则使用此接口获取键值
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:17 10:03
    * ********************************************************************/
    std::string GetValueT(const std::string& strSecKeyName);

    /********************************************************************
    * 函数功能: 根据关键字获取键值（数字）
    * 输入参数: strSecKeyName:关键字
    * 输出参数: 无
    * 返 回 值: 键值
    * 注意事项: 如果键值为数字，可以直接调用此接口返回int型数字
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:16 19:30
    * ********************************************************************/
    int GetValueInt(const std::string& strSecKeyName);

    /********************************************************************
    * 函数功能: 根据关键字获取键值（double型数字）
    * 输入参数: strSecKeyName:关键字
    * 输出参数: 无
    * 返 回 值: 键值
    * 注意事项: 如果键值为double型数字，可以直接调用此接口返回double型数字
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:16 19:30
    * ********************************************************************/
    double GetValueDouble(const std::string& strSecKeyName);

    void CloseFile();
private:
    /********************************************************************
    * 函数功能: 判断一行字符串是否是注释行
    * 输入参数: chLineStr::传入字符串
    * 输出参数: 无
    * 返 回 值: true:注释行，false:非注释行
    * 注意事项: 
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:12 11:10
    * ********************************************************************/
    bool IsAnnotate(char* chLineStr);

    /********************************************************************
    * 函数功能: 判断是否是空行
    * 输入参数: chLineStr::传入字符串
    * 输出参数: 无
    * 返 回 值: true:空行，false:非空行
    * 注意事项: 
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:12 14:07
    * ********************************************************************/
    bool IsBlankLine(char* chLineStr);

    /********************************************************************
    * 函数功能: 判断一行字符是区域还是键，注释，还是空行类型
    * 输入参数: chLineStr::传入字符串
    * 输出参数: iFrontBlankNum:字符串前面空格个数,iBackBlankNum:字符串结尾空格个数
    * 返 回 值: 行类型
    * 注意事项: 
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:12 14:23
    * ********************************************************************/
    LineType GetLineType(char* chLineStr,int& iFrontBlankNum,int& iBackBlankNum);

    bool GetKeyNameAndValue(int iFrontBlankNum,int iBackBlankNum,char* chText,char*& chName,char*& chValue);

    /********************************************************************
    * 函数功能: 当文件时UTF-8带BOM头编码格式的时候，文件的开头会带EF BB BF三个字节的标志，导致乱码，此函数用于去除BOM头
    * 输入参数: chLineStr:文本指针
    * 输出参数: chRemove:修改后的文本指针
    * 返 回 值: true:文本带有BOM头，去除成功，false:文本不带有BOM头
    * 注意事项: 
    * 创 建 者: zhanghuihui
    * 创建日期: 2015:6:15 15:48
    * ********************************************************************/
    bool RemoveBomHeader(char* chLineStr,char*& chRemove);

    void AddBlankLine(char* chLineStr);
    void AddSectionLine(char* chLineStr,int iFrontBlankNum,int iBackBlankNum);
    void AddKeyLine(char* chLineStr,int iFrontBlankNum,int iBackBlankNum);
    void AddValueLine(char* chLineStr,int iFrontBlankNum,int iBackBlankNum);

    void AnalyzeFile(char* chLineStr);
    std::string WstringToString(const std::wstring& wstrSrc);
	std::wstring StringToWstring(const std::string& strSrc);


private:
    fstream m_fStreamFile;
    std::vector<SECTIONINFO> m_vecLineStr;
    ErrorType m_errorType;
    char m_chFileName[FILE_PATH_LEN];
};

#define CFGFILE_INSTANCE CConfigFile::GetInstance()
#endif