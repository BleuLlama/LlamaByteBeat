/*
	Glitch
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
#include <math.h>
#include <stdlib.h>

#include <ctype.h>	/* for toupper() */
#include <string.h>	/* for strchr() */
#include "glitch.h"

/* ********************************************************************** */


/* stristr
 *
 *	case insensitive strstr, yoinked off the net.
 */ 
static const char *stristr( const char *haystack, const char *needle )
{
   if ( !*needle )
   {
      return haystack;
   }
   for ( ; *haystack; ++haystack )
   {
      if ( toupper(*haystack) == toupper(*needle) )
      {
         /*
          * Matched starting char -- loop through remaining chars.
          */
         const char *h, *n;
         for ( h = haystack, n = needle; *h && *n; ++h, ++n )
         {
            if ( toupper(*h) != toupper(*n) )
            {
               break;
            }
         }
         if ( !*n ) /* matched all of 'needle' to null termination */
         {
            return haystack; /* return the start of the match */
         }
      }
   }
   return 0;
}

/* ********************************************************************** */


/* struct pGlitch
 *
 *	This is our structure where we hold everything associated
 *	with a parsed in glitch 
 */
#ifdef NEVER
#define kGlitchMaxName	20	/* really, it's 16, but let's relax that */
#define kGlitchMaxSteps	512	/* really it's 256, but let's relax that */

typedef struct pGlitch {
	char name[kGlitchMaxName];
	long tokens[kGlitchMaxSteps];
	int nTokens;

	long stack[256];
	int sp;
} pGlitch;
#endif


/* glitchDestroy
 *
 *	destroy/free a passed in structure
 *	the passed in pointer is invalid after this call
 */
void glitchDestroy( pGlitch * pg )
{
	if( pg )
	{
		free( pg );
	}
}


/* glitchParse
 *
 *	Take in a URI or coded string, and return a new
 *	glitch structure, filled in appropriately, or 
 *	NULL if it failed for whatever reason
 *	anything allocated needs to be freed with gitchDestroy()
 */
pGlitch* glitchParse( char * gstr )
{
	pGlitch * pg = NULL;
	int i;
	int lastWasNumber;
	long v;
	char * ps;


	/* check to see if there's a "glitch://" on the front,
		skip it if there is.
	*/
	if( stristr( gstr, "glitch://" ) == gstr ) {
		/* skip the schema:// */
		gstr += 9;
	}

	/* allocate space for the struct */
	pg = (pGlitch *)calloc( 1, sizeof( struct pGlitch ));

	/* okay.  first, let's extract the name */

	/* seek to the first ! for the name */
	ps = strchr( gstr, '!' );
	if( ps == NULL ) {
		printf( "Invalid format Error with name!\n" );
		glitchDestroy( pg );
		return NULL;
	}

	/* copy the name over */
	for( i=0 ; gstr != ps ; gstr++, i++ )
	{
		pg->name[i] = *gstr;
	} 
	gstr++; /* go past the first ! */

	/* okay. we have the name extracted.  Now, let's walk through
	   the rest of the string, and extract ze tokenz. */
	pg->nTokens = 0;
	lastWasNumber = 0;
	while( *gstr ) {
		v = *gstr++;

		/* check for number (encoded as hex) */
		if( v >= '0' && v <= '9' ) {
			pg->tokens[pg->nTokens] <<= 4;
			pg->tokens[pg->nTokens] += v - '0';
			lastWasNumber = 1;
		} else 
		if( v >= 'A' && v <= 'F' ) {
			pg->tokens[pg->nTokens] <<= 4;
			pg->tokens[pg->nTokens] += v - 'A' + 10;
			lastWasNumber = 1;
		} else {
			if( lastWasNumber ) {
				pg->nTokens++;
			}

			/* tis not a number, it's a token */
			if( v != '.' ) { /* don't add if it's the number separator */
			    pg->tokens[pg->nTokens] = -v;
			    pg->nTokens++;
			}

			lastWasNumber = 0;
		}
	}
	if( lastWasNumber ) { pg->nTokens++; }  /* final bit if we end on a number */
	

	/* and return the struct */
	return pg;
}


/* glitchTokenToString
 *
 *	take in the token (URI version), and return a decoded string 
 */
char * glitchTokenToString( long t )
{
	switch( t )
	{
	case( 'a' ):	return "t";

	case( 'd' ):	return "*";
	case( 'e' ):	return "/";
	case( 'f' ):	return "+";
	case( 'g' ):	return "-";

	case( 'h' ):	return "%";
	case( 'j' ):	return "<<";
	case( 'k' ):	return ">>";

	case( 'l' ):	return "&";
	case( 'm' ):	return "|";
	case( 'n' ):	return "^";

	case( 'o' ):	return "~";

	case( 'p' ):	return "DUP";
	case( 'q' ):	return "PICK";
	case( 'r' ):	return "SWAP";

	case( 's' ):	return "<";
	case( 't' ):	return ">";
	case( 'u' ):	return "=";

	/* and for completeness... */
	case( '!' ):	return "\\n";
	case( '.' ):	return ".";
	}
	return "?UNK";
}


/* glitchPush
 *
 *	push an item onto the glitch stack
 */
void glitchPush( pGlitch *pg, long v )
{
	if( !pg ) return;

	pg->stack[ pg->sp ] = v;
	pg->sp++;
	pg->sp &= 0xff; /* stay within 0..255 */

}


/* glitchPop
 *
 *	remove an item from the glitch stack
 */
long glitchPop( pGlitch *pg )
{
	if( !pg ) return 0;

	pg->sp--;
	pg->sp &= 0xff;
	
	return 0xff & pg->stack[ pg->sp ];
}


/* glitchPick
 *
 *	pick an item off of the stack
 */
long glitchPick( pGlitch *pg, long idx )
{
	if( !pg ) return 0;
	idx = ( pg->sp - idx -1 ) & 0xff;

	return 0xff & pg->stack[ idx ];
}


/* glitchEvaluate
 *
 * 	evaluate the stored formula with the passed in 't'
 */
long glitchEvaluate( pGlitch * pg, long t )
{
	int pp = 0;
	int z;
	long a, b;
	if( !pg ) return 0;
	
	pg->sp = 0; /* reset the stack pointer */

	for(pp = 0 ; pp < pg->nTokens ; pp++ )
	{
		switch( pg->tokens[pp] * -1 )
		{
		case( 'a' ): glitchPush( pg, t ); break;

		case( 'd' ): a = glitchPop( pg ); b = glitchPop( pg ); glitchPush( pg, a*b ); break;
		case( 'e' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( a > 0 ) glitchPush( pg, b/a );
			else glitchPush( pg, 0 );
			break;
		case( 'f' ): a = glitchPop( pg ); b = glitchPop( pg ); glitchPush( pg, a+b ); break;
		case( 'g' ): a = glitchPop( pg ); b = glitchPop( pg ); glitchPush( pg, a-b ); break;

		case( 'h' ): 
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( a > 0 ) glitchPush( pg, b%a );
			else glitchPush( pg, 0 );
			break;

		case( 'j' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( a >= 32 ) glitchPush( pg, 0 );
			else {
				for( z=0 ; z<a ; z++ ) b<<=1;
				glitchPush( pg, b );
			}
			break;
		case( 'k' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( a >= 32 ) glitchPush( pg, 0 );
			else {
				for( z=0 ; z<a ; z++ ) b>>=1;
				glitchPush( pg, b );
			}
			break;

		case( 'l' ): a = glitchPop( pg ); b = glitchPop( pg ); glitchPush( pg, a&b ); break;
		case( 'm' ): a = glitchPop( pg ); b = glitchPop( pg ); glitchPush( pg, a|b ); break;
		case( 'n' ): a = glitchPop( pg ); b = glitchPop( pg ); glitchPush( pg, b^a ); break;

		case( 'o' ): a = glitchPop( pg ); glitchPush( pg, ~a ); break;

		case( 'p' ): a = glitchPop( pg ); glitchPush( pg, a ); glitchPush( pg, a ); break;
		case( 'q' ): a = glitchPop( pg ); glitchPush( pg, glitchPick( pg, a )); break;
		case( 'r' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			glitchPush( pg, a );
			glitchPush( pg, b );
			break;

		case( 's' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( b<a ) glitchPush( pg, 0xffffffff );
			else glitchPush( pg, 0 );
			break;
		case( 't' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( b>a ) glitchPush( pg, 0xffffffff );
			else glitchPush( pg, 0 );
			break;
		case( 'u' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( b==a ) glitchPush( pg, 0xffffffff );
			else glitchPush( pg, 0 );
			break;

		default:
			if( pg->tokens[pp] >= 0 ) {
				glitchPush( pg, pg->tokens[pp] );
			}
		}
	}

	return( glitchPick( pg, 0 ));
}


/* glitchCountUseTokens
 *
 *	rather than the nTokens in the struct, which includes
 *	newline tokens, this one returns only the usable tokens
 *	(count of every token minus newline tokens)
 */
int glitchCountUseTokens( pGlitch * pg )
{
	int v = 0;
	int i;

	if( !pg ) return 0;

	for( i=0 ; i < pg->nTokens ; i++ )
	{
		/* count all, but ignore newlines */
		if( pg->tokens[i] != (-1 * '!' ) ) v++;
			
	}
	return v;
}


/* glitchLineToBuffer
 *
 *	pretty-text a line to a buffer
 *	returns the line number if found, -1 if not found
 */
int glitchLineToBuffer( pGlitch * pg, int line, char * buf, int maxBuf )
{
	int l;
	int t;
	long tok;
	char tbuf[16];

	if( !buf ) return -1;
	buf[0] = '\0';

	if( !pg ) return -1;

	/* walk through the token list for the specified line */

	l = 0;
	for( t=0 ; t<pg->nTokens ; t++ )
	{
		tok = pg->tokens[t];

		/* we're in the right line! dump that stuff out! */
		if( l == line ) {
			if( tok < 0 ) {
				if( tok == ( -1 * '!' ) ) {
					return line; 
				} else {
					strncat( buf, glitchTokenToString( -1 * tok ), maxBuf );
				}
			} else {
				snprintf( tbuf, 16, "%ld", tok );
				strncat( buf, tbuf, maxBuf );
			}
			strncat( buf, " ", maxBuf );
		}

		/* check for newline */
		if( tok == (-1*'!') ) 
		{
			l++;
		}
	}

	if( l == line ) return line;
	return -1;
}


/* glitchDump
 *
 *	Pretty-print the passed in glitch to stdout
 */
void glitchDump( pGlitch * pg )
{
	int i;

	if( !pg ) {
		printf( "NULL glitch passed in.\n" );
		return;
	}

	/* meta */
	printf( "     Name: %s\n", pg->name );
	printf( "   Tokens: %d (%d in-use)\n", pg->nTokens, glitchCountUseTokens( pg ) );
	/* some example datapoints */
	printf( "      t=0: %ld\n", glitchEvaluate( pg, 0 ));
	printf( "      t=1: %ld\n", glitchEvaluate( pg, 1 ));
	printf( "      t=2: %ld\n", glitchEvaluate( pg, 2 ));
	/* leftover stack */
	printf( "    Stack: %d\n", pg->sp );

	/* program */
	printf( "  Listing:\n" );

	for( i=0 ; i<16 ; i++ ) {
		char buf[ 512 ];
		if( glitchLineToBuffer( pg, i, buf, 512 ) >= 0 )
		{
			printf( "          %2d: %s\n", i, buf );
		}
	}


	/* dump out a whole mess of data */
	/*
	for( v=0 ; v<100 ; v++ ) {
		printf( "%2ld:%2ld ", v, glitchEvaluate( pg, v ));
	}
	printf( "\n" );
	*/

}
