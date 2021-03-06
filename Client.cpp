#include "Client.h"

Client::Client()
{
	this->ioClient = new boost::asio::io_service();
	this->clientSocket = new boost::asio::ip::tcp::socket(*ioClient);
	this->clientResolver = new boost::asio::ip::tcp::resolver(*ioClient);
}

Client::~Client()
{
	delete ioClient;
	delete clientSocket;
	delete clientResolver;
}

bool Client::ConnectToServer(const char * ipServer, const char * portNumber)
{	
	bool connected = false;
	try 
	{
		endpoint = this->clientResolver->resolve(boost::asio::ip::tcp::resolver::query(ipServer, portNumber));
		boost::asio::connect(*clientSocket, endpoint);
		connected = true;
	}
	catch (std::exception a)
	{
		//std::cout << "Error al conectarse con el servidor, recuerde que este debe estar escuchando el puerto" << std::endl;
		//exit(4);
	}
	this->clientSocket->non_blocking(true);	
	return connected;
}
 
bool Client::sendData(char * dataToSend_t, unsigned int sizeData)
{
	boost::system::error_code error;
	this->clientSocket->write_some(boost::asio::buffer(dataToSend_t, sizeData), error);
	if ((error.value() == WSAEWOULDBLOCK) || error)
		return false;
	else
		return true;
}

size_t Client::receiveDataFromServer(char * buffer_t, int bufferSize)
{
	size_t messageLenght = 0;
	boost::system::error_code error;
	char bufferTemp[900];
	do
	{
		messageLenght = this->clientSocket->read_some(boost::asio::buffer(bufferTemp, 900), error);
	}while (error.value() == WSAEWOULDBLOCK);
	if (error)
		messageLenght = MY_ERROR;
	else if (messageLenght <= bufferSize)//evaluo si entra en lo que me mandaron
	{
		for (size_t i = 0; i < messageLenght; i++)
			buffer_t[i] = bufferTemp[i];
	}
	else
		messageLenght = MY_ERROR;
	return messageLenght;
}

size_t Client::NBReceiveDataFromServer(char * buffer_t, int bufferSize)
{

	size_t messageLenght = 0;
	boost::system::error_code error;
	char bufferTemp[900];
	messageLenght = this->clientSocket->read_some(boost::asio::buffer(bufferTemp, 900), error);

	if (error.value() == WSAEWOULDBLOCK)//si no se leyo nada devuelvo MY_EMPTY
		messageLenght = MY_EMPTY;
	else if (error)
		messageLenght = MY_ERROR;
	else if (messageLenght != 0)//si se recivio mensaje
	{
		if (messageLenght <= bufferSize)//evaluo si entra en lo que me mandaron
		{
			for (size_t i = 0; i <messageLenght; i++)
				buffer_t[i] = bufferTemp[i];
		}
		else
			messageLenght = MY_ERROR;
	}
	return messageLenght;
}
