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
#include <string.h>
#include <unistd.h>
#include "glitch.h"

#ifdef USEPORTAUDIO
#include "paHelper.h"
#endif
#ifdef USESDL
#include "SDL/SDL.h"
#include "SDL/SDL_audio.h"
#endif


/* ********************************************************************** */

long t = 0;
long bufferSize = 1;
float volume = 1.0;

pGlitch * theGlitch = NULL;
int lastV;

uint8_t * displayBuffer = NULL; /* should be 512 bytes large */

/* makeDisplayBuffer
 *
 *	allocate the space for the display buffer, if needed
 */
void makeDisplayBuffer( long framesPerBuffer )
{
	if( displayBuffer ) return;

	displayBuffer = (uint8_t *)calloc( framesPerBuffer, sizeof( uint8_t ) );
	bufferSize = framesPerBuffer;
}


/* computeT
 *
 *	pipe T through the glitch to get a result
 */
long computeT( long t )
{
	long v;

        /* put your algoRHYTHM in here... */
	/* well, it's really more of an equation, but whatever */
/*
        v = (( t >> 10 ) & 42) * t;
	v = t&t>>8;
	v = (t*5&t>>7)|(t*3&t>>10);
*/
	if( !theGlitch ) {
		v = 42;
	} else {
		v = glitchEvaluate( theGlitch, t );
	}

        return (v & 0x0ff);
}

/* **************************************** */

/* bytebeatCallback
 *
 *	the callback that PortAudio or libSDL will use to ask us to
 *	provide more audio dataz
 */
#ifdef USEPORTAUDIO
/* PortAudio's version */
static int bytebeatCallback( const void *inputBuffer, 
			   void *outputBuffer,
                           unsigned long len, /* framesPerBuffer, */
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
#endif
#ifdef USESDL
/* libSDL's version */
static void bytebeatCallback( void * userData, Uint8 *outputBuffer, int len )
#endif
{
        /* Cast data passed through stream to our structure. */
        uint8_t *out = (uint8_t*)outputBuffer;
	uint8_t *db = displayBuffer;
        unsigned int i;
        uint8_t v;

#ifdef USEPORTAUDIO
        (void) inputBuffer; /* Prevent unused variable warning. */
#endif

	makeDisplayBuffer( len );
	db = displayBuffer;

        for( i=0; i<len/2; i++ )
        {
                v = (uint8_t)computeT( t+i );
		if( i==0 ) lastV = v;

		*db++ = v; 
		*db++ = v;

		/* adjust volume for actual output only */
		v = (uint8_t)( (float) v * (float) volume );

                *out++ = v;
                *out++ = v;
        }

        t=t+i;
#ifdef USEPORTAUDIO
        return 0;
#endif
}


#ifdef USESDL

/* soundInit
 *
 *	the libSDL version of sound system initialization
 */
int soundInit( void )
{
	SDL_AudioSpec fmt;

	fmt.freq = 16000;
	fmt.format = AUDIO_U8;
	fmt.channels = 1;
	fmt.samples = 512;
	fmt.callback = bytebeatCallback;
	fmt.userdata = NULL;

	if( SDL_OpenAudio( &fmt, NULL ) < 0 ){
		fprintf( stderr, "Unable to open audio: %s\n", SDL_GetError());
		return -1;
	}

	return 0;
}
#endif


/* ********************************************************************** */

/* the keys for our color pairs for the screen */
#define kColorHudA	(1)
#define kColorHudB	(2)
#define kColorFrame	(3)
#define kColorFrameT	(4)
#define kColorVisPixel	(5)
#define kColorLine	(6)
#define kColorLineSel	(7)

#define kColorVisGrn	(kColorVisPixel)
#define kColorVisYel	(8)
#define kColorVisRed	(9)


/* initScreen
 *	initialize the curses screen, and get it all setup the way we want
 */
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
	init_pair( kColorVisPixel, COLOR_GREEN, COLOR_GREEN );
	init_pair( kColorLine, COLOR_BLUE, COLOR_BLACK );
	init_pair( kColorLineSel, COLOR_YELLOW, COLOR_BLACK );
	init_pair( kColorVisYel, COLOR_YELLOW, COLOR_YELLOW );
	init_pair( kColorVisRed, COLOR_RED, COLOR_RED );
}


/* deinitScreen
 *
 *	close down curses, restore stuff
 */
void deinitScreen( void )
{
	curs_set( 1 );
	endwin();
}


/* ********************************************************************** */

/* volumeDown
 *
 *	turn down the volume 1/10th
 */
void volumeDown( void )
{
	volume -= 0.1;
	if( volume < 0 ) volume = 0;
}


/* volumeUp
 *
 *	turn up the volume 1/10th
 */
void volumeUp( void )
{
	volume += 0.1;
	if( volume > 1.0 ) volume = 1.0;
}


/* ********************************************************************** */


/* showHUD
 *
 *	show the heads up display
 */
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

/*
	attron( COLOR_PAIR( kColorHudB ) );
	move( 0, 0 );
	printw( "Vol: %0.2f", volume );
*/
	/*attroff( A_BOLD );*/
/*
	attroff( COLOR_PAIR( kColorHudB ) );
*/
}



WINDOW * vw;
int vww, vwh;
int vis = 1;

/* nextVis
 *
 *	advance to the next visualizer
 */
void nextVis( void )
{
	vis ++;
	if( vis > 1 ) vis = 0;
}


/* showVis
 *
 *	display the current visualizer to the screen
 */
void showVis( int mx, int my )
{
	int hp;

	if( !vw ) {
		vww = mx;
		vwh = my-19;
		vw = newwin( vwh, vww, 0, 0);
	}

	/* visualizer */
	wclear( vw );
	wattron( vw, COLOR_PAIR( kColorFrame ) );
	wborder(vw, '|', '|', '-', '-', '+', '+', '+', '+');
	wattroff( vw, COLOR_PAIR( kColorFrame ) );

	if( vis == 0 ) {
		/* dotty display */
		if( displayBuffer ) {
			wattron( vw, COLOR_PAIR( kColorVisPixel ) );
			for( hp = 1; hp < vww-1 ; hp++)
			{
				float px = (float)(hp-1)/(float)(vww-1);
				float py = (float)displayBuffer[(int)(px*bufferSize)] / 255.0;
				int y = vwh - 2 - (int) ((vwh-2) * py);
				wmove( vw, y, hp );
				wprintw( vw, "-" );
			}
			wattroff( vw, COLOR_PAIR( kColorVisPixel ) );
		}

	} else if( vis == 1 ) {
		/* vbars display */
		if( displayBuffer ) {
			attr_t c;
			for( hp = 1; hp < vww-1 ; hp++)
			{
				float px = (float)(hp-1)/(float)(vww-1);
				float py = (float)displayBuffer[(int)(px*bufferSize)] / 255.0;
				int hh = (int) ((vwh-2) * py);
				int y = (vwh - 2 - hh)/2;

				if( py > 0.85 ) c = COLOR_PAIR( kColorVisRed );
				else if( py > 0.70 ) c = COLOR_PAIR( kColorVisYel );
				else c = COLOR_PAIR( kColorVisGrn );

				if( hh < 1 ) hh = 1;

				wattron( vw, c );
				wmove( vw, y+1, hp );
				wvline( vw, '#' , hh );
				wattroff( vw, c );
			}
		}
	}

	/* title, status */
	wattron( vw, COLOR_PAIR( kColorFrameT ));
	wmove( vw, 0, 4 );
	wprintw( vw, " Visualizer: %s ", (vis == 0)? "Dot Waveform" : "Intensity Bars" );
	wmove( vw, 0, vww-15 );
	wprintw( vw, " t = %ld ", t );
	wattroff( vw, COLOR_PAIR( kColorFrameT ));

	/* wrefresh( vw ); */
	wnoutrefresh( vw );
}




WINDOW * ew;
int eww, ewh;

int eline = 0;

/* showEdit
 *
 *	display the editor window to the screen
 */
void showEdit( int mx, int my )
{
	int l;
	int offs;
	char buf[256];

	if( !ew ) {
		eww = mx;
		ewh = 18;
		ew = newwin( ewh, eww, my-ewh, 0 );
	}

	/* wclear( ew ); */
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
	offs = 25;
	if( theGlitch ) {
		if( theGlitch->name && strlen( theGlitch->name )) {
			wmove( ew, 0, 11 );
			wprintw( ew, ": \"%s\" ", theGlitch->name );
			offs += strlen( theGlitch->name );
		}

		wmove( ew, 0, offs );
		wprintw( ew, " (0x%02X)  -  %d Tokens  ", lastV & 0x0ff, glitchCountUseTokens( theGlitch ) );
		
	}
	wattroff( ew, COLOR_PAIR( kColorFrameT ));

	/* wrefresh( ew ); */
	wnoutrefresh( ew );
}


/* handleEditorKey
 *
 *	handle key input for the editor fields
 */
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

/* usage
 *
 *	display command line usage
 */
void usage( char * pn )
{
	fprintf( stderr, "Usage: %s [option] [glitch]\n", pn );
	fprintf( stderr, "\n" );
	fprintf( stderr, "Options:\n" );
	fprintf( stderr, "    -listing    print out the program and exit\n" );
	fprintf( stderr, "    -tokens     print the number of tokens and exit\n" );
	fprintf( stderr, "    -name       print the name and exit\n" );
	fprintf( stderr, "    -full       print full info and exit\n" );
	fprintf( stderr, "    -help       print thisinfo and exit\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "Glitch:\n" );
	fprintf( stderr, "  The glitch can be specified in a number of ways:\n" );
	fprintf( stderr, "  glitch://foo!aa    full uri on the line\n" );
	fprintf( stderr, "  foo!aa             just the song data\n" );
}


/* handleOptions
 *
 *	handle some command line options (i know it's messy, sorry.)
 */
char * handleOptions( int argc, char ** argv )
{
	int ac = 1;
	int i;
	char buf[512];

	for( ac = 1 ; ac<argc ; ac++ ) {
		if( !strcmp( argv[ac], "-help" )) {
			usage( argv[0] );
			return NULL;
		}

		else if(   !strcmp( argv[ac], "-tokens" )
			|| !strcmp( argv[ac], "-name" )
			|| !strcmp( argv[ac], "-listing" )
			|| !strcmp( argv[ac], "-full" ) ) {

			theGlitch = glitchParse( argv[ac+1] );
			if( !theGlitch ) {
				fprintf( stderr, "Unable to parse glitch!\n" );
				return NULL;
			}

			if( !strcmp( argv[ac], "-tokens" )) {
				printf( "Tokens: %d\n", glitchCountUseTokens( theGlitch ));
			}

			if( !strcmp( argv[ac], "-name" )) {
				printf( "Name: %s\n", theGlitch->name );
			}

			if( !strcmp( argv[ac], "-listing" )) {
				i=0;
				while( ( glitchLineToBuffer( theGlitch, i, buf, 512 ) >= 0 ))
				{
					printf( "%4d:  %s\n", i, buf );
					i++;
				}
			}

			if( !strcmp( argv[ac], "-full" )) {
				glitchDump( theGlitch );
			}

			glitchDestroy( theGlitch );

			return NULL;
		}

		else {
			return argv[ac];
		}
	}
	
	return argv[argc];
}

/* main
 *
 *	do main-type-stuff
 */
int main( int argc, char ** argv )
{
	char * glstr = NULL;

#ifdef USEPORTAUDIO
	PaStream * stream;
	PaError err;
#endif
#ifdef USESDL
	int err;
#endif

	int ch;
	int done = 0;
	int mx, my;

	/* check args */
	glstr = handleOptions( argc, argv );
	if( !glstr ) {
		return -2;
	}


	/* initialize the audio system */
#ifdef USEPORTAUDIO
	stream = soundInit( bytebeatCallback, NULL );
	if( !stream ) {
		return -1;
	}
#endif
#ifdef USESDL
	err = soundInit();
	if( err < 0 ) {
		return -1;
	}
#endif

	/* parse in the glitch */
	theGlitch = glitchParse( argv[1] );
	if( !theGlitch ) {
		fprintf( stderr, "Unable to parse glitch!\n" );
		return -2;
	}


	/* initialize the screen */
	initScreen();


        /* start audio playback/generation */
#ifdef USEPORTAUDIO
        NULLif( soundStart( stream ) == paNoError )
        {
	}
#endif
#ifdef USESDL
	SDL_PauseAudio( 0 );
#endif


	/* display the TUI */
	while( !done )
	{
		getmaxyx( stdscr, my, mx );
		ch = getch();
		if( ch == -1)
		{
			/*clear();*/
			showVis( mx, my );
			showHUD( mx, my );
			showEdit( mx, my );

			doupdate();
#ifdef USEPORTAUDIO
			Pa_Sleep(50);
#endif 
#ifdef USESDL
			usleep( 50 * 1000 );
#endif
		}
		handleEditorKey( ch );

		/*
		if( ch == 'V' ) volumeUp();
		if( ch == 'v' ) volumeDown();
		*/

		if( ch == 'v' ) nextVis();
		if( ch == 'q' )
		{
			done = 1;
		}
	}
	
	/* shut down the TUI */
	deinitScreen();

	/* turn off the audio */
#ifdef USEPORTAUDIO
	err = soundDeinit( stream );
#endif
#ifdef USESDL
	SDL_CloseAudio();
#endif

	/* destroy the glitch */
	glitchDestroy( theGlitch );
	return 0;
}
