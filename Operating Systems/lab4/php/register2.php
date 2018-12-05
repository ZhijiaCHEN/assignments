<?php
	session_start();
    require_once("config.php");

	use PHPMailer\PHPMailer\PHPMailer;
	use PHPMailer\PHPMailer\Exception;
	use PHPMailer\PHPMailer\SMTP;
	
	require '../PHPMailer-master/src/Exception.php';
	require '../PHPMailer-master/src/PHPMailer.php';
	require '../PHPMailer-master/src/SMTP.php';
    

    $FirstName=$_POST["FirstName"];
    $LastName=$_POST["LastName"];
    $Email=$_POST["Email"];
    $Password = $_POST["inputpwd"];
	// print "FN=$FirstName LN=$LastName <br>";
	// print "Email=$Email <br>";
	// print date('Y-m-d h:i:s');
	$con = mysqli_connect(SERVER,USER,PASSWORD,DATABASE);
	if (!$con) {
		$_SESSION["Message"]="Database Connection Failure: ".mysqli_error($con);
		$_SESSION["RegState"] = -1;
		header("location:../index.php");
		exit();
	}
	// print "Connect success <br>";
	
	$Acode = rand();
	$Rdatetime = date("Y-m-d h:i:s");

	$query = "INSERT INTO Visitors (Email, Password, FirstName,LastName,Rdatetime, Acode) VALUES ('$Email','$Password','$FirstName','$LastName','$Rdatetime','$Acode');";
	$result = mysqli_query($con, $query);
	// print "Finished query <br>";
	if ($result) {
		// Build authentication email
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
			$mail->setFrom("zhijia.chen@temple.edu", "Zhijia Chen"); // Change to your email and name
			$mail->addReplyTo("zhijia.chen@temple.edu","Zhijia Chen"); // Change to your email and name
			print "After setting up mail from and reply <br>"; 	
			$msg = "Please click link to complete registration: http://cis-linux2.temple.edu/~tuh17884/Final_Project/php/authenticate.php?Email=$Email&Acode=$Acode";
			$mail->addAddress($Email,"$FirstName $LastName");
			$mail->Subject = "Welcome";
			$mail->Body = $msg;
			$mail->send();
			
			$_SESSION["RegState"] = 0;
			$_SESSION["Message"] = "Registration success. Email sent.";
			header("Location:../index.php");
			exit();

		} catch (phpmailerException $e) {
			$_SESSION["Message"] = "Mailer error: ".$e->errorMessage();
			$_SESSION["RegState"] = 4;
			print "Mail send failed: ".$e->errorMessage;
			header("location:../index.php");
			exit();
		}
	
	} 
	$_SESSION["RegState"]=1;
	$_SESSION["Message"] = "Registration Query Failure: ".mysqli_error($con);
	header("Location:../index.php");
	exit();
?>
