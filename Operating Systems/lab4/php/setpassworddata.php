<?php
	session_start();
	require_once("config.php");
	$con = mysqli_connect(SERVER, USER, PASSWORD, DATABASE);

	if(isset($_GET['Email'])){
		$mail = $_GET["Email"];
		$_SESSION["RegState"] = 2;
		header("location: ../index.php?Email=".$mail);
		exit;
	}
	if(isset($_POST['inputPassword'])){
		$mail = $_POST["Email"];
		$password =$_POST["inputPassword"];
		$con_password= $_POST["confirmPassword"];
		$auth_data = "UPDATE Visitors SET Password='$password' WHERE Email='$mail'";
		// echo $auth_data;die;
		$result = mysqli_query($con, $auth_data);
		$_SESSION["RegState"] = 0; // To force the login view
		$_SESSION["Message"] = "Password Updated.Now You can login.";
		header("location:../index.php");
		exit();
	}
?>