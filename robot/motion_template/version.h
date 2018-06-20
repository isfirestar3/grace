#ifndef __VERSION_H__
#define __VERSION_H__

#if defined ARM_LINUX
#undef ARM_LINUX
#endif

#if _WIN32
#define ARM_LINUX	(0)
#else
#define ARM_LINUX       (1)
#endif

#endif