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
#include "glitch.h"

/* http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/ */

/* ********************************************************************** */

long t = 0;
long bufferSize = 1;
float volume = 1.0;

#undef SLOWUPDATE

pGlitch * theGlitch = NULL;

long computeT( long t )
{
	long v;

	
        /* put your algoRHYTHM in here... */
	/* well, it's really more of an equation, but whatever */
        v = (( t >> 10 ) & 42) * t;
/*
	v = t&t>>8;
	v = (t*5&t>>7)|(t*3&t>>10);
*/

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
		if( !theGlitch ) {
			v = 42;
		} else {
			v = glitchEvaluate( theGlitch, t+i );
		}

                /*v = computeT( t+i ); */
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

#define kColorHudA	(1)
#define kColorHudB	(2)
#define kColorFrame	(3)
#define kColorFrameT	(4)
#define kColorPixel	(5)
#define kColorLine	(6)
#define kColorLineSel	(7)

void initScreen( void )
{
	initscr();
	cbreak();
	noecho();
	timeout( 0 );
	curs_set( 0 );

	nonl();
	intrflush( stdscr, FALSE );
	keypad( stdscr, TRUE );

	start_color();
	init_pair( kColorHudA, COLOR_WHITE, COLOR_BLACK );
	init_pair( kColorHudB, COLOR_YELLOW, COLOR_BLACK );
	init_pair( kColorFrame, COLOR_BLUE, COLOR_BLUE );
	init_pair( kColorFrameT, COLOR_CYAN, COLOR_BLACK );
	init_pair( kColorPixel, COLOR_GREEN, COLOR_GREEN );
	init_pair( kColorLine, COLOR_BLUE, COLOR_BLACK );
	init_pair( kColorLineSel, COLOR_YELLOW, COLOR_BLACK );
}

void deinitScreen( void )
{
	curs_set( 1 );
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


/* ********************************************************************** */


void showHUD( int mx, int my )
{
	/* draw some stats */
	/*attron( A_BOLD ); */
/*
	attron( COLOR_PAIR( kColorHudA ) );
	move( 0, 0 );
	printw( "Time: %ld", t );
	attroff( COLOR_PAIR( kColorHudA ) );
*/

	attron( COLOR_PAIR( kColorHudB ) );
	move( 0, 0 );
	printw( "Vol: %0.2f", volume );
	/*attroff( A_BOLD );*/
	attroff( COLOR_PAIR( kColorHudB ) );
}

WINDOW * vw;
int vww, vwh;

void showVis( int mx, int my )
{
	int hp;

	if( !vw ) {
		vww = mx;
		vwh = 20;
		vw = newwin( vwh, vww, 1, 0);
	}

	/* visualizer */
	wclear( vw );
	wattron( vw, COLOR_PAIR( kColorFrame ) );
	wborder(vw, '|', '|', '-', '-', '+', '+', '+', '+');
	wattroff( vw, COLOR_PAIR( kColorFrame ) );

	if( displayBuffer ) {
		wattron( vw, COLOR_PAIR( kColorPixel ) );
		for( hp = 1; hp < vww-1 ; hp++)
		{
			float px = (float)(hp-1)/(float)(vww-1);
			float py = displayBuffer[(int)(px*bufferSize)];
			int y = vwh - 2 - (int) ((vwh-2) * py);
			wmove( vw, y, hp );
			wprintw( vw, "-" );
		}
		wattroff( vw, COLOR_PAIR( kColorPixel ) );
	}

	/* title, status */
	wattron( vw, COLOR_PAIR( kColorFrameT ));
	wmove( vw, 0, 4 );
	wprintw( vw, " Visualizer ");
	wmove( vw, 0, vww-14 );
	wprintw( vw, " t=%ld ", t );
	wattroff( vw, COLOR_PAIR( kColorFrameT ));

	wrefresh( vw );
}


WINDOW * ew;
int eww, ewh;

int eline = 0;

void showEdit( int mx, int my, int yp )
{
	int l;
	char buf[256];

	if( !ew ) {
		eww = mx;
		ewh = 18;
		ew = newwin( ewh, eww, yp, 0 );
	}

	wclear( ew );
	wattron( ew, COLOR_PAIR( kColorFrame ) );
	wborder(ew, '|', '|', '-', '-', '+', '+', '+', '+');
	wattroff( ew, COLOR_PAIR( kColorFrame ) );

	/* line numbering */
	for( l=0; l<16 ; l++ )
	{
		if( l == eline ) {
			wattron( ew, COLOR_PAIR( kColorLineSel ));
		} else {
			wattron( ew, COLOR_PAIR( kColorLine ));
		}
		wmove( ew, 1+l, 2 );
		wprintw( ew, "%02d:", l );
	
		glitchLineToBuffer( theGlitch, l, buf, 256 );
		wmove( ew, 1+l, 7 );
		wprintw( ew, "%s", buf );
		
		if( l == eline ) {
			wattroff( ew, COLOR_PAIR( kColorLineSel ));
		} else {
			wattroff( ew, COLOR_PAIR( kColorLine ));
		}
	}


	/* title, status */
	wattron( ew, COLOR_PAIR( kColorFrameT ));
	wmove( ew, 0, 4 );
	wprintw( ew, " Editor " );
	if( theGlitch && theGlitch->name && strlen( theGlitch->name )) {
		wmove( ew, 0, 11 );
		wprintw( ew, ": \"%s\" ", theGlitch->name );
	}
	wattroff( ew, COLOR_PAIR( kColorFrameT ));

	wrefresh( ew );
}


int handleEditorKey( int ch )
{
	int ret = 0;

	if( ch == KEY_DOWN ) { 
		eline++;
		if( eline > 15 ) eline = 0;
		ret = 1;
	}
	if( ch == KEY_UP ) {
		eline--;
		if( eline < 0 ) eline = 15;
		ret = 1;
	}

	return ret;
}



/* ********************************************************************** */

int main( int argc, char ** argv )
{
	PaStream * stream;
	PaError err;

	int ch;
	int done = 0;
	int mx, my;
#ifdef SLOWUPDATE
	int refresh = 1;
#endif

	if( argc != 2 ) {
		fprintf( stderr, "Error: specify a glitch on the command line\n" );
		return -2;
	}


	stream = soundInit( bytebeatCallback, NULL );
	if( !stream ) {
		return -1;
	}

	theGlitch = glitchParse( argv[1] );
	if( !theGlitch ) {
		fprintf( stderr, "Unable to parse glitch!\n" );
		return -2;
	}

/*
		glitchDump( theGlitch );
	return -1;
*/
	initScreen();


        /* start playback */
        if( soundStart( stream ) == paNoError )
        {
	}

	getmaxyx( stdscr, my, mx );



	while( !done )
	{
		ch = getch();
		if( ch == -1
#ifdef SLOWUPDATE
			&& refresh
#endif
			 )
		{
#ifdef SLOWUPDATE
			refresh = 0;
#endif
			/*clear();*/
			showVis( mx, my );
			showHUD( mx, my );
			showEdit( mx, my, 23 );

			/*refresh(); */
			Pa_Sleep(50);
		}
#ifdef SLOWUPDATE
		if( ch >= ' ' && ch <= '~' ) refresh = 1;
		refresh +=
#endif
		handleEditorKey( ch );

		if( ch == 'V' ) volumeUp();
		if( ch == 'v' ) volumeDown();
		if( ch == 'q' )
		{
			done = 1;
		}
	}
	
	deinitScreen();

	err = soundDeinit( stream );

	glitchDestroy( theGlitch );
	return 0;
}
