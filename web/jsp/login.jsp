<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ page import="org.esg.node.utils.Constants, org.esg.node.utils.UserSession" %>
<%@ taglib prefix="s" uri="/struts-tags" %>

<%
	StringBuffer requestURL = request.getRequestURL();
	String pathURL = requestURL.substring(0, requestURL.length() - request.getServletPath().length()) + "/";
	String guest = request.getParameter("guest");
	String iframe = request.getParameter("iframe");
	if(guest != null && guest.equals("1")) {
		session.setAttribute(Constants.USER_SESSION, new UserSession(1, "guest", "guest"));
	}
%>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
		<link rel="stylesheet" type="text/css" href="../css/style.css" />
		<link rel="stylesheet" type="text/css" href="<%= pathURL %>js/ext/resources/css/ext-all.css" />
		<script type="text/javascript" src="<%= pathURL %>js/ext/adapter/ext/ext-base.js"></script>
		<script type="text/javascript" src="<%= pathURL %>js/ext/ext-all.js"></script>
		<title>Login Page</title>
		<% if(guest != null && guest.equals("1")) { %>
			<script type="text/javascript">
			//<![CDATA[
				document.location = '<%= pathURL %>' + unescape('<%= iframe %>') + '&iframe='+escape('<%= iframe %>');
			//]]>
			</script>
		<% } %>
		
		<script type="text/javascript">
		//<![CDATA[
			function login() {
				var username = document.getElementById('username').value;
				var password = document.getElementById('password').value;
				
				Ext.Ajax.request({
					url: '<s:url action="Login" namespace="/secure" encode="false" />',
					success: function(response, opts) {
						var user = JSON.parse(response.responseText);
						if(user != null) {
							document.location= '<%= pathURL %>' + unescape('<%= iframe %>') + '&iframe='+escape('<%= iframe %>');
						}
						else {
							Ext.MessageBox.alert('Status', 'User not found!');
						}
					},
					scope: this,
					params: {
						username: username,
						password: password
					}
				});				
			} 
		//]]>
		</script>		
	</head>
	<body>	
		<table align="center">
			<tbody>
				<tr>
					<td>
						<fieldset style="width:300;">
							<legend style="margin-left: 15px;">Login</legend>
							<div style="margin-left: 20px; margin-top: 10px;">
								<s:form onsubmit="login(); return false;">
									<p style="margin-top: 10px;"><s:textfield id="username" name="username" label="Username" labelposition="left" /></p>
									<p style="margin-top: 10px;"><s:password id="password" name="password" label="Password" labelposition="left" /></p>
									<br />
									<s:submit align="center" value="Enter" />
								</s:form>
							</div>
						</fieldset>
					</td>
				</tr>
			</tbody>
		</table>
	</body>
</html>
