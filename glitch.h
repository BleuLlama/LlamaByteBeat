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



/* ********************************************************************** */

/* struct pGlitch
 *
 *	This is our structure where we hold everything associated
 *	with a parsed in glitch 
 */
#define kGlitchMaxName	20	/* really, it's 16, but let's relax that */
#define kGlitchMaxSteps	512	/* really it's 256, but let's relax that */

typedef struct pGlitch {
	char name[kGlitchMaxName];
	long tokens[kGlitchMaxSteps];
	int nTokens;

	long stack[256];
	int sp;
} pGlitch;


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


/* glitchTokenToString
 *
 *	take in the token (URI version), and return a decoded string 
 */
char * glitchTokenToString( long t );


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


/* glitchLineToBuffer
 *
 *	pretty-text a line to a buffer
 *	returns the line number if found, -1 if not found
 */
int glitchLineToBuffer( pGlitch * pg, int line, char * buf, int maxBuf );


/* glitchDump
 *
 *	Pretty-print the passed in glitch to stdout
 */
void glitchDump( pGlitch * pg );
