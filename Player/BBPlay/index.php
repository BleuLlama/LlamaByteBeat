<?php
// 1. check for ?glitch=....
$glitch = $_GET["glitch"];
if( $glitch == "" )
{
	// 2. check for ?g=....
	$glitch = $_GET["g"];
}

if( $glitch == "" )
{
	// 3. check for just ?=....
	$glitch = $_SERVER["QUERY_STRING"];
}

if( $glitch == "" )
{
	// none found. let's inject one
    $glitch = "glitch://your_glitch_here!867.5309";
}

// okay. we probably have something now...

$pos = stripos( $glitch, "glitch://" );
if( $pos === false ) {
	// not necessary as of 1.05
	$glitch = "glitch://" . $glitch;
}

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
  <head>
    <!-- charset must remain utf-8 to be handled properly by Processing -->
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />

    <title>ByteBeat Player</title>

    <style type="text/css">
      /* <![CDATA[ */
	
	body {
  	  margin: 60px 0px 0px 55px;
	  font-family: verdana, geneva, arial, helvetica, sans-serif; 
	  font-size: 11px; 
	  background-color: #ddddcc; 
	  text-decoration: none; 
	  font-weight: normal; 
	  line-height: normal; 
	}
		 
	a          { color: #3399cc; }
	a:link     { color: #3399cc; text-decoration: underline; }
	a:visited  { color: #3399cc; text-decoration: underline; }
	a:active   { color: #3399cc; text-decoration: underline; }
	a:hover    { color: #3399cc; text-decoration: underline; }

      /* ]]> */
    </style>    
  </head>

  <body>
    <div id="content">
      <div id="BBPlay_container">

	<!-- This version plays nicer with older browsers, 
	     but requires JavaScript to be enabled. 
	     http://java.sun.com/javase/6/docs/technotes/guides/jweb/deployment_advice.html
	     http://download.oracle.com/javase/tutorial/deployment/deploymentInDepth/ -->
	<script type="text/javascript"
		src="http://www.java.com/js/deployJava.js"></script>
	<script type="text/javascript">
	  /* <![CDATA[ */

	  var attributes = { 
            code: 'BBPlay.class',
            archive: 'BBPlay.jar,jl1.0.jar,jsminim.jar,minim-spi.jar,minim.jar,mp3spi1.9.4.jar,tritonus_aos.jar,tritonus_share.jar,core.jar',
            width: 512, 
            height: 256,
          };
          var parameters = { 
            image: 'loading.gif',
            centerimage: 'true',
	    glitch: '<?php print $glitch; ?>'
          };
          var version = '1.5';
          deployJava.runApplet(attributes, parameters, version);

          /* ]]> */
        </script>
        
	<noscript> <div>
	  <!--[if !IE]> -->
	  <object classid="java:BBPlay.class" 
            	  type="application/x-java-applet"
            	  archive="BBPlay.jar,jl1.0.jar,jsminim.jar,minim-spi.jar,minim.jar,mp3spi1.9.4.jar,tritonus_aos.jar,tritonus_share.jar,core.jar"
            	  width="512" height="256"
            	  standby="Loading Processing software..." >
            
	    <param name="archive" value="BBPlay.jar,jl1.0.jar,jsminim.jar,minim-spi.jar,minim.jar,mp3spi1.9.4.jar,tritonus_aos.jar,tritonus_share.jar,core.jar" />
	    
	    <param name="mayscript" value="true" />
	    <param name="scriptable" value="true" />
	    
	    <param name="image" value="loading.gif" />
	    <param name="boxmessage" value="Loading Processing software..." />
	    <param name="boxbgcolor" value="#FFFFFF" />
	  <!--<![endif]-->

	    <!-- For more instructions on deployment, 
		 or to update the CAB file listed here, see:
		 http://java.sun.com/javase/6/webnotes/family-clsid.html
		 http://java.sun.com/javase/6/webnotes/install/jre/autodownload.html -->
	    <object classid="clsid:8AD9C840-044E-11D1-B3E9-00805F499D93"
		    codebase="http://java.sun.com/update/1.6.0/jinstall-6u20-windows-i586.cab"
		    width="512" height="256"
		    standby="Loading Processing software..."  >
	      
	      <param name="code" value="BBPlay" />
	      <param name="archive" value="BBPlay.jar,jl1.0.jar,jsminim.jar,minim-spi.jar,minim.jar,mp3spi1.9.4.jar,tritonus_aos.jar,tritonus_share.jar,core.jar" />
	      
	      <param name="mayscript" value="true" />
	      <param name="scriptable" value="true" />
	      
	      <param name="image" value="loading.gif" />
	      <param name="boxmessage" value="Loading Processing software..." />
	      <param name="boxbgcolor" value="#FFFFFF" />
	      
	      <p>
		<strong>
		  This browser does not have a Java Plug-in.
		  <br />
		  <a href="http://www.java.com/getjava" title="Download Java Plug-in">
		    Get the latest Java Plug-in here.
		  </a>
		</strong>
	      </p>
	      
	    </object>
	    
	  <!--[if !IE]> -->
	  </object>
	  <!--<![endif]-->

	</div> </noscript>

      </div>
      


<?php 
if( $glitch == "" ) {
?>
<p>
You can scroll through the builtin tunes, or add a tune to the URL like so:
<pre>
    http://umlautllama.com/p5/glitch/?glitch://Tune_Here!aad
</pre>
</p>

<?php
} else {
?>
<h2> <?php print $glitch; ?></h2>
<?php
}
?>
      <p>


	
controls:<br/>
<ul>
    <li> click with mouse along the right edge to adjust volume</li>
	<li> + to go to the next tune,  - to go to the previous tune</li>
	<li> 0 to reset time to 0 </li>
	<li> m to force volume to 0 </li>
	<li> p to pause time </li>
	<li> v to change visualizer</li>
      
</ul>
      </p>
      
      
      <p>
	Source code: <a href="BBPlay.pde">BBPlay</a> <a href="ByteBeatTune.pde">ByteBeatTune</a> <a href="ToneThing.pde">ToneThing</a> 
      </p>
      
      <p>
	Built with <a href="http://processing.org" title="Processing.org">Processing</a>
      </p>
	<ul>
	<li> add ?glitch=glitch://NAME!etc... to the url to pass in a complete glitch:// URI</li>
	<li> Examples of use: (all of these are valid)
		<ul>
	<!--
			<li>  http://umlautllama.com/p5/glitch?glitch=glitch://Example_glitch!a4d</li>
			<li>  http://umlautllama.com/p5/glitch?g=glitch://Example_glitch!a4d</li>
	-->
			<li>  http://umlautllama.com/p5/glitch?glitch://Example_glitch!a4d</li>

	<!--
			<li>  http://umlautllama.com/p5/glitch?glitch=Example_glitch!a4d</li>
			<li>  http://umlautllama.com/p5/glitch?g=Example_glitch!a4d</li>
	-->
			<li>  http://umlautllama.com/p5/glitch?Example_glitch!a4d</li>
		</ul>
	</li>
	<li> this php is also available as a <a href="index.txt">plaintext file</a>, so you can easily include/modify it for your own site.</li>
	</ul>
    </div>
  </body>
</html>
