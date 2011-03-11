<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<html>
	<head>
		<s:include value="commons/homeLayout.jsp" />
		
		<script type="text/javascript">
		//<![CDATA[
			Ext.onReady(function() {
				buildLayout(new Ext.Panel({
					region: "center", 
					frame: true,
					//autoHeight: true,
					layout: "hbox",
					layoutConfig: {
					 	 pack: 'center',
					     align: 'top'		                    	                        
                    },
					items: [
						new Ext.Panel({
							frame: false,
							height: 100,
							html: '<br/><br/><p class="activationStatus">' + (
							<s:property value="success" default="false" />? '<s:text name="activationStatus.success" />': 
							'<s:text name="activationStatus.failure" />') + '</p>'
						})
					]					
				}));
			});
		//]]>
		</script>
		<meta HTTP-EQUIV="Refresh" CONTENT='6;URL=<s:url action="Home" namespace="/" encode="false" />' />
	</head>
	<body>
		<s:include value="commons/header.jsp" />	
	</body>
</html>
