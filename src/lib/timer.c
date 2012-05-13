#include <SDL/SDL_timer.h>

#include "timer.h"

void timer_setup() {
    timer.scale         = 1.0f;
    timer.fps           = 0;
    timer.elapsed_time  = 0;

    timer_reset();
}

void timer_update() {
    timer.current_time  = SDL_GetTicks();
    timer.elapsed_time  = timer.current_time - timer.last_time;
    timer.last_time     = timer.current_time;
    timer.update_period += timer.elapsed_time;
    timer.frames++;

    if (timer.update_period > 1000) {
        timer.fps = (float)timer.frames / (timer.update_period / 1000.0f);
	timer.update_period -= 1000;
        timer.frames = 0;
    }
}

void timer_reset() {
    timer.last_time = timer.current_time = SDL_GetTicks();
    timer.frames = 0;
    timer.update_period = 0;
}

void timer_sleep(long ms) {
    SDL_Delay(ms);
}
