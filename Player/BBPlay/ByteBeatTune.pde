// ByteBeatTune
//
//  Parses in a glitch: url to an array
//  allows you to evaluate a number
//
//  2011 Scott Lawrence (BleuLlama/@Yorgle)
//  yorgle@gmail.com
//
//  This is released under an MIT license.



class ByteBeatTune
{
  String original;
  String name;
  String[] description = new String[20];
  boolean ok;
  
  String[] p;
  int maxSz = 512;
  int nTokens = 0;

  int Xa,Ya,Za;
  int X1,Y1;
  int X2,Y2;
  
  ByteBeatTune( String t )
  {
    ok = false;
    parse( t );
    
  }
  
  void parse( String t )
  {
    original = t;
    name = "";
    p = new String[300];
    
    if( t.toUpperCase().indexOf( "GLITCH://" ) != 0 ) {
      /*
      println( "ERROR PARSING: missing glitch://" );
      ok = false;
      return;
      */
      /* nope.  now it's okay to have glitch:// missing. */
      t = "glitch://" + t;
    }
    
    int firstBang = t.indexOf( "!", 8 );
    if( firstBang < 0 ) {
      println( "FORMAT ERROR" );
      ok = false;
    }
    
    name = t.substring( 9, firstBang ).replace( '_', ' ' );

    
    println( "Song: " + t );
//    println ("Song title is \"" + name + "\"" );
    
    int idx = 9 + name.length();
    int pAddr = 0;
    boolean lastWasNumber = false;
    nTokens = 0;
    
    for( int i=idx+1 ; i < t.length() ; i++ )
    {
      String token =  t.substring( i, i+1 );
      
      if( token.equals( "#" )) {
        i = t.length();  // skip to the end
        
      } else if( token.equals( "." )) {
        // do nothing 
        lastWasNumber = false;
        pAddr++;
        
      } else if( token.matches( "[a-z!]" )) {
        p[ pAddr ] = token;
        lastWasNumber = false;
        pAddr++;
        nTokens++;
        
      } else {
        // numeric?
        if( lastWasNumber ) {
          pAddr--;
          p[ pAddr ] += token;
        } else {
          p[ pAddr ] = token;
          nTokens++;
        }
        lastWasNumber = true;
        pAddr++;
      }
    }
    
    /*
    for( int j =0 ; j < 256 ; j++ )
    {
      if( p[j] != null  ) {
        print( " " + p[j] + " " );
      }
    }
    println( " -- " + nTokens + " tokens" );
    */
    
    /*
    for( int x=0 ; x<4 ; x++ ) {
      long v = evaluate( x );
      println( " " + x + ": " + v );
    }
    */
    makeDescription();
  }
  
  long[] stack = new long[256];
;
  int stackP = 0;
  
  void push( long v )
  {
    v &= 0x00ffffffff;
    stack[stackP & 0x0ff] = v;
    stackP++;
  }
  
  long pop()
  {
    stackP--;
    return( stack[stackP & 0x0ff] );
  }
  
  long peek( long v )
  {
    // 0 = top, 1 = 1 down, etc.
    long idx = stackP-(v+1);
    
    return( stack[int(idx) & 0x0ff] );
  }
  
  void makeDescription()
  {
    int l = 0;
    
    for( int i=0 ; i<description.length ; i++ )
    {
      description[i] = "";
    }
    
    for( int pp = 0 ; pp < p.length ; pp++ )
    {      
      if( p[pp] != null && p[pp] != "" ) {
        if( p[pp].equals("!") ) {
          l++;
        } 
        else if( p[pp].equals("a") ) { description[l] += "t "; }

        else if( p[pp].equals("b") ) { description[l] += "PUT "; } /* undefined behaviors */
        else if( p[pp].equals("c") ) { description[l] += "DROP "; } /* undefined behaviors */
        
        else if( p[pp].equals("d") ) { description[l] += "* "; }
        else if( p[pp].equals("e") ) { description[l] += "/ "; }
        else if( p[pp].equals("f") ) { description[l] += "+ "; }
        else if( p[pp].equals("g") ) { description[l] += "- "; }
        
        else if( p[pp].equals("h") ) { description[l] += "% "; }
        else if( p[pp].equals("j") ) { description[l] += "<< "; }
        else if( p[pp].equals("k") ) { description[l] += ">> "; }
        
        else if( p[pp].equals("l") ) { description[l] += "& "; }
        else if( p[pp].equals("m") ) { description[l] += "| "; }
        else if( p[pp].equals("n") ) { description[l] += "^ "; }

    
        else if( p[pp].equals("o") ) { description[l] += "~ "; }

        else if( p[pp].equals("p") ) { description[l] += "DUP "; }
        else if( p[pp].equals("q") ) { description[l] += "PICK "; }
        else if( p[pp].equals("r") ) { description[l] += "SWAP "; }

        else if( p[pp].equals("s") ) { description[l] += "< "; }
        else if( p[pp].equals("t") ) { description[l] += "> "; }
        else if( p[pp].equals("u") ) { description[l] += "= "; }

        else if( p[pp].equals("v") ) { description[l] += "X2 "; }
        else if( p[pp].equals("w") ) { description[l] += "Y2 "; }
        else if( p[pp].equals("x") ) { description[l] += "X1 "; }
        else if( p[pp].equals("y") ) { description[l] += "Y1 "; }
        else if( p[pp].equals("X") ) { description[l] += "Xa "; }
        else if( p[pp].equals("Y") ) { description[l] += "Ya "; }
        else if( p[pp].equals("Z") ) { description[l] += "Za "; }
        
        else {
          // number
          int v = Integer.parseInt(p[pp], 16);
          description[l] += v;
          description[l] += " ";
        }
      }
      
    }
    
    // count proper tokens
    nTokens = 0;
    for( int pp = 0 ; pp< p.length ; pp++ )
    {
      if( p[pp] != null && p[pp].matches( "[a-z0-9A-F]+" )) {
        nTokens++;
      }
    }
  }
    
  long evaluate( long t )
  {
    long a;
    long b;
    
    // now, we just need to walk down the thing
    for( int pp = 0 ; pp < p.length ; pp++ )
    {
      String token = p[ pp ];      
      if( token != null ) {
        //print( token );
        if( token.matches( "[a-z!]" )) {
          
          if( token.equals( "v" )){  push( X2 ); }
          if( token.equals( "w" )){  push( Y2 ); }
          if( token.equals( "x" )){  push( X1 ); }
          if( token.equals( "y" )){  push( Y1 ); }
          if( token.equals( "X" )){  push( Xa ); }
          if( token.equals( "Y" )){  push( Ya ); }
          if( token.equals( "Z" )){  push( Za ); }
          
          if( token.equals( "a" )){  push( t ); }
          if( token.equals( "c" )){  a=pop(); } // c - drop
          if( token.equals( "d" )){  a=pop(); b=pop(); push( b*a ); }                                   // d  *
          if( token.equals( "e" )){  a=pop(); b=pop(); if( a>0 ) {push( b/a ); } else { push( 0 ); } }  // e  /
          if( token.equals( "f" )){  a=pop(); b=pop(); push( b+a ); }                                   // f  +
          if( token.equals( "g" )){  a=pop(); b=pop(); push( b-a ); }                                   // g  -
          
          if( token.equals( "h" )){  a=pop(); b=pop(); if( a>0 ) {push( b%a ); } else { push( 0 ); } }  // h  %
          if( token.equals( "j" )){ 
            a=pop();
            b=pop(); // workaround because java is annoying.
            println( "shift " + a + " " + b );
            if( a >= 32 ) {
              push( 0 );
            } else {
              for( int i=0 ; i < a ; i++ ) { b = b << 1; }
              push( b );
            }
          }    // j  <<
          
          if( token.equals( "k" )){  
           a=pop();
           b=pop(); // workaround because java is annoying.
           if( a >= 32 ) {
              push( 0 );
            } else {
              for( int i=0 ; i < a ; i++ ) { b = b >>> 1; }
              push( b );
            }
          }    // k  >>
          
          if( token.equals( "l" )){  a=pop(); b=pop(); push( b&a ); }      // l  &
          if( token.equals( "m" )){  a=pop(); b=pop(); push( b|a ); }      // m  |
          if( token.equals( "n" )){  a=pop(); b=pop(); push( b^a ); }      // n  ^
          
          if( token.equals( "o" )){  a=pop(); a^=0xff; push( a ); }        // ~
          
          if( token.equals( "p" )){  a=pop(); push(a); push(a); }          // p  DUP
          if( token.equals( "q" )){  a=pop(); push( peek(a) ); }            // q  PICK
          if( token.equals( "r" )){  a=pop(); b=pop(); push(a); push(b); } // r  SWAP
          
          if( token.equals( "s" )){  a=pop(); b=pop(); if( b<a ){ push( 0x00ffffffff ); }else{ push( 0 ); } }  // <
          if( token.equals( "t" )){  a=pop(); b=pop(); if( b>a ){ push( 0x00ffffffff ); }else{ push( 0 );  } } // > 
          if( token.equals( "u" )){  a=pop(); b=pop(); if( b==a ){ push( 0x00ffffffff); }else{ push( 0 );  } } // =
          // do the operation
        } else {
          // push back the number onto the stack
          try {
            push( Integer.parseInt( token, 16 ) );
          } finally {}
          //print( "(" + stack[stackP-1] + ")" );
        }
      }
    }
    
    return pop();
  }
}
