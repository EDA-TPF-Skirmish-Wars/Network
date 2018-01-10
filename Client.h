#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#define MY_ERROR -1
#define MY_EMPTY 0
class Client
{
public:
	Client();
	~Client();
	bool ConnectToServer(const char* ipServer="localhost", const char* portNumber="12345");
	bool sendData(char * dataToSend_t, unsigned int sizeData);
	size_t receiveDataFromServer(char * buffer_t, int bufferSize);
	size_t NBReceiveDataFromServer(char * buffer_t, int bufferSize);
protected:
	boost::asio::ip::tcp::resolver* clientResolver;
	boost::asio::ip::tcp::socket* clientSocket;
	boost::asio::ip::tcp::resolver::iterator endpoint;
	boost::asio::io_service* ioClient;
};

