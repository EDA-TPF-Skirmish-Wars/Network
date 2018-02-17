#pragma once

//Para usar esta libreria, Allegro TIENE que estar inicializado previamente!!!
void timerMiliseconds(unsigned int milis);

void timerNB(unsigned int milis);	//Non Blocking timer

bool isTimerFinished();