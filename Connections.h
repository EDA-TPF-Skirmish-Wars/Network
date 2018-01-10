#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "Server.h"
#include "Client.h"

#define BUFFER_SIZE_C	257

enum move_s {ATTACK, PURCHASE, MOVE, PASS, QUIT};
typedef char * p2char;
typedef char byte;
typedef struct {
	unsigned int sizeOfMapName;
	char * mapName;
	int checksum;
}map_c;


class Connections
{
public:
	Connections();
	~Connections();
	bool establishConnection();												//1°
	void setName(char * name, unsigned int size);							//2°
	bool sendMessage(move_s move, int data1, int data2, int data3, int data4);		//Bloqueante hasta que llegue un ACK o haya un timeout
	bool initGame(void * callback(char* mapName,unsigned int mapNameSize,int checksum)=NULL, unsigned int sizeOfMapName = 0, int checksum = 0, char * mapName = NULL); 
	//initGame devuelve un true si es su turno de jugar o false si es turno del oponente, en caso de ser cliente recibe un callback con el
	//nombre del mapa, el tamaño del nombre y el checksum del mapa como argumentos. En caso de ser servidor se deve enviar como argumentos
	//primero un NULL seguido de el tamaño del nombre del mapa sorteado, el checksum, y por ultimo puntero al nombre.
	bool amIServer();														//3°
	bool waitForMyTurn();
protected:
	void clearBuffer();
	bool isServer;
	void * SoC;			//Server or Client
	p2char nameP1, nameP2;
	unsigned int nameSize;
	unsigned int nameSizeP2;
	byte buffer[BUFFER_SIZE_C];
	char data2Send[BUFFER_SIZE_C];						//creo un buffer para enviar datos
};

#endif