#if !defined AGV_SHELL_PROTO_HPP
#define AGV_SHELL_PROTO_HPP

#include "proto_definion.h"
#include "serialize.hpp"
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

		struct agv_shell_proto_head : public nsp::proto::proto_interface {
			nsp::proto::proto_crt_t<uint8_t> op = 0x0C;
			nsp::proto::proto_crt_t<uint8_t> fn = 'C';
			nsp::proto::proto_crt_t<uint8_t> ln = 'K';
			nsp::proto::proto_crt_t<uint8_t> type;

			agv_shell_proto_head() {}
			agv_shell_proto_head(int ty) {
				type = ty;
			}
			~agv_shell_proto_head() {}

			virtual const int length() const override  {
				return op.length() + fn.length() + ln.length() + type.length();
			}

			static const int type_length()
			{
				return
					decltype(op)::type_length() +
					decltype(fn)::type_length() +
					decltype(ln)::type_length() +
					decltype(type)::type_length();
			}

			virtual unsigned char *serialize(unsigned char *bytes) const override  {
				unsigned char *pos = op.serialize(bytes);
				pos = fn.serialize(pos);
				pos = ln.serialize(pos);
				pos = type.serialize(pos);
				return pos;
			}

			virtual const unsigned char *build(const unsigned char *bytes, int &cb) override  {
				const unsigned char *pos = op.build(bytes, cb);
				pos = fn.build(pos, cb);
				pos = ln.build(pos, cb);
				pos = type.build(pos, cb);
				return pos;
			}
		};
	}
}


#endif

