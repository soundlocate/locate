#ifndef _LOGGER_H
#define _LOGGER_H

#include <iostream>

#include <cstdio>
#include <cassert>
#include <ctime>

#include "util/types.h"

class Logger {
public:
	Logger(const char * filename);

	Logger(FILE * file);

	int log(double freq, v3 pos);

	~Logger();

private:
	FILE * file;
};

#endif
