<html>
<head>

<link rel="stylesheet" type="text/css" href="/css/fonts-min.css">
<script type="text/javascript" src="/javascript/jquery-latest.js"></script>   
<link rel='stylesheet' type='text/css' href='/css/global.css'>

<script language="javascript" type="text/javascript">

<?php 

	$output = shell_exec('cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies');
	$output = trim($output);
	$output = explode (" ",$output);

	$current_governor = shell_exec('cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor');
	$current_governor = trim($current_governor);

	$current_freq = shell_exec('cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq');

		$supportedResolutions = null;

		if(file_exists("../../supported_resolutions.txt")==true)
		{
			$supportedResolutions = file ("../../supported_resolutions.txt",FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
	
		}

		if($supportedResolutions == null || count($supportedResolutions) == 0)
		{
			echo "supported_resolutions.txt doesn't exist or is empty";
			exit; 
		}
	?>

	var supportedResolutions=new Array(); 

	<?php
		for($x = 0;$x<count($supportedResolutions);$x++)
		{
			echo "supportedResolutions[".$x."]=\"".$supportedResolutions[$x]."\";";    
		}
	?>

	var screenWidth = 0;
	var screenHeight = 0;

	for(var i=0; i<supportedResolutions.length; i++) 
	{
		var value = supportedResolutions[i].split('x');

		screenWidth = value[0];
		screenHeight = value[1];

		if(screen.width >= screenWidth && screen.height >= screenHeight)
			break;
	}

	$('head').append('<link rel="stylesheet" type="text/css" href="/css/'+screenWidth+'x'+screenHeight+'.css" />'); 

</script>

</head>
<body class="unselectable" style = "-webkit-user-select: none;-moz-user-select: none;color:white;">
<div class = 'complete_container'>


<?php
if ($current_governor != "userspace")
{
	echo "<p>Sorry the frequency can not be changed. <br><br> CPU Governor is currently set to <b>$current_governor</b>. <br>Only when the <b>userspace governor</b> is used can the frequency be manually changed. <br><br> Please use the Set Userspace Governor Matrix application and then try again.</p>";

}
else
{

?>

<span id = "updated" style = "text-decoration:blink; visibility:hidden;" >Updated </span>Current Frequency: <span id = "current_freq"><?php echo $current_freq/1000 ?> Mhz</span>
<br>
<br>

<?php
	for($i=0;$i<count($output);$i++)
	{
	
		echo "<div style = 'float:left; padding:5px;'>";
		echo "<a href = '#' value = 'setclockspeed.sh $output[$i]' class = 'myLink' ><img class = 'app_icon' src= '/apps/images/power-icon.png' ></a>";
		$output[$i] = $output[$i]/1000;
		echo "<p>Set $output[$i] Mhz</p>";
		echo "</div>";
	
	}


?>

<script>


$(".complete_container").delegate("img", "mousedown", function(e)
{
       e.preventDefault();
});

$(".complete_container").delegate("a", "click", function(e)
{
	e.preventDefault();
	e.stopPropagation();
	update($(this).attr("value"));
}); 

function update(command)
{
	var frequency = command.split(" ")[1];
	//This is a fix for IE browsers. IE likes to cache Ajax results. Therefore, adding a random string will prevent the browser from caching the Ajax request.
	var uri = "/execute-command.php?command="+encodeURIComponent(command);

	// Adds a random string to the end of the $_GET Query String for page accessed.
	// This prevents IE from caching the Ajax request.
	uri = uri + "&rand="+Math.round((Math.random()*2356))+Math.round((Math.random()*4321))+Math.round((Math.random()*3961));

	$.get(uri, function(data)
	{
		fail_count = 0;
		$('#updated').css('visibility', 'visible');
		$('#current_freq').html(frequency/1000 + " Mhz");
		setTimeout(setInvisible,3000);
	})
	//If a file cant be read or some other error related to trying to retrieve this file then JQuery executes the error function.
	//This sometimes occurs when the browser tries to read the output file before output file is even created.
	.error(function()
	{
		//Might need to handle errors
	});

}

function setInvisible()
{
	$('#updated').css('visibility', 'hidden');
}



</script>

<?php

}
?>

</body>
</html>

