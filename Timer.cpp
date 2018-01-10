#include "Timer.h"
#include <allegro5\allegro5.h>

void timerMiliseconds(unsigned int milis)
{
	ALLEGRO_TIMER * timer;
	ALLEGRO_EVENT_QUEUE * queue;
	ALLEGRO_EVENT ev;
	timer = al_create_timer((1.0/1000) *milis);
	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_start_timer(timer);
	do {
		al_wait_for_event(queue, &ev);
	}while (ev.type != ALLEGRO_EVENT_TIMER);
	al_destroy_event_queue(queue);
	al_destroy_timer(timer);
	return;
}