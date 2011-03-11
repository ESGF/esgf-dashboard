<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%
	String iframe = request.getParameter("iframe");
%>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
		<title>Access Denied</title>
		<script type="text/javascript">
			var pars = unescape('<%= iframe %>').split('?')[1];
			// setTimeout("redirect();", 6000);
			function redirect() {
				pars = pars.replace("guest=1","guest=0");
				document.location = '<s:url action="LoginMini" namespace="/secure" encode="false" />?' + pars + '&iframe=' + escape('<%= iframe %>');
			}
		</script>
	</head>
	<body>
		<p style="text-align: center;">
			Access denied for this user, please <a href="#" onclick="redirect();">sign in</a> as a different user.
		</p> 
	</body>
</html>
