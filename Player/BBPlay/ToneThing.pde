// ToneThing
//
//  Tone generator for minim, using a ByteBeatTune as its input
//
//  2011 Scott Lawrence (BleuLlama/@Yorgle)
//  yorgle@gmail.com
//
//  This is released under an MIT license.



class ToneThing implements AudioSignal
{
  long ticks = 0;
  long val = 0;
  int sc = 2;
  boolean pause;
  float volume = 1.0;
  
  ByteBeatTune bbt;
  
  void reset()
  {
    ticks = 0;
  }
  
  void updateAcc( int x, int y, int z)
  {
    bbt.Xa = x;
    bbt.Ya = y;
    bbt.Za = z;
  }

  void updatePad1( int x, int y )
  {
    bbt.X1 = x;
    bbt.Y1 = y;
  }

  void updatePad2( int x, int y )
  {
    bbt.X2 = x;
    bbt.Y2 = y;
  }

  
  void setBBT( ByteBeatTune pbbt )
  {
    bbt = pbbt;
  }
  
  void generate( float[] samp )
  {
    if( samp == null ) return;
    
    for( int i=0 ; i < samp.length/2 ; i += 1  )  // /2 == /sc
    {
      long t = ticks + i;
      
     //val = (t>>7|t|t>>6) * 10+4 * (t&t>>13|t>>6); // atari bin 2

      val = bbt.evaluate( t );
      samp[i*2] = ((val & 0x0ff)/256.0)*volume;
      samp[i*2+1] = samp[i*2];
    }
  }
  
  void generate( float[] left, float[] right )
  {
    generate( left );

    // copy it to save cycles.
    for( int i=0 ; i<left.length ; i++ )
    {
      right[i] = left[i];
    }
    
    if( !pause ) {
      ticks += left.length/sc;
    }
  }
}
