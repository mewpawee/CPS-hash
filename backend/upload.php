<?php
    $humidity = $_GET['humidity'];
    $temp = $_GET['temperature'];
    date_default_timezone_set("Asia/Bangkok");

    $host = "localhost";
    $user = "DB_USER";
    $pass = "DB_PASS";
    $daba = "Database name";

    $con = mysqli_connect($host,$user,$pass,$daba);
    $query = "INSERT INTO egg (id, humidity, temp, date) VALUES (NULL, '".$humidity."', '".$temp."','".date("Y/m/d h:i:sa")."');";
    $query = mysqli_query($con,$query);


    echo 'Data is inserted';
    

	//USE CASE  -> localhost/upload.php?humidity="Your humidity"&temperature="Your Temp"
?>

