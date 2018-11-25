<?php
    $humidity = $_GET['humidity'];
    $temp = $_GET['temperature'];
    $light = $_GET['light'];
    $humidifier = $_GET['humidifier'];
    $position =  $_GET['position'];
    date_default_timezone_set("Asia/Bangkok");

    $host = "localhost";
    $user = "root";
    $pass = "";
    $daba = "cps_data";

    $con = mysqli_connect($host,$user,$pass,$daba);
    $query = "INSERT INTO egg (id, humidity, temp, light, humidifier, position, date) VALUES (NULL, '".$humidity."', '".$temp."','".$light."','".$humidifier."','".$position."','".date("Y/m/d h:i:sa")."');";
    $query = mysqli_query($con,$query);


    echo 'Data is inserted';
    

	//USE CASE  -> localhost/upload.php?humidity="Your humidity"&temperature="Your Temp"&light="Your light"&humidifier="Your humidifier"&position="Your position"
?>

