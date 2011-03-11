<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%
	boolean mini = request.getParameter("mini")!=null && request.getParameter("mini").toString().equals("1");
	String iframe = request.getParameter("iframe");
%>
<html>
	<head>
		<s:include value="commons/layout.jsp" />
		<script type="text/javascript" src='http://maps.google.com/maps/api/js?sensor=false&language=<s:property value="locale.language" />'></script>
		<script type="text/javascript" src="../js/content/allProjects.js"></script>
		<script type="text/javascript" src="../js/content/commonsProject.js"></script>
		<script type="text/javascript">
		//<![CDATA[
	        var numProjects = '<s:property value="projects.size" />';
	        var projects = { id: new Array(numProjects), name: new Array(numProjects) };
	        var projectsVisibility = new Array(numProjects);			
			// <s:iterator value="projects" status="counter">
				projects.id[<s:property value="#counter.count-1" />] = <s:property value="id" />;
				projects.name[<s:property value="#counter.count-1" />] = '<s:property value="name" />';
				projectsVisibility[<s:property value="#counter.count-1" />] = true; 
			// </s:iterator>
			
			function loadAllProjectsMap() {
				var url = '<s:url action="HostPosition" namespace="/json" encode="false" />';
				Ext.Ajax.request({
		            url: url,
		            success: onSuccess,
		            failure: onFailure,
		            scope: this,
		            params: {
						idProjects: projects.id,
						endDate: (new Date).format("U")
					}
		        });	
			}
			Ext.onReady(function () {
				buildAllProjectsTab('<s:text name="allProjects.panel.main.title" />', '<s:text name="allProjects.panel.map.title" />', 
					'<s:text name="allProjects.descriptionPanel.title" />', '<s:text name="help.allProjects.descriptionPanel" />', 
					'<s:text name="help.allProjects.mapPanel" />', '<s:text name="help.allProjects.gridPanel" />');
				loadAllProjectsMap();
			});
			var openedWindow = null;
			function loadProjectActivityChart(idHost, title, numServices) {
				var limit = 6;
				buildWindowContent("");
				if(openedWindow != null)
					openedWindow.close();
				openedWindow = openWindow("", parseInt((numServices-1) / limit) + 1, idHost, numServices, title);
				openedWindow.show();
				reloadChart(0, idHost, numServices);
			}

			function reloadChart(page, idClickedHost, numServices) {
				var limit = 6;
				var timeSpan = null;
				//if(Ext.getCmp("radio_timeSpan").getValue())
				if(document.getElementById('radio_timeSpan')) {
					timeSpan = document.getElementById('radio_timeSpan').value;					
				}
				else timeSpan = 5;
				var h = 70 + (page < parseInt(numServices/limit)? limit: (numServices%limit)) * 20;
				document.getElementById("divActivityChart").setAttribute("style", "width: 600; height: " + h);				
				var endDate = new Date();
				var url = '<s:url action="HostActivityChart" namespace="/json" encode="false" />?idServer=' + idClickedHost + 
				'&numServices=' + numServices + '&start=' + page*limit + "&limit=" + limit + "&timeSpan=" + timeSpan + 
				"&endDate=" + endDate.format("U");				
				swfobject.embedSWF("../flash/ofc.swf", "graficoAttivita", "590", h, "9.0.0", "../flash/expressInstall.swf", 
					{ "data-file" : urlencode(url), "loading": '<s:text name="chart.loadingMessage" />' }, { 
						allowScriptAccess: "always",
			            bgcolor: "#ffffff",
			            wmode: "transparent" // opaque
		            }, false
				);
				openedWindow.setHeight(h+90);				
			}
			
			function ridireziona(idProject) {
				document.getElementById('idProjectClicked').value = idProject;
				document.getElementById('projectForm').submit();
			}
			
			function changeName(val, metaData, record, rowIndex, colIndex, store) {
				return '<a href="#" onclick="ridireziona(' + record.get("id") + '); return false;">' + val + '</a>';
			}
			
			function changeStartDate(val, metaData, record, rowIndex, colIndex, store) {
				return val + " " + record.get("startTime");
			}

			function changeEndDate(val, metaData, record, rowIndex, colIndex, store) {
				return val + " " + record.get("endTime");
			}
			var allHosts = null;
			function loadGrid(hosts) {
				allHosts = hosts;
				function buildStore(url) {
					return new Ext.data.JsonStore({
						root: 'projects',
			        	totalProperty: 'totalCount',
				        fields: [
				            {name: 'id', type: 'int'}, 'name', 'startDate', 'startTime', 'endDate', 'endTime',
				            'percentages[0]', 'percentages[1]', 'percentages[2]', 'percentages[3]', 'percentages[4]'
				        ],
				        remoteSort: true,
				        sortInfo: { field: 'percentages[0]', direction: 'DESC' },
				        proxy: new Ext.data.HttpProxy({
				            url: url
				        })
				    });
				}		
				var endDate = new Date();
				var url = '<s:url action="MostActiveProjects" namespace="/json" encode="false" />';
				var params = '?numProjects=' + numProjects + "&endDate=" + endDate.format("U");
				var store = buildStore(url + params);

				var grid = new Ext.grid.GridPanel({
					id: "gridProjects",
					height: 240,
			        store: store,
			        trackMouseOver: true,
			        disableSelection: false,
			        bodyCssClass: "grid",
			        loadMask: true,
			        columns: [
			            {
				            align: "center",
				            header: '<s:text name="allProjects.projectsGrid.columns.visible" />',
				            dataIndex: 'null',
				            defaultValue: true,
				            sortable: false,
				            hideable: false,
				            menuDisabled: true,
				            fixed: true,
				            width: 50,
				            renderer: function(val, metaData, record, rowIndex, colIndex, store) {
				            	var id = record.get("id");
			            		var index = projects.id.indexOf(id);
			            		var checked = index == -1 || !projectsVisibility[index]? '': 'checked="checked"';
				            	return '<input type="checkbox" ' + checked + ' onchange="changeMarkerStatus(' + index + ', this.checked);" ></input>';				            	
			            	}
				        },
				        {
				            header: '<s:text name="allProjects.projectsGrid.columns.name" />',
				            dataIndex: "name",
				            sortable: true,
				            renderer: changeName
				        }, {
				            header: '<s:text name="allProjects.projectsGrid.columns.startDate" />',
				            dataIndex: "startDate",
				            sortable: true,
				            renderer: changeStartDate
				        }, {
				        	header: '<s:text name="allProjects.projectsGrid.columns.endDate" />',
				            dataIndex: "endDate",
				            sortable: true,
				            renderer: changeEndDate 
				        }, {
				            header: '<s:text name="allProjects.projectsGrid.columns.percentage0" />',
				            dataIndex: "percentages[0]",
				            sortable: true,
				            renderer: changePercentage
				        }, {
				            header: '<s:text name="allProjects.projectsGrid.columns.percentage1" />',
				            dataIndex: "percentages[1]",
				            sortable: true,
				            renderer: changePercentage
				        }, {
				            header: '<s:text name="allProjects.projectsGrid.columns.percentage2" />',
				            dataIndex: "percentages[2]",
				            sortable: true,
				            renderer: changePercentage
				        }, {
				            header: '<s:text name="allProjects.projectsGrid.columns.percentage3" />',
				            dataIndex: "percentages[3]",
				            sortable: true,
				            renderer: changePercentage
				        }, {
				            header: '<s:text name="allProjects.projectsGrid.columns.percentage4" />',
				            dataIndex: "percentages[4]",
				            sortable: true,
				            renderer: changePercentage
				        }
				    ],
			        bbar: new Ext.PagingToolbar({
			            pageSize: <s:text name="allProjects.projectsGrid.limit" />,
			            store: store,
			            displayInfo: true,
			            displayMsg: '<s:text name="allProjects.projectsGrid.displayMsg" />',
			            emptyMsg: '<s:text name="allProjects.projectsGrid.emptyMsg" />',
			            items: [
							'-', {
								text: '<s:text name="grid.button.update" />',
								handler: function (button, eventObject) {
									endDate = new Date();
									params = '?numProjects=' + numProjects + "&endDate=" + endDate.format("U");
									store = buildStore(url + params);
									store.load({params:{start:0, limit:<s:text name="allProjects.projectsGrid.limit" />}});
									grid.reconfigure(store, grid.getColumnModel());
									grid.getBottomToolbar().bindStore(store);
									panel.setTitle(String.format('<s:text name="allProjects.panel.description.title" />', 
											Ext.util.Format.date(endDate), endDate.format("H:i")));
								}
							}, {
			                	text: '<s:text name="grid.button.exportXML" />',
			                	handler: function (button, eventObject) {
				                	var url = '<s:url action="MostActiveProjectsXML" namespace="/xsl" encode="false" />';
				                	var state = store.getSortState();
			        				window.open(url + params + "&sort=" + state.field + "&dir=" + state.direction);
			                	}			                	
			                }, {
			                	text: '<s:text name="grid.button.exportCSV" />',
			                	handler: function (button, eventObject) {
			                		var url = '<s:url action="MostActiveProjectsCSV" namespace="/stream" encode="false" />';
			                		var state = store.getSortState();
			        				window.open(url + params + "&sort=" + state.field + "&dir=" + state.direction);
			                	}
			                }
			            ]
			        }),
			        stripeRows: true,
			        viewConfig: {
			            forceFit: true
					}
				});
				var panel = Ext.getCmp("gridPanel");
				panel.setTitle(String.format('<s:text name="allProjects.panel.description.title" />', 
						Ext.util.Format.date(endDate), endDate.format("H:i"))); 
				panel.add(grid);	    		
	    		panel.doLayout(false, true);
	    		grid.syncSize();
	    		store.load({params:{start:0, limit:<s:text name="allProjects.projectsGrid.limit" />}});
			}

			function changeMarkerStatus(index, checked) {
				if(index == -1) return;
				projectsVisibility[index] = checked;
				var records = new Array();
				for(var i = 0; i < numProjects; i ++)
					if(projectsVisibility[i])
						records.push(projects.id[i]);
				
				for(var i = 0; i < allHosts.length; i ++) {
					var markerRequired = false;
					for(var j = 0; j < records.length; j ++)
						if(allHosts[i].idProjects.indexOf(records[j]) != -1) {
							markerRequired = true; 
							break;
						}
					markers[i].setVisible(markerRequired);														
				}
			}

			function markerOnClickListener(host) {
				if(openedWindow != null)
					openedWindow.close();
				var list = "<p>";
				for(var i = 0; i < numProjects; i ++)
					if(host.idProjects.indexOf(projects.id[i]) != -1)
						list += projects.name[i] + ", ";
				if(numProjects > 0)
					list = list.substring(0, list.length-2);
				list += "</p>";
				var unknown = '<s:text name="allProjects.infoWindow.table.unknownValue" />';
				return String.format('<table class="infoWindowTable"><tbody>' +
					'<tr><td><s:text name="allProjects.infoWindow.table.row.host" /></td><td>{0}</td></tr>' +
					'<tr><td><s:text name="allProjects.infoWindow.table.row.ip" /></td><td>{1}</td></tr>' +
					'<tr><td><s:text name="allProjects.infoWindow.table.row.city" /></td><td>{2}</td></tr>' + 
					'<tr><td><s:text name="allProjects.infoWindow.table.row.activity" /></td><td>{3} <s:text name="allProjects.infoWindow.table.row.activity.timeFrame" /></td></tr>' + 
					'<tr><td><s:text name="allProjects.infoWindow.table.row.services" /></td><td>{4}&nbsp;&nbsp;&nbsp;' + 
						'<a href="#" onclick="loadProjectActivityChart(' + host.id + ', \'{5}\', ' + host.numInstances + ');return false;"><s:text name="allProjects.infoWindow.table.row.services.details" /></a></td></tr>' + 
					'<tr><td><s:text name="allProjects.infoWindow.table.row.projects" /></td><td>{6}</td></tr>' + 
					'</tbody></table>', 
					host.name==null? unknown: host.name, host.ip, host.city==null? unknown: host.city, 
					host.activity==null? unknown: (host.activity + "%"), host.numInstances, host.name==null? host.ip: host.name, 
					list);				
			}

			var permLinkWindow = null;
			function showPermLink() {
				if(!permLinkWindow) {
					var url = String.format('monitoring/AllProjectsMini?request_locale={0}', '<s:property value="locale.language" />');
					permLinkWindow = new Ext.Window({
						title: '<s:text name="allProjects.permLink" />',
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
		<s:include value="parts/infoWindow.jsp" />
	</head>
	<body>
		<s:include value="commons/header.jsp" />
	
		<div id="container" class="x-hide-display">
			<div id="map_canvas" style="height: 500;"></div>
		</div>
		
		<div id="descriptionContent" class="x-hide-display">
			<table>
				<tbody>
					<tr>
						<td><p style="font-weight: bold;"><s:text name="allProjects.descriptionPanel.startDate" />:</p></td>
						<td><p><s:property value="startDate" /></p></td>
					</tr>
					<tr>
						<td><p style="font-weight: bold;"><s:text name="allProjects.descriptionPanel.projectsNumber" />:</p></td>
						<td><p><s:property value="projects.size" /></p></td>
					</tr>
					<tr>
						<td><p style="font-weight: bold;"><s:text name="allProjects.descriptionPanel.hostsNumber" />:</p></td>
						<td><p><s:property value="numHosts" /></p></td>
					</tr>
					<tr>
						<td><p style="font-weight: bold;"><s:text name="allProjects.descriptionPanel.servicesNumber" />:</p></td>
						<td><p><s:property value="numServices" /></p></td>
					</tr>					
					<tr>
						<td><p style="font-weight: bold;"><s:text name="allProjects.descriptionPanel.description" />:</p></td>
						<td><p><s:text name="allProjects.descriptionPanel.description.text" /></p></td>
					</tr>
					<tr>
						<td><p style="font-weight: bold;"><s:text name="allProjects.permLink" /></p></td>
						<td>
							<p>
								<a href="#" onclick="showPermLink(); return false;">
									code
								</a>
							</p>
						</td>
					</tr>
				</tbody>
			</table>
		</div>
		
		<div style="display: none;">
			<form method="post" id="projectForm" action='<%if(mini) { %><s:url action="ProjectMini" namespace="/monitoring" encode="false"/> <% } else { %><s:url action="Project" namespace="/monitoring" encode="false"/><% } %>'>
				<s:hidden id="idProjectClicked" name="idProject" />				
				<input type="hidden" name="iframe" value="<%=iframe %>" />
			</form>
		</div>
		
		<div id="infoWindow" style="display: none;"></div>
	</body>
</html>
