## CPS-hash
Hi all, this folder will contain the user interface about how temperature&humidity are doing by get data from Device then upload to Host

## Requirement
* Apache2
* PHP
* PHPmyAdmin (For mysql server)

Install website by using this [Guide](https://www.rosehosting.com/blog/install-phpmyadmin-on-ubuntu-16-04/)

Then copy "data.php" and "upload.php" to your web root folder

On data.php and upload.php line 6 - 9 
```	
	$host = "localhost";
	$user = "DB_USER";
	$pass = "DB_PASS";
	$daba = "DATABASE NAME";
```

Please config your Database user according to your PHPmyadmin setting.

And Open phpmyadmin (localhost/phpmyadmin)

On the top of the page there's a comlumn call import.

Download 'database.sql' and put it in there

Done!
