#pragma once

#include "var.h"
#ifdef _USRDLL
#define METH dllexport
#else
#define METH dllimport
#endif
#include <stdint.h>

#ifndef EXP
#if _WIN32
#define EXP(type)  extern "C" _declspec(METH) type __stdcall
#else
#define EXP(type)  extern "C" type
#endif
#endif


EXP(int) netdev_create(var__functional_object_t *object);
EXP(int) netdev_uinit();






