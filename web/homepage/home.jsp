<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<html>

	<head>
		<s:include value="../jsp/commons/include.jsp" />
		
		<script type="text/javascript">
		//<![CDATA[
			swfobject.embedSWF(pathURL+"flash/imagerotator.swf", "imageRotator", "100%", "100%", "9.0.0", pathURL+"flash/expressInstall.swf",  
				{ file: pathURL+"rotator/playlist.xml", screencolor: "0xE0EDF8",  shuffle: false, rotatetime: 5 }, { 
					allowScriptAccess: "always",
		            bgcolor: "#ffffff",
		            width: "100%", 
		            height: "100%",
		            wmode: "transparent", // opaque
		            allowfullscreen: true
		        }, false
		     );	
			Ext.onReady(function() {
				Ext.QuickTips.init();
				Ext.form.Field.prototype.msgTarget = 'side';
				
				var viewport = new Ext.Viewport({
					layout: 'border',
					items: [
					    {
				        	region: 'center',
				        	width: '88%',
				        	frame: false,	
				        	bgcolor: "black",autoScroll: true,
				        	margins: '3 3 0 3',
				        	items: [
					        	new Ext.Panel({							   		
							    	frame: true,
							    	autoHeight: true,	    	 
							    	//height: 125,
							    	contentEl: "northEl"									
						        }),
							    new Ext.Panel({
									autoScroll: true,									
									width: "100%",
									height: "100%",
									contentEl: "container"
								})
							]
				        },
				        {
				        	region: 'east',
				        	width: '6%',
				        	bodyStyle: 'background-image: url(' + bgPath + ')',
				        	frame: false
				        },
				        {
				        	region: 'west',
				        	width: '6%',
				        	bodyStyle: 'background-image: url(' + bgPath + ')',
				        	frame: false
				        }
				    ]
				});	
				
				loadNews();
			});
			
			function loadNews() {
				Ext.Ajax.request({
					url: '<s:url action="News" namespace="/json" encode="false" />',
					success: function(response, opts) {
						var data = JSON.parse(response.responseText);
						var html = '';
						for(var i = 0; i < data.length; i ++) 
							html += String.format('<p><span style="font-weight: bold;">{0}</span><br />{1}</p><br />', data[i].date, data[i].text);
						document.getElementById('newsScroller').innerHTML=html;						
					},
					failure: onFailure,
					params: {
						start: 0,
						limit: 5
					},
					scope: this
				});
			}			
		//]]>
		</script>
		<link href="homepage/styles.css" media="screen" rel="Stylesheet" type="text/css">
		<link href="homepage/front.css" media="screen" rel="Stylesheet" type="text/css">
	</head>
	<body>
	
	<s:include value="../jsp/commons/header.jsp" />
		<div id="container" class="x-hide-display">
			<div id="doc2">				
			<div id="bd">
				<div id="frontpage_body">
					<center>
					<div class="box" style="width: 70%;">
						<b class="xtop"><b class="xb1"></b><b class="xb2"></b><b class="xb3"></b><b class="xb4"></b></b>
						<div class="content" style="height: 29em;">
							<!--  <div id="imageRotator"></div>
							<img src="rotator/image1.png" width="100%" height="100% "alt="" />-->
							<p style="font-size: 108%; margin: 0.2em 0pt 0.2em; text-align: center;">
								<b>ESGF Dashbard</b>
							</p>
							<img src="img/global_view.png" width="100%" alt="" />
						</div>
						<b class="xbottom"><b class="xb5"></b><b class="xb6"></b><b class="xb7"></b><b class="xb1"></b></b>
					</div>
					</center>
					<!-- Register / Login -->					
					<div class="box" style="float: right; width: 30%;">
						<b class="xtop"><b class="xb1"></b><b class="xb2"></b><b class="xb3"></b><b class="xb4"></b></b>
						<div class="content" style="height: 29em;">
						
							<!-- <center>
								<s:a action="RegistrationPage" namespace="/secure" title="Register for an account on ESGF Dashboard" cssClass="register_button" onclick="alert('This functionality will be available soon'); return false;"/>
							</center> -->  
						    <p style="font-size: 108%; margin: 0.2em 0pt 0.2em; text-align: center;">
								<b><!-- or --> Login:</b>
							</p>
							
							<div id="login">
								<b class="xtop"><b class="xb1"></b><b class="xb2"></b><b class="xb3"></b><b class="xb4"></b></b>
								<div class="login">
									<form onsubmit='login(document.getElementById("loginUsername").value, document.getElementById("loginPassword").value); return false;'>
										<p>
										  <label for="loginUsername">
											  Username:
										  </label>
										</p>
										<p>
											<input id="loginUsername" type="text">
										</p>
										<p>
											<label for="loginPassword">
												Password:
											</label>
										</p>
										<p>
											<input id="loginPassword" type="password">
										</p>										
										<p style="margin-top: 0.8em;">
											<input name="commit" value="" class="submit" title="Log in to ESGF Dashboard" type="submit">
										</p>										
									</form>
								</div>
								<b class="xbottom"><b class="xb4"></b><b class="xb3"></b><b class="xb2"></b><b class="xb1"></b></b>
								<p style="font-size: 108%; margin: 0.2em 0pt 0.2em; text-align: center;">
									<b>or try as guest:</b>
								</p>
								<b class="xtop"><b class="xb1"></b><b class="xb2"></b><b class="xb3"></b><b class="xb4"></b></b>
								<div class="login">
									<form onsubmit='alert("This functionality will be available soon"); return false; login(document.getElementById("loginGuest").value, ""); return false;'>
										<p>
										  <label for="loginGuest">
											  Username:
										  </label>
										</p>
										<p>
											<input id="loginGuest" type="text" value="guest">
										</p>
										<p style="margin-top: 0.8em;">
											<input name="commit" value="" class="submit" title="Log in to ESGF Dashboard as Guest" type="submit">
										</p>										
									</form>
								</div>
								<b class="xbottom"><b class="xb4"></b><b class="xb3"></b><b class="xb2"></b><b class="xb1"></b></b>
								<div style="height: 1.2em"></div>
								<div id="hd" class="frontpage_header">
									<!-- <img alt="Logo" src="img/title.png" class="logo"> -->
									<div class="text">
									  Welcome to the ESGF-Dashboard, the distributed monitoring system of the Earth System Grid Federation
									</div>
									<div class="clearer">&nbsp;</div>
								</div>									
							</div>							
						</div>						
						<b class="xbottom"><b class="xb5"></b><b class="xb6"></b><b class="xb7"></b><b class="xb1"></b></b>
					</div>
								
					<br class="clearer">&nbsp;<br>
				
					<!-- About -->
					<div class="box" style="width: 30%;">
						<b class="xtop"><b class="xb1"></b><b class="xb2"></b><b class="xb3"></b><b class="xb4"></b></b>
						<div class="content" style="height: 16em; padding: 0.3em 1.5em;">
							<p class="heading">
								About
							</p>
							<p>
								ESGF Dashboard is a ....
							</p>
							<p>
								You need to be a registered user to get access to the full ESGF Dashboard features.
								Registration is completely free!
							</p>
							</div>
							<b class="xbottom"><b class="xb5"></b><b class="xb6"></b><b class="xb7"></b><b class="xb1"></b></b>
						</div>
						<div class="box-invisible" style="width: 5%">
							<b class="xtop"><b class="xb1"></b><b class="xb2"></b><b class="xb3"></b><b class="xb4"></b></b>
							<div class="content" style="height: 16em; padding: 0.3em 1.5em; overflow: hidden;">
							</div>
							<b class="xbottom"><b class="xb5"></b><b class="xb6"></b><b class="xb7"></b><b class="xb1"></b></b>
						</div>
						<!-- Who -->						
						<div class="box" style="width: 30%;">
							<b class="xtop"><b class="xb1"></b><b class="xb2"></b><b class="xb3"></b><b class="xb4"></b></b>
							<div class="content" style="height: 16em; padding: 0.3em 1.5em;">
								<p class="heading">
									Who?
								</p>	
								<p>
									ESGF Dashboard is a research effort...
								</p>
							</div>
							<b class="xbottom"><b class="xb5"></b><b class="xb6"></b><b class="xb7"></b><b class="xb1"></b></b>
						</div>
						<div class="box-invisible" style="width: 5%">
							<b class="xtop"><b class="xb1"></b><b class="xb2"></b><b class="xb3"></b><b class="xb4"></b></b>
							<div class="content" style="height: 16em; padding: 0.3em 1.5em; overflow: hidden;">
							</div>
							<b class="xbottom"><b class="xb5"></b><b class="xb6"></b><b class="xb7"></b><b class="xb1"></b></b>
						</div>
						<!-- Latest Announcements -->
						<div class="box" style="width: 30%;">
							<b class="xtop"><b class="xb1"></b><b class="xb2"></b><b class="xb3"></b><b class="xb4"></b></b>
							<div class="content" style="height: 16em; padding: 0.3em 1.5em; overflow: hidden;">
								<p class="heading">
								<!-- 
									<img src="feed-icon.png" alt="Subscribe to site announcements icon" title="Subscribe to site announcements RSS feed">
								-->
									Announcements
								</p>								
								<marquee id="newsScroller" hspace="5" height="12em" width="100%" onmouseout="javascript:this.start()" onmouseover="javascript:this.stop()" scrollamount="1" scrolldelay="2" direction="up">
									<p>
										ESGF Dashboard v1.0 released today!<br/>Please register yourself to get free/full access to the system.
									</p>
								</marquee>							
							</div>
							<b class="xbottom"><b class="xb5"></b><b class="xb6"></b><b class="xb7"></b><b class="xb1"></b></b>
						</div>
						
						<br class="clearer">&nbsp;<br>						
	        </div>
	      </div>
				<div id="ft">
			        <%-- <p class="copyright">
						Copyright 2010
						<a href="http://www.unisalento.it/" target="_blank">The University of Salento</a>						
					</p>--%>				
			    </div>
			    <div style="padding-bottom: 0.5em; text-align: center;">
				</div>
		</div>
      </div>
	</body>
	</html>
