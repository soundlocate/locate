#ifndef _COMMANDLINEOPTIONS_H
#define _COMMANDLINEOPTIONS_H

#include <string>
#include <argp.h>

#include <iostream>

#include "config.h"
#include "util/types.h"

class CommandLineOptions {
public:
	CommandLineOptions(int argc, char ** argv) {
		argp_parse(&argp, argc, argv, 0, 0, &args);
	}

	std::string fftIp() {
		return args.fftIp;
	}

    u16 fftPort() {
		return args.fftPort;
	}

	std::string guiIp() {
		return args.guiIp;
	}

    u16 guiPort() {
		return args.guiPort;
	}

	u16 websocketPort() {
		return args.websocketPort;
	}
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

	// supported options (no custom options for now)
    struct argp_option options[2] = {
		{ 0 }
	};

	// struct for holding the argument info
	struct arguments
	{
		std::string fftIp;
		u16 fftPort;

		std::string guiIp;
		u16 guiPort;

		u16 websocketPort;
	};

	// parsing of a single argument
	static error_t parse_opt (int key, char *arg, struct argp_state *state) {

		// get the struct for saving the arguments
		arguments * arguments = (struct arguments *) state->input;

		switch (key)
		{
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

	// argp parser definiton
	struct argp argp = { options, parse_opt, args_doc, doc.c_str() };

	arguments args;
};

#endif
