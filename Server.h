#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <cstdint>
#define MY_ERROR -1
#define MY_EMPTY 0


class Server
{
public:
	Server(UINT32 port=12345);
	/*Recibe como paramtro un int, que representa el puerto que el srvidor escucha,
	en caso que ese puerto no sea indicado, el puerto por defecto es 12345.*/
	~Server();
	void waitForClient();
	void setPortNumber(UINT32 port = 12345);
	UINT32 getPortNumber();
	size_t receiveDataFromClient(char * buffer_t, int bufferSize);
	/*Previamente se deve llamar a waitForClient()
	recibe como parametros un arreglo de char(buffer) y una int,
	con la cantidad de elementos de dicho arreglo.
	si se pudo recibir toda la info devuelve la cantidad de datos que recibio, si no pudo devuelve un -1*/
	size_t NBReceiveDataFromClient(char * buffer_t, int bufferSize);
	/*Previamente se deve llamar a waitforClient()
	recibe como parametros un arreglo de char(buffer) y un int,
	con la cantidad de elementos de dicho arreglo.
	devuelve la longitud del mensaje recibido si recibio, y si no recibio devuelve -1
	nota: NO ES BLOQUEANTE*/
	bool sendData(char * dataToSend_t, unsigned int sizeData);
	/*Recibe un arreglo de char, que son lo elementos que mandara.Tambien recibe
	un int con la cantidad de elementos que necesita enviar
	Devuelve un true si pudo enviar y un false en el caso contrario.*/
protected:
	UINT32 portNumber;
	boost::asio::io_service*  ioServer;
	boost::asio::ip::tcp::socket* ServerSocket;
	boost::asio::ip::tcp::acceptor* conectionServerAceptor;
	boost::asio::ip::tcp::resolver::iterator endpoint;
};

