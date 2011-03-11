<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ page import="org.esg.node.utils.Constants, org.esg.node.utils.UserSession" %>
<%@ page import="com.opensymphony.xwork2.ActionContext" %>
<%@ taglib prefix="s" uri="/struts-tags" %>

<%
	String mini = request.getParameter("mini");
	String iframe = request.getParameter("iframe");
	StringBuffer requestURL = request.getRequestURL();
	String pathURL = requestURL.substring(0, requestURL.length() - request.getServletPath().length()) + "/";
	UserSession user = (UserSession)session.getAttribute(Constants.USER_SESSION);
	Integer idGuest = Integer.parseInt(ActionContext.getContext().getValueStack().findValue("getText(\"user.guestId\")").toString());
%>

<div id="northEl" class="x-hide-display">
	<table width="100%" border="0">
	<tbody>
		<tr valign="top">
			<td align="center" width="70%">
				<p style="font-size: 20px; color: brown; text-align: center;"><s:text name="northern.title" /></p>
				<%-- <img src="<%= pathURL %>img/title.jpg" height="100%" width="100%" alt='<s:text name="northern.title" />' />--%>
			</td>
			<td style="display: none;">
				<s:form target="_top" name="formLingua" action="Home" namespace="/">
					<select name="request_locale" id="locale" onchange="formLingua.submit()">
						<option value="it">Italiano</option>
						<option value="en">English</option>
					</select>					
				</s:form>				
			</td>			
		</tr>
	</tbody>
	</table>
	<script type="text/javascript">
	//<![CDATA[
		document.getElementById('locale').value='<s:property value="locale.language.toLowerCase()"/>';
	//]]>
	</script>
	<div id="left" class="x-hide-display">
		<s:form id="formProject" action="Project" namespace="/monitoring">
			<s:hidden name="idProject" id="idProject" />
		</s:form>
		<s:form id="formServer" action="Server" namespace="/monitoring">
			<s:hidden name="idProject" id="idProjectServer" />
			<s:hidden name="idServer" id="idServer" />
		</s:form>
	</div>
</div>

<div id="loginExternalContainer" class="x-hide-display">
	<div id="loginContainer">
		<table width="100%" cellspacing="0">
			<tbody>
				<tr>
					<td>
						<p style="text-align: left; font-weight: bold;">
							<% if(mini != null && mini.equals("1")) { %>
							<a href="#" onclick="document.location=pathURL+unescape('<%= iframe %>') + '&iframe=' + escape('<%= iframe %>');"><s:text name="northern.link.gotoHome" /></a>
							<% } else { %>
							<s:a action="Home" namespace="/"><s:text name="northern.link.gotoHome" /></s:a>
							<% } %>
						</p>
					</td>
					<td>
						<form onsubmit='login(document.getElementById("loginUsername").value, document.getElementById("loginPassword").value); return false;'>
							<p style="text-align: right; vertical-align: top;">
								<s:text name="login.form.username" /> <input id="loginUsername" type="text" name="username" style="height: 22px">
								<s:text name="login.form.password" /> <input id="loginPassword" type="password" name="password" style="height: 22px">
								<input type="submit" value='<s:text name="login.form.signin" />'>
								<% if(mini == null || !mini.equals("1")) { %>
								&nbsp;&nbsp;
								<a style="font-weight: bold;" href='<s:url action="RegistrationPage" namespace="/secure" encode="false" />'><s:text name="login.form.subscribe" /></a>
								<% } %>
							</p>
						</form>
					</td>
				</tr>
			</tbody>
		</table>		
	</div>
</div>

<div id="loggedInExternalContainer" class="x-hide-display">
	<div id="loggedInContainer">
		<table width="100%" cellspacing="0">
			<tbody>
				<tr>
					<td>
						<p style="text-align: left; font-weight: bold;">
							<% if(mini != null && mini.equals("1")) { %>
							<a href="#" onclick="document.location=pathURL+unescape('<%= iframe %>') + '&iframe=' + escape('<%= iframe %>');"><s:text name="northern.link.gotoHome" /></a>
							<% } else { %>
							<s:a action="Home" namespace="/"><s:text name="northern.link.gotoGlobalView" /></s:a>
							<% } %>
						</p>
					</td>
					<td>
						<form action='<% if(mini != null && mini.equals("1")) { %><s:url action="LogoutMini" namespace="/secure" encode="false" /><% } else { %><s:url action="Logout" namespace="/secure" encode="false" /><% } %>' method="post">
							<input type="hidden" name="iframe" value="<%= iframe %>" />
							<p style="text-align: right;">
								<s:text name="loggedIn.welcomeMsg" />&nbsp;
								<span id="loggedInUserSpan"></span>
								<input type="submit" value='<s:text name="login.form.signout" />' />
								<%
									if((mini == null || !mini.equals("1")) && (user != null && user.getId()!=idGuest)) {
								%>
								&nbsp;&nbsp;								
								<a style="font-weight: bold;" href='<s:url action="UserProfile" namespace="/secure" encode="false" />'><s:text name="login.form.profile" /></a>
								<%
									}
								%>			
							</p>
						</form>
					</td>
				</tr>
			</tbody>
		</table>
	</div>
</div>
<%
	if(user != null) {
%>
<script type="text/javascript">
//<![CDATA[
    generateWelcomeMsg('<%=user.getName()%>', '<%=user.getSurname()%>');
//]]>
</script>
<%  }%>
