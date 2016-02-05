#include <iostream>
#include "FFTStream.h"

int main(int argc, char ** argv) {
	FFTStream stream(argv[1], std::atoi(argv[2]));

	for(auto packet : stream) {
		std::cout << "freq: " << packet.sines[0].freq << std::endl;
		std::cout << "amplitude: " << packet.sines[0].amplitude << std::endl;
		std::cout << "phase: " << packet.sines[0].phase << std::endl;
	}
}
