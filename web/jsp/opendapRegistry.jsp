<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<html>
	<head>
		<s:include value="commons/layout.jsp" />
		
		<script type="text/javascript">
		//<![CDATA[
			var idProject = <s:property value="idProject" />;

			Ext.onReady(function() {
				var tabs = Ext.getCmp('tab-panel');
				tabs.add({
					title: '<s:text name="opendapRegistry.title" />',
					frame: true,
					autoScroll: true,
					items: [
						new Ext.Panel({
							margins: '3 0 3 3',
						    cmargins: '3 3 3 3',
						    title: '<s:text name="opendapRegistry.information.title" />',
							frame: true,
							collapsible: true,
							titleCollapse: true,
							collapsed: true,
							contentEl: 'registryInformation',
							tools: [{        			
								id: "help",
								qtip: "Help",
								handler: function(event, toolEl, panel, tc) {
									openHelpWindow(toolEl, '<s:text name="help.opendapRegistry.descriptionPanel" />');				
								}
							}]
						}),
						new Ext.Panel({
							title: '<s:text name="opendapRegistry.dataBrowsers.title" />',
							items: [											
								new Ext.TabPanel({							
									activeTab: 0,
									defaults: { autoScroll:true },
									enableTabScroll: true,
							        frame: true,
							        items: [
										// <s:iterator value="services" status="counter">
											{
												title: '<s:property value="hostName" />',
												frame: true,										
									            autoHeight: true,
									            width: '100%',
									            items: [
											        new Ext.Panel({
												    }),
											        new Ext.Panel({									        
											        	height: 600,
											        	width: '100%',
											        	tbar: {
											        		items: [
																new Ext.Button({
																	iconCls: 'homeIcon',
																	tooltip: 'Home',
																	handler: function() {
																		document.getElementById('iframe_opendap<s:property value="#counter.count" />').setAttribute("src", document.getElementById('iframe_opendap<s:property value="#counter.count" />').src);					
																	}	
																}), 
																new Ext.Panel({ width: 10 }),
																new Ext.Panel({
																	html: '<p>Port: <s:property value="portNumber" /></p>'
																}),
																'->',
																new Ext.Button({
																	iconCls: "helpButton",
																	tooltip: 'Help',
																	handler: function(button, eventObject) {
																		openHelpWindow(button, '<s:text name="help.opendapRegistry.dataBrowserPanel" />');								
																	}					
																})
													        ]
											        	},
														html: '<iframe id="iframe_opendap<s:property value="#counter.count" />" width="100%" height="100%" frameborder="0" marginheight="0" marginwidth="0" src="<s:property value="url" />"></iframe>'
												    })
											    ]							             
											},
										// </s:iterator>					        	
									]
								})
							]
						})
					]
				}).show();
				
				var treePanel = Ext.getCmp('tree-panel'); 
				if(treePanel)
					treePanel.loader.on('load', function() {
						var nodo = treePanel.root.findChild('myObject', idProject, false);
						nodo.expand();
						nodo = nodo.findChild('myObject', "opendapRegistry", false);
						nodo.select();
						nodo.setText("<b>" + nodo.attributes.text + "</b>");
					});
			});

			var permLinkWindow = null;
			function showPermLink() {
				if(!permLinkWindow) {
					var url = String.format('opendap/OpendapRegistryMini?request_locale={0}&idProject={1}', '<s:property value="locale.language" />', idProject);
					permLinkWindow = new Ext.Window({
						title: 'Permalink',
			            width: 400,
			            height: 250,
			            closeAction: 'hide',
			            plain: true,
			            items: [
					        new Ext.Panel({
				                deferredRender: false,
				                frame: true,
				                title: 'Authenticated',
				                border: false,
				                html: '<p>' + 
			                	Ext.util.Format.htmlEncode('<iframe src="'+ pathURL + url + '&iframe=' + escape(url) + '" height="600" width="100%"></iframe>') + '</p>'
				            }),
				            // new Ext.Panel({height: 10, frame: true}),
				            new Ext.Panel({
				            	deferredRender: false,
				                frame: true,
				                title: 'Guest',
				                border: false,
				                html: '<p>' + 
			                	Ext.util.Format.htmlEncode('<iframe src="'+ pathURL + url + '&guest=1&iframe=' + escape(url+'&guest=1') + '" height="600" width="100%"></iframe>') + '</p>'
				            })
			            ],
			            buttons: [
			                {
				                text: 'Close',
				                handler: function(){
				            		permLinkWindow.hide();
				                }
				            }
			            ]
			        });
				}
				permLinkWindow.show();	
			}			
		//]]>
		</script>
	</head>	
	<body>
		<s:include value="commons/header.jsp" />
				
		<div class="x-hide-display">
			<div id="registryInformation">
				<table cellspacing="5">
					<tbody>
						<tr>
							<td>
								<p style="font-weight: bold;"><s:text name="opendapRegistry.information.servicesNumber" />: </p>
							</td>
							<td>
								<p><s:property value="services.size" /></p>
							</td>
						</tr>
						<tr>
							<td><p style="font-weight: bold;"><s:text name="opendapRegistry.information.permLink" />: </p></td>
							<td><p><a href="#" onclick="showPermLink(); return false;">code</a></p></td>
						</tr>
					</tbody>
				</table>
			</div>
		</div>
	</body>
</html>
