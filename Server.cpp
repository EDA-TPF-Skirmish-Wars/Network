#include "Server.h"

/*Server()
recibe como paramtro un int, que representa el puerto que el srvidor escucha,
en caso que ese puerto no sea indicado, el puerto por defecto es 12345.*/
Server::Server(UINT32 port)
{
	portNumber = port;
	this->ioServer = new boost::asio::io_service();
	this->ServerSocket = new boost::asio::ip::tcp::socket(*ioServer);
	this->conectionServerAceptor = new boost::asio::ip::tcp::acceptor(*ioServer,
	boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), portNumber));
	return;
}

Server::~Server()
{
	conectionServerAceptor->close();
	ServerSocket->close();
	delete conectionServerAceptor;
	delete ServerSocket;
	delete ioServer;
}

void Server::setPortNumber(UINT32 port)
{
	this->portNumber = port;
	return;
}

UINT32 Server::getPortNumber()
{
	return this->portNumber;
}

void Server::waitForClient()
{
	this->conectionServerAceptor->accept(*ServerSocket);
	this->ServerSocket->non_blocking(true);
	return;
}

/*receiveDataFromClient()
previamente se deve llamar a waitForClient()
recibe como parametros un arreglo de char(buffer) y una int,
con la cantidad de elementos de dicho arreglo.
si se pudo recibir toda la info devuelve la cantidad de datos que recibio, si no pudo devuelve un -1*/
size_t Server::receiveDataFromClient(char * buffer_t, int bufferSize)
{
	size_t messageLength = 0;
	boost::system::error_code error;
	char bufferTemp[900];
	do
	{
		messageLength = this->ServerSocket->read_some(boost::asio::buffer(bufferTemp, 900), error);
	} while (error.value() == WSAEWOULDBLOCK);
		
	if (error)
		messageLength = MY_ERROR;
	else if (messageLength <= bufferSize)//evaluo si entra en lo que me mandaron
	{
		for (size_t i = 0; i < messageLength; i++)
			buffer_t[i] = bufferTemp[i];
	}
	else
		messageLength = MY_ERROR;
	return messageLength;
}

/*Previamente se deve llamar a waitforClient()
recibe como parametros un arreglo de char(buffer) y un int,
con la cantidad de elementos de dicho arreglo.
devuelve la longitud del mensaje recibido si recibio, y si no recibio devuelve -1
nota: NO ES BLOQUEANTE*/
size_t Server::NBReceiveDataFromClient(char * buffer_t, int bufferSize)
{
	size_t messageLength = 0;
	boost::system::error_code error;
	char bufferTemp[900];
	messageLength = this->ServerSocket->read_some(boost::asio::buffer(bufferTemp,900),error);
	if (error.value() == WSAEWOULDBLOCK)//si no se leyo nada devuelvo MY_EMPTY
		messageLength = MY_EMPTY;
	else if(error)
		messageLength = MY_ERROR;
	else if (messageLength != 0)//si se recibio mensaje
	{
		if (messageLength <= bufferSize)//evaluo si entra en lo que me mandaron
		{
			for (size_t i = 0; i < messageLength; i++)
				buffer_t[i] = bufferTemp[i];
		}
		else
			messageLength = MY_ERROR;
	}
	return messageLength;
}



/* sendData()
recibe un arreglo de char, que son lo elementos que mandara.Tambien recibe
un int con la cantidad de elementos que necesita enviar
Devuelve un true si pudo enviar y un false en el caso contrario.*/
bool Server::sendData(char * dataToSend_t, unsigned int sizeData)
{
	boost::system::error_code error;
	this->ServerSocket->write_some(boost::asio::buffer(dataToSend_t, sizeData), error);
	if ((error.value() == WSAEWOULDBLOCK) || error)
		return false;
	else
		return true;
}