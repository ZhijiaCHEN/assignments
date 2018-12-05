<?php
	session_start();
	require_once("config.php");
	$con = mysqli_connect(SERVER, USER, PASSWORD, DATABASE);
	header("location:../index.php");
	if(isset($_GET["Email"])){
		$_SESSION["RegState"] = 2;
	header("location:../index.php");

	/*$email= 	$_GET["Email"];
	$password = 	$_GET["inputPassword"];
	$con_password = $_GET["confirmPassword"];

	if($password !=$con_password){
		$_SESSION["Message"] = "Passwords do not Match. please Try again";
		$_SESSION["RegState"] = 2;
		//header("location:../index.php");
		//exit();
		}
		else{

	
	$query = "Update Visitors set Password='$password' where Email='$_SESSION[resetpwd]'";
	$result = mysqli_query($con, $query);
	$_SESSION["RegState"] = 0; // To force the login view
	$_SESSION["Message"] = "Password Updated.Now You can login.";
	header("location:../index.php");
	exit();
	}*/
}
?>