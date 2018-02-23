#include "Connections.h"
#include "Client.h"
#include "Server.h"
#include "Timer.h"
#include <iostream>

#define NAME_C	0x10
#define NAME_IS_C	0x11
#define ACK_C		0x01
#define MAP_IS_C	0x12
#define	YOU_START_C	0x20
#define	I_START_C	0x21
#define	PASS_C	0x30
#define	MOVE_C	0x31
#define PURCHASE_C	0x32
#define	ATTACK_C 	0x33
#define YOU_WON_C	0x40
#define PLAY_AGAIN_C	0x50
#define GAME_OVER_C	0x51
#define ERROR_C	0xFE
#define QUIT_C	0xFF

#define DEFAULT_TIMEOUT	120000		//CAMBIAR A 120000
#define HOST	"localhost"
#define PORT	"13225"
#define PORT_NUM	13225

using namespace std;


Connections::Connections()
{
	isServer=false;
	for (int i = 0; i < BUFFER_SIZE_C; i++)	//seteo el buffer en 0
		buffer[i]=0;
}

Connections::~Connections()
{
	if (SoC != NULL)
		delete SoC;
	if (nameP1 != NULL)
		delete nameP1;
	if (nameP2 != NULL)
		delete nameP2;
	return;
}

bool Connections::establishConnection()
{
	SoC = new Client;
	Client * client = (Client *)SoC;
		isServer = !(client->ConnectToServer(HOST,PORT));		//intento conectarme como cliente
	if(!isServer)													//si conecte, salgo de la funcion
		return true;		
	int num;
	do {
		num = rand() % 5000;	//en caso de no haber conectado hago un random de un numero de entre 2000 y 5000
	} while (num < 2000);
	timerMiliseconds(num);
	isServer = !(client->ConnectToServer(HOST,PORT));	//vuelvo a intentar conectar como cliente
	if(isServer)					//en caso de no poder, creo un servidor
	{
		delete SoC;
		SoC = new Server(PORT_NUM);
		Server * server = (Server *)SoC;
		server->waitForClient();		//espero a que un cliente se conecte
	}
	return true;					//salgo de la funcion
}

bool Connections::amIServer()
{return isServer;}

char Connections::initGame(void * callback(char* mapName, unsigned int mapNameSize, int checksum), unsigned int sizeOfMapName, int checksum, char * mapName)
{
	bool answer;
	bool exit = false;
	if (isServer)
	{
		Server * server = (Server *)SoC;
		data2Send[0] = NAME_C;								//creo el paquete name y lo mando
		server->sendData(data2Send, 1);
		timerNB(DEFAULT_TIMEOUT);
		do {												//espero hasta recibir un paquete name is
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
			if (!exit)
				exit = isTimerFinished();
		} while (buffer[0] != NAME_IS_C && exit != true);
		int sizeOfName = buffer[1];							//guardo el nombre
		nameP2 = new char[sizeOfName];						//en nameP2
		nameSizeP2 = sizeOfName;
		for (int i = 0; i < sizeOfName; i++)
			nameP2[i] = buffer[1 + i];
		data2Send[0] = ACK_C;								//mando un ACK
		server->sendData(data2Send, 1);
		clearBuffer();										//limpio el buffer recibido anteriormente
		timerNB(DEFAULT_TIMEOUT);
		do {												//espero a que me llegue un paquete name
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
			if (!exit)
				exit = isTimerFinished();
		} while (buffer[0] != NAME_C && exit != true);
		data2Send[0] = NAME_IS_C;							//creo el paquete name is
		data2Send[1] = nameSize;
		for (unsigned int i = 0; i < nameSize; i++)
			data2Send[1 + i] = nameP1[i];
		server->sendData(data2Send, nameSize + 2);				//lo envio
		clearBuffer();										//limpio el buffer recibido anteriormente
		timerNB(DEFAULT_TIMEOUT);
		do {												//espero a que me llegue un paquete ACK
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
			if (!exit)
				exit = isTimerFinished();
		} while (buffer[0] != ACK_C && exit != true);
		data2Send[0] = MAP_IS_C;							//Genero un paquete map_is
		data2Send[1] = sizeOfMapName;						//con los datos recibidos de la funcion
		for (unsigned int i = 0; i < sizeOfMapName; i++)
			data2Send[2 + i] = mapName[i];
		data2Send[1 + sizeOfMapName + 1] = checksum;
		server->sendData(data2Send, sizeOfMapName + 3);		//envio el paquete
		clearBuffer();
		timerNB(DEFAULT_TIMEOUT);
		do {												//espero a que me llegue un paquete ACK
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
			if (!exit)
				exit = isTimerFinished();
		} while (buffer[0] != ACK_C && exit != true);
		if ((rand() % 2) != 0)								//Sorteo que jugador va a empezar
		{
			data2Send[0] = YOU_START_C;						//en caso de que salga 1 empieza el cliente
			server->sendData(data2Send, 1);					//y envio el paquete you start
			answer = false;
		}
		else
		{
			data2Send[0] = I_START_C;						//caso contrario le envio el paquete i start
			server->sendData(data2Send, 1);
			clearBuffer();
			timerNB(DEFAULT_TIMEOUT);
			do {											//y espero a que me llegue un paquete ACK para poder continuar
				server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
				if (!exit)
					exit = isTimerFinished();
			} while (buffer[0] != ACK_C && exit != true);
			answer = true;
		}
	}
	else											//En caso de ser cliente
	{
		Client * client = (Client *)SoC;
		clearBuffer();
		timerNB(DEFAULT_TIMEOUT);							// espero 2 minutos
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
			if (!exit)
				exit = isTimerFinished();
		} while (buffer[0] != NAME_C && exit != true);		//veo si recibo datos del servidor o si se acabo el tiempo
		data2Send[0] = NAME_IS_C;							//creo el paquete name is
		data2Send[1] = nameSize;
		for (unsigned int i = 0; i < nameSize; i++)
			data2Send[1 + i] = nameP1[i];
		client->sendData(data2Send, nameSize + 2);				//lo envio
		clearBuffer();
		timerNB(DEFAULT_TIMEOUT);
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
			if (!exit)
				exit = isTimerFinished();
		} while (buffer[0] != ACK_C && exit != true);
		data2Send[0] = NAME_C;
		client->sendData(data2Send, 1);
		clearBuffer();
		timerNB(DEFAULT_TIMEOUT);
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
			if (!exit)
				exit = isTimerFinished();
		} while (buffer[0] != NAME_IS_C && exit != true);
		int sizeOfName = buffer[1];							//guardo el nombre
		nameP2 = new char[sizeOfName];						//en nameP2
		nameSizeP2 = sizeOfName;
		for (int i = 0; i < sizeOfName; i++)
			nameP2[i] = buffer[1 + i];
		data2Send[0] = ACK_C;								//mando un ACK
		client->sendData(data2Send, 1);
		clearBuffer();
		timerNB(DEFAULT_TIMEOUT);
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
			if (!exit)
				exit = isTimerFinished();
		} while (buffer[0] != MAP_IS_C && exit != true);
		char * tempMapName;
		tempMapName = new char[buffer[1]];
		for (unsigned int i = 0; i < buffer[0]; i++)
			tempMapName[i] = buffer[2 + i];
		callback(tempMapName, buffer[1], buffer[2+buffer[1]]);
		//delete tempMapName;
		data2Send[0] = ACK_C;								//mando un ACK
		client->sendData(data2Send, 1);
		clearBuffer();
		timerNB(DEFAULT_TIMEOUT);
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
			if (!exit)
				exit = isTimerFinished();
		} while (buffer[0] != YOU_START_C && buffer[0] != I_START_C && exit != true);
		data2Send[0] = ACK_C;								//mando un ACK
		client->sendData(data2Send, 1);
		if (buffer[0] == YOU_START_C)
			answer = true;
		else
			answer = false;
	}
	if (exit == true)
	{
		cout << "Conxion fallida" << endl;		//ver que hacer en este caso (caso de que halla habido un problema en la conexion)
		answer = ERROR_COM;
	}
	return answer;
}

void Connections::clearBuffer()				//funcion propia que inicializa en 0 la variable buffer
{
	for(unsigned int i = 0; i < BUFFER_SIZE_C; i++)	
		buffer[i] = 0;
	return;
}

void Connections::setName(char * name, unsigned int size)		//funcion de inicializacion del nombre del jugador
{
	nameSize = size;
	nameP1 = new char[size];
	for (unsigned int i = 0; i < size; i++)
		nameP1[i] = name[i];
	return;
}

bool Connections::waitForMyTurn(bool callback(move_s move, int data1, int data2, int data3, int data4, int data5))
{
	bool answer;
	if (isServer)											//primero me fijo si inicialmente fui servidor o cliente para castear el SoC y utilizar las funciones correctas
	{
		Server * server = (Server *)SoC;
		clearBuffer();
		server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
	}
	else
	{
		Client * client = (Client *)SoC;
		clearBuffer();
		client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
	}
	if (buffer[0] == MOVE_C)							//una vez recivido los datos, checkeo si estos son validos y en caso de ser asi, llamo al callback con los datos recividos
	{
		answer = callback(MOVE, buffer[1], buffer[2], buffer[3], buffer[4], 0);
	}
	else if (buffer[0] == PURCHASE_C)
	{
		answer = callback(PURCHASE, buffer[1], buffer[2], buffer[3], buffer[4], 0);
	}
	else if (buffer[0] == ATTACK_C)
	{
		answer = callback(ATTACK, buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
	}
	else
		answer = false;

	if (answer == true)			//en caso de que el callback me devuelva un true, envio un ACK
		data2Send[0] = (char)ACK_C;
	else
	{
		if(buffer[0] != QUIT_C)
			data2Send[0] = (char)ERROR_C;	// caso contrario envio un paquete error
		else
			data2Send[0] = (char)ACK_C;
	}
	if (isServer)
	{
		Server * server = (Server *)SoC;
		server->sendData(data2Send, 1);
	}
	else
	{
		Client * client = (Client *)SoC;
		client->sendData(data2Send, 1);
	}

	return answer; // si estuvo todo OK devuelvo un true, caso contrario un false
}

bool Connections::sendMessage(move_s move, int data1, int data2, int data3, int data4, int data5)
{

	//VER CASO DE PAQUETE ATTACK
	bool exit = false;
	if (move == MOVE)					//seteo el paquete que voy a enviar
		data2Send[0] = (char)MOVE_C;
	else if (move == PURCHASE)
		data2Send[0] = (char)PURCHASE_C;
	else if (move == ATTACK)
		data2Send[0] = (char)ATTACK_C;
	else if (move == PASS)
		data2Send[0] = (char)PASS_C;
	else if (move == QUIT)
		data2Send[0] = (char)QUIT_C;
	data2Send[1] = data1;	//si bien para algunos paquetes la cantidad de datos puede sobrar, envio todos los datos que tengo en las variables, porque
	data2Send[2] = data2;	//los datos no inicializados se setean en 0, y el que recibe el mensaje debe checkear que los datos esten bien, por lo cual no hace falta
	data2Send[3] = data3;	//ver que datos envio para cada paquete.
	data2Send[4] = data4;
	data2Send[5] = data5;
	timerNB(DEFAULT_TIMEOUT);
	if (isServer)			//envio el paquete y espero a recibir un ACK
	{
		Server * server = (Server *)SoC;
		server->sendData(data2Send, 6);
		clearBuffer();
		do {
			if (!exit)
				exit = isTimerFinished();
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);				//esto en realidad no va a ir asi falta checkear otros mensajes, por ahora para debuguear lo dejop asi
		} while (buffer[0] != ACK_C && exit != true && buffer[0]!=ERROR_C);
	}
	else
	{
		Client * client = (Client *)SoC;
		client->sendData(data2Send, 6);
		clearBuffer();
		do {
			if (!exit)
				exit = isTimerFinished();
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
		} while (buffer[0] != ACK_C  && exit != true && buffer[0] != ERROR_C);
	}


	if (exit || buffer[0] == ERROR_C)
		return false;
	else
		return true;

}