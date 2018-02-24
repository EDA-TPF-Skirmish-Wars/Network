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
	void establishConnection();
	/*Primera funcion a utilizar. Se encarga de (segun el protocolo), conectarse con otra maquina al puerto 13225.*/

	void setName(char * name, unsigned int size);
	/*Segunda funcion a utilizar.
	Funcion para setear el nombre del jugador que se encuentra en esta maquina, esto se hace para enviar el nombre
	hacia la otra PC y asi la otra persona pueda ver nuestro nombre.*/

	bool amIServer();
	/*Tercera funcion a utilizar, esta funcion se utiliza nada mas para ver si uno es cliente o servidor, ya que
	dependiendo si uno es servidor o cliente, se debe llamar a la cuarta funcion (initGame()) con diferentes agumentos
	devuelve true si es Servidor, y false en caso contrario.*/

	char initGame(void * callback(char* mapName, unsigned int mapNameSize, int checksum) = NULL,\
		unsigned int sizeOfMapName = 0, int checksum = 0, char * mapName = NULL);
	/*Cuarta funcion a utilizar, initGame() devuelve un true si es su turno de jugar, false si es turno del oponente
	o ERROR si hubo un problema de conexion, en caso de ser cliente recibe un callback con el nombre del mapa, el tamaño
	del nombre y el checksum del mapa como argumentos. En caso de ser servidor se deve enviar como argumentos primero un 
	NULL seguido de el tamaño del nombre del mapa sorteado, el checksum, y por ultimo puntero al nombre.*/

	bool sendMessage(move_s move, int data1 = 0, int data2 = 0, int data3 = 0, int data4 = 0, int data5 = 0,\
		bool callback(move_s move, int data1, int data2, int data3, int data4, int data5) =NULL);
	/*Funcion para enviar mensajes de movimiento de tablero, ataque, compra, etc. Dependiendo de que mensaje se quiera
	enviar, debe llenar los argumentos data1, data2,etc. Es decir si se quiere mover una pieza, se debe poner en data1
	la fila de la pieza antes de moverla, en data2 la columna de la pieza antes de moverla, en data3 la fila despues de moverla
	y en data4 la columna despues de moverla. Para ver cada mensaje en especifico, seguir el orden del protocolo de comunicacion.
	Nota: Es bloqueante hasta que llegue un ACK o halla un timeout.
	El callback se utiliza para el caso donde se envia un ATTACK, que despues de enviarlo, el jugador atacado, responde con
	otro paquete attack.*/

	bool waitForMyTurn(bool callback(move_s move,int data1, int data2 , int data3, int data4, int data5),\
	int callbackResponseAttack(void)); 
	/*Funcion que espera a recibir una jugada del jugador contrario, ni bien la recibe llama al callback con la movida que
	hizo el contrario y todos los datos para actualizar en el tablero nuestro, luego, cuando el callback le devuelve un
	true (es decir que todo esta bien), envia un ACK y sale de la funcion con un true, en caso de que la movida no sea valida,
	el callback debe devolver un false, por lo cual la funcion envia un paquete ERROR, y sale de la funcion con un false, por 
	lo que se da como finalizada la partida y perdida la conexion. La funcion callbackAttackResponse() es una funcion que debe
	devolver el dado tirado por el jugador de esta computadora como respuesta a un ataque recibido, asi puede responder con otro
	paquete atack segun las reglas del juego.*/

	unsigned int getOpponentNameSize();
	/*Funcion que devuelve el tamaño de caracteres que posee el nombre del jugador oponente*/

	char * getOpponentName();
	/*Funcion que devuelve un puntero al nombre del jugador oponente*/

private:
	bool isServer;
	void * SoC;			//Server or Client
	p2char nameP1, nameP2;
	unsigned int nameSize;
	unsigned int nameSizeP2;
	void clearBuffer();
	char buffer[BUFFER_SIZE_C];
	char data2Send[BUFFER_SIZE_C];
};

#endif