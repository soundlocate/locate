#ifndef _COMMANDLINEOPTIONS_H
#define _COMMANDLINEOPTIONS_H

#include <string>
#include <argp.h>

#include <cassert>
#include <iostream>
#include <vector>

#include "CSVTable.h"
#include "config.h"
#include "util/types.h"

class CommandLineOptions {
public:
	CommandLineOptions(int argc, char ** argv);

	std::string fftIp() const;

	u16 fftPort() const;

	std::string guiIp() const;

    u16 guiPort() const;

	u16 websocketPort() const;

	f64 accuracy() const;

	f64 clusterSize() const;

	u64 maxKeep() const;

	u64 meanWindow() const;

	f64 keepTime() const;

	u64 log() const;

	char * logfilename() const;

	u64 micCount() const;

	f64 * mics() const;

	u64 dissimilarityFunction() const;
private:

	// documentation of the program
	std::string doc = "soundLocate -- locate objects passively by sound\v"
		"FFTIP\t\tip address of a fft server\n"
		"FFTPORT\t\tthe respective port of the fft server\n"
		"GUIIP\t\tip address of a gui server\n"
		"GUIPORT\t\tthe respective port of the gui server\n"
		"WEBSOCKETPORT\tthe port for the websocket server";

	// documentation of non option arguments
    char args_doc[255] = "FFTIP FFTPORT GUIIP GUIPORT WEBSOCKETPORT";

	// super(pd, "soundLocate", "-d " + dataAlgorithms, "-a " + accuracy, "-c " + maxClusterSize, "-k" + maxKeep, "-m " + meanWindow, logfile != null ? "-l " + logfile : "", "-p " + positionFile)

    //ToDo(robin) : support multiple algorithms set from commandline
	// supported options (no custom options for now)
    struct argp_option options[10] = {
		{ "dataAlgorithms", 'd', "STRING", 0, "only PhaseOnly atm"},
		{ "accuracy"      , 'a', "DOUBLE", 0, "accuracy of numeric solution"},
		{ "clusterSize"   , 'c', "DOUBLE", 0, "max cluster size"},
		{ "keep"          , 'k', "COUNT", 0, "maximum number of positions to keep"},
		{ "meanwindow"    , 'w', "COUNT", 0, "size of averaging windowx"},
		{ "timekeep"      , 't', "DOUBLE", 0, "time to keep old positions"},
		{ "logfile"       , 'l', "FILE", 0, "if set use set logfile to save located positions"},
		{ "positionfile"  , 'p', "FILE", 0, "filename of micfile"},
		{ "dissimilarity" , 'f', "DOUBLE", 0, "which dissimilarity function to use [meanDist, meanDirection]"},
		{ 0, 0, 0, 0, 0, 0 }
	};

	// struct for holding the argument info
	struct arguments
	{
		std::string fftIp;
		u16 fftPort;

		std::string guiIp;
		u16 guiPort;

		u16 websocketPort;

		std::vector<u16> algorithms = {0}; // 0 -> phaseonly, 1 -> phaseandvelocity, 2-> phaseandamplitdue
		u64 dissimilarity = 0; // 0 -> meanDirection, 1 -> meanDist
		f64 accuracy = 0.01;
		f64 clusterSize = 0.1;
		u64 maxKeep = 10;
		u64 meanWindow = 3;
		f64 keepTime = 0.5;

		bool log = false;
		char * logfilename;

		u64 micCount = 8;
		double * mics;

		arguments() {
			mics = new double[micCount * 3];
			double distBetween = 0.42;
			int i = 0;

			mics[i++] = 0;
			mics[i++] = 0;
			mics[i++] = 0;

			mics[i++] = 0;
			mics[i++] = distBetween;
			mics[i++] = 0;

			mics[i++] = distBetween;
			mics[i++] = 0;
			mics[i++] = 0;

			mics[i++] = distBetween;
			mics[i++] = distBetween;
			mics[i++] = 0;

			mics[i++] = 0;
			mics[i++] = 0;
			mics[i++] = distBetween;

			mics[i++] = distBetween;
			mics[i++] = 0;
			mics[i++] = distBetween;

			mics[i++] = 0;
			mics[i++] = distBetween;
			mics[i++] = distBetween;

			mics[i++] = distBetween;
			mics[i++] = distBetween;
			mics[i++] = distBetween;
		}
	};

	// argp parser definiton
	struct argp argp = { options, parse_opt, args_doc, doc.c_str(), 0, 0, 0};

	arguments args;

	// parsing of a single argument
	static error_t parse_opt (int key, char *arg, struct argp_state *state) {
		// get the struct for saving the arguments
		arguments * arguments = (struct arguments *) state->input;

		switch (key)
		{
		case 'f': {
			std::string s(arg);

			if(s == "meanDist") {
				arguments->dissimilarity = 1;
			} else if(s == "meanDirection") {
				arguments->dissimilarity = 0;
			} else {
				std::cerr << "invalid dissimilarity function " << arg << std::endl;
				argp_usage(state);
			}

			break;
		}
		case 'd': {
			std::string s(arg);

			if(s == "PhaseOnly") {
				arguments->algorithms.push_back(0);
			} else if(s == "PhaseAndVelocity") {
				arguments->algorithms.push_back(1);
			} else if(s == "PhaseAndAmplitude") {
				arguments->algorithms.push_back(2);
			} else {
				std::cerr << "invalid locate algorithm " << arg << std::endl;
				argp_usage(state);
			}

			break;
		}
		case 'a': {
			try {
				arguments->accuracy = std::stod(arg);
			} catch(std::invalid_argument) {
				std::cout << "invalid accuracy " << arg << std::endl;
				argp_usage(state);
			}

			break;
		}
		case 'c': {
			try {
				arguments->clusterSize = std::stod(arg);
			} catch(std::invalid_argument) {
				std::cout << "invalid clusterSize " << arg << std::endl;
				argp_usage(state);
			}

			break;
		}
		case 'k': {
			try {
				arguments->maxKeep = std::stod(arg);
			} catch(std::invalid_argument) {
				std::cout << "invalid maxKeep " << arg << std::endl;
				argp_usage(state);
			}

			break;
		}
		case 'w': {
			try {
				arguments->meanWindow = std::stoi(arg);
			} catch(std::invalid_argument) {
				std::cout << "invalid meanWindow " << arg << std::endl;
				argp_usage(state);
			}

			break;
		}
		case 't': {
			try {
				arguments->keepTime = std::stod(arg);
			} catch(std::invalid_argument) {
				std::cout << "invalid keepTime " << arg << std::endl;
				argp_usage(state);
			}

			break;
		}
		case 'l':
			arguments->log = true;
			arguments->logfilename = arg;
			break;
		case 'p': {
			CSVTable t(arg);
			auto rows = t.getRows();
			arguments->micCount = rows.size();
			arguments->mics = new double[rows.size() * 3];
			int i = 0;

			try {
				for(auto row : rows) {
					assert(row.size() == 3);

					arguments->mics[i++] = std::stod(row[0]);
					arguments->mics[i++] = std::stod(row[1]);
					arguments->mics[i++] = std::stod(row[2]);
				}
			} catch(std::invalid_argument) {
				std::cout << "invalid positionfile " << arg << std::endl;
				argp_usage(state);
			}

			break;
		}
		case ARGP_KEY_ARG: /* non option argument */
			// check if more than all needed arguments are passed (and abort if so)
			if (state->arg_num >= 5) {
				argp_usage(state);
			}

			// parse the arguments
			try {
				switch (state->arg_num) {
				case 0: /* fftip */
					arguments->fftIp = std::string(arg);
					break;
				case 1: /* fftport */
					arguments->fftPort = std::stoi(arg);
					break;
				case 2: /* guiop */
					arguments->guiIp = std::string(arg);
					break;
				case 3: /* guiport */
					arguments->guiPort = std::stoi(arg);
					break;
				case 4:
					arguments->websocketPort = std::stoi(arg);
					break;
				}
			} catch(std::invalid_argument) {
				std::cout << "invalid port: " << arg << std::endl;
				argp_usage(state);
			}

			break;

		case ARGP_KEY_END: /* end of arguments */
			// check for too few non option arguments
			if (state->arg_num < 5) {
				argp_usage(state);
			}
			break;

		default:
			return ARGP_ERR_UNKNOWN;
		}
		return 0;
	}
};

#endif
