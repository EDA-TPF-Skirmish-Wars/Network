#include "Connections.h"
#include "Client.h"
#include "Server.h"
#include "Timer.h"
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


Connections::Connections()
{
	isServer=false;
	for (int i = 0; i < BUFFER_SIZE_C; i++)	//seteo el buffer en 0
		buffer[i]=0;
}

bool Connections::establishConnection()
{
	SoC = new Client;
	Client * client = (Client *)SoC;
	isServer = !(client->ConnectToServer("localhost","13225"));		//intento conectarme como cliente
	if(!isServer)													//si conecte, salgo de la funcion
		return true;
	int num = rand()%5000;			//en caso de no haber conectado hago un randon de un numero de entre 2000 y 5000
	while(num < 2000)
		num = rand()%5000;
	timerMiliseconds(num);
	isServer = !(client->ConnectToServer("localhost","13225"));	//vuelvo a intentar conectar como cliente
	if(isServer)					//en caso de no poder, creo un servidor
	{
		delete SoC;
		SoC = new Server(13225);
		Server * server = (Server *)SoC;
		server->waitForClient();		//espero a que un cliente se conecte
	}
	return true;					//salgo de la funcion
}

bool Connections::amIServer()
{return isServer;}

bool Connections::initGame(void * callback(char* mapName, unsigned int mapNameSize, int checksum), unsigned int sizeOfMapName, int checksum, char * mapName)
{
	bool answer;
	if(isServer)
	{
		Server * server = (Server *)SoC;
		data2Send[0] =NAME_C;								//creo el paquete name y lo mando
		server->sendData(data2Send, 1);
		do {												//espero hasta recibir un paquete name is
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
		}while (buffer[0] != NAME_IS_C);
		int sizeOfName = buffer[1];							//guardo el nombre
		nameP2 = new char[sizeOfName];						//en nameP2
		nameSizeP2 = sizeOfName;
		for(int i = 0; i<sizeOfName;i++)				
			nameP2[i]=buffer[2+i];
		data2Send[0] = ACK_C;								//mando un ACK
		server->sendData(data2Send, 1);
		clearBuffer();										//limpio el buffer recibido anteriormente
		do {												//espero a que me llegue un paquete name
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
		}while (buffer[0] != NAME_C);
		data2Send[0]= NAME_IS_C;							//creo el paquete name is
		data2Send[1]=nameSize;
		for(int i=0;i<nameSize;i++)
			data2Send[1+i]=nameP1[i];
		server->sendData(data2Send,nameSize+2);				//lo envio
		clearBuffer();										//limpio el buffer recibido anteriormente
		do {												//espero a que me llegue un paquete ACK
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
		} while (buffer[0] != ACK_C);
		data2Send[0] = MAP_IS_C;							//Genero un paquete map_is
		data2Send[1] = sizeOfMapName;						//con los datos recibidos de la funcion
		for (unsigned int i = 0; i < sizeOfMapName; i++)
			data2Send[1 + i] = mapName[i - 1];
		data2Send[1 + sizeOfMapName + 1] = checksum;
		server->sendData(data2Send, sizeOfMapName + 3);		//envio el paquete
		clearBuffer();										
		do {												//espero a que me llegue un paquete ACK
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
		} while (buffer[0] != ACK_C);
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
			do {											//y espero a que me llegue un paquete ACK para poder continuar
				server->receiveDataFromClient(buffer, BUFFER_SIZE_C);
			} while (buffer[0] != ACK_C);
			answer = true;
		}
	}
	else
	{
		Client * client = (Client *)SoC;
		clearBuffer();
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
		} while (buffer[0] != NAME_C);
		data2Send[0] = NAME_IS_C;							//creo el paquete name is
		data2Send[1] = nameSize;
		for (int i = 0; i<nameSize; i++)
			data2Send[1 + i] = nameP1[i];
		client->sendData(data2Send, nameSize + 2);				//lo envio
		clearBuffer();
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
		} while (buffer[0] != ACK_C);
		data2Send[0] = NAME_C;
		client->sendData(data2Send, 1);
		clearBuffer();
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
		} while (buffer[0] != NAME_IS_C);
		int sizeOfName = buffer[1];							//guardo el nombre
		nameP2 = new char[sizeOfName];						//en nameP2
		nameSizeP2 = sizeOfName;
		for (int i = 0; i<sizeOfName; i++)
			nameP2[i] = buffer[2 + i];
		data2Send[0] = ACK_C;								//mando un ACK
		client->sendData(data2Send, 1);
		clearBuffer();
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
		} while (buffer[0] != MAP_IS_C);
		callback((char *)buffer[2], buffer[1], buffer[2 + buffer[1]]);
		data2Send[0] = ACK_C;								//mando un ACK
		client->sendData(data2Send, 1);
		clearBuffer();
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
		} while (buffer[0] != YOU_START_C || buffer[0] != I_START_C);
		if (buffer[0] == YOU_START_C)
			answer = true;
		else
			answer = false;
	}
	return answer;
}

void Connections::clearBuffer()
{
	for(unsigned int i = 0; i < BUFFER_SIZE_C; i++)	
		buffer[i] = 0;
	return;
}

void Connections::setName(char * name, unsigned int size)
{
	nameSize = size;
	nameP1 = new char[size];
	for (unsigned int i = 0; i < size; i++)
		nameP1[i] = name[i];
	return;
}

bool Connections::waitForMyTurn(bool * callback(move_s move, int data1, int data2, int data3, int data4, int data5))
{
	bool answer;
	if (isServer)
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
	if (buffer[0] == MOVE_C)
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

	if (answer == true)
		data2Send[0] = ACK_C;
	else
		data2Send[0] = ERROR_C;
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

	return answer;
}

bool Connections::sendMessage(move_s move, int data1, int data2, int data3, int data4, int data5)
{
	if (move == MOVE)
		data2Send[0] = MOVE_C;
	else if (move == PURCHASE)
		data2Send[0] = PURCHASE_C;
	else if (move == ATTACK)
		data2Send[0] = ATTACK_C;
	data2Send[1] = data1;
	data2Send[2] = data2;
	data2Send[3] = data3;
	data2Send[4] = data4;
	data2Send[5] = data5;
	if (isServer)
	{
		Server * server = (Server *)SoC;
		server->sendData(data2Send, 6);
		clearBuffer();
		do {
			server->receiveDataFromClient(buffer, BUFFER_SIZE_C);				//esto en realidad no va a ir asi falta checkear otros mensajes, por ahora para debuguear lo dejop asi
		} while (buffer[0] != ACK_C);
	}
	else
	{
		Client * client = (Client *)SoC;
		client->sendData(data2Send, 6);
		clearBuffer();
		do {
			client->receiveDataFromServer(buffer, BUFFER_SIZE_C);
		} while (buffer[0] != ACK_C);
	}
	return true;
}