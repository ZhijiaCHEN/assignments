<?php
session_start();
require_once("config.php");
$con = mysqli_connect(SERVER, USER, PASSWORD, DATABASE);
// get web data 
	$mail = $_GET["Email"];
	$Acode = $_GET["Acode"];
	$query = "Update Visitors set Status=1 where Email='$mail' and Acode='$Acode'";
	$result = mysqli_query($con, $query);
	$_SESSION["RegState"] = 0; // To force the login view
	$_SESSION["Message"] = "Registration Finally complete.Now You can login.";
	header("location:../index.php");
	exit();

?>