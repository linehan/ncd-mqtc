#ifndef _NMNSCU
#define _NMNSCU

#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>

int ncurses_configure(void)
{
        cbreak();	       /* Do not wait for newline (line buffering) */
        noecho();              /* Do not echo input to screen */
        nodelay(stdscr, true); /* Calls to getch() are non-blocking */ 
        keypad(stdscr, TRUE);  /* Enable special keys */
        curs_set(0);           /* Do not display cursor */
        start_color();

        /* 
         * ncurses sets the ESC key delay at 100ms by default, and this
         * is way too slow. According to Wolfram Alpha, the duration of
         * a typical human blink is 100ms-400ms.
         */
        if (getenv("ESCDELAY") == NULL) {
                ESCDELAY = 25;
        }

        return 1;
}

int ncurses_start(void)
{
        setlocale(LC_ALL,""); /* Must be set for UTF-8 support */
        initscr();            /* Start ncurses */
        ncurses_configure();  /* Setup ncurses */

        return 1;
}

int ncurses_stop(void)
{
        endwin();
        return 2;
}


/*
 * nsleep -- nanosleep made easy
 */
void nsleep(long nanoseconds)
{
        const struct timespec ts = { .tv_nsec = nanoseconds };
        nanosleep(&ts, NULL);
}


#endif
