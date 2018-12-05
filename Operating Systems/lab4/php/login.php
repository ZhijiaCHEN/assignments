<?php
	session_start();
	require_once("config.php");
	$con = mysqli_connect(SERVER, USER, PASSWORD, DATABASE);
	$Email = $_POST["Email"];
	$pwd = $_POST["Password"];

	$login_data = "select * from Visitors where Email= '$Email' and Password='$pwd' and Status=1";
	$result = mysqli_query($con, $login_data);
	if ($result->num_rows ==1) {
	$_SESSION['login']=$Email;	
    $_SESSION["Message"] = "Login successful";
    header("location:../dashboard.php");
   	}
   	else{
   $status_data = "select * from Visitors where Email= '$Email' and Password='$pwd' and Status=0";
	$result = mysqli_query($con, $status_data);
	if ($result->num_rows ==1) {
	$_SESSION["Message"] = "You must Confirm the Authentication Link from Email First";
	}	
    else{
	$_SESSION["Message"] = "Invalid Credentials";
	}
	$_SESSION["RegState"] = 0;
	header("location:../index.php");
	exit();

   	}


?>