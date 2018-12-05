<?php

	session_start();
	//$_SESSION["RegState"] = 6;
	//header("location:../index.php");
	//exit();

	use PHPMailer\PHPMailer\PHPMailer;
	use PHPMailer\PHPMailer\Exception;
	use PHPMailer\PHPMailer\SMTP;
	
	require '../PHPMailer-master/src/Exception.php';
	require '../PHPMailer-master/src/PHPMailer.php';
	require '../PHPMailer-master/src/SMTP.php';


	require_once("config.php");
	$con = mysqli_connect(SERVER, USER, PASSWORD, DATABASE);
	
	if(isset($_POST['auth_pwd'])){
	$Email = $_POST["inputmail"];
	$_SESSION['resetpwd']= $mail;
	$auth_data = "select * from Visitors where Email= '$Email'";
	$result = mysqli_query($con, $auth_data);
	if ($result->num_rows > 0) {
	$Message = "Please click the link to Change Password: http://cis-linux2.temple.edu/~tuh17884/Final_Project/php/setpassworddata.php?Email=$Email";	

	$mail= new PHPMailer(true);
	try { 
		$mail->SMTPDebug = 2;
		$mail->IsSMTP();
		$mail->Host="smtp.gmail.com";
		$mail->SMTPAuth=true;
		$mail->Username="cis105223053238@gmail.com"; // Do NOT change
		$mail->Password = 'g+N3NmtkZWe]m8"M'; // Do NOT change
		$mail->SMTPSecure = "ssl";
		$mail->Port=465;
		$mail->SMTPKeepAlive = true;
		$mail->Mailer = "smtp";
		$mail->setFrom("class@temple.edu", "CIS5512 Student"); // Change to your email and name
		$mail->addReplyTo("shi@temple.edu","Justin Y. Shi"); // Change to your email and name
		print "After setting up mail from and reply <br>"; 	
		$msg = $Message;
		$mail->addAddress($Email,"Suraj Verma");
		$mail->Subject = "Update Password";
		$mail->Body = $msg;
		$mail->send();
		
		$_SESSION["RegState"] = 0;
		$_SESSION["Message"] = "Please Click the Link On your Registered Email Address to Update the password";
		header("location:../index.php");
		exit();

	} catch (phpmailerException $e) {
		$_SESSION["Message"] = "Mailer error: ".$e->errorMessage();
		$_SESSION["RegState"] = 4;
		print "Mail send failed: ".$e->errorMessage;
		header("location:../index.php");
		exit();
	}
	}
	else{
	$_SESSION["RegState"] = 6;
	$_SESSION["Message"] = "Invalid Email Address";
	header("location:../index.php");
	exit();
}
	}
?>