#if !defined AGV_SHELL_PROTO_HPP
#define AGV_SHELL_PROTO_HPP

#include "proto_definion.h"
#include <memory>

namespace nsp {
	namespace proto {

		enum errorno_t :int32_t {
			kSuccessful = 0,

			kUnsuccessful = -100,
			kFailCreate,                    // 创建网络对象失败
			kFailConnected,                 // 连接服务器失败
			kInvalidParameter,				// 参数错误
			kInsufficientResource,			// 资源不足
			kFatalSyscall,					// 系统错误
			kLogicError,					// 逻辑错误
			kSecurityFatal,					// 安全校验错误
			kRequestTimeout,				// 请求超时
			kRuntimeError,					// C/C++运行时错误
			kObjectError,                   // 对象错误
			kItemNoFound,					// 没有找到对象
			kNoConnected,					// 没有TCP连接
			kInvalidSzie,					// 无效的长度
			kBuildError,                    // 解包失败
			kEndpointExist,					//endpoint对象已经存在
			kFailDisconnect,				//网络断链
			kItemExist						//对象已经存在
		};
	}
}


#endif

