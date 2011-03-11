<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ page import="org.esg.node.utils.Constants" %>
<%@ taglib prefix="s" uri="/struts-tags" %>

<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<title><s:text name="head.title" /></title>

<%
	String theme = request.getParameter("theme");
	StringBuffer requestURL = request.getRequestURL();
	String pathURL = requestURL.substring(0, requestURL.length() - request.getServletPath().length()) + "/";
%>

<link rel="shortcut icon" href="<%= pathURL %>img/favicon.jpg" />
<link rel="icon" href="<%= pathURL %>img/favicon.jpg" />
<link rel="stylesheet" type="text/css" href="<%= pathURL %>js/ext/resources/css/ext-all.css" />

<% if(theme!=null && theme.equals("gray")) { %>
	<link rel="stylesheet" type="text/css" href="<%= pathURL %>js/ext/resources/css/xtheme-gray.css" />
<% } %>

<link rel="stylesheet" type="text/css" href="<%= pathURL %>css/style.css" />

<% if(theme!=null && theme.equals("gray")) { %>
	<link rel="stylesheet" type="text/css" href="<%= pathURL %>css/style-gray.css" />
<% } %>

<%--
<script type="text/javascript" src="<%= pathURL %>js/jquery-1.4.2.js"></script>
<script type="text/javascript" src="<%= pathURL %>js/ext/adapter/jquery/ext-jquery-adapter.js"></script>
--%>
<script type="text/javascript" src="<%= pathURL %>js/ext/adapter/ext/ext-base.js"></script>
<script type="text/javascript" src="<%= pathURL %>js/ext/ext-all.js"></script>
<script type="text/javascript" src="<%= pathURL %>js/ext/src/locale/ext-lang-<s:property value="locale.language.toLowerCase()"/>.js"></script>
<script type="text/javascript" src="<%= pathURL %>js/json/json2.js"></script>
<script type="text/javascript">
//<![CDATA[
    var userIsLogged = <%= session.getAttribute(Constants.USER_SESSION) != null %>;
    var pathURL = '<%=pathURL %>';
	var grayCssPath = "<%= pathURL %>js/ext/resources/css/xtheme-gray.css";
	var bgPath = "<%= pathURL %>img/background2.jpg";
	var projectTreePath = '<s:url action="ProjectTree" namespace="/json" encode="false" />';
	var navigatorTitle = '<s:text name="navigator.title" />';
	var globalViewText = '<s:text name="treeNode.global.view" />';
	var gridUnknownPercentageValue = '<s:text name="grid.columns.percentage.unknownValue" />';

	var northPanel = new Ext.Panel({
		region: "north",
		//height: 163,
		autoHeight: true,
		layout: 'fit',
		//layout: "border",		
		items: [
		     new Ext.Panel({		    	 
		    //	 region: "north",
		    	 frame: true,
		    	 //height: 125,
		    	 autoHeight: true,		    	 
		    	 contentEl: "northEl"		    	 
		     }),
		     new Ext.Panel({
			     height: 38,
		    //	 region: "center",
		    	 id: "loginForm",
		    	 margins: '1 0 1 0',	             
		    	 frame: true,		    	
		    	 contentEl: userIsLogged? "loggedInContainer": "loginContainer"		    	 
		     })
		]
	});
	
	function login(username, password) {
		Ext.Ajax.request({
			url: '<s:url action="Login" namespace="/secure" encode="false" />',
			success: function(response, opts) {
				var user = JSON.parse(response.responseText);
				if(user != null) {
					var parent = document.getElementById("loginContainer").parentNode;
					document.getElementById("loginExternalContainer").appendChild(document.getElementById("loginContainer"));
					parent.appendChild(document.getElementById("loggedInContainer"));
					generateWelcomeMsg(user.name, user.surname);
					document.location='<s:url action="Home" namespace="/" encode="false" />';
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
<script type="text/javascript" src="<%= pathURL %>js/commons.js"></script>
