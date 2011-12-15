#include <stdio.h>
#include "SDL/SDL.h"
#include "SDL/SDL_audio.h"
 
int t=0;
 
void callbackfn(void *unused, Uint8 *stream, int len);
 
int main( int argc, char **argv ){
    SDL_AudioSpec fmt;
 
    fmt.freq = 8000;
    fmt.format = AUDIO_U8;
    fmt.channels = 1;
    fmt.samples = 512;
    fmt.callback = callbackfn;
    fmt.userdata = NULL;
 
    if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_PauseAudio(0);
 
    getch();
 
    SDL_CloseAudio();
    return 0;
}
 
void callbackfn(unused, stream, len)
void *unused;
Uint8 *stream;
int len;
{
        int i;
        for( i=0; i < len; i++,t++) {
		/*
                stream[i] = (t*5&(t>>7))|(t*3&(t>>8));
		*/
		stream[i] = (( t >> 10 ) & 42) * t;
        }
}
