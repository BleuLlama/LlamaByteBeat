// BBPlay
//
//  Java player for bytebeat tunes
//
//  2011,2012 Scott Lawrence (BleuLlama/@Yorgle)
//  yorgle@gmail.com
//
//  This is released under an MIT license.

// 1.0  - 2011/12/09 first full release
// 1.01 -            volume slider knob, time display
// 1.02 - 2011/12/10 << and >> were backwards for program display
// 1.03 -            sound generatopm was the wrong rate
// 1.04 -            updated the font to "Ready-P9", removed others
// 1.05 -            typo with ^/~ fixed
// 1.06 - 2011/13/10 Stack issues fixed, no longer requires glitch://, OP_DROP added (b)
// 1.07 - 2012/05/22 Added pad and accelerometer support

import ddf.minim.*;
import ddf.minim.signals.*;


String[] algos = 
  { 
    // some of scott's discoveries
    "padTest!avl!awd!f",
    "guitar2!a3kal!a3000h400sl!80qD0h3d!ff4eFFl!pp",
    "glitch://Jenny!363.14BD",
    "glitch://fun!a309hamag",
    "glitch://phaser!a!pFFh80s!1q2dn",
    "glitch://example_sequencer!1.4.3.4.2.1.2.3!aoBk8hqad",
    
    // nyarla
    "glitch://2muchop!1!0!aFFe6had!!aaadda4eg8k!aada2000eh!a400e3h2fqad80h!0!!2.1.2.1.4.1.2.1!2.1.2.1.4.1.3.2!!a400e10hqFf!q",
    "glitch://glitch4life!a1k1000.8eha80!a80dmlp0k1000.2dh!mre",
    
    // from the youtube videos
    "glitch://genmusic2_atari!a7kaa6kmmAd!aaDka6kmmd",
    
    // madgarden
    "glitch://roboducky!a5k2ad35hd!aBk9l1fd!a4km!aBk13l1fadl!a11k5l1fd!a8kn",

    // misc demo tunes from Glitch Machine
    "glitch://the_42_melody!aAk2Alad",
    "glitch://barbarian2!a8k3h1f!aDk5l9gdad9e!p5fn!a6km",
    "glitch://bytescream!aAk!a5daf7kl!ad",
    "glitch://eerie_arpeggio!aCkAl1f!9f!a8Ce4l1fd!ad9e!p9fm!a6km20g",
    "glitch://glitch_machine!a10k4h1f!aAk5h2ff!aCk3hg!ad3e!p!9fm!a4kl13f!aCk7Fhn",
    "glitch://1916!aAk64h6Ergad14e!aAk64hAfad14e!g!p8dn!a7km!a5kf!aEk40hn",
    "glitch://octo!a2k14had!a2000he!a8!a11k3h1fde!m!aEk7Fhn!20g",
    "glitch://pipe_symphony!aEk5h5f!a11k2h!a9k3hdf!aDk4hg!ad4e!p5fm!a11k2h1rg!a5kdm",
    "glitch://query!aCk1l6d3f!a10k3hfad!p3dn!1g!a4km",
    "glitch://robot_toothbrush!a320eAm!ad55l!ag!a4el",
    "glitch://rolling!a9da4kl!a5da7kl!a3da400el!mm1g",
    "glitch://sergeant!a38da7Bhe!a2km!a6km!aDk3h1f!a10k5hf!ad!pDfm!n",
    "glitch://sidewalk!a5da9kl!a2da5kl!m!a11k5lad!a4kmm",
    "glitch://starlost!aFFha1FFhn3d",
    "glitch://wistful!aa!aEk4h5f!a13k1l1fd!Adhe!a5kl!a11k2lg"
  };
int currentAlgo = 0;

/////////////////////////////////////////////

Minim m;
AudioOutput out;
ToneThing tt;
ByteBeatTune bbt;

PFont font;
long t;
boolean drawVol = false;
int vis = 1;

int Xa, Ya, Za;
int X1, Y1, X2, Y2;

int versionShow;
String versionText = "v1.07 2012-May-22\n\nScott Lawrence\n\nyorgle@gmail.com\n\ntwitter.com/@yorgle";

/////////////////////////////////////////////


void setup()
{
  // screen setup
  size( 512, 256, P2D );
  strokeWeight( 1 );
  noSmooth();
  background( 0, 0, 64 );
  frameRate( 30 );
  t=0;
  
  // load the font
  font = loadFont( "Ready-P9-8.vlw" );
  textFont( font, 8 );
  textMode( SCREEN );
  
  // setup audio
  m = new Minim( this );
  out = m.getLineOut( Minim.STEREO, 512, 16000 );
  
  // set up slot 0
  String webAlgo = param( "glitch" );
  if( webAlgo != null )
  {
    algos[0] = webAlgo;
  }
  
  // set up the music
  selectAlgo( 0 );
  // 
}

void stop()
{
  out.close();
  m.stop();
  super.stop();
}

/////////////////////////////////////////////


void selectAlgo( int a )
{
  // remove the signal generator
  out.clearSignals();
  
  // remove the BBT
  if( tt != null ) {
    tt.setBBT( null );
  }
  bbt = null;
  
  if( a >= algos.length ) { a = 0; };
  if( a < 0 ) { a = algos.length-1; };
  
  bbt = new ByteBeatTune( algos[a] );
  if( tt == null ) {
    tt = new ToneThing();
  }
  tt.setBBT( bbt );
  out.addSignal( tt );
  currentAlgo = a;
  
  // update pads
  tt.updateAcc( Xa, Ya, Za );
  tt.updatePad1( X1, Y1 );
  tt.updatePad2( X2, Y2 );
}

void selectNextAlgo()
{
  selectAlgo( currentAlgo+1 );
}


void selectPrevAlgo()
{
  selectAlgo( currentAlgo-1 );
}


void keyPressed()
{
  if( key == '0' ) { tt.reset(); }
  if( key == 'v' ) { vis++; visTitleShow = 90; }
  if( key == 'p' ) { if( tt.pause ) { tt.pause = false; } else { tt.pause = true; } }
  if( key == '=' ) { selectNextAlgo(); tt.reset(); }
  if( key == '+' ) { selectNextAlgo(); }
  if( key == '-' ) { selectPrevAlgo(); tt.reset(); }
  if( key == 'm' ) { tt.volume = 0.0; }
  if( key == 's' ) { versionShow = 90; }
}

/////////////////////////////////////////////


String[] visNames = {
  "No visulaizer",
  "Linegraph",
  "Dotgraph",
  "VCS Bars"
};

int visTitleShow = 0;
void drawVis( int x, int y, int w, int h )
{
  int hw = w/2;

  pushMatrix();
  translate( x, y );
  
  // clear the view
  noStroke();
  fill( 0, 0, 0, 64 );
  rect( 0, 0, w, h );

  stroke( 0, 255, 0 );
      
  if( vis > 3 ) { vis = 0; } // bounds check

  if( vis == 0 ) { vis = 1; } // skip blank hack
  
  if( vis > 0 )
  {
    for( int i=0 ; i<255 ; i ++ )
    {      
      int vi = i*tt.sc;
      
      if( vis == 1 ) {
        // linegraph thing
        line( i*w/256,  h-out.left.get(vi)*h,  (i+1)*w/256, h-out.left.get(vi+tt.sc)*h );
      }

      if( vis == 2 ) {
        // dotgraph thing
        point( i * w / 256, h-out.left.get(vi)*h );
      }
      
      if( vis == 3 ) {
        // trippyboxes
        float csamp = out.left.get(vi);
        float psamp = csamp;
        
        csamp = csamp * 128;
        stroke( 128 - csamp, 128 + csamp, 192 - csamp );
        
        psamp = psamp * w/2; 
        line( w/2 - psamp, i*h/256,  w/2 + psamp, i*h/256 );
      }
    }
  }

  if( visTitleShow > 0 ) {
    visTitleShow--;
    fill( 255, 255, 0 );
    textAlign( CENTER );
    text( visNames[vis], w/2, h-10 );
  }

  popMatrix();
}

/////////////////////////////////////////////

String ticksToString( long ticks )
{
  long secs = ticks / 8000;
  
  long hours = secs / 3600,
       remainder = secs % 3600,
       minutes = remainder / 60,
       seconds = remainder % 60,
       hsec = (ticks % 8000) / 80;

  return ( (hours < 10 ? "0" : "") + hours
    + ":" + (minutes < 10 ? "0" : "") + minutes
    + ":" + (seconds < 10 ? "0" : "") + seconds
    + "." + (hsec < 10 ? "0" : "") + hsec
    );
  }

void drawHeading( int x, int y, int w, int h )
{
  pushMatrix();
  translate( x, y );
  
  // HUD  
  noStroke();
  
  // backfill
  fill( 0, 0, 64 );
  rect( 0, 0, w, h );
  
  // song title
  fill( 255 );
  textAlign( CENTER );
  text( "\"" + bbt.name + "\"", x+w/2, 27 );
  String st = " Tokens";
  if( bbt.nTokens == 1 ) {
    st = " Step";
  }
  
  // number of tokens
  textAlign( LEFT );
  fill( 255, 192, 128 );
  text( bbt.nTokens + st, x+3, 14 );
  
  // value
  fill( 255, 192, 128 );
  
  float v = out.left.get(frameCount % 256 );
  
  int val = int (v * 256);
  text( java.lang.Integer.toHexString(val).toUpperCase(), x+w/2 + 10, 14 );
  noFill();
  stroke( 255, 192, 128 );
  pushMatrix();
    translate( w/2-5, 5 );
    line( 0, 0, 0, 10 );
    line( 10, 0, 10, 10 );
    line( 2, 10 - v * 10,  8, 10 - v *10 );
  popMatrix();
  
  // time
  textAlign( RIGHT );
  text( ""+tt.ticks, x+w-3, 14 );
  text( ticksToString( tt.ticks ), x+w-3, 26 );

  // song text
  for( int l=0 ; l<bbt.description.length ; l++ )
  {
    if( (l & 1) != 1 ) {
      // formfeed boxes
      noStroke();
      fill( 255, 32 );
      rect( 0, 32 + (l*14), w, 14 );
    }
    
    // opcodes
    fill( 255, 192, 128 );
    textAlign( CENTER );
    text( bbt.description[l], x+w/2, 42 + (l*14) );
    
    // line number
    textAlign( RIGHT );
    text( "" + l, x+15, 42 + (l*14) );
  }
  
  popMatrix();
}

void draw()
{
  t++;
//  background( 0, 0, 0 );
  
  drawVis( 0, 0, 256, 256 );
  drawHeading( 256, 0, 256, 256 );
  
  // volume
  if( drawVol ) {
    fill( random(255), random(255), random(255), 64 );
    noStroke();
    rect( 0, height - (tt.volume*height), width, height );
    
    ellipseMode( CENTER );
    stroke( 128 );
    fill( 192 );
    rect( width-12, height - (tt.volume*height), 4, height+2 );
    ellipse( width-10, height - (tt.volume*height), 19, 19 );
  }
  
  if( versionShow > 0 ) {
    versionShow--;
    noStroke();
    textAlign( CENTER );
    fill( 255 );
    text( versionText, 128, 128 );
  }
  
  // draw pad dots
  
  noStroke();
  fill( 255, 64 );
  ellipseMode( CENTER );
  ellipse( X1, Y1, 10, 10 );
  ellipse( X2 + width/2, Y2, 10, 10 );
}

/////////////////////////////////////////////

void mouseDragged()
{ 
  // check for volume
  if( mouseY < 0 || mouseY > height ) {
    drawVol = false;
  }
  
  if( drawVol ) {
    if( mouseX > width - 20 && mouseX < width) {
      float v = 1.0 - (float(mouseY) / float(height));
      if( v<0 ) { v = 0.0; }
      if( v>1.0 ) { v = 1.0; }
      tt.volume = v;
      drawVol = true;
    } else {
      drawVol = false;
    }
  }
  
  // okay, handle pad stuff
  if( mouseX < (width/2) ) {
    X1 = int( float(mouseX)/float(width/2) * 255.0 );
    Y1 = int( float(mouseY)/float(height) * 255.0 );
    tt.updatePad1( X1, Y1 );
  } else {
    X2 = int( float(mouseX - (width/2))/float(width/2) * 255.0 );
    Y2 = int( float(mouseY)/float(height) * 255.0 );
    tt.updatePad2( X2, Y2 );
  }
}

void mouseReleased()
{
  drawVol = false;
}

void mousePressed()
{
  mouseDragged();
}
