<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<%
	String parameter = request.getParameter("mini"); 
	String iframe = request.getParameter("iframe");
	boolean mini = parameter!=null && parameter.equals("1");
	parameter = request.getParameter("showProject");
	boolean tabProjectOn = parameter==null || !parameter.equals("0");
%>

<html>
	<head>
		<s:include value="commons/layout.jsp" />
		<link rel="stylesheet" type="text/css" href="../css/Spinner.css" />
		<script type="text/javascript" src="../js/ext/ux/Spinner.js"></script>
		<script type="text/javascript" src="../js/ext/ux/SpinnerField.js"></script>
		<script type="text/javascript">
		//<![CDATA[
			var nullValue = '{"bg_colour":"#DFE8F6", "border":1, "elements":[{"type":"pie","values":[]}],"title":{"style":"{font-size: 12px; color:#000000; font-family: Arial; text-align: center;}","text":"No Data to Show"}}';

			var barTypes = [ "bar", "bar_glass", "bar_filled", "bar_3d", "bar_sketch", "bar_cylinder", 
			     			 "bar_cylinder_outline", "bar_round_glass", "bar_round", "bar_dome" ];
			var tabProjectOn = <%=tabProjectOn? 1: 0 %>; 
			var idServer    = '<s:property value="idServer" />';
			var idProject   = '<s:property value="idProject" />';
			var numTabs	    = '<s:property value="server.services.size" />'*1;
			var idServices  = new Array(numTabs);
			var servicesType= new Array(numTabs);
			var services    = new Array(numTabs);
			var tipoGrafici = new Array(numTabs*4);
			var endDateTime = new Date();
			var startDateTime = endDateTime.add(Date.MINUTE, -7*24*60);
			var rttDiagramColor = new Array(numTabs);
			var pieDiagramColor = new Array(numTabs);
			var pieOffDiagramColor = new Array(numTabs);
			var activityDiagramSettings = new Array(numTabs);
			
			var dataAvgHostActivityPie = 'null';
			function get_data_avgHostActivityPie() {
				if(dataAvgHostActivityPie == 'null') return nullValue;
				return dataAvgHostActivityPie;
			}
			var dataPie = new Array(numTabs);
			function get_data_pie(id) {	
				if(dataPie[id] == 'null') return nullValue;// 'null';
				var dati = JSON.parse(dataPie[id]);
				if(!Ext.getCmp("checkboxPieChartAnimation"+id).pressed)
					dati.elements[0].animate = null; // annulla l'animazione
				if(tipoGrafici[4*id] == 1) {
					var menu = Ext.getCmp("pieBarType" + id).menu.items;
					for(var i = 0; i < menu.length; i ++)
						if(menu.get(i).checked) {
							dati.elements[0].type = barTypes[menu.get(i).value];
							break;
						}
								
					dati.elements[0].colour = pieDiagramColor[id].available;
					dati.elements[1].colour = pieDiagramColor[id].failure;
				}
				for(var i = 0; i < dati.elements[0].values.length; i ++)
					if(dati.elements[0].values[i] != null) {
						if(dati.elements[0].values[i].colour == '<s:text name="pieChart.slice.available.colour" />')
							dati.elements[0].values[i].colour = pieDiagramColor[id].available;
						else dati.elements[0].values[i].colour = pieDiagramColor[id].failure;
					}
				return JSON.stringify(dati);
			}
			var dataPieOff = new Array(numTabs);
			function get_data_pie_off(id) {
				if(dataPieOff[id] == 'null') return nullValue;// return 'null';
				var dati = JSON.parse(dataPieOff[id]);
				if(!Ext.getCmp("checkboxPieChartOffAnimation"+id).pressed)
					dati.elements[0].animate = null; // annulla l'animazione
				if(tipoGrafici[4*id+1] == 1) {
					var menu = Ext.getCmp("pieOffBarType" + id).menu.items;
					for(var i = 0; i < menu.length; i ++)
						if(menu.get(i).checked) {
							dati.elements[0].type = barTypes[menu.get(i).value];
							break;
						}
					dati.elements[0].colour = pieOffDiagramColor[id].down;
					dati.elements[1].colour = pieOffDiagramColor[id].unreachable;
					dati.elements[2].colour = pieOffDiagramColor[id].error;
				}
				for(var i = 0; i < dati.elements[0].values.length; i ++)
					if(dati.elements[0].values[i] != null) {
						switch(dati.elements[0].values[i].colour) {
							case '<s:text name="activityChart.element.inactive.colour" />':
								dati.elements[0].values[i].colour = pieOffDiagramColor[id].down; break;
							case '<s:text name="activityChart.element.timeout.colour" />':
								dati.elements[0].values[i].colour = pieOffDiagramColor[id].unreachable; break;
							default:
								dati.elements[0].values[i].colour = pieOffDiagramColor[id].error;
						}
					}
				return JSON.stringify(dati);
			}
			var dataActivity = new Array(numTabs);			
			function get_data_activity(id) {
				if(dataActivity[id] == 'null') return nullValue;// return 'null';
				var dati = JSON.parse(dataActivity[id]);
				
				if(tipoGrafici[4*id+2] == 0) {
					var menu = Ext.getCmp("activityBarType" + id).menu.items;
					for(var i = 0; i < menu.length; i ++)
						if(menu.get(i).checked) {
							dati.elements[0].type = dati.elements[1].type = barTypes[menu.get(i).value];
							if(!activityDiagramSettings[id].onOff)
								dati.elements[2].type = dati.elements[3].type = barTypes[menu.get(i).value];
							break;
						}
				}
				if(tipoGrafici[4*id+2] != 1) {
					dati.elements[0].colour = activityDiagramSettings[id].activeColor;
					if(tipoGrafici[4*id+2] == 3)
						dati.elements[0].fill = activityDiagramSettings[id].activeColor;
					if(activityDiagramSettings[id].onOff) {					
						dati.elements[1].colour = activityDiagramSettings[id].failureColor;
						if(tipoGrafici[4*id+2] == 3)
							dati.elements[1].fill = activityDiagramSettings[id].failureColor;
					}
					else {
						dati.elements[1].colour = activityDiagramSettings[id].downColor;
						dati.elements[2].colour = activityDiagramSettings[id].unreachableColor;
						dati.elements[3].colour = activityDiagramSettings[id].errorColor;
						if(tipoGrafici[4*id+2] == 3) {
							dati.elements[1].fill = activityDiagramSettings[id].downColor;
							dati.elements[2].fill = activityDiagramSettings[id].unreachableColor;
							dati.elements[3].fill = activityDiagramSettings[id].errorColor;
						}
					}
				} else {
					dati.elements[0].keys[0].colour = dati.elements[0].colours[0] = activityDiagramSettings[id].activeColor;
					if(activityDiagramSettings[id].onOff) 
						dati.elements[0].keys[1].colour = dati.elements[0].colours[1] = activityDiagramSettings[id].failureColor;					
					else {
						dati.elements[0].keys[1].colour = dati.elements[0].colours[1] = activityDiagramSettings[id].downColor;
						dati.elements[0].keys[2].colour = dati.elements[0].colours[2] = activityDiagramSettings[id].unreachableColor;
						dati.elements[0].keys[3].colour = dati.elements[0].colours[3] = activityDiagramSettings[id].errorColor;
					}
				}
				var idCheckbox = activityDiagramSettings[id].onOff? [ 'checkboxFailure' ]: 
					[ 'checkboxInactivity', 'checkboxTimeout', 'checkboxError' ];				
				var animate = Ext.getCmp("checkboxActivityChartAnimation"+id).pressed;
				if(tipoGrafici[4*id+2] != 1) {
					if(!Ext.getCmp("checkboxActivity"+id).checked)
						dati.elements[0] = {};					
					if(!animate)
						dati.elements[0]['on-show'] = null;
					for(var i = 0; i < idCheckbox.length; i ++) {
						if(!Ext.getCmp(idCheckbox[i] + id).checked)
							dati.elements[i+1] = {};
						if(!animate)
							dati.elements[i+1]['on-show'] = null;
					}
				}
				else if(!animate)
					dati.elements[0]['on-show'] = null;
				dati['x_axis']['grid-colour'] = dati['y_axis']['grid-colour'] = 
					Ext.getCmp('checkboxActivityChartGrid' + id).pressed? '<s:text name="chart.defaultGridColor" />' : '<s:text name="chart.defaultBgColor" />';
				// #FBE8C4
				return JSON.stringify(dati);
			}
			var dataRTT = new Array(numTabs);
			function get_data_rtt(id) {
				if(dataRTT[id] == 'null') return nullValue;//return 'null';
				var dati = JSON.parse(dataRTT[id]);
				if(tipoGrafici[4*id+3] == 0) {
					var menu = Ext.getCmp("rttBarType" + id).menu.items;
					for(var i = 0; i < menu.length; i ++)
						if(menu.get(i).checked) {
							dati.elements[0].type = barTypes[menu.get(i).value];							
							break;
						}
				}
				if(!Ext.getCmp("checkboxRttChartAnimation"+id).pressed)
					dati.elements[0]['on-show'] = null; // annulla l'animazione
				dati['x_axis']['grid-colour'] = dati['y_axis']['grid-colour'] = 
					Ext.getCmp('checkboxRttChartGrid' + id).pressed? '<s:text name="chart.defaultGridColor" />' : '<s:text name="chart.defaultBgColor" />';
				dati.elements[0].colour = rttDiagramColor[id];
				if(tipoGrafici[4*id+3] == 2)
					dati.elements[0].fill = rttDiagramColor[id];
				return JSON.stringify(dati);
			}
			
			function toggleRttChart(This, checked) {
				var index = This.panelIndex;
				var swf = document.getElementById("graficoRTT" + index);
				if(swf.load)
					swf.load(get_data_rtt(index));
			}
			function onPieItemToggleUnique(index, tipoGrafico) {
				tipoGrafici[index*4] = tipoGrafico;
				Ext.getCmp("pieBarType" + index).setVisible(tipoGrafico == 1);
				if(checkForm(index)) {
					alert('<s:text name="server.form.messages.invalidForm" />');
					return;
				}
				loadPieChart(index);
			}
			function onPieOffItemToggleUnique(index, tipoGrafico) {
				tipoGrafici[index*4+1] = tipoGrafico;
				Ext.getCmp("pieOffBarType" + index).setVisible(tipoGrafico == 1);
				if(checkForm(index)) {
					alert('<s:text name="server.form.messages.invalidForm" />');
					return;
				}
				loadPieChartOff(index);
			}
			function onActItemToggleUnique(index, tipoGrafico) {
				Ext.getCmp("activityBarType" + index).setVisible(tipoGrafico == 0);
				var embed = (tipoGrafici[index*4+2] == -1 && tipoGrafici[index*4+2] != tipoGrafico);
				if(tipoGrafici[index*4+2] == -1) {
					Ext.getCmp("gridActivity"+index).destroy();
					if(widgets[index].indexOf(getActivityWidget) != -1)
						Ext.getCmp("dash_gridActivity"+index).destroy();
					dataActivity[index] = 'null';
				}
				tipoGrafici[index*4+2] = tipoGrafico;
				if(checkForm(index)) {
					alert('<s:text name="server.form.messages.invalidForm" />');
					return;
				}
				var idCheckbox = [ 'menuActivity', 'menuFailures', "menuZoomOut",
				   				   'checkboxActivityChartGrid', 'checkboxActivityChartAnimation' ];
				var i;
				for(i = 0; i < idCheckbox.length; i ++)
					Ext.getCmp(idCheckbox[i]+index).setVisible(tipoGrafico != -1);
				
				if(tipoGrafico == -1)
					loadActivityGrid(index);
				else
					loadActivityChart(index, embed);
			}

			function onRttItemToggleUnique(index, tipoGrafico) {
				Ext.getCmp("rttBarType" + index).setVisible(tipoGrafico == 0);
				var embed = (tipoGrafici[index*4+3] == -1 && tipoGrafici[index*4+3] != tipoGrafico);
				if(tipoGrafici[index*4+3] == -1) {
					Ext.getCmp("gridRTT"+index).destroy();
					if(widgets[index].indexOf(getRttWidget) != -1)
						Ext.getCmp("dash_gridRTT"+index).destroy();
					dataRTT[index] = 'null';
				}
				tipoGrafici[index*4+3] = tipoGrafico;
				if(checkForm(index)) {
					alert('<s:text name="server.form.messages.invalidForm" />');
					return;
				}
				var idCheckbox = [ 'checkboxRttChartGrid', 'checkboxRttChartAnimation', 'colorMenuRTT' ];
				for(var i = 0; i < idCheckbox.length; i ++)
					Ext.getCmp(idCheckbox[i]+index).setVisible(tipoGrafico != -1);
				if(tipoGrafico == -1)
					loadRttGrid(index);
				else
					loadRTTChart(index, embed);
			}
		//]]>
		</script>
		<% if(tabProjectOn) { %>
			<s:action name="ProjectMini" var="project">
				<s:param name="idProject" value="idProject" />
			</s:action>
			<s:push value="#project">
				<s:include value="parts/projectHeaderPart.jsp" />
			</s:push>
		<% } %>
		<s:include value="parts/infoWindow.jsp" />
		<s:include value="parts/layoutServiceTab.jsp" />
		<s:include value="parts/layoutServiceGrid.jsp" />
		<script type="text/javascript" src='http://maps.google.com/maps/api/js?sensor=false&language=<s:property value="locale.language" />'></script>
		<script type="text/javascript" src ="../js/content/server.js"></script>
		<script type="text/javascript">
		//<![CDATA[
			// <s:iterator value="server.services" status="counter">
				dataPie[<s:property value="#counter.count-1" />] = dataPieOff[<s:property value="#counter.count-1" />] = 
				dataActivity[<s:property value="#counter.count-1" />] =	dataRTT[<s:property value="#counter.count-1" />] = 'null';
				rttDiagramColor[<s:property value="#counter.count-1" />] = '<s:text name="rttChart.element.colour" />';
				pieDiagramColor[<s:property value="#counter.count-1" />] = { available: '<s:text name="pieChart.slice.available.colour" />', 
					failure: '<s:text name="pieChart.slice.failure.colour" />' };
				pieOffDiagramColor[<s:property value="#counter.count-1" />] = { down: '<s:text name="activityChart.element.inactive.colour" />', 
					unreachable: '<s:text name="activityChart.element.timeout.colour" />', error: '<s:text name="activityChart.element.error.colour" />' };
				activityDiagramSettings[<s:property value="#counter.count-1" />] = {
					activeColor: '<s:text name="activityChart.element.active.colour" />',
					failureColor: '<s:text name="activityChart.element.failure.colour" />',
					downColor: '<s:text name="activityChart.element.inactive.colour" />',
					unreachableColor: '<s:text name="activityChart.element.timeout.colour" />',
					errorColor: '<s:text name="activityChart.element.error.colour" />',
					onOff: true,
					activityInterval: new Array()
				};
				
				function onPieItemToggle<s:property value="#counter.count-1" />(item, pressed) {
					if(! pressed) return;
					onPieItemToggleUnique(<s:property value="#counter.count-1" />, item.tipoGrafico);
				}
				function onPieOffItemToggle<s:property value="#counter.count-1" />(item, pressed) {
					if(! pressed) return;
					onPieOffItemToggleUnique(<s:property value="#counter.count-1" />, item.tipoGrafico);
					
				}
				function onActItemToggle<s:property value="#counter.count-1" />(item, pressed) {
					if(! pressed) return;
					onActItemToggleUnique(<s:property value="#counter.count-1" />, item.tipoGrafico);
				}
				function onRttItemToggle<s:property value="#counter.count-1" />(item, pressed) {
					if(! pressed) return;
					onRttItemToggleUnique(<s:property value="#counter.count-1" />, item.tipoGrafico);					
				}
				function onActivityCheck<s:property value="#counter.count-1" />(This, checked) {
					var index = <s:property value="#counter.count-1" />;
					var swf = document.getElementById("graficoAttivita" + index);
					if(swf.load)
						swf.load(get_data_activity(index));
				}
				function load<s:property value="#counter.count-1" />() {
					var index = <s:property value="#counter.count-1" />;
					fillContainerCharts("", index);
					idServices[index] = <s:property value="id" />;
					// <s:if test="type==null">
						servicesType[index] = null;
					// </s:if><s:else>
						servicesType[index] = '<s:property value="type.name()" />';
					// </s:else>
					services[index] = addServiceTab(index, '<s:if test="name==null"><s:property value="port" /></s:if><s:else><s:property value="name" /></s:else>');

					Ext.getCmp('dash_tablePanel'+index).on("resize", function () {
						if(Ext.getCmp("tab-panel").getActiveTab()==Ext.getCmp("tab-panel").get(index+1+tabProjectOn) && 
								Ext.getCmp("tabGrafici"+index).getActiveTab()==Ext.getCmp("tabGrafici"+index).get(0)) {
							var table = Ext.getCmp('dash_tablePanel'+index); 
							table.removeAll();
							table.add(createDashboardElements(index));
							table.doLayout(false, true);							
						}
					});	
				}
			// </s:iterator>
		//]]>
		</script>			
		<script type="text/javascript">
		//<![CDATA[
			function redirectToService(id) {
				var tabs = Ext.getCmp("tab-panel");
				for(var i = 0; i < idServices.length; i ++)
					if(id == idServices[i]) {
						tabs.setActiveTab(i+1+tabProjectOn);
						break;
					}
			}
			
			function changeName(val, metaData, record, rowIndex, colIndex, store) {
				return '<a href="#" onclick="redirectToService(' + record.get('id') + '); return false;">' + val + '</a>';
			}
		    
			function loadServicesGrid() {
				function buildStore(url) {
					return new Ext.data.JsonStore({
				        root: 'services',
				        totalProperty: 'totalCount',
				        remoteSort: true,
				        sortInfo: { field: 'percentages[0]', direction: 'DESC' },
				        fields: [
				            {name: 'id', type: 'int'}, 'name', {name: 'port', type: 'int'}, 
				            'percentages[0]', 'percentages[1]', 'percentages[2]', 'percentages[3]', 'percentages[4]'
				        ],
				        proxy: new Ext.data.HttpProxy({
				            url: url 
				        })
				    });	
				}
				var endDate = new Date();
				var url = '<s:url action="MostActiveServices" namespace="/json" encode="false" />';
				var params = '?idProject=' + idProject + '&idServer=' + idServer + '&numInstances=' + numTabs + "&endDate=" + endDate.format("U");
				var store = buildStore(url + params);
				var grid = new Ext.grid.GridPanel({
					height: 220,
					store: store,
			        bodyCssClass: "grid",
			        trackMouseOver: true,
			        disableSelection: false,
			        loadMask: true,
			        columns: [
				        {
				            header: '<s:text name="server.servicesGrid.columns.name" />',
				            dataIndex: "name",
				            sortable: true,
				            renderer: changeName				            
				        }, {
				            header: '<s:text name="server.servicesGrid.columns.port" />',
				            dataIndex: "port",
				            sortable: true
				        }, {
				            header: '<s:text name="server.servicesGrid.columns.percentage0" />',
				            dataIndex: "percentages[0]",
				            sortable: true,
				            renderer: changePercentage
				        }, {
				            header: '<s:text name="server.servicesGrid.columns.percentage1" />',
				            dataIndex: "percentages[1]",
				            sortable: true,
				            renderer: changePercentage
				        }, {
				            header: '<s:text name="server.servicesGrid.columns.percentage2" />',
				            dataIndex: "percentages[2]",
				            sortable: true,
				            renderer: changePercentage
				        }, {
				            header: '<s:text name="server.servicesGrid.columns.percentage3" />',
				            dataIndex: "percentages[3]",
				            sortable: true,
				            renderer: changePercentage
				        }, {
				            header: '<s:text name="server.servicesGrid.columns.percentage4" />',
				            dataIndex: "percentages[4]",
				            sortable: true,
				            renderer: changePercentage
				        }
				    ],
			        bbar: new Ext.PagingToolbar({
			            pageSize: <s:text name="server.servicesGrid.limit" />,
			            store: store,
			            displayInfo: true,
			            displayMsg: '<s:text name="server.servicesGrid.displayMsg" />',
			            emptyMsg: '<s:text name="server.servicesGrid.emptyMsg" />',
			            items: [
							'-', {
								text: '<s:text name="grid.button.update" />',
								handler: function(button, eventObject) {
									endDate = new Date();
									var params = '?idProject=' + idProject + '&idServer=' + idServer + '&numInstances=' + numTabs + "&endDate=" + endDate.format("U");
									store = buildStore(url + params);
									store.load({params:{start:0, limit:<s:text name="server.servicesGrid.limit" />}});
									grid.reconfigure(store, grid.getColumnModel());
									grid.getBottomToolbar().bindStore(store);
									panel.setTitle(String.format('<s:text name="server.panel.servicesGrid.title" />', 
											Ext.util.Format.date(endDate), endDate.format("H:i")));
								}
							}, {
			                	text: '<s:text name="grid.button.exportXML" />',
			                	handler: function(button, eventObject) {
				                	var url = '<s:url action="MostActiveServicesXML" namespace="/xsl" encode="false" />';
				                	window.open(url + params);
			                	}		                	                                
			                }, {
			                	text: '<s:text name="grid.button.exportCSV" />',
			                	handler: function(button, eventObject) {
			                		var url = '<s:url action="MostActiveServicesCSV" namespace="/stream" encode="false" />';
				                	window.open(url + params);
			                	}		                	 
			                }
			            ]
			        }),
			        stripeRows: true,
			        renderTo: 'gridMostActiveServices',
			        viewConfig: {
			            forceFit: true
					}
				});
	    		store.load({params:{start:0, limit:<s:text name="server.servicesGrid.limit" />}});
	    		var panel = Ext.getCmp("serverGridPanel");
	    		panel.setTitle(String.format('<s:text name="server.panel.servicesGrid.title" />', 
	    				Ext.util.Format.date(endDate), endDate.format("H:i"))); 
	    		grid.syncSize();
			}

			function loadAvgHostActivityPieChart(timeSpan, embedSwf, serverName, embedMap) {
				var urlPie = '<s:url action="AvgHostActivityPieChart" namespace="/json" encode="false" />';
				Ext.Ajax.request({
		            url: urlPie,
		            success: function (response, opts) {
		            	var dati = JSON.parse(response.responseText);
						dataAvgHostActivityPie = JSON.stringify(dati.pieChart);	
						if(embedSwf) {
							swfobject.embedSWF("../flash/ofc.swf", "avgHostActivityPieChart", "100%", "170", "9.0.0", "../flash/expressInstall.swf",  
								{ "get-data": "get_data_avgHostActivityPie", "loading": '<s:text name="chart.loadingMessage" />' }, { 
									allowScriptAccess: "always",
						            bgcolor: "#ffffff",
						            wmode: "transparent" // opaque
						        }, false
							);							
						}
						else { 
							var swf = document.getElementById("avgHostActivityPieChart");
							if(swf && swf.load)
								swf.load(get_data_avgHostActivityPie());
						}
						if(embedMap)
							createHostMap(<s:property value="server.latitude" default="null" />, <s:property value="server.longitude" default="null" />, serverName, dati.activity);
		            },
					failure: onFailure,
		            scope: this,
		            params: {
		            	idProject: idProject,
		            	idServer: idServer,
		            	endDateTime: endDateTime.format("U"),
		            	timeSpan: timeSpan
		            }
				});				
			}
			
			function loadPieChart(x) {
				var urlPie = [ '<s:url action="TotalActivityPieChart" namespace="/json" encode="false" />', 
							   '<s:url action="TotalActivityBarChart" namespace="/json" encode="false" />' ];
				Ext.Ajax.request({
		            url: urlPie[tipoGrafici[4*x]],
		            success: function (response, opts) {
			            dataPie[x] = response.responseText;
			            var activeTab = services[x].items[1].getActiveTab();
			            if(activeTab == null) ;
			            else if(activeTab.panelEl == "pie") {
							var swf = document.getElementById("graficoTorta" + x);
							if(swf.load)
								swf.load(get_data_pie(x));
						}
			            else if(activeTab.panelEl == "dashboard" && widgets[x].indexOf(getPieWidget) != -1) {
			            	var swf = document.getElementById("dash_graficoTorta" + x);
			            	if(swf.load)
								swf.load(get_data_pie(x));			            	
			            }
		            },
		            failure: onFailure,
		            scope: this,
		            params: getParams(x)
		        });
			}

			function loadPieChartOff(x) {
				var urlPie = [ '<s:url action="TotalActivityPieChartOff" namespace="/json" encode="false" />',
							   '<s:url action="TotalActivityBarChartOff" namespace="/json" encode="false" />' ];
				Ext.Ajax.request({
		            url: urlPie[tipoGrafici[4*x+1]],
		            success: function (response, opts) {
			            dataPieOff[x] = response.responseText;
			            var activeTab = services[x].items[1].getActiveTab();
			            if(activeTab == null) ;
			            else if(activeTab.panelEl == "pieOff") {
							var swf = document.getElementById("grafico1Torta" + x);
							if(swf.load)
								swf.load(get_data_pie_off(x));
						}
			            else if(activeTab.panelEl == "dashboard" && widgets[x].indexOf(getPieOffWidget) != -1) {
			            	var swf = document.getElementById("dash_grafico1Torta" + x);
			            	if(swf.load)
								swf.load(get_data_pie_off(x));
			            }
		            },
		            failure: onFailure,
		            scope: this,
		            params: getParams(x)
		        });
			}
			
			function loadActivityChart(x, embed, parameters) {
				var urlActivity = [ '<s:url action="ActivityBarChart" namespace="/json" encode="false" />' , 
				   					'<s:url action="ActivityStackChart" namespace="/json" encode="false" />' , 
				    				'<s:url action="ActivityLineChart" namespace="/json" encode="false" />',
				    				'<s:url action="ActivityAreaChart" namespace="/json" encode="false" />'
				    			  ];
  			  	var params = parameters? parameters: getParams(x);
  			  	Ext.Ajax.request({
		            url: urlActivity[tipoGrafici[4*x+2]], 
		            success: function (response, opts) {
	            		dataActivity[x] = response.responseText;	            		
	            		var activeTab = services[x].items[1].getActiveTab();
	            		if(embed) {
	            			swfobject.embedSWF("../flash/ofc.swf", "graficoAttivita"+x, "100%", "350", "9.0.0", "../flash/expressInstall.swf",
    							{ "get-data": "get_data_activity", "id": x, "loading": '<s:text name="chart.loadingMessage" />' }, { 
    								allowScriptAccess: "always",
    					            bgcolor: "#ffffff",
    					            wmode: "transparent" // opaque
    					        }, false
    						);
    						if(widgets[x].indexOf(getActivityWidget) != -1)	
        						swfobject.embedSWF("../flash/ofc.swf", "dash_graficoAttivita"+x, "100%", "250", "9.0.0", "../flash/expressInstall.swf",
	    							{ "get-data": "get_data_activity", "id": x, "loading": '<s:text name="chart.loadingMessage" />' }, { 
	    								allowScriptAccess: "always",
	    					            bgcolor: "#ffffff",
	    					            wmode: "transparent" // opaque
	    					        }, false
    						);
    						return;
    					}
	            		if(activeTab == null) ;
	            		else if(activeTab.panelEl=="activity") {
		            		var swf = document.getElementById("graficoAttivita" + x);
		            		if(swf.load)
								swf.load(get_data_activity(x));
			            }
			            else if(activeTab.panelEl == "dashboard" && widgets[x].indexOf(getActivityWidget) != -1) {
			            	var swf = document.getElementById("dash_graficoAttivita" + x);
			            	if(swf.load)
								swf.load(get_data_activity(x));
			            }
		            },
		            failure: onFailure,
		            scope: this,
		            params: params
		        });
			}

			function loadRTTChart(x, embed) {
				var urlRTT = [ '<s:url action="RTTBarChart" namespace="/json" encode="false" />' , 
							   '<s:url action="RTTLineChart" namespace="/json" encode="false" />', 
							   '<s:url action="RTTAreaChart" namespace="/json" encode="false" />'
							 ];
				Ext.Ajax.request({
		            url: urlRTT[tipoGrafici[4*x+3]], 
		            success: function (response, opts) {
		            	dataRTT[x] = response.responseText;		            	
		            	var activeTab = services[x].items[1].getActiveTab();
		            	if(embed) {
		            		swfobject.embedSWF("../flash/ofc.swf", "graficoRTT"+x, "100%", "350", "9.0.0", "../flash/expressInstall.swf",
								{ "get-data": "get_data_rtt", "id": x, "loading": '<s:text name="chart.loadingMessage" />' }, { 
									allowScriptAccess: "always",
						            bgcolor: "#ffffff",
						            wmode: "transparent" // opaque
						        }, false
							);
		            		if(widgets[x].indexOf(getRttWidget) != -1)
			            		swfobject.embedSWF("../flash/ofc.swf", "dash_graficoRTT"+x, "100%", "250", "9.0.0", "../flash/expressInstall.swf",
									{ "get-data": "get_data_rtt", "id": x, "loading": '<s:text name="chart.loadingMessage" />' }, { 
										allowScriptAccess: "always",
							            bgcolor: "#ffffff",
							            wmode: "transparent" // opaque
							        }, false
								);
			            	return;
		            	}
		            	if(activeTab == null) ;
		            	else if(activeTab.panelEl=="rtt") {
			            	var swf = document.getElementById("graficoRTT" + x);
			            	if(swf.load)
								swf.load(get_data_rtt(x));
		            	}
			            else if(activeTab.panelEl == "dashboard" && widgets[x].indexOf(getRttWidget) != -1) {
			            	var swf = document.getElementById("dash_graficoRTT" + x);
			            	if(swf.load)
								swf.load(get_data_rtt(x));
			            }
		            },
		            failure: onFailure,
		            scope: this,
		            params: getParams(x)
		        });
			}
			
			function reloadSWF(x) {
				if(checkForm(x)) {
					alert('<s:text name="server.form.messages.invalidForm" />');
					return;
				}
				loadPieChart(x);
				loadPieChartOff(x);
				if(tipoGrafici[4*x+2] == -1)
					reloadActivityGrid(x);
				else {
					Ext.getCmp("menuZoomOut" + x).setDisabled(true);
					var params = getParams(x);
					while(activityDiagramSettings[x].activityInterval.length != 0)
						activityDiagramSettings[x].activityInterval.pop();
					activityDiagramSettings[x].activityInterval.push(params);
					loadActivityChart(x, false, params);
				}
				if(tipoGrafici[4*x+3] == -1)
					reloadRttGrid(x);
				else
					loadRTTChart(x, false);
				loadSummary(x);
			}

			var openedServerWindow = null;
			function loadHostActivityChart(title) {
				var limit = 6;
				var prefix = "server_";
				var numServices = <s:property value="server.numInstances" />;
				buildWindowContent(prefix);
				if(openedServerWindow != null) openedServerWindow.close();
				openedServerWindow = openWindow(prefix, parseInt((numServices-1) / limit) + 1, idServer, numServices, title);
				openedServerWindow.show();
				reloadHostActivityChart(0, numServices);
			}

			function reloadHostActivityChart(page, numServices) {
				var prefix = "server_";
				var limit = 6;
				var timeSpan = null;
				if(document.getElementById(prefix+'radio_timeSpan')) {
					timeSpan = document.getElementById(prefix+'radio_timeSpan').value;					
				}
				else timeSpan = 5;
				var h = 70 + (page < parseInt(numServices/limit)? limit: (numServices%limit)) * 20;
				document.getElementById(prefix+"divActivityChart").setAttribute("style", "width: 600; height: " + h);
				var endDate = new Date();
				var url = '<s:url action="HostActivityChart" namespace="/json" encode="false" />?idServer=' + idServer + 
				'&numServices=' + numServices + '&start=' + page*limit + "&limit=" + limit + "&timeSpan=" + timeSpan + 
				"&endDate=" + endDate.format("U") + "&idProject=" + idProject;

				swfobject.embedSWF("../flash/ofc.swf", prefix+"graficoAttivita", "590", h, "9.0.0", "../flash/expressInstall.swf", 
					{ "data-file" : urlencode(url), "loading": '<s:text name="chart.loadingMessage" />' }, { 
						allowScriptAccess: "always",
			            bgcolor: "#ffffff",
			            wmode: "transparent" // opaque
		            }, false
				);
				openedServerWindow.setHeight(h+90);
			}

			function toggleForm(enabled, index) {
				Ext.getCmp("dataInizio"+index).setDisabled(!enabled);
				Ext.getCmp("oraInizio"+index).setDisabled(!enabled);
				Ext.getCmp("dataFine"+index).setDisabled(!enabled);
				Ext.getCmp("oraFine"+index).setDisabled(!enabled);
				Ext.getCmp("comboTimeSpans"+index).setDisabled(enabled);
				// Ext.getCmp("submitForm"+index).setDisabled(!enabled);
			}

			function zoomActivity(element, zoomOut) {
				var format = Ext.form.DateField.prototype.format.replace('y', 'Y') + " H:i";
				var interval;
				var index = 0;
				for(; Ext.getCmp("tab-panel").getActiveTab()!=Ext.getCmp("tab-panel").get(index); index ++);
				index -= 1+tabProjectOn;
				if(zoomOut) {
					interval = activityDiagramSettings[index].activityInterval[activityDiagramSettings[index].activityInterval.length-2];
					activityDiagramSettings[index].activityInterval.pop();
					if(activityDiagramSettings[index].activityInterval.length == 1) 
						Ext.getCmp("menuZoomOut" + index).setDisabled(true);
				} else {
					Ext.getCmp("menuZoomOut" + index).setDisabled(false);
					interval = activityDiagramSettings[index].activityInterval[activityDiagramSettings[index].activityInterval.length-1];
					var stepsNumber = interval.spansNumber;
					
					var start = Date.parseDate(interval.startDate + " " + interval.startTime, format).format("U");
					var end = Date.parseDate(interval.endDate + " " + interval.endTime, format).format("U");
					var step = end - start;
					step =  parseInt(step / stepsNumber) + (step%stepsNumber == 0? 0: 1)
					element = (element - 1) / 2;
					var startDate = new Date((start+step*element)*1000); 
					var endDate = new Date((start+step*(element+1))*1000);					
					activityDiagramSettings[index].activityInterval.push(interval = {
						idServiceInstance: interval.idServiceInstance,
						startDate: Ext.util.Format.date(startDate),
						startTime: startDate.format("H:i"),
						endDate: Ext.util.Format.date(endDate),
						endTime: endDate.format("H:i"),
						spansNumber: stepsNumber,
						onOff: activityDiagramSettings[index].onOff
					});
				}				
				loadActivityChart(index, false, interval);
			}

			var serverPermLinkWindow = null;
			function showServerPermLink() {
				if(!serverPermLinkWindow) {
					var url = String.format('monitoring/ServerMini?request_locale={0}&idProject={1}&idServer={2}&showProject=0', '<s:property value="locale.language" />', idProject, idServer);
					serverPermLinkWindow = new Ext.Window({
						title: '<s:text name="server.descriptionTable.items.permLink" />',
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
				            		serverPermLinkWindow.hide();
				                }
				            }
			            ]
			        });
				}
				serverPermLinkWindow.show();
			}

			function loadSummary(index) {
				var url = '<s:url action="Summary" namespace="/json" encode="false" />';
				Ext.Ajax.request({
					url: url,
					success: function(response, opts) {
						var allData = JSON.parse(response.responseText);
						Ext.Ajax.request({
							url: url,
							success: function(response1, opts1) {
								var data = JSON.parse(response1.responseText);
								var prefix = "dash_";
								var dashNode = document.getElementById(prefix+"divDescription"+index);
								
								var nodes = [ document.getElementById("divDescription"+index) ];
								if(widgets[index].indexOf(getDescriptionWidget) != -1 && dashNode!=null)
									nodes[1] = dashNode;
																
								for(var i = 0; i < nodes.length; i ++) {
									var cells = nodes[i].getElementsByTagName("span");
	
									var text = document.createTextNode(data==null || data.availability==null? "-": data.availability + "%");
									cells[0].replaceChild(text, cells[0].firstChild);

									text = document.createTextNode(data==null || data.avgStdDev.avg==null? "-": parseInt(data.avgStdDev.avg/10.+.5)/100. + " msec");
									cells[2].replaceChild(text, cells[2].firstChild);
									
									text = document.createTextNode(data==null || data.avgStdDev.stdDev==null? "-": parseInt(data.avgStdDev.stdDev/10.+.5)/100. + " msec");
									cells[4].replaceChild(text, cells[4].firstChild);

									text = document.createTextNode(allData==null || allData.availability==null? "-": allData.availability + "%");
									cells[1].replaceChild(text, cells[1].firstChild);
									
									text = document.createTextNode(allData==null || allData.avgStdDev.avg==null? "-": parseInt(allData.avgStdDev.avg/10.+.5)/100. + " msec");
									cells[3].replaceChild(text, cells[3].firstChild);
									
									text = document.createTextNode(allData==null || allData.avgStdDev.stdDev==null? "-": parseInt(allData.avgStdDev.stdDev/10.+.5)/100. + " msec");
									cells[5].replaceChild(text, cells[5].firstChild);									
								}
							},
							failure: onFailure,
							params: getParams(index)
						});
					},
					failure: onFailure,
					params: {
						idServiceInstance: idServices[index]
					}
				});
			}
			
			function loadAll() {
				var serverName  = '<s:if test="server.name==null"><s:property value="server.ip" /></s:if><s:else><s:property value="server.name" /></s:else>';
				for(var i = 0; i < numTabs; i ++)
					eval('load'+i+'()');
				buildServiceTab(String.format('<s:text name="server.tab.main.title" />', serverName), 
						'<s:text name="chart.loadingMessage" />', '<s:text name="server.panel.description.title" />', 
						'<s:text name="server.panel.map.title" />', '<s:text name="server.panel.servicesGrid.title" />',
						'<s:text name="help.server.mapPanel" />', '<s:text name="help.server.descriptionPanel" />', '<s:text name="help.server.gridPanel" />');
				loadServicesGrid();
				loadAvgHostActivityPieChart(<s:text name="default.timespan" />, true, serverName, true);
				for(var i = 0; i < numTabs; i ++)
					switch(servicesType[i]) {
						//case "OPeNDAP": buildOpendapLayout(i); break;
					}
				for(var i = 0; i < numTabs; i ++)
					reloadSWF(i);								
			}
			
			Ext.onReady(loadAll);
		//]]>
		</script>
		<s:include value="parts/layoutOpendapService.jsp" />
	</head>
	<body>		
		<s:include value="commons/header.jsp" />
		
		<% if(tabProjectOn) { %>
			<s:push value="#project">
				<s:include value="parts/projectBodyPart.jsp" />
			</s:push>
		<% } %>
		<div id="containerMap" class="x-hide-display">
			<div id="server_map_canvas" style="height: 100%;"></div>
		</div>
		
		<div id="content" style="height: 360;" class="x-hide-display">
			<table border="0" cellspacing="0" align="left">
				<tbody>
					<tr height="5px"></tr>
					<tr>
						<td class="descriptionTableLeft"><s:text name="server.descriptionTable.items.name" /></td>
						<td class="descriptionTableRight"><s:property value="server.name" default="-" /></td>
					</tr>
					<tr>
						<td class="descriptionTableLeft"><s:text name="server.descriptionTable.items.ip" /></td>
						<td class="descriptionTableRight"><s:property value="server.ip" /></td>
					</tr>
					<tr>
						<td class="descriptionTableLeft"><s:text name="server.descriptionTable.items.projectName" /></td>
						<td class="descriptionTableRight">
							<% if(!tabProjectOn && mini) { %>
								<s:property value="server.projectName" />
							<% } else { %>
							<a href="#" onclick='if(tabProjectOn) Ext.getCmp("tab-panel").setActiveTab(0); else document.getElementById("projectForm").submit(); return false;'>
								<s:property value="server.projectName" />
							</a>
							<% } %>
						</td>
					</tr>
					<tr>
						<td class="descriptionTableLeft"><s:text name="server.descriptionTable.items.city" /></td>
						<td class="descriptionTableRight"><s:property value="server.city" default="-" /></td>
					</tr>
					<tr>
						<td class="descriptionTableLeft"><s:text name="server.descriptionTable.items.numServices" /></td>
						<td class="descriptionTableRight">
							<s:property value="server.numInstances" />&nbsp;&nbsp;&nbsp;
							<a href="#" onclick='loadHostActivityChart("<s:if test="server.name==null"><s:property value="server.ip" /></s:if><s:else><s:property value="server.name" /></s:else>"); return false;'>
								<s:text name="server.descriptionTable.items.numServices.details" />
							</a>
						</td>
					</tr>
					<tr>
						<td class="descriptionTableLeft"><s:text name="server.descriptionTable.items.permLink" /></td>
						<td class="descriptionTableRight">
							<a href="#" onclick="showServerPermLink(); return false;">code</a>
						</td>						
					</tr>
					<tr>
						<td class="descriptionTableLeft">Time Interval</td>
						<td class="descriptionTableRight">
							<select onchange="loadAvgHostActivityPieChart(this.value, false, null, false);">
								<option value="5">5 minutes</option>
								<option value="60">1 hour</option>
								<option value="1440">1 day</option>
								<option value="10080">1 week</option>
								<option value="43200">1 month</option>
							</select>
						</td>
					</tr>
					<tr style="height: 5px;"></tr>
				</tbody>
			</table>
			<div id="avgHostActivityPieChart"></div>							
		</div>
		
		<div id="pagingGrid" class="x-hide-display">
			<div id="gridMostActiveServices"></div>
		</div>
		
		<div id="containerCharts" class="x-hide-display"></div>
		
		<div id="containerForms" class="x-hide-display">
			<s:iterator value="server.services" status="counter">
				<div id='divDescription<s:property value="#counter.count-1"/>'>
					<table border="0" cellspacing="0" align="center">
						<caption class="descriptionTableCaption">Summary</caption>
						<tbody>
							<tr style="height: 15px;"></tr>
							<tr>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.name" /></td>
								<td class="descriptionTableRight"><s:property value="name" default="-" /></td>
								<td class="descriptionTablePadding">&nbsp;</td>								
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.hostName" /></td>
								<td class="descriptionTableRight">
									<a href="#" onclick='Ext.getCmp("tab-panel").setActiveTab(tabProjectOn); return false;'>
										<s:if test="server.name==null">
											<s:property value="server.ip" />
										</s:if>
										<s:else>
											<s:property value="server.name" />
										</s:else>
									</a>
								</td>
							</tr>
							<tr>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.port" /></td>
								<td class="descriptionTableRight"><s:property value="port" /></td>
								<td class="descriptionTablePadding">&nbsp;</td>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.projectName" /></td>
								<td class="descriptionTableRight">
									<% if(!tabProjectOn && mini) { %>
										<s:property value="server.projectName" />
									<% } else { %>
									<a href="#" onclick='if(tabProjectOn) Ext.getCmp("tab-panel").setActiveTab(0); else document.getElementById("projectForm").submit(); return false;'>
										<s:property value="server.projectName" />
									</a>
									<% } %>
								</td>
							</tr>
							<tr>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.mailAdmin" /></td>
								<td class="descriptionTableRight">
									<s:if test="mailAdmin==null">
										<s:text name="server.serviceDescriptionTable.items.mailAdmin.unknown" />
									</s:if>
									<s:else>
										<a href="mailto: <s:property value="mailAdmin" />">
											<s:property value="mailAdmin" />
										</a>
									</s:else>
								</td>
								<td class="descriptionTablePadding">&nbsp;</td>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.institution" /></td>
								<td class="descriptionTableRight">
									<s:if test="institution==null">-</s:if>
									<s:else><s:property value="institution" /></s:else>
								</td>
							</tr>
							<tr>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.startDate" /></td>
								<td class="descriptionTableRight"><s:property value="startDate" /> <s:property value="startTime" /></td>
								<td class="descriptionTablePadding">&nbsp;</td>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.endDate" /></td>
								<td class="descriptionTableRight"><s:property value="endDate" /> <s:property value="endTime" default="&nbsp;" escape="false" /></td>
							</tr>
							<tr>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.periodAvailability" /></td>
								<td class="descriptionTableRight"><span>&nbsp;</span></td>
								<td class="descriptionTablePadding">&nbsp;</td>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.totalAvailability" /></td>
								<td class="descriptionTableRight"><span>&nbsp;</span></td>
							</tr>
							<tr>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.periodRTT" /></td>
								<td class="descriptionTableRight"><span>&nbsp;</span></td>
								<td class="descriptionTablePadding">&nbsp;</td>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.totalRTT" /></td>
								<td class="descriptionTableRight"><span>&nbsp;</span></td>
							</tr>
							<tr>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.periodDevStdRTT" /></td>
								<td class="descriptionTableRight"><span>&nbsp;</span></td>
								<td class="descriptionTablePadding">&nbsp;</td>
								<td class="descriptionTableLeft"><s:text name="server.serviceDescriptionTable.items.totalDevStdRTT" /></td>
								<td class="descriptionTableRight"><span>&nbsp;</span></td>
							</tr>							
						</tbody>
					</table>
				</div>
			</s:iterator>
		</div>
		
		<div id="server_infoWindow" style="display: none;"></div>
		
		<div style="display: none;">
			<s:form method="post" id="projectForm" action="Project" namespace="/monitoring">
				<s:hidden name="idProject" value="%{idProject}" />
				<input type="hidden" name="mini" value="<%=mini? 1: 0 %>" />"
				<input type="hidden" name="iframe" value="<%=iframe %>" />
			</s:form>
		</div>
	</body>
</html>
