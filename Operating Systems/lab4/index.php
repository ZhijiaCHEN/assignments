<?php
session_start();
if (!isset($_SESSION["RegState"])) {
	$_SESSION["RegState"] = 0;
} 

?>
<!doctype html>
<html lang="en">
	<head>
		<meta charset="utf-8">
		<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
		<meta name="description" content="">
		<meta name="author" content="">
		<link rel="icon" href="images/favicon.ico">


		<title>CIS5512 Final Project</title>

		<!-- Bootstrap core CSS -->
		<link href="css/bootstrap.min.css" rel="stylesheet">

		<!-- Custom styles for this template -->
		<link href="css/signin.css" rel="stylesheet">
	</head>
	<body class="text-center">
		<div class="row">
		<?php
		if ($_SESSION["RegState"] == 0) {
		?>
			<form action="php/login.php" method="post" class="col form-signin">
			  <img class="mb-4" src="https://getbootstrap.com/docs/4.0/assets/brand/bootstrap-solid.svg" alt="" width="72" height="72">
			  <h1 class="h3 mb-3 font-weight-normal">CIS5512 Final Project (**Enter group id**)</h1>
			  <label for="inputEmail" class="sr-only">Email address</label>
			  <input type="email" id="inputEmail" name="Email" class="form-control" placeholder="Email address" required autofocus>
			  <label for="inputPassword" class="sr-only">Password</label>
			  <input type="password" id="inputPassword" name="Password" class="form-control" placeholder="Password" required>
			  <div class="checkbox mb-3">
				<label>
				  <input type="checkbox" name="rememberMe" value="remember-me"> Remember me
				</label>
			  </div>
			  <button class="btn btn-lg btn-primary btn-block" type="submit">Sign in</button>
			  <?php if(isset($_SESSION["Message"])){?>
			  <div id="messageLogin" class="alert alert-info mt-2" role="alert"><?php  echo $_SESSION["Message"]; ?></div> <?php }?>
			  <a href="php/register.php">Register</a> | <a href="php/register_email.php">Forget?</a>
			</form>
		<?php
		} 
		if ($_SESSION["RegState"] == 1) {  // When "register" is clicked
		?>
			<form action="php/register2.php" method="post" class="col form-signin">
			  <img class="mb-4" src="https://getbootstrap.com/docs/4.0/assets/brand/bootstrap-solid.svg" alt="" width="72" height="72">
			  <h1 class="h3 mb-3 font-weight-normal">CIS5512 Final Projecr Registration</h1>
			  <label for="inputFirstName" class="sr-only">First Name</label>
			  <input type="text" id="inputFirstName" name="FirstName" class="form-control" placeholder="First Name" required autofocus>
			  <label for="inputLastName" class="sr-only">Last Name</label>
			  <input type="text" id="inputLastName" name="LastName" class="form-control" placeholder="Last Name" required>
			  <label for="inputEmail" class="sr-only">Email address</label>
			  <input type="email" id="inputEmail" name="Email" class="form-control" placeholder="Email address" required autofocus>
			  <label for="Passwordvalue" class="sr-only">Password</label>
			  <input type="password" id="inputpwd" name="inputpwd" class="form-control" placeholder="Password" required autofocus>
			  <label for="Passwordconfirmvalue" class="sr-only">Confirm Password</label>
			  <input type="password" id="inputpwd" name="cnfpwd" class="form-control" placeholder="Confirm Password" required autofocus>
			  <button type="submit" class="btn btn-lg btn-primary btn-block" type="submit">Register</button>
			  <?php if(isset($_SESSION["Message"])){?>
			  <div id="messageRegister" class="alert alert-info mt-2" role="alert"><?php  echo $_SESSION["Message"]; ?></div> <?php }?>
			  <a href="php/clearAll.php"><button type="button" class="mt-2">Return</button></a>
			</form>
		<?php
		}
		if ($_SESSION["RegState"] == 6) { // when "forget" is clicked
		?>
			<form action="php/resetPassword.php" method="post" class="col form-signin">
			  <img class="mb-4" src="https://getbootstrap.com/docs/4.0/assets/brand/bootstrap-solid.svg" alt="" width="72" height="72">
			  <h1 class="h3 mb-3 font-weight-normal">CIS5512 Final Project Reset Password</h1>
			  <label for="inputEmail" class="sr-only">Registered Email address</label>
			  <input type="email" name="inputmail"  id="inputEmail" class="form-control" placeholder="Registered email address" required autofocus>
			  <button type="submit" class="btn btn-lg btn-primary btn-block" type="submit" name="auth_pwd">Authenticate</button>
			  <?php if(isset($_SESSION["Message"])){?><div id="messageResetPassword" class="alert alert-info mt-2" role="alert"><?php echo $_SESSION["Message"]; ?></div><?php }?>
			  <a href="php/clearAll.php"><button type="button" class="mt-2">Return</button></a>
			</form>
		<?php
		}
		if ($_SESSION["RegState"] == 2) { // Set new Password
		?>
			<form action="php/setpassworddata.php" method="post" class="col form-signin">
				<input type="hidden" name="Email" value="<?php echo $_GET['Email'] ?>">
			  <img class="mb-4" src="https://getbootstrap.com/docs/4.0/assets/brand/bootstrap-solid.svg" alt="" width="72" height="72">
			  <h1 class="h3 mb-3 font-weight-normal">CIS5512 Final Project Set Password</h1>
			  <input type="hidden" id="inputEmail" class="form-control" value="<?php $_SESSION["Email"]; ?>">
			  <label for="inputPassword" class="sr-only">Enter a password</label>
			  <input type="password" name="inputPassword" id="inputPassword" class="form-control" placeholder="Password" required>
			   <label for="confirmPassword" class="sr-only">Confirm password</label>
			  <input type="password" name="confirmPassword" id="confirmPassword" class="form-control" placeholder="Confirm Password" required>
			  <button type="submit" class="btn btn-lg btn-primary btn-block" type="submit">Set New Password</button>
			  <?php if(isset($_SESSION["Message"])){?><div id="messageSetPassword" class="alert alert-info mt-2" role="alert"><?php echo $_SESSION["Message"]; ?></div><?php }?>
			  <a href="php/clearAll.php"><button type="button" class="mt-2">Return</button></a>
			  <!--<a href="php/clearAll.php"><button type="button" class="mt-2">Login</button></a> -->
			</form>
		<?php
		} 
		?>
		</div>

	<!-- Bootstrap core JavaScript
	================================================== -->
	<!-- Placed at the end of the document so the pages load faster -->
		<script src="js/jquery-3.2.1.slim.min.js"></script>
		<script>window.jQuery || document.write('<script src="js/jquery-slim.min.js"><\/script>')</script>
		<script src="js/popper.min.js"></script>
		<script src="js/bootstrap.min.js"></script>
		<script src="//ajax.googleapis.com/ajax/libs/jquery/1.4/jquery.min.js"></script>
		<script>
			$( document ).ready(function() {
				$('.alert').delay(5000).fadeOut(1000);
			});
		</script>
	</body>
</html>

