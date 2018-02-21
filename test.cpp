#include <iostream>
#include "allegro5\allegro.h"
#include "Connections.h"
#include "Client.h"
#include "Server.h"
#include "Timer.h"

void  * callback(char * mapname, unsigned int mapNameSize, int checksum);
bool callback2(move_s move, int data1, int data2, int data3, int data4, int data5);


int main(void)
{
	al_init();
	Connections testConnection;
	//getchar();
	testConnection.establishConnection();
	char nombre[4] = { 'I','A','N','2' };
	testConnection.setName(nombre, 4);
	char temp;
	if (!testConnection.amIServer())
		temp = testConnection.initGame(&callback);
	bool temp2;
	if (temp)
	{
		timerMiliseconds(10000);
		do
		{
			temp2 = testConnection.sendMessage(ATTACK);
		} while (temp2);
	}
	else
	{
		testConnection.waitForMyTurn(&callback2);
	}
	return 0;
	return 0;
}

void * callback(char * mapname, unsigned int mapNameSize, int checksum)
{
	timerMiliseconds(1);
	return NULL;
}

bool callback2(move_s move, int data1, int data2, int data3, int data4, int data5)
{
	timerMiliseconds(1);
	return true;
}