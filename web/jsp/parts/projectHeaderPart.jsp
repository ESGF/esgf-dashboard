<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<script type="text/javascript" src="https://www.google.com/jsapi"></script>
<script type="text/javascript" src="../js/earth/earthInit.js"></script>
<script type="text/javascript" src="../js/content/project.js"></script>
<script type="text/javascript" src="../js/content/commonsProject.js"></script>

<script type="text/javascript">
//<![CDATA[
    function loadAllProject() {
    	buildProjectTab('<s:text name="project.tab.main.title"><s:param><s:property value="project.name" /></s:param></s:text>', '<s:text name="project.panel.map.availability.title" />', 
    			'<s:text name="project.panel.map.registeredusers.title" />','<s:text name="project.panel.map.nodetype.title" />', '<s:text name="project.panel.map.title" />','<s:text name="project.panel.description.title" />', 
				'<s:text name="project.panel.projectAvailability" />', '<s:text name="project.panel.projectFailure" />', '<s:text name="project.panel.serviceDistribution" />',
				'<s:text name="project.panel.userDistribution" />',
				'<s:text name="help.project.descriptionPanel" />', '<s:text name="help.project.mapPanel" />', '<s:text name="help.project.gridPanel" />');
    	loadAvgProjectActivityPieChart(true, <s:text name="default.timespan" />);
    	loadAvgProjectActivityBarChartOff(true, <s:text name="default.timespan" />);
    	loadServicesDistribution();
    	loadUsersDistribution();
    }       
    
	function loadProjectMap() {
		var url = '<s:url action="HostPositionBySingleProject" namespace="/json" encode="false" />';
		
		Ext.Ajax.request({
            url: url,
            success: onSuccess,
            failure: onFailure,
            scope: this,
            params: {
				idProjects: [ idProject ],
				endDate: (new Date).format("U")
			}
        });	
	}
	
	function loadRegisteredUsersMap() {
		var url = '<s:url action="HostPositionBySingleProjectRegUsers" namespace="/json" encode="false" />';

		Ext.Ajax.request({
           url: url,
           success: onSuccessUsers,
           failure: onFailure,
           scope: this,
           params: {
				idProjects: [ idProject ],
				endDate: (new Date).format("U")
			}
       });
	}

	function loadNodeTypeMap() {
		var url = '<s:url action="HostPositionBySingleProject" namespace="/json" encode="false" />';

		Ext.Ajax.request({
           url: url,
           success: onSuccessNodeType,
           failure: onFailure,
           scope: this,
           params: {
				idProjects: [ idProject ],
				endDate: (new Date).format("U")
			}
       });
	}

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
		if(document.getElementById('radio_timeSpan')) {
			timeSpan = document.getElementById('radio_timeSpan').value;					
		}
		else timeSpan = 5;
		var h = 70 + (page < parseInt(numServices/limit)? limit: (numServices%limit)) * 20;
		document.getElementById("divActivityChart").setAttribute("style", "width: 600; height: " + h);				
		var endDate = new Date();
		var url = '<s:url action="HostActivityChart" namespace="/json" encode="false" />?idServer=' + idClickedHost + 
		'&numServices=' + numServices + '&start=' + page*limit + "&limit=" + limit + "&timeSpan=" + timeSpan + 
		"&endDate=" + endDate.format("U") + "&idProject=" + idProject;

		swfobject.embedSWF("../flash/ofc.swf", "graficoAttivita", "590", h, "9.0.0", "../flash/expressInstall.swf", 
			{ "data-file" : urlencode(url), "loading": '<s:text name="chart.loadingMessage" />' }, { 
				allowScriptAccess: "always",
	            bgcolor: "#ffffff",
	            wmode: "transparent" // opaque
            }, false
		);
		openedWindow.setHeight(h+90);
	}

	var hostsVisibility;
	function loadGrid(hosts) {
		var numHosts = hosts.length;
		hostsVisibility = new Array(numHosts);
		for(var i = 0; i < numHosts; hostsVisibility[i ++] = true);			
		function buildStore(url) {
			return new Ext.data.JsonStore({
		        root: 'hosts',
		        totalProperty: 'totalCount',
		        remoteSort: true,
		        sortInfo: { field: 'percentages[0]', direction: 'DESC' },
		        fields: [
		            {name: 'id', type: 'int'}, 'name', 'ip', 
		            'percentages[0]', 'percentages[1]', 'percentages[2]', 'percentages[3]', 'percentages[4]'
		        ],
		        proxy: new Ext.data.HttpProxy({
		            url: url 
		        })
		    });
		}
		var endDate = new Date();
		var url = '<s:url action="MostActiveHosts" namespace="/json" encode="false" />';
		var params = '?idProject=' + idProject + '&numHosts=' + numHosts + "&endDate=" + endDate.format("U");
		var store = buildStore(url + params);

		var grid = new Ext.grid.GridPanel({
			id: "hostsGrid",
			height: 240,
			store: store,
	        trackMouseOver: true,
	        disableSelection: false,
	        bodyCssClass: "grid",
	        loadMask: true,
	        columns: [
				{
				    align: "center",
				    header: '<s:text name="project.hostsGrid.columns.visible" />',
				    dataIndex: 'null',
				    defaultValue: true,
				    sortable: false,
				    hideable: false,
				    menuDisabled: true,
				    fixed: true,
				    width: 50,
				    renderer: function(val, metaData, record, rowIndex, colIndex, store) {
				    	var id = record.get("id");
				    	var index;
				    	for(index = 0; index < numHosts && hosts[index].id != id; index ++) ;
						var checked = index == numHosts || !hostsVisibility[index]? '': 'checked="checked"';
				    	return '<input type="checkbox" ' + checked + ' onchange="changeMarkerStatus(' + index + ', this.checked);" ></input>';				            	
					}
				}, {
		            header: '<s:text name="project.hostsGrid.columns.name" />',
		            dataIndex: "name",
		            sortable: true,
		            renderer: function(val, metaData, record, rowIndex, colIndex, store) {
		            	return '<a href="#" onclick="redirectToHost(' + record.get("id") + '); return false;">' + val + '</a>';
		            }
		        }, {
		            header: '<s:text name="project.hostsGrid.columns.ip" />',
		            dataIndex: "ip",				      
		            sortable: true		            
		        }, {
		            header: '<s:text name="project.hostsGrid.columns.percentage0" />',
		            dataIndex: "percentages[0]",
		            sortable: true,
		            renderer: changePercentage
		        }, {
		            header: '<s:text name="project.hostsGrid.columns.percentage1" />',
		            dataIndex: "percentages[1]",
		            sortable: true,
		            renderer: changePercentage
		        }, {
		            header: '<s:text name="project.hostsGrid.columns.percentage2" />',
		            dataIndex: "percentages[2]",
		            sortable: true,
		            renderer: changePercentage
		        }, {
		            header: '<s:text name="project.hostsGrid.columns.percentage3" />',
		            dataIndex: "percentages[3]",
		            sortable: true,
		            renderer: changePercentage
		        }, {
		            header: '<s:text name="project.hostsGrid.columns.percentage4" />',
		            dataIndex: "percentages[4]",
		            sortable: true,
		            renderer: changePercentage
		        }
		    ],
	        bbar: new Ext.PagingToolbar({
	            pageSize: <s:text name="project.hostsGrid.limit" />,
	            store: store,
	            displayInfo: true,
	            displayMsg: '<s:text name="project.hostsGrid.displayMsg" />',
	            emptyMsg: '<s:text name="project.hostsGrid.emptyMsg" />',
	            items: [
					'-',
					{
						text: '<s:text name="grid.button.update" />',
						handler: function (button, eventObject) {
							endDate = new Date();
							params = '?idProject=' + idProject + '&numHosts=' + numHosts + "&endDate=" + endDate.format("U");
							store = buildStore(url + params);
							store.load({params:{start:0, limit:<s:text name="project.hostsGrid.limit" />}});
							grid.reconfigure(store, grid.getColumnModel());
							grid.getBottomToolbar().bindStore(store);
							panel.setTitle(String.format('<s:text name="project.panel.grid.title" />', 
									Ext.util.Format.date(endDate), endDate.format("H:i")));
						}
					}, {
	                	text: '<s:text name="grid.button.exportXML" />',
	                	handler: function(button, eventObject) {
		                	var url = '<s:url action="MostActiveHostsXML" namespace="/xsl" encode="false" />';
		                	window.open(url + params);
	                	}		                	                                
	                }, {
	                	text: '<s:text name="grid.button.exportCSV" />',
	                	handler: function(button, eventObject) {
	                		var url = '<s:url action="MostActiveHostsCSV" namespace="/stream" encode="false" />';
		                	window.open(url + params);
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
   		panel.add(grid);
   		panel.setTitle(String.format('<s:text name="project.panel.grid.title" />', 
				Ext.util.Format.date(endDate), endDate.format("H:i"))); 
   		panel.doLayout(false, true);
		grid.syncSize();
		store.load({params:{start:0, limit:<s:text name="project.hostsGrid.limit" />}});
	}

	function changeMarkerStatus(index, checked) {
		if(index == -1) return;
		markers[index].setVisible(hostsVisibility[index] = checked);
	}
	
	function markerOnClickListener(host) {
		if(openedWindow != null)
			openedWindow.close();
		var list = '<p><s:property value="project.name" /></p>';
		var unknown = '<s:text name="project.infoWindow.table.unknownValue" />';
		return String.format('<table class="infoWindowTable"><tbody>' +
			'<tr><td><s:text name="project.infoWindow.table.row.host" /></td><td><a href="#" onclick="redirectToHost({0}); return false;">{1}</a></td></tr>' +
			'<tr><td><s:text name="project.infoWindow.table.row.ip" /></td><td>{2}</td></tr>' +
			'<tr><td><s:text name="project.infoWindow.table.row.city" /></td><td>{3}</td></tr>' + 
			'<tr><td><s:text name="project.infoWindow.table.row.regusers" /></td><td>{8}</td></tr>' +			
			'<tr><td><s:text name="project.infoWindow.table.row.activity" /></td><td>{4} <s:text name="project.infoWindow.table.row.activity.timeFrame" /></td></tr>' + 
			'<tr><td><s:text name="project.infoWindow.table.row.services" /></td><td>{5}&nbsp;&nbsp;&nbsp;' + 
				'<a href="#" onclick="loadProjectActivityChart(' + host.id + ', \'{6}\', ' + host.numInstances + ');return false;"><s:text name="project.infoWindow.table.row.services.details" /></a></td></tr>' + 
			'<tr><td><s:text name="project.infoWindow.table.row.projects" /></td><td>{7}</td></tr>' + 
			'</tbody></table>', host.id, 
			host.name==null? unknown: host.name, host.ip, host.city==null? unknown: host.city, 
			host.activity==null? unknown: (host.activity + "%"), host.numInstances, host.name==null? host.ip: host.name, 
			list,host.regUsers);				
	}
	var permLinkWindow = null;
	function showPermLink() {
		if(!permLinkWindow) {
			var url = String.format('monitoring/ProjectMini?request_locale={0}&idProject={1}', '<s:property value="locale.language" />', idProject);
			permLinkWindow = new Ext.Window({
				title: '<s:text name="project.permLink" />',
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

	var dataAvgProjectActivityPie = 'null';
	function get_data_avgProjectActivityPie() {
		if(dataAvgProjectActivityPie == 'null') return nullValue; 
		return dataAvgProjectActivityPie;		
	}
	var dataAvgProjectActivityBarOff = 'null';
	function get_data_avgProjectActivityBarOff() {		
		if(dataAvgProjectActivityBarOff == 'null') return nullValue; 
		return dataAvgProjectActivityBarOff;		
	}	
	var dataServicesDistributionPie = 'null';
	function get_data_servicesDistributionPie() {
		if(dataServicesDistributionPie == 'null') return nullValue; 
		return dataServicesDistributionPie;		
	}
	
	var dataUsersDistributionPie = 'null';
	function get_data_usersDistributionPie() {
		if(dataUsersDistributionPie == 'null') return nullValue; 
		return dataUsersDistributionPie;		
	}
		
	function loadAvgProjectActivityPieChart(embed, timeSpan) {
		var urlPie = '<s:url action="AvgProjectActivityPieChart" namespace="/json" encode="false" />';
		Ext.Ajax.request({
            url: urlPie,
            success: function (response, opts) {           
				dataAvgProjectActivityPie = response.responseText;	
				if(embed)
					swfobject.embedSWF("../flash/ofc.swf", "avgProjectActivityPieChart", "100%", "170", "9.0.0", "../flash/expressInstall.swf",  
						{ "get-data": "get_data_avgProjectActivityPie", "loading": '<s:text name="chart.loadingMessage" />' }, { 
							allowScriptAccess: "always",
				            bgcolor: "#ffffff",
				            wmode: "transparent" // opaque
				        }, false
					);
				else {
					var swf = document.getElementById("avgProjectActivityPieChart");
					if(swf && swf.load)
						swf.load(get_data_avgProjectActivityPie());
				}	
            },
			failure: onFailure,
            scope: this,
            params: {
            	idProject: idProject,
            	endDateTime: (new Date()).format("U"),
            	timeSpan: timeSpan 
            }
		});
	}

	function loadAvgProjectActivityBarChartOff(embed, timeSpan) {
		var url = '<s:url action="AvgProjectActivityBarChartOff" namespace="/json" encode="false" />';
		Ext.Ajax.request({
            url: url,
            success: function (response, opts) {           
				dataAvgProjectActivityBarOff = response.responseText;
				if(embed)	
					swfobject.embedSWF("../flash/ofc.swf", "avgProjectActivityBarChartOff", "100%", "180", "9.0.0", "../flash/expressInstall.swf",  
						{ "get-data": "get_data_avgProjectActivityBarOff", "loading": '<s:text name="chart.loadingMessage" />' }, { 
							allowScriptAccess: "always",
				            bgcolor: "#ffffff",
				            wmode: "transparent" // opaque
				        }, false
					);
				else {
					var swf = document.getElementById("avgProjectActivityBarChartOff");
					if(swf && swf.load)
						swf.load(get_data_avgProjectActivityBarOff()); 
				}				
            },
			failure: onFailure,
            scope: this,
            params: {
            	idProject: idProject,
            	endDateTime: (new Date()).format("U"),
            	timeSpan: timeSpan            	
            }
		});
	}
	
	var serviceDistributionWindow = null;
	function loadServicesDistribution() {
		var urlPie = '<s:url action="ServicesOnHostsByProjectPieChart" namespace="/json" encode="false" />';
		Ext.Ajax.request({
            url: urlPie,
            success: function (response, opts) {            
				dataServicesDistributionPie = response.responseText;
				swfobject.embedSWF("../flash/ofc.swf", "servicesDistributionPieChart", "100%", "170", "9.0.0", "../flash/expressInstall.swf",  
					{ "get-data": "get_data_servicesDistributionPie", "loading": '<s:text name="chart.loadingMessage" />' }, { 
						allowScriptAccess: "always",
			            bgcolor: "#ffffff",
			            wmode: "transparent" // opaque
			        }, false
				);								
            },
			failure: onFailure,
            scope: this,
            params: {
            	idProject: idProject            	
            }
		});	
	}

	var usersDistributionWindow = null;
	function loadUsersDistribution() {
		var urlPie = '<s:url action="UsersOnHostsByProjectPieChart" namespace="/json" encode="false" />';
		Ext.Ajax.request({
            url: urlPie,
            success: function (response, opts) {            
				dataUsersDistributionPie = response.responseText;
				swfobject.embedSWF("../flash/ofc.swf", "usersDistributionPieChart", "100%", "170", "9.0.0", "../flash/expressInstall.swf",  
					{ "get-data": "get_data_usersDistributionPie", "loading": '<s:text name="chart.loadingMessage" />' }, { 
						allowScriptAccess: "always",
			            bgcolor: "#ffffff",
			            wmode: "transparent" // opaque
			        }, false
				);								
            },
			failure: onFailure,
            scope: this,
            params: {
            	idProject: idProject            	
            }
		});	
	}
	
	
	function reloadAvgProjectActivityCharts(timeSpan) {
		loadAvgProjectActivityPieChart(false, timeSpan);
		loadAvgProjectActivityBarChartOff(false, timeSpan);		 
	}
//]]>
</script>
