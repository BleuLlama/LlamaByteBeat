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

#define kCursorCharA	"_"	/* make sure it's a "" string */
#define kCursorCharB	"#"	/* make sure it's a "" string */

/* ********************************************************************** */
/* Utility */


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
/* Structure */

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


	if( !gstr ) 
	{
		fprintf( stderr, "No glitch specified\n");
		return NULL;
	}

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
		fprintf( stderr, "Invalid format Error with name!\n" );
		fprintf( stderr, " (%s)\n", gstr );
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

		/* check for number (encoded as hex -- decode it) */
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
	

	glitchRefreshTPL( pg );

	return pg;
}


/* glitchRefreshTPL
 *
 *	refresh tokens per line count 
 */
void glitchRefreshTPL( pGlitch * pg )
{
	/* another quick pass to count the tokens per line */
	int l = 0;
	int i = 0;

	for( i=0 ; i<pg->nTokens ; i++ )
	{
		if( pg->tokens[i] == (-1 * '!')) {
			l++;
		} else {
			pg->tokensPerLine[l]++;
		}	
	}
}


/* glitchTokenToString
 *
 *	take in the token (URI version), and return a decoded string 
 */
char * glitchTokenToString( long t )
{
	switch( t )
	{
	case( 'a' ):	return "t";		/* OP_T */

	case( 'b' ):	return "PUT";		/* OP_PUT */
	case( 'c' ):	return "DROP";		/* OP_DROP */

	case( 'd' ):	return "*";		/* OP_MUL */
	case( 'e' ):	return "/";		/* OP_DIV */
	case( 'f' ):	return "+";		/* OP_ADD */
	case( 'g' ):	return "-";		/* OP_SUB */

	case( 'h' ):	return "%";		/* OP_MOD */
	case( 'j' ):	return "<<";		/* OP_LSHIFT */
	case( 'k' ):	return ">>";		/* OP_RSHIFT */

	case( 'l' ):	return "&";		/* OP_AND */
	case( 'm' ):	return "|";		/* OP_OR */
	case( 'n' ):	return "^";		/* OP_XOR */

	case( 'o' ):	return "~";		/* OP_NOT */

	case( 'p' ):	return "DUP";		/* OP_DUP */
	case( 'q' ):	return "PICK";		/* OP_PICK */
	case( 'r' ):	return "SWAP";		/* OP_SWAP */

	case( 's' ):	return "<";		/* OP_LT */
	case( 't' ):	return ">";		/* OP_GT */
	case( 'u' ):	return "=";		/* OP_EQ */

	/* and for completeness... */
	case( '!' ):	return "\\n";		/* OP_NOP */
	case( '.' ):	return ".";		/* OP_NOP */

	/* tenative opcodes */
		/* accelerometer */
	case ( 'X' ):	return "Xa";		/* OP_ACCX */
	case ( 'Y' ):	return "Ya";		/* OP_ACCY */
	case ( 'Z' ):	return "Za";		/* OP_ACCZ */

		/* touch pad 1 */
	case ( 'x' ):	return "X1";		/* OP_PAD1X */
	case ( 'y' ):	return "Y1";		/* OP_PAD1X */

		/* touch pad 2 */
	case ( 'v' ):	return "X2";		/* OP_PAD2X */
	case ( 'w' ):	return "Y2";		/* OP_PAD2X */
	}
	return "?UNK";
}


/* ********************************************************************** */
/* Runtime */

/* glitchPush
 *
 *	push an item onto the glitch stack
 */
void glitchPush( pGlitch *pg, long v )
{
	if( !pg ) return;

	pg->stack[ pg->sp & 0x0ff ] = v;
	pg->sp++;
}


/* glitchPop
 *
 *	remove an item from the glitch stack
 */
long glitchPop( pGlitch *pg )
{
	if( !pg ) return 0;

	pg->sp--;
	
	return pg->stack[ pg->sp & 0x0ff ];
}


/* glitchPick
 *
 *	pick an item off of the stack
 */
long glitchPick( pGlitch *pg, long idx )
{
	if( !pg ) return 0;
	idx = ( pg->sp - idx -1 ) & 0xff;

	return pg->stack[ idx ];
}


/* glitchPut
 *
 *	put an item off of the stack
 */
void glitchPut( pGlitch *pg, long idx, long v )
{
	if( !pg ) return;
	idx = ( pg->sp - idx -1 ) & 0xff;
	pg->stack[ idx ] = v;
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
	
	for(pp = 0 ; pp < pg->nTokens ; pp++ )
	{
		/* in the program array, numbers are stored as 0..FF */
		/* tokens are stored as -1 * OP_TOKEN */
		switch( pg->tokens[pp] * -1 )
		{
		/* a - OP_T */
		case( 'a' ):
			glitchPush( pg, t );
			break;

		/* b - OP_PUT */
		case( 'b' ): 
			a = glitchPop( pg );
			b = glitchPop( pg );
			glitchPush( pg, b ); /* put it back on the stack - no effect to it */
			glitchPut( pg, b, a );
			break;

		/* c - OP_DROP */
		case( 'c' ):
			a = glitchPop( pg );
			break;

		/* d - OP_MUL */
		case( 'd' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			glitchPush( pg, a*b );
			break;

		/* e - OP_DIV */
		case( 'e' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( a > 0 ) glitchPush( pg, b/a );
			else glitchPush( pg, 0 );
			break;

		/* f - OP_ADD */
		case( 'f' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			glitchPush( pg, a+b );
			break;

		/* g - OP_SUB */
		case( 'g' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			glitchPush( pg, a-b );
			break;

		/* h - OP_MOD */
		case( 'h' ): 
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( a > 0 ) glitchPush( pg, b%a );
			else glitchPush( pg, 0 );
			break;

		/* j - OP_LSHIFT */
		case( 'j' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( a >= 32 ) glitchPush( pg, 0 );
			else {
				for( z=0 ; z<a ; z++ ) b<<=1;
				glitchPush( pg, b );
			}
			break;

		/* k - OP_RSHIFT */
		case( 'k' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( a >= 32 ) glitchPush( pg, 0 );
			else {
				for( z=0 ; z<a ; z++ ) b>>=1;
				glitchPush( pg, b );
			}
			break;

		/* l - OP_AND */
		case( 'l' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			glitchPush( pg, a&b );
			break;

		/* m - OP_OR */
		case( 'm' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			glitchPush( pg, a|b );
			break;

		/* n - OP_XOR */
		case( 'n' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			glitchPush( pg, b^a );
			break;

		/* o - OP_NOT */
		case( 'o' ):
			a = glitchPop( pg );
			glitchPush( pg, ~a );
			break;

		/* p - OP_DUP */
		case( 'p' ):
			a = glitchPop( pg );
			glitchPush( pg, a );
			glitchPush( pg, a );
			break;

		/* q - OP_PICK */
		case( 'q' ):
			a = glitchPop( pg );
			glitchPush( pg, glitchPick( pg, a ));
			break;

		/* r - OP_SWAP */
		case( 'r' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			glitchPush( pg, a );
			glitchPush( pg, b );
			break;

		/* s - OP_LT */
		case( 's' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( b<a ) glitchPush( pg, 0xffffffff );
			else glitchPush( pg, 0 );
			break;
		/* t - OP_GT */
		case( 't' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( b>a ) glitchPush( pg, 0xffffffff );
			else glitchPush( pg, 0 );
			break;
		/* u - OP_EQ */
		case( 'u' ):
			a = glitchPop( pg );
			b = glitchPop( pg );
			if( b==a ) glitchPush( pg, 0xffffffff );
			else glitchPush( pg, 0 );
			break;

		/* ******* Tentative Opcodes ******* */
		/* X - OP_ACCX  -- accelerometer */
		case( 'X' ):
			glitchPush( pg, (pg->Xa & 0xff) );
			break;
			
		/* Y - OP_ACCY  -- accelerometer */
		case( 'Y' ):
			glitchPush( pg, (pg->Ya & 0xff) );
			break;
			
		/* Z - OP_ACCZ  -- accelerometer */
		case( 'Z' ):
			glitchPush( pg, (pg->Za & 0xff) );
			break;
			
		/* x - OP_PAD1X  -- touch pad 1 X */
		case( 'x' ):
			glitchPush( pg, (pg->X1 & 0xff) );
			break;

		/* y - OP_PAD1Y  -- touch pad 1 Y */
		case( 'y' ):
			glitchPush( pg, (pg->Y1 & 0xff) );
			break;
			
		/* x - OP_PAD2X  -- touch pad 2 X */
		case( 'v' ):
			glitchPush( pg, (pg->X2 & 0xff) );
			break;

		/* y - OP_PAD2Y  -- touch pad 2 Y */
		case( 'w' ):
			glitchPush( pg, (pg->Y2 & 0xff) );
			break;

		default:
			/* it's a number, just push it */
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


/* ********************************************************************** */
/* Display */

/* glitchLineToBufferskCursor
 *
 *	pretty-text a line to a buffer, with our without the cursor
 *	returns the line number if found, -1 if not found
 */
int glitchLineToBufferAskCursor( pGlitch * pg, int line, char * buf, int maxBuf, int showCursor )
{
	int l;
	int t;
	int tpl = 0;
	long tok;
	int cursorShown = 0;
	char tbuf[16];
	static int kc=0;

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
			if( showCursor == 1 ) {
				if( l == pg->cursorLine  &&  tpl == pg->cursorPos ) {
					if( kc & 1 ) {
						strncat( buf, kCursorCharA " ", maxBuf );
					} else {
						strncat( buf, kCursorCharB " ", maxBuf );
					}
					kc++;
					cursorShown = 1;
				} else {
					strncat( buf, "  ", maxBuf );
				}
			}
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
			if( showCursor == 0 ) {
				strncat( buf, " ", maxBuf );
			}
			tpl++;
		}

		/* check for newline */
		if( tok == (-1*'!') ) 
		{
			l++;
		}
	}

	/* check for end of line stuff for showing the cursor... */
	if( showCursor )
	{
		if( !cursorShown ) {
			/* check to see which line to put it on. */
			if( line == pg->cursorLine ) {
				if( kc & 1 ) {
					strncat( buf, kCursorCharA, maxBuf ); /* no need for extra space, since it's EOL */
				} else {
					strncat( buf, kCursorCharB, maxBuf );
				}
				kc++;
			}
		}

		/* need this to clear the space (perhaps more redraw/line clear is necessary?) */
		strncat( buf, "       ", maxBuf );
	}

	if( l == line ) return line;
	return -1;
}


/* glitchLineToBuffer
 *
 *	display a line without the cursor in it
 */
int glitchLineToBuffer( pGlitch * pg, int line, char * buf, int maxBuf )
{
	return glitchLineToBufferAskCursor( pg, line, buf, maxBuf, 0 );
}

/* glitchLineToBufferWithCursor
 *
 *	display a line with the cursor in it
 */
int glitchLineToBufferWithCursor( pGlitch * pg, int line, char * buf, int maxBuf )
{
	return glitchLineToBufferAskCursor( pg, line, buf, maxBuf, 1 );
}

/* glitchDump
 *
 *	Pretty-print the passed in glitch to stdout
 */
void glitchDump( pGlitch * pg )
{
	int i;
	int sp0;

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
	sp0 = pg->sp;
	printf( "      t=2: %ld\n", glitchEvaluate( pg, 2 ));
	/* leftover stack */
	printf( "    Stack: %d\n", pg->sp - sp0 );

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



/* ********************************************************************** */
/* Editing */

/* movement */

/* glitchCursorReset
 *
 *	moves the cursor to the start position
 */
void glitchCursorReset( pGlitch * pg )
{
	if( !pg ) return;

	pg->cursorPos = 0;
	pg->cursorLine = 0;
}


/* glitchCursorMoveStartOfLine
 *
 *	moves cursor to HOME for line (0)
 */
void glitchCursorMoveStartOfLine( pGlitch * pg )
{
	if( !pg ) return;

	pg->cursorPos = 0;
}


/* glitchCursorMoveEndOfLine
 *
 *	moves cursor to END for line
 */
void glitchCursorMoveEndOfLine( pGlitch * pg )
{
	if( !pg ) return;

	pg->cursorPos = pg->tokensPerLine[ pg->cursorLine ]; 
}


/* glitchCursorMoveOnLine
 *
 * 	moves the cursor on the current line
 */
void glitchCursorMoveOnLine( pGlitch * pg, int direction )
{
	if( !pg || direction == 0 ) return;

	if( direction < 0 ) {
		pg->cursorPos--;
		if( pg->cursorPos < 0 ) {
			pg->cursorPos = pg->tokensPerLine[ pg->cursorLine ]; 
		}
	} else {
		pg->cursorPos++;
		if( pg->cursorPos >pg->tokensPerLine[ pg->cursorLine ] ) {
			pg->cursorPos = 0;
		}
	}
}


/* glitchCursorMoveLines
 *
 * 	moves the cursor between lines
 */
void glitchCursorMoveLines( pGlitch * pg, int direction )
{
	if( !pg || direction == 0 ) return;

	if( direction > 0 ) pg->cursorLine++;
	else pg->cursorLine--;

	pg->cursorLine &= 0x0F;

	pg->cursorPos = 0;
}


/* Edit */


static int glitchIndexOfLineCol( pGlitch * pg, int line, int col  )
{
	int ll = 0;
	int cc = 0;
	int idx = 0;

	if( !pg ) return 0;


	for( idx = 0 ; idx < kGlitchMaxTokens ; idx++ )
	{
		if( ll == line && cc == col ) return idx;

		if( pg->tokens[idx] == (-1 * '!' )) {
			ll++;
			cc = 0;
		} else {
			cc++;
		}
	}
	
	
	return 0;
}

/* glitchSlideTokensIn
 *
 *	take everything from idx+1 to the end, and slide it in.
 *	removes the value at [idx]
 */
static void glitchSlideTokensIn( pGlitch * pg, int idx )
{
	int i;
	if( !pg || idx<0 || idx>kGlitchMaxTokens-1 || idx > pg->nTokens ) return;

	for( i=idx ; i<kGlitchMaxTokens-1 ; i++ ) {
		pg->tokens[i] = pg->tokens[i+1];
	}
	pg->nTokens--;
}


/* glitchSlideTokensOut
 *
 *	takes everything from idx to the end and moves it out
 *	inserts a '.' nop character in its place
 */
static void glitchSlideTokensOut( pGlitch * pg, int idx )
{
	int i;
	if( !pg || idx<0 || idx>kGlitchMaxTokens-1 || idx > pg->nTokens ) return;


	for( i=kGlitchMaxTokens-2 ; i > idx ; i-- ) {
		pg->tokens[i+1] = pg->tokens[i];
	}
	pg->tokens[idx] = 69;

	if( pg->nTokens < kGlitchMaxTokens ) pg->nTokens++;
}


/* glitchInsert
 *
 *	Insert the opcode or number at the current position
 */
void glitchInsert( pGlitch * pg, long v )
{
	int idx;
	if( !pg ) return;

	idx = glitchIndexOfLineCol( pg, pg->cursorLine, pg->cursorPos );
	glitchSlideTokensOut( pg, idx );
	pg->tokens[idx] = v;
	pg->cursorPos++;
}

/* glitchAppendToNumber
 *
 *	curr *= 10;  curr += v;
 */ 
void glitchAppendToNumber( pGlitch * pg, long v )
{
	int idx;
	if( !pg ) return;

	idx = glitchIndexOfLineCol( pg, pg->cursorLine, pg->cursorPos );
	if( idx == 0 ) {
		glitchInsert( pg, v );
		return;
	}

	pg->tokens[idx-1] *= 10;
	pg->tokens[idx-1] += v;
}


/* glitchRemovePrevious (Backspace)
 *
 *	Remove the previous token on the current line
 *	If at the beginning of the line, do nothing
 */
void glitchRemovePrevious( pGlitch * pg )
{
	int idx;
	if( !pg ) return;

	idx = glitchIndexOfLineCol( pg, pg->cursorLine, pg->cursorPos );

	if( idx < 1 ) return;
	if( pg->tokens[idx-1] == ( -1 * '!' ) ) return;
		
	pg->cursorPos--;
	glitchSlideTokensIn( pg, idx-1 );

	glitchRefreshTPL( pg );
}


/* glitchRemoveNext (Delete)
 *
 *	Remove the next token on the current line
 *	If at the end of line, do nothing
 */
void glitchRemoveNext( pGlitch * pg )
{
	int idx;
	if( !pg ) return;

	idx = glitchIndexOfLineCol( pg, pg->cursorLine, pg->cursorPos );

	if( pg->tokens[idx] == ( -1 * '!' ) ) return;
		
	glitchSlideTokensIn( pg, idx );

	glitchRefreshTPL( pg );
}
