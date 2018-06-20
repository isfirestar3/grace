#pragma once

#include <functional>
#include <memory>
#include "os_util.hpp"

namespace nsp{
	namespace proto {
		enum errorno_t : int32_t
		{
			kSuccessful = 0,

			kUnsuccessful = -100,

			kInvalidParameter,				// 参数错误
			kInsufficientResource,			// 资源不足
			kFatalSyscall,					// 系统错误
			kLogicError,					// 逻辑错误
			kSecurityFatal,					// 安全校验错误
			kRequestTimeout,				// 请求超时
			kRuntimeError,					// C/C++运行时错误
			kObjectError,					// 对象错误
			kItemNoFound,					// 没有找到对象
			kNoConnected,					// 没有TCP连接
			kNoLogin,                       // 未登陆
			kInvalidSzie,					// 无效的长度
			kBuildError,                    // 解包失败
		};
	}
}



namespace motion {
	struct  asio_data
	{
		nsp::proto::errorno_t err_ = nsp::proto::errorno_t::kSuccessful;
	public:
		asio_data() {};
		~asio_data() {};
		asio_data& operator=(const asio_data&lref)
		{
			if (this == &lref) return *this;
			set_err(lref.err_);
			return *this;
		}

		void set_err(nsp::proto::errorno_t err) { err_ = err; };
		int  get_err()const { return err_; };
	};
	struct asio_block
	{
		asio_block() = delete;

		asio_block(const std::function<void(const void*)>&callback, const void*usr = nullptr, const int usr_len = 0){
			callback_function_ = callback;
			usr_ = usr;
			cb_ = usr_len;
			tick_ = ::GetTickCount();
		}

		asio_block(const std::function<void(const std::shared_ptr<asio_block>&)>&callback, int hold_value){
			object_callback_function_ = callback;
			tick_ = ::GetTickCount();
			UNREFERENCED_PARAMETER(hold_value);
		}
	
		~asio_block() {};
		void refresh() { tick_ = nsp::os::gettick(); };
	public:
		std::function<void(const void*)> callback_function_;
		std::function<void(const std::shared_ptr<asio_block>&)> object_callback_function_;

		unsigned char*ack_ = nullptr;
		const void *usr_ = nullptr; // 用户上下文(如果需要存放请求ID,用于执行请求匹配，则在这里附加自己的结构化数据)
		int cb_;
		uint64_t tick_;
		int error_ = -1;
	};
}



