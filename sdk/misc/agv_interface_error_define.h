/*!
 * file agv_interface_error_define.h
 * date 2017/08/14 16:07
 *
 * author chenkai
 *
 * brief 
 *
 * note
*/

#ifndef __AGV_INTERFACE_ERROR_DEFINE_H__
#define __AGV_INTERFACE_ERROR_DEFINE_H__

enum AgvInterfaceError
{
    AgvInterfaceError_PathSearch = -1000,
    AgvInterfaceError_SendNav,
    AgvInterfaceError_SendOpt,
    AgvInterfaceError_OK = 0,
};


#endif