<%@ page language="java" contentType="text/html; charset=UTF-8"

    pageEncoding= "UTF-8"%>

<%@ page import="java.sql.*"%>

 

<h2> JDBC드라이버 테스트 </h2>

 

<%

	Connection conn = null;
	Statement state = null;
	ResultSet rs = null;
 

  try{
			
	Class.forName("org.mariadb.jdbc.Driver");
	conn = DriverManager.getConnection("jdbc:mariadb://localhost:3306/test","root","1234");
	
	if(conn == null){
		throw new Exception("데이터베이스 연결 불가");
	}
	
	else {
		System.out.println("데이터베이스 연결 성공");
	}
	
  }finally{
	  
  }
%>