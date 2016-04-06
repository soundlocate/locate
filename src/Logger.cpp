#include "Logger.h"

Logger::Logger(const char * filename) {
	FILE * file;

	if((file = fopen(filename, "w")))
		this->file = file;
	else
		std::cerr << "error opening log file " << filename << std::endl;
}

Logger::Logger(FILE * file) {
	assert(file);

	this->file = file;
}

int Logger::log(double freq, v3 pos) {
	return fprintf(file, "%ld, %lf, %lf, %lf, %lf\n", time(0), freq, pos.x, pos.y, pos.z);
}

Logger::~Logger() {
	fflush(file);
	fclose(file);
}
