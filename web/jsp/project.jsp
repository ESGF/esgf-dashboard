<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<%
	String parameter = request.getParameter("mini"); 
	boolean mini = parameter!=null && parameter.equals("1");
%>

<html>
	<head>
		<s:include value="commons/layout.jsp" />
		<script type="text/javascript" src='http://maps.google.com/maps/api/js?sensor=false&language=<s:property value="locale.language" />'></script>
		<script type="text/javascript">
		//<![CDATA[
			var idProject = '<s:property value="idProject" />';
			var nullValue = '{"bg_colour":"#DFE8F6", "border":1, "elements":[{"type":"pie","values":[]}],"title":{"style":"{font-size: 12px; color:#000000; font-family: Arial; text-align: center;}","text":"No Data to Show"}}';			
			Ext.onReady(function () {
				//alert('versione nuova1');
				loadAllProject();
				//alert('versione nuova2');
				//loadProjectMap();
				//loadRegisteredUsersMap();
				//loadNodeTypeMap();
			});

			function redirectToHost(idHost) {
				document.getElementById('idServerClicked').value = idHost;
				document.getElementById('serverForm').submit();
			}
		//]]>
		</script>
		<s:include value="parts/projectHeaderPart.jsp" />
		<s:include value="parts/infoWindow.jsp" />
	</head>
	
	<body>
		<s:include value="commons/header.jsp" />		
		
		<s:include value="parts/projectBodyPart.jsp" />
	</body>
</html>
