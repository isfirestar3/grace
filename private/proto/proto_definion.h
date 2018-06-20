#ifndef PROTO_DEFINION_HEAD_20161115
#define PROTO_DEFINION_HEAD_20161115

#define UINT32_LENGTH 4
#define PROTO_HEAD_LENGTH 20
#define TYPE_OFFSET 4

#include<stdint.h>
#include <memory>
#include <stdlib.h>
#include "icom/nisdef.h"

#define DISCOVER_PORT 409
#define TYPE_LENGTH 4
#define COMMON_REQUEST_ELEMENT_LENGTH 12
#define INVALID_PACK_TYPE		((uint32_t)~0)


#ifdef _MSC_VER // for MSVC
#define forceinline __forceinline
#elif defined __GNUC__ // for gcc on Linux/Apple OS X
#define forceinline __inline__ __attribute__((always_inline))
#else
#define forceinline
#endif

typedef struct _HEADER
{
	uint32_t  start_head_;
	uint32_t  cb_;
}head_t;


int STD_CALL proto__base_parser(void *dat, int cb, int *pkt_cb);


int STD_CALL proto__base_builder(void *dat, int cb);


int STD_CALL proto__base_length();



#ifdef __cplusplus
#include "serialize.hpp"
#endif

namespace nsp {
	namespace proto {
        #pragma pack(push, 1)
		// –≠“ÈÕ∑
		struct proto_head : public nsp::proto::proto_interface
		{
			nsp::proto::proto_crt_t<unsigned int> id_ = 0;
			nsp::proto::proto_crt_t<unsigned int> type_ = 0;
			nsp::proto::proto_crt_t<unsigned int> size_ = 0;
			nsp::proto::proto_crt_t<unsigned int> err_ = 0;
			nsp::proto::proto_crt_t<unsigned int> fd_ = 0;
			nsp::proto::proto_crt_t<unsigned int> token_ = 0;

			proto_head(uint32_t type, uint32_t id = 0, uint32_t size = 0, uint32_t token = 0, uint32_t err = 0, uint32_t fd = 0)
			{
				id_ = id;
				type_ = type;
				size_ = size;
				err_ = err;
				fd_ = fd;
				token_ = token;
			}

			proto_head(const proto_head  &lref)
			{
				if (&lref != this) {
					id_ = lref.id_;
					type_ = lref.type_;
					err_ = lref.err_;
					token_ = lref.token_;
					fd_ = lref.fd_;
					size_ = lref.size_;
				}
			}

			proto_head(){}


			static const int type_length()
			{
				return
					decltype(id_)::type_length() +
					decltype(type_)::type_length() +
					decltype(err_)::type_length() +
					decltype(token_)::type_length() +
					decltype(fd_)::type_length() +
					decltype(size_)::type_length();
			}

			virtual const int length() const override
			{
				return id_.length() +
					type_.length() +
					err_.length() +
					token_.length() +
					fd_.length() +
					size_.length();
			}

			const int head_checksum() const
			{
				return id_ + type_ + token_;
			}

			virtual unsigned char *serialize(unsigned char *bytes) const override
			{
				unsigned char *pos = id_.serialize(bytes);
				pos = type_.serialize(pos);
				pos = size_.serialize(pos);
				pos = err_.serialize(pos);
				pos = token_.serialize(pos);
				pos = fd_.serialize(pos);

				return pos;
			}
			virtual const unsigned char *build(const unsigned char *bytes, int &cb) override
			{
				const unsigned char *pos = id_.build(bytes, cb);
				pos = type_.build(pos, cb);
				pos = size_.build(pos, cb);
				pos = err_.build(pos, cb);
				pos = token_.build(pos, cb);
				pos = fd_.build(pos, cb);
				return pos;
			}
		};
        #pragma pack(pop)


		template<class T>
		std::shared_ptr<T> shared_for(const void *data, uint32_t cb)
		{
			if (nullptr == data || cb <= 0) {
				return std::shared_ptr<T>(nullptr);
			}
			const unsigned char *pos = (const unsigned char *)data;

			try {
				std::shared_ptr<T> s_ptr = std::make_shared<T>();
				if (!s_ptr->build(pos, (int&)cb)) {
					return std::shared_ptr<T>(nullptr);
				}
				return s_ptr;
			}
			catch (std::bad_alloc &) {
				return std::shared_ptr<T>(nullptr);
			}
		}

	}
}

#endif