<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ page import="org.esg.node.utils.Constants, org.esg.node.utils.SqlQuery" %>

<%@ taglib prefix="s" uri="/struts-tags" %>

<html>
	<head>
		<% if(session.getAttribute(Constants.USER_SESSION) == null) { %>
			<s:include value="jsp/commons/homeLayout.jsp" />
		<% } else { %>
			<s:include value="jsp/commons/layout.jsp" />
		<% } %>
		<script type="text/javascript">
		//<![CDATA[
			/*swfobject.embedSWF(pathURL+"flash/imagerotator.swf", "imageRotator", "800", "500", "9.0.0", pathURL+"flash/expressInstall.swf",  
				{ file: pathURL+"rotator/playlist.xml", width: "800", height: 500 }, { 
					allowScriptAccess: "always",
		            bgcolor: "#ffffff",
		            wmode: "transparent", // opaque
		            allowfullscreen: true
		        }, false
		     );*/	
			Ext.onReady(function() {
				buildLayout(new Ext.Panel({
					region:"center",
					frame: false,
					contentEl: "container"
				}));
			});								
		//]]>
		</script>
	</head>
	<body>
		<s:include value="jsp/commons/header.jsp" />
		
		<div style="display: none">
			<div id="container">
				<div id="imageRotator"></div>
			</div>
		</div>
	</body>
</html>
