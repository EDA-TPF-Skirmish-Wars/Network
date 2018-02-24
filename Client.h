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
	/*metodo bloqueante que espera a conectarse con un servidor
	recibe como paramteros 2 string. el primero, ipserver, es la ip del servidor
	y el segundo string, portnumber, es el numero del puerto en el cual el servidor
	esta escuchando.*/
	bool sendData(char * dataToSend_t, unsigned int sizeData);
	/*recibe un arreglo de char, que son lo elementos que mandara. tambien recibe
	un int con la cantidad de elementos que se necesitan enviar
	devuelve true si se pudo enviar el paquete y false en caso contrario*/
	size_t receiveDataFromServer(char * buffer_t, int bufferSize);
	/*Previamente se deve llamar a connectToServer()
	recibe como paramteros un arreglo de char(buffer) y una int,
	con la cantidad de elementos de dicho arreglo.
	Si se puedo recibir toda la info devuelve la longitud de lo recibido, caso contrario
	devuelve un -1.*/
	size_t NBReceiveDataFromServer(char * buffer_t, int bufferSize);
	/*Previamente se deve llamar a connectToServer()
	recibe como paramteros un arreglo de char(buffer) y una int,
	con la cantidad de elementos de dicho arreglo.
	Si se puedo recibir toda la info devuelve la longitud de lo recibido, caso contrario
	devuelve un -1.
	NO ES BLOQUEANTE.*/
protected:
	boost::asio::ip::tcp::resolver* clientResolver;
	boost::asio::ip::tcp::socket* clientSocket;
	boost::asio::ip::tcp::resolver::iterator endpoint;
	boost::asio::io_service* ioClient;
};

