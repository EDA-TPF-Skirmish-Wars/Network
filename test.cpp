#include <iostream>
#include "allegro5\allegro.h"
#include "Connections.h"
#include "Client.h"
#include "Server.h"
#include "Timer.h"

void  * callback(char * mapname, unsigned int mapNameSize, int checksum);


int main(void)
{
	al_init();
	Connections testConnection;
	//getchar();
	testConnection.establishConnection();
	char nombre[4] = { 'I','A','N','2' };
	testConnection.setName(nombre, 4);
	if (!testConnection.amIServer())
		testConnection.initGame(&callback);
	timerMiliseconds(1);
	return 0;
}

void * callback(char * mapname, unsigned int mapNameSize, int checksum)
{
	timerMiliseconds(1);
	return NULL;
}