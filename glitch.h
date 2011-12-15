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


#include <stdint.h>

/* ********************************************************************** */
/* Structure */

/* struct pGlitch
 *
 *	This is our structure where we hold everything associated
 *	with a parsed in glitch 
 */
#define kGlitchMaxName		20	/* really, it's 16, but let's relax that */
#define kGlitchMaxTokens	512	/* really it's 256, but let's relax that */
#define kGlitchMaxLines		20	/* really, it's 16, but let's relax that */

typedef struct pGlitch {
	/* storage */
	char name[kGlitchMaxName];
	long tokens[kGlitchMaxTokens];
	int nTokens;

	/* for runtime */
	uint32_t stack[0xff];	/* stack.  sp is masked with 0xff -- circular */
	int sp;			/* current stack pointer, persistant between calls */

	/* for editing */
	int tokensPerLine[kGlitchMaxLines];
	int cursorLine;
	int cursorPos;
} pGlitch;


/* opcode list */
#define kOP_T		(-1 * 'a')
#define kOP_PUT		(-1 * 'b')
#define kOP_DROP	(-1 * 'c')
#define kOP_MUL		(-1 * 'd')
#define kOP_DIV		(-1 * 'e')
#define kOP_ADD		(-1 * 'f')
#define kOP_SUB		(-1 * 'g')
#define kOP_MOD		(-1 * 'h')
#define kOP_LSHIFT	(-1 * 'j')
#define kOP_RSHIFT	(-1 * 'k')
#define kOP_AND		(-1 * 'l')
#define kOP_OR		(-1 * 'm')
#define kOP_XOR		(-1 * 'n')
#define kOP_NOT		(-1 * 'o')
#define kOP_DUP		(-1 * 'p')
#define kOP_PICK	(-1 * 'q')
#define kOP_SWAP	(-1 * 'r')
#define kOP_LT		(-1 * 's')
#define kOP_GT		(-1 * 't')
#define kOP_EQ		(-1 * 'u')
#define kOP_NEWLINE	(-1 * '!')
#define kOP_NUMNOP	(-1 * '.')


/* glitchDestroy
 *
 *	destroy/free a passed in structure
 *	the passed in pointer is invalid after this call
 */
void glitchDestroy( pGlitch * pg );


/* glitchParse
 *
 *	Take in a URI or coded string, and return a new
 *	glitch structure, filled in appropriately, or 
 *	NULL if it failed for whatever reason
 *	anything allocated needs to be freed with gitchDestroy()
 */
pGlitch* glitchParse( char * gstr );


/* glitchRefreshTPL
 *
 *	refresh tokens per line count 
 */
void glitchRefreshTPL( pGlitch * pg );


/* glitchTokenToString
 *
 *	take in the token (URI version), and return a decoded string 
 */
char * glitchTokenToString( long t );


/* ********************************************************************** */
/* Runtime */

/* glitchPush
 *
 *	push an item onto the glitch stack
 */
void glitchPush( pGlitch *pg, long v );


/* glitchPop
 *
 *	remove an item from the glitch stack
 */
long glitchPop( pGlitch *pg );


/* glitchPick
 *
 *	pick an item off of the stack
 */
long glitchPick( pGlitch *pg, long idx );


/* glitchPut
 *
 *	put an item onto the stack (overwrite)
 */
void glitchPut( pGlitch *pg, long idx, long v );


/* glitchEvaluate
 *
 * 	evaluate the stored formula with the passed in 't'
 */
long glitchEvaluate( pGlitch * pg, long t );


/* glitchCountUseTokens
 *
 *	rather than the nTokens in the struct, which includes
 *	newline tokens, this one returns only the usable tokens
 *	(count of every token minus newline tokens)
 */
int glitchCountUseTokens( pGlitch * pg );


/* ********************************************************************** */
/* Display */

/* glitchLineToBufferskCursor
 *
 *      pretty-text a line to a buffer, with our without the cursor
 *      returns the line number if found, -1 if not found
 */
int glitchLineToBufferAskCursor( pGlitch * pg, int line, char * buf, int maxBuf, int showCursor );


/* glitchLineToBuffer
 *
 *	pretty-text a line to a buffer
 *	returns the line number if found, -1 if not found
 */
int glitchLineToBuffer( pGlitch * pg, int line, char * buf, int maxBuf );


/* glitchLineToBufferWithCursor
 *
 *      display a line with the cursor in it
 */
int glitchLineToBufferWithCursor( pGlitch * pg, int line, char * buf, int maxBuf );


/* glitchDump
 *
 *	Pretty-print the passed in glitch to stdout
 */
void glitchDump( pGlitch * pg );


/* ********************************************************************** */
/* Editing */

/* movement */

/* glitchCursorReset
 *
 *	moves the cursor to the start position
 */
void glitchCursorReset( pGlitch * pg );


/* glitchCursorMoveStartOfLine
 *
 *	moves cursor to HOME for line (0)
 */
void glitchCursorMoveStartOfLine( pGlitch * pg );


/* glitchCursorMoveEndOfLine
 *
 *	moves cursor to END for line
 */
void glitchCursorMoveEndOfLine( pGlitch * pg );


/* glitchCursorMoveOnLine
 *
 * 	moves the cursor on the current line
 */
void glitchCursorMoveOnLine( pGlitch * pg, int direction );


/* glitchCursorMoveLines
 *
 * 	moves the cursor between lines
 */
void glitchCursorMoveLines( pGlitch * pg, int direction );


/* edit */

/* glitchInsert
 *
 *	Insert the opcode or number at the current position
 */
void glitchInsert( pGlitch * pg, long v );


/* glitchAppendToNumber
 *
 *	curr *= 10;  curr += v;
 */ 
void glitchAppendToNumber( pGlitch * pg, long v );


/* glitchRemovePrevious (Backspace)
 *
 *	Remove the previous token on the current line
 *	If at the beginning of the line, do nothing
 */
void glitchRemovePrevious( pGlitch * pg );


/* glitchRemoveNext (Delete)
 *
 *	Remove the next token on the current line
 *	If at the end of line, do nothing
 */
void glitchRemoveNext( pGlitch * pg );
