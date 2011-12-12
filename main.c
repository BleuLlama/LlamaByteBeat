/*
	LlamaByteBeat
	2011-Dec
	Scott Lawrence
	yorgle@gmail.com
*/

/* 
    Copyright (C) 2011 Scott Lawrence

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/


#include <stdio.h>
#include <curses.h>
#include <math.h>
#include <stdlib.h>
#include "paHelper.h"

/* http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/ */

/* ********************************************************************** */

long t = 0;
long bufferSize = 1;
float volume = 1.0;


long computeT( long t )
{
	long v;
        /* put your algoRHYTHM in here... */
	/* well, it's really more of an equation, but whatever */
/*
        v = (( t >> 10 ) & 42) * t;
	v = t&t>>8;
*/
	v = (t*5&t>>7)|(t*3&t>>10);

        return (v & 0x0ff);
}


float * displayBuffer = NULL;

static int bytebeatCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
        /* Cast data passed through stream to our structure. */
        float *out = (float*)outputBuffer;
	float *db = displayBuffer;
        unsigned int i;
        float v;

        (void) inputBuffer; /* Prevent unused variable warning. */

	if( !displayBuffer ) {
		displayBuffer = (float *)calloc( framesPerBuffer, sizeof( float ) );
		bufferSize = framesPerBuffer;
	}

	db = displayBuffer;

        for( i=0; i<framesPerBuffer/2; i++ )
        {
                /* this returns a value 0..255, we need floats */
                v = computeT( t+i );
                v /= 255.0;

		*db++ = v;
		*db++ = v;

		/* adjust volume for actual output only */
		v *= volume;

                *out++ = v;
                *out++ = v;

        }
        t=t+i;
        return 0;
}





/* ********************************************************************** */

void initScreen( void )
{
	initscr();
	cbreak();
	noecho();
	timeout( 0 );

	nonl();
	intrflush( stdscr, FALSE );
	keypad( stdscr, TRUE );


	start_color();
	init_pair( 1, COLOR_YELLOW, COLOR_BLACK );
	init_pair( 2, COLOR_GREEN, COLOR_GREEN );
	init_pair( 3, COLOR_CYAN, COLOR_CYAN );
}

void deinitScreen( void )
{
	endwin();
}


/* ********************************************************************** */

void volumeDown( void )
{
	volume -= 0.1;
	if( volume < 0 ) volume = 0;
}

void volumeUp( void )
{
	volume += 0.1;
	if( volume > 1.0 ) volume = 1.0;
}

int main( int argc, char ** argv )
{
	int ch;
	int done = 0;
	int hp;
	int mx, my;

	WINDOW * vw;
	int vww, vwh;

	PaStream * stream;
	PaError err;

	stream = soundInit( bytebeatCallback, NULL );
	if( !stream ) {
		return -1;
	}

	initScreen();

	getmaxyx( stdscr, my, mx );

        /* start playback */
        if( soundStart( stream ) == paNoError )
        {
	}

	timeout( 0 );

	vww = 64;
	vwh = 32;
	vw = newwin( vwh, vww, 2, 1);

	while( !done )
	{
		ch = getch();
		if( ch == -1 )
		{
			/*clear();*/
			wclear( vw );
			/* draw some stats */
			/*attron( A_BOLD ); */
			attron( COLOR_PAIR(1) );
			move( 0, 0 );
			printw( "Time: %ld    Volume: %0.2f", t, volume );
			attroff( COLOR_PAIR(1) );
			/*attroff( A_BOLD );*/

			/* visualizer */
			if( displayBuffer ) {
				wattron( vw, COLOR_PAIR(3) );
				wborder(vw, '|', '|', '-', '-', '+', '+', '+', '+');
				wattroff( vw, COLOR_PAIR(3) );

				wattron( vw, COLOR_PAIR(2) );
				for( hp = 1; hp < vww-2 ; hp++)
				{
					float px = (float)(hp-1)/(float)(vww-2);
					float py = displayBuffer[(int)(px*bufferSize)];
					int y = vwh - 2 - (int) ((vwh-2) * py);
					wmove( vw, y, hp );
					wprintw( vw, "-" );
				}
				wattroff( vw, COLOR_PAIR(2) );
			}


			wrefresh(vw);
			refresh(); 
			Pa_Sleep(100);
		}
		if( ch == 'V' ) volumeUp();
		if( ch == 'v' ) volumeDown();
		if( ch == 'q' )
		{
			done = 1;
		}
	}
	
	deinitScreen();

	err = soundDeinit( stream );
	return 0;
}
