/* 
 * File:   timer.h
 * Author: jami
 *
 * Created on April 20, 2012, 1:38 AM
 */

#ifndef TIMER_H
#define	TIMER_H

typedef struct _timing_t {
    long update_period;
    long current_time;
    long elapsed_time;
    long last_time;
    long frames;
    float fps;
    float scale;
} timing_t;

timing_t timer;

void timer_setup();
void timer_update();
void timer_reset();
void timer_sleep(long);

#endif	/* TIMER_H */

