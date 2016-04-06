#include "FFTStream.h"

FFTStream::FFTStream(const char * ip, unsigned short port) {
	assert(std::strlen(ip) > 0);
	assert(port != 0);

	server = new sf::TcpSocket;
	server->setBlocking(true);

	std::cout << ip << std::endl;
	std::cout << port << std::endl;

	// Connect to the server
	while(server->connect(sf::IpAddress(ip), port) != sf::Socket::Done) {
		//NOOP, lets just wait until the server comes up
	}

	status = connected;

	// Connected, now lets get the mic count
	std::size_t received_size;
	server->receive(&numMic, sizeof(unsigned int), received_size);
	assert(received_size == sizeof(unsigned int));

	++(*this);
}

FFTStream::FFTStream(FFTSteamStatus newStatus) : currentPacket(), status(newStatus) {}

unsigned int FFTStream::numMics() {
	return numMic;
};

FFTStream& FFTStream::operator++() {
	std::size_t received_size;
	std::size_t to_receive_size = numMic * 3 * sizeof(double);

	// allocate buffer
	char * rawData = new char[to_receive_size];

	// receive one packet
	if(server->receive(rawData, to_receive_size, received_size) == sf::Socket::Disconnected) {
		status = eos; // the server closed
	}
	assert(received_size == to_receive_size);

	// convert raw data to packet
	currentPacket = FFTPacket(rawData, numMic);

	// deallocate buffer
	delete[] rawData;

	return *this;
}

bool FFTStream::operator==(const FFTStream& rhs) {
	if(rhs.status == status)
		return currentPacket == rhs.currentPacket;
	else
		return rhs.status == status;
}

bool FFTStream::operator!=(const FFTStream& rhs) {
	if(rhs.status == status)
		return currentPacket != rhs.currentPacket;
	else
		return rhs.status != status;
}

FFTPacket& FFTStream::operator*() {
	return currentPacket;
}

FFTStream FFTStream::begin() {
	return *this;
}

FFTStream FFTStream::end() {
	return FFTStream(eos);
}
