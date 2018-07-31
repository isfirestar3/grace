#pragma once

#include <stdio.h>
#include "getopt.h"
#include <string>
#include <stdint.h>

#define SHELL_PORT 4410
#define FTS_PORT 4411
#define FTS_LONG_PORT 4412

struct parament_run{
	std::string ipv4_;
	uint16_t shell_port_;
	uint16_t fts_port_;
	uint16_t fts_long_port_;
};

/* 不可见短参数索引定义 */
enum opt_invisible_indx {
	kInvisibleOptIndex_GetHelp = 'h',
	kInvisibleOptIndex_GetVersion = 'v',
	kInvisibleOptIndex_ShellPort,
	kInvisibleOptIndex_FtsPort,
	kInvisibleOptIndex_FtsLongPort,
	kInvisibleOptIndex_Default
};

static const struct option long_options[] = {
	{ "help", no_argument, NULL, kInvisibleOptIndex_GetHelp },
	{ "version", no_argument, NULL, kInvisibleOptIndex_GetVersion },
	{ "shell-port", required_argument, NULL, kInvisibleOptIndex_ShellPort },
	{ "fts-port", required_argument, NULL, kInvisibleOptIndex_FtsPort },
	{ "fts-long-port", required_argument, NULL, kInvisibleOptIndex_FtsLongPort },
	{ "default", no_argument, NULL, kInvisibleOptIndex_Default },
	{ NULL, 0, NULL, 0 }
};

int check_argv(int argc, char** argv);