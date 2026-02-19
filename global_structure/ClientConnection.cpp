#include "ClientConnection.hpp"

ClientConnection::ClientConnection()
	:	fd(0),
		readBuffer(""),
		writeBuffer(""),
		state(DONE)
{}

ClientConnection::ClientConnection(const ClientConnection& other)
{
	fd = other.fd;
	readBuffer = other.readBuffer;
	writeBuffer = other.writeBuffer;
	request = other.request;
	response = other.response;
	state = other.state;
}

ClientConnection& ClientConnection::operator=(const ClientConnection& other)
{
	if (this != &other)
	{
		fd = other.fd;
		readBuffer = other.readBuffer;
		writeBuffer = other.writeBuffer;
		request = other.request;
		response = other.response;
		state = other.state;
	}
	return *this;
}

ClientConnection::~ClientConnection() {}
