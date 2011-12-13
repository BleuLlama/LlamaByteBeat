/*
	paHelper
	2011-Dec
	Scott Lawrence
	yorgle@gmail.com

	some stuff to make PortAudio a touch simpler.
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

#include "portaudio.h"
#include "paHelper.h"

/* NOTE: future change;  switch this over to allow for libSDL audio
	ref: http://paste2.org/p/1817840
*/

/* ********************************************************************** */

void soundErrorTerminate( PaError err )
{
        Pa_Terminate();
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
}


PaStream * soundInit( PaStreamCallback * call, void * data )
{
	PaError err;
	PaStream * str = NULL;
	
	/* Initialize library before making any other calls. */
	err = Pa_Initialize();
	if( err == paNoError )
	{
		/* Open an audio I/O stream. */
		err = Pa_OpenDefaultStream( &str,
			0,		  /* no input channels */
			1,		  /* mono output */
			paFloat32,  /* 32 bit floating point output */
			16000,		/* 16000 samples per second */
			512,		/* frames per buffer */
			call,
			data /* data pointer */ );
	}

	if( err != paNoError ) {
		soundErrorTerminate( err );
		str = NULL;
	}

	return str;
}


PaError soundDeinit( PaStream * str )
{
	PaError err = paNoError;

	err = Pa_StopStream( str );
	if( err == paNoError ) {
	    err = Pa_CloseStream( str );
	}
	if( err != paNoError ) {
		soundErrorTerminate( err );
	}
	Pa_Terminate();
	return paNoError;
}

PaError soundStart( PaStream * str )
{
	PaError err;

	err = Pa_StartStream( str );
	if( err != paNoError ) {
		soundErrorTerminate( err );
		return( err );
	}
	return paNoError;
}
