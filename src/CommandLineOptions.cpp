#include "CommandLineOptions.h"

// global vars for argp:
const char *argp_program_version = APPLICATION_VERSION_STRING;
const char *argp_program_bug_address = APPLICATION_VENDOR_EMAIL;

CommandLineOptions::CommandLineOptions(int argc, char ** argv) {
	argp_parse(&argp, argc, argv, 0, 0, &args);
}

std::string CommandLineOptions::fftIp() const {
	return args.fftIp;
}

u16 CommandLineOptions::fftPort() const {
	return args.fftPort;
}

std::string CommandLineOptions::guiIp() const {
	return args.guiIp;
}

u16 CommandLineOptions::guiPort() const {
	return args.guiPort;
}

u16 CommandLineOptions::websocketPort() const {
	return args.websocketPort;
}

f64 CommandLineOptions::accuracy() const {
	return args.accuracy;
}

f64 CommandLineOptions::clusterSize() const {
	return args.clusterSize;
}

u64 CommandLineOptions::maxKeep() const {
	return args.maxKeep;
}

u64 CommandLineOptions::meanWindow() const {
	return args.meanWindow;
}

f64 CommandLineOptions::keepTime() const {
	return args.keepTime;
}

u64 CommandLineOptions::log() const {
	return args.log;
}

char * CommandLineOptions::logfilename() const {
	return args.logfilename;
}

u64 CommandLineOptions::micCount() const {
	return args.micCount;
}

f64 * CommandLineOptions::mics() const {
	return args.mics;
}

u64 CommandLineOptions::dissimilarityFunction() const {
	return args.dissimilarity;
}
