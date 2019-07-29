<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8"%>

<%@ page import="java.io.PrintWriter" %>

<!DOCTYPE html>

<html>

<head>

<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">

<meta name="viewport" content="width=device-width" , initial-scale="1.0">

<link rel="stylesheet" href="css/bootstrap.css">

<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">

 

<title>JSP 게시판</title>

</head>

<body>

<!-- 로그인이 된 사람들의 세션을 만들어 준다. 

회원값이 있는 사람들은 userID에 정보가 담기게 되고

아닌 사람은 null값을 가지게 된다.

-->

<%
	String userID = null;
	if(session.getAttribute("userID") != null){
		userID = (String) session.getAttribute("userID");
	}
%>

<nav class="navbar navbar-default">

<div class="naver-header">

	<button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#bs-example-navbar-collapse-1" aria-expanded="false">
		<span class="icon-bar"></span> 
		<span class="icon-bar"></span> 
		<span class="icon-bar"></span>
	</button>

	<a class="navbar-brand" href="main.jsp">JSP 게시판 웹 사이트</a>

</div>


<div class="collapse navbar-collapse" id="bs-example-navbar-collapse-1">

	<ul class="nav navbar-nav">
		<li class="active"><a href="main.jsp">메인</a>
		<li><a href="board.jsp">게시판</a>
	</ul>
	
	<!-- 접속하기는 로그인안한 사람만 보여주게하기 -->
	<%
		if(userID == null){
	%>

	<ul class="nav navbar-nav navbar-right">

		<li class="dropdown">

			<a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false">접속하기<span class="caret"></span></a>

			<ul class="dropdown-menu">
				<li><a href="login.jsp">로그인</a></li>
				<li><a href="join.jsp">회원가입</a></li>
			</ul>

		</li>

	</ul>


<% 

	}else{

%>	

	<ul class="nav navbar-nav navbar-right">

		<li class="dropdown">
		
			<a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false">회원관리<span class="caret"></span></a>
		
			<ul class="dropdown-menu">
				<li><a href="logoutAction.jsp">로그아웃</a></li>
			</ul>

		</li>
		
	</ul> 

<%
	}
 %>

		</div>
	</nav>		

	<div class="container">
<!-- 	<p>&nbsp;</p><center>

	<img src="mainImage.png" width="100%"> -->

		<div class="jumbotron">
			<div class="container">
				<h1>웹사이트 소개</h1>
				<p>임시 웹사이트 입니다.</p>
				<p><a class="btn btn-primary btn-pull" href="#" role="button">자세히 알아보기</a></p>
			</div>
		</div>
	</div>

	

<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js"></script>	
<script src="js/bootstrap.min.js"></script> 
</body>
</html>