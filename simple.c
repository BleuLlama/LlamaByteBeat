/*
	simple.c
	2011-Dec
	Scott Lawrence
	yorgle@gmail.com

 	* this was originally saw.c from the portaudio examples
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
#include <math.h>
#include "paHelper.h"
#include "portaudio.h"

/* ********************************************************************** */
long t = 0;

long computeT( long t )
{
	/* put your algorithm in here... */
	long v = (( t >> 10 ) & 42) * t;

	return (v & 0x0ff);
}


static int bytebeatCallback( const void *inputBuffer, void *outputBuffer,
			   unsigned long framesPerBuffer,
			   const PaStreamCallbackTimeInfo* timeInfo,
			   PaStreamCallbackFlags statusFlags,
			   void *userData )
{
	/* Cast data passed through stream to our structure. */
	float *out = (float*)outputBuffer;
	unsigned int i;
	float v;

	(void) inputBuffer; /* Prevent unused variable warning. */

	for( i=0; i<framesPerBuffer/2; i++ )
	{
		/* this returns a value 0..255, we need floats */
		v = computeT( t+i );
		v /= 255.0;

		*out++ = v;
		*out++ = v;
	}
	t=t+i;
	return 0;
}

/* ********************************************************************** */

int main( int argc, char ** argv )
{
	int z;

	PaStream *stream;
	PaError err;
	
	printf( "Simple example: The 42 Melody\n" );

	stream = soundInit( bytebeatCallback, NULL );
	if( !stream ) {
		return -1;
	}

	/* start playback */
	if( soundStart( stream ) == paNoError )
	{
		/* run for a few sconds. */
		for( z=0 ; z<100 ; z++ )
		{
			printf( "%d%% complete,  t=%ld\n", z, t );
			Pa_Sleep(250);
		}
	}

	err = soundDeinit( stream );

	return err;
}
