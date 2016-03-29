#ifndef _LOGGER_H
#define _LOGGER_H

#include <iostream>

#include <cstdio>
#include <cassert>
#include <ctime>

#include "util/types.h"

class Logger {
public:
	Logger(const char * filename) {
		FILE * file;

		if((file = fopen(filename, "w")))
			this->file = file;
		else
			std::cerr << "error opening log file" << filename << std::endl;
	}

	Logger(FILE * file) {
		assert(file);

		this->file = file;
	}

	int log(double freq, v3 pos) {
		return fprintf(file, "%ld, %lf, %lf, %lf, %lf\n", time(0), freq, pos.x, pos.y, pos.z);
	}

	~Logger() {
		fflush(file);
		fclose(file);
	}

private:
	FILE * file;
};

#endif
