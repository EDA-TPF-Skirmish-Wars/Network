//////////////////////////////////////////////////////////////////////////////////////
// Libreria Connections																//
//	Encargada de manejar todas las conecciones, es decir, mensajes enviados entre	//
//	computadoras, espera de turnos, y establecimiento de conexion para el juego		//
//	Skimirsh Wars, TP Final EDA 1er cuatrimestre 2017								//
//Trabajo Hecho por:																//
//	-Ian Diaz																		//
//	-Martina Maspero																//
//																		21/02/2018	//
//////////////////////////////////////////////////////////////////////////////////////


#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <iostream>
#define BUFFER_SIZE_C	257
#define ERROR_COM	-1

typedef enum {ATTACK, PURCHASE, MOVE, PASS, QUIT}move_s;
typedef char * p2char;


class Connections
{
public:
	Connections();
	~Connections();
	bool establishConnection();												//1°		(anda perfecto) (por ahora se conecta al localhost)
	void setName(char * name, unsigned int size);							//2°		(anda perfecto)
	bool sendMessage(move_s move, int data1 = 0, int data2 = 0, int data3 = 0, int data4 = 0, int data5 = 0);		//Bloqueante hasta que llegue un ACK o haya un timeout
	char initGame(void * callback(char* mapName,unsigned int mapNameSize,int checksum) = NULL, unsigned int sizeOfMapName = 0, int checksum = 0, char * mapName = NULL); 
	// (anda bien, falta ver porque el delete de tempMapName esta dando problemas, por lo demas, la funcion anda perfecto)
	
	//4°  initGame devuelve un true si es su turno de jugar, false si es turno del oponente o ERROR si hubo un problema de coneccion, en caso de ser cliente recibe 
	//un callback con el nombre del mapa, el tamaño del nombre y el checksum del mapa como argumentos. En caso de ser servidor se deve enviar como argumentos
	//primero un NULL seguido de el tamaño del nombre del mapa sorteado, el checksum, y por ultimo puntero al nombre.
	bool amIServer();														//3° (anda perfecto)
	bool waitForMyTurn(bool callback(move_s move,int data1, int data2 , int data3, int data4, int data5)); // esta funcion espera a recibir una jugada del jugador contrario
	//ni bien la recibe llama al callback con la movida que hizo el contrario y todos los datos para actualizar en el tablero nuestro, luego, cuando el callback le devuelve u
	//true (es decir que todo esta bien), envia un ack y sale de la funcion con un true, en caso de que la movida no sea valida, el callback debe devolver un false, por lo cual
	//la funcion envia un paquete ERROR, y sale de la funcion con un false, por lo que se da como finalizada la partida y perdida la conexion
	//(anda perfecto)
protected:
	bool isServer;
	void * SoC;			//Server or Client
	p2char nameP1, nameP2;
	unsigned int nameSize;
	unsigned int nameSizeP2;
private:
	void clearBuffer();
	char buffer[BUFFER_SIZE_C];
	char data2Send[BUFFER_SIZE_C];						//creo un buffer para enviar datos
};

#endif