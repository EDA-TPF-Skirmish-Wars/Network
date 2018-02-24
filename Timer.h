#pragma once
//Para usar esta libreria, Allegro TIENE que estar inicializado previamente!


void timerMiliseconds(unsigned int milis); 
/*Funcion Bloqueante que requiere de la cola de eventos de allegro
su funcion es bloquear el flujo del programa durante la cantidad de milisegundos especificada como un
argumento de tipo unsigned int.
Ejemplo:
	{
		...
		timerMiliseconds(120000);		//Espera 120 segundos para continuar el flujo del programa
		...
	}
*/

void timerNB(unsigned int milis);
/*Funcion de timer NO bloqueante, su funcion es crear un timer que corra "de fondo" y,
utilizando en complementario la funcion que se especifica mas abajo, verificas si el timer creato termino o no.
*/

bool isTimerFinished();
/*Funcion que necesita la funcion anteriormente mencionada para funcionar, primero se deve inicializar un timerNB
y luego verificar si termino con esta funcion. Devuelve true en caso de que el timer halla terminado y false en caso 
contrario.
Ejemplo:
	{
		...
		timerNB(120000);
		while(!isTimerFinished())
		{
			doOtherStuff();
		}
		...
	}
*/