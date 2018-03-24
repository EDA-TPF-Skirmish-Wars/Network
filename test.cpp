#include <iostream>
#include "allegro5\allegro.h"
#include "Connections.h"
#include "Client.h"
#include "Server.h"
#include "Timer.h"

using namespace std;

void  * callback(char * mapname, unsigned int mapNameSize, int checksum);
bool callback2(move_s move, int data1, int data2, int data3, int data4, int data5);
int callbackResponseAttack(void);


int main(void)
{
	if (!al_init())
	{
		cout << "Allegro no pudo inicializarse" << endl;
		return 0;
	}
	Connections testConnection;
	cout << "Press ENTER to start testing" << endl;
	//getchar();
	testConnection.establishConnection();
	char nombre[4] = { 'I','A','N','2' };
	testConnection.setName(nombre, 4);
	char temp = false;
	if (!testConnection.amIServer())
	{
		cout << "You're client" << endl;
		temp = testConnection.initGame(&callback);
	}
	else
	{
		cout << "You're Server" << endl;
		char a = 'a';
		temp = testConnection.initGame(NULL,1,2,&a);
	}
	if (!testConnection.amIServer())
	{
		do {
			temp = testConnection.waitForMyTurn(&callback2, &callbackResponseAttack);
		} while (temp == -1);
	}
	if (temp == false)
		return 0;
	bool temp2 = true;
	do
	{
		cout << "What message do you want to send? ATTACK (a) PURCHASE (c) MOVE (m) PASS (p) QUIT (q)" << endl;
		char c = getchar();
		getchar();
		switch (c) {
		case 'a':
			temp2=testConnection.sendMessage(ATTACK,5,5,6,6,6,&callback2);
			break;
		case 'c':
			temp2 = testConnection.sendMessage(PURCHASE, 3, 4, 1, 1);
			break;
		case 'm':
			temp2 = testConnection.sendMessage(MOVE, 0, 0, 0, 1);
			break;
		case 'p':
			temp2 = testConnection.sendMessage(PASS);
			break;
		case 'q':
			temp2 = testConnection.sendMessage(QUIT);
			break;
		default:
			temp2 = testConnection.sendMessage(QUIT);
			break;
		}
		if (temp2)
		{
			do {
				temp = testConnection.waitForMyTurn(&callback2, &callbackResponseAttack);
			} while (temp == -1);
		}
	} while (temp && temp2);

	return 0;
}

void * callback(char * mapname, unsigned int mapNameSize, int checksum)
{
	return NULL;
}

bool callback2(move_s move, int data1, int data2, int data3, int data4, int data5)
{
	switch (move) {
	case ATTACK:
		cout << "Recibi ATTACK desde F" << data1 << " C" << data2 << "	hacia F" << data3 << " C" << data4 << "		Con dado" << data5 << endl;
		break;
	case PURCHASE:
		cout << "Recibi PURCHASE de Unidad 1:" << data1 << "    Unidad 2:" << data2 << "   F" << data3 << " C" << data4 << endl;
		break;
	case MOVE:
		cout << "Recibi MOVE desde F" << data1 << " C" << data2 << "  hacia F" << data3 << " C" << data4 << endl;
		break;
	case PASS:
		cout << "Recibi un PASS" << endl;
		break;
	case QUIT:
		cout << "Recibi QUIT" << endl;
		return false;
		break;
	default:
		cout << "Recibi algo mal" << endl;
		break;
	}
	return true;
}

int callbackResponseAttack(void)
{
	srand(time(NULL));
	return (rand() % 6);
}