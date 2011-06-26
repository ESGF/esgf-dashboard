<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<script type="text/javascript">
//<![CDATA[
    function createContainerChartsElement(idExternal, idInternal) {
    	var containerCharts = document.getElementById('containerCharts');
    	var extDiv = document.createElement("div");
		var intDiv = document.createElement("div");
		extDiv.setAttribute("id", idExternal);
		intDiv.setAttribute("id", idInternal);
		extDiv.appendChild(intDiv);
		containerCharts.appendChild(extDiv);
    }
    
	function fillContainerCharts(prefix, index) {
		createContainerChartsElement(prefix + "divPieChart" + index, prefix + "graficoTorta" + index);
		createContainerChartsElement(prefix + "divPieChartOff" + index, prefix + "grafico1Torta" + index); 
		createContainerChartsElement(prefix + "divActivityChart" + index, prefix + "graficoAttivita" + index);
		createContainerChartsElement(prefix + "divRTTChart" + index, prefix + "graficoRTT" + index);
		tipoGrafici[index*4] = tipoGrafici[index*4+2] = tipoGrafici[index*4+3] = 0;
		tipoGrafici[index*4+1] = 1;
	}
		
	function onExpand(button, eventObject) {
		services[button.myParams.index].items[1].setActiveTab(button.myParams.panelEl);
	}
	
	function onRestore(button, eventObject) {
		services[button.myParams.index].items[1].setActiveTab(0);
	}
		
	function createDashboardToolbar(index, panelEl) {
		var elementi = ['<s:text name="help.server.dashboard.main.totalAvailabilityChart" />',
		         		'<s:text name="help.server.dashboard.main.failureChart" />',
		        		'<s:text name="help.server.dashboard.main.availabilityChart" />',
		        		'<s:text name="help.server.dashboard.main.rttChart" />',
		        		'<s:text name="help.server.dashboard.main.summary" />',
		        		'<s:text name="help.server.dashboard.main.localRegistry" />',
		        		'<s:text name="help.server.dashboard.main.gridDbVO" />',
		        		'<s:text name="help.server.dashboard.main.publishedDb" />' 
		        	   ];		
		return {
        	items: [
            	'->',
				new Ext.Button({
					iconCls: "helpButton",
					tooltip: 'Help',
					handler: function(button, eventObject) {
    					openHelpWindow(button, elementi[panelEl-1]);
    				}					
	    		}), 
				new Ext.Button({
	    	    	iconCls: "expandDashBoardItemIcon",
	    	    	tooltip: '<s:text name="server.toolbar.button.dashboardGo" />',
	    	    	handler: onExpand,
	    	    	myParams: {
						index: index,
						panelEl: panelEl
	    	    	}
	    		})	    		
             ]
        };
	}

	function getPieWidget(index) {
		var prefix = "dash_";
		var span = Ext.getCmp('spinner_pie'+index).getValue()*1;
		if(document.getElementById(prefix + "divPieChart" + index) == null) {
			createContainerChartsElement(prefix + "divPieChart" + index, prefix + "graficoTorta" + index);
			swfobject.embedSWF("../flash/ofc.swf", prefix+"graficoTorta"+index, "100%", "250", "9.0.0", "../flash/expressInstall.swf",
				{ "get-data": "get_data_pie", "id": index, "loading": '<s:text name="chart.loadingMessage" />' }, { 
					allowScriptAccess: "always",
		            bgcolor: "#ffffff",
		            wmode: "transparent" // opaque
		        }, false
			);
		}
		return {
			id: prefix+"panel_pie"+index,
			span: span,
			autoScroll: false,
			contentEl: prefix+"divPieChart"+index,
			panelEl: 1
		};
	}

	function getPieOffWidget(index) {
		var prefix = "dash_";
		var span = Ext.getCmp('spinner_pieOff'+index).getValue()*1;
		if(document.getElementById(prefix + "divPieChartOff" + index) == null) {
			createContainerChartsElement(prefix + "divPieChartOff" + index, prefix + "grafico1Torta" + index);
			swfobject.embedSWF("../flash/ofc.swf", prefix+"grafico1Torta"+index, "100%", "250", "9.0.0", "../flash/expressInstall.swf",
				{ "get-data": "get_data_pie_off", "id": index, "loading": '<s:text name="chart.loadingMessage" />' }, { 
					allowScriptAccess: "always",
		            bgcolor: "#ffffff",
		            wmode: "transparent" // opaque
		        }, false
			);
		}
		return {
			id: prefix+"panel_pieOff"+index,
			span: span,
			autoScroll: false,
			contentEl: prefix+"divPieChartOff"+index,
			panelEl: 2
		};
	}

	function getActivityWidget(index) {
		var prefix = "dash_";
		var span = Ext.getCmp('spinner_activity'+index).getValue()*1;

		if(document.getElementById(prefix + "divActivityChart" + index) == null) {
			createContainerChartsElement(prefix + "divActivityChart" + index, prefix + "graficoAttivita" + index);
			if(tipoGrafici[4*index+2] != -1) {			
				swfobject.embedSWF("../flash/ofc.swf", prefix+"graficoAttivita"+index, "100%", "250", "9.0.0", "../flash/expressInstall.swf",
					{ "get-data": "get_data_activity", "id": index, "loading": '<s:text name="chart.loadingMessage" />' }, { 
						allowScriptAccess: "always",
			            bgcolor: "#ffffff",
			            wmode: "transparent" // opaque
			        }, false
				);
			}
			else 
				updateDashActivityGrid(index);
		}
		return {
			id: prefix+"panel_activity"+index,
			span: span,
			autoScroll: false,
			contentEl: prefix+"divActivityChart"+index,
			panelEl: 3
		};
	}

	function getRttWidget(index) {
		var prefix = "dash_";
		var span = Ext.getCmp('spinner_rtt'+index).getValue()*1;

		if(document.getElementById(prefix + "divRTTChart" + index) == null) {
			createContainerChartsElement(prefix + "divRTTChart" + index, prefix + "graficoRTT" + index);
			if(tipoGrafici[4*index+3] != -1)
				swfobject.embedSWF("../flash/ofc.swf", prefix+"graficoRTT"+index, "100%", "250", "9.0.0", "../flash/expressInstall.swf",
					{ "get-data": "get_data_rtt", "id": index, "loading": '<s:text name="chart.loadingMessage" />' }, { 
						allowScriptAccess: "always",
			            bgcolor: "#ffffff",
			            wmode: "transparent" // opaque
			        }, false
				);
			else 
				updateDashRttGrid(index);
		}
		return {
			id: prefix+"panel_rtt"+index,
			span: span,
			autoScroll: false,
			contentEl: prefix+"divRTTChart"+index,
			panelEl: 4
		};
	}

	function getDescriptionWidget(index) {
		var prefix = "dash_";
		var span = Ext.getCmp('spinner_description'+index).getValue()*1;
		if(document.getElementById(prefix + "divDescription" + index) == null) {
			var node = document.getElementById("divDescription"+index).cloneNode(true);
			node.setAttribute("id", prefix + "divDescription" + index);
		}
		return {
			id: prefix+"panel_description"+index,
			span: span,
			autoScroll: false,
			contentEl: node,
			panelEl: 5
		};
	}
	
	function createDashboardElements(index) {				
		var prefix = "dash_";
		var items = new Array();
		var tSpan = 0;		
		for(var i = 0; i < widgets[index].length; i ++) {
			var widget = widgets[index][i](index);
			if(tSpan == 0) {
		        items.push({
	        		id: prefix+"column" + i,
	    			layout: "column",
	    			height: 280,
	    			items : new Array()  			
	    		});
	        }
			if(tSpan + widget.span == 100)
				tSpan = 0;
			else if(tSpan + widget.span > 100) {
				//items[items.length] = new Ext.Panel({columnWidth: .333, html:"&nbsp;"});
				if(tSpan == 0) { alert('<s:text name="server.form.messages.invalidForm" />'); break; }				
				tSpan = 0; i --; continue;
			}
			else { tSpan += widget.span; items[items.length-1].tSpan = tSpan; }
			items[items.length-1].items.push(new Ext.Panel({
	            id: widget.id,
	            height: 280, 
	            autoScroll: widget.autoScroll,
	        	contentEl: widget.contentEl,
	        	columnWidth: widget.span/100,
	        	layout: "fit",
	        	tbar: createDashboardToolbar(index, widget.panelEl)
	        }));	        
		}		
		return items; 
	}

	var widgets = new Array(numTabs);
	function createDashboard(index) {
		var prefix = "dash_";
		widgets[index] = [ getPieWidget, getPieOffWidget, getActivityWidget, getRttWidget, getDescriptionWidget ];
		switch(servicesType[index]) {			
		}
		return new Ext.Panel({
			id: "dash_tablePanel"+index,
			layout: "table",
			layoutConfig: { 
				tableAttrs: {
					style: {
               			width: '100%'	                			
                    }
                }, columns: 1 
            },
            // applied to child components
            defaults: { frame: false },
            items: createDashboardElements(index)
		});		
	}

	function toggleDashboardChild(index, widget, checked) {
		Ext.getCmp("tabGrafici"+index).setActiveTab(0);
		var table = Ext.getCmp("dash_tablePanel" + index);
		if(checked)
			widgets[index].push(widget);
		else 
			widgets[index].remove(widget);
		
		table.removeAll();
		table.add(createDashboardElements(index));
		table.doLayout(false, true);
	}

	function createFormDxField(label1, label2, index, item, widget, defaultSpan) {
		return new Ext.Panel({
			layout: "hbox",
			layoutConfig: {
				pack: "start",
                align: "middle"
            },
            items: [
				new Ext.Panel({  
					width: 80,
					html: "<p>" + label2 + "</p>"	            	
				}),
				new Ext.form.Checkbox({
					id: "checkboxShow" + item + index,
					style: { marginBottom: '3px' }, 
					handler: function(checkbox, checked) {
						toggleDashboardChild(index, widget, checked);
					},
					checked: true
				}),
				new Ext.Panel({ width: 10 }),
				new Ext.Panel({
					width: 80,
					html: "<p>" + label1 + "</p>"
				}),
				new Ext.ux.form.SpinnerField({
					id: "spinner_" + item + index,
					width: 60,
					name: "size",
				    minValue: 0,
					maxValue: 100,
					value: defaultSpan,
					allowDecimals: true,
					decimalPrecision: 1,
					incrementValue: 0.5,
					alternateIncrementValue: 2.1,
					accelerate: true							
				})				
	    	]
	    });
	}
	
	function createForm(i) {
		var dataI = new Ext.form.DateField({
			id: 'dataInizio'+i,
			width: 170,
			allowBlank: false,
			maxValue: new Date(),
			format: Ext.form.DateField.prototype.format.replace('y', 'Y'),
			value: startDateTime==""? new Date() : startDateTime,
			myIndex: i
		});
		
		var timeI = new Ext.form.TimeField({
			id: 'oraInizio'+i,
			width: 80,
			allowBlank: false,
			format: 'H:i',
			value: startDateTime==""? new Date() : startDateTime
		});
		
		var dataF = new Ext.form.DateField({
			id: 'dataFine'+i,
			width: 170,
			allowBlank: false,
			maxValue: new Date(),
			format: Ext.form.DateField.prototype.format.replace('y', 'Y'),
			value: endDateTime==""? new Date() : endDateTime,
			myIndex: i
		});
		
		var timeF = new Ext.form.TimeField({
			id: 'oraFine'+i,
			width: 80,
			allowBlank: false,
			format: 'H:i',
			value: endDateTime==""? new Date() : endDateTime 
		});
		
		dataI.on('change', function (This, newValue, oldValue) {
			if(This.isValid()) {
				var dataFine = Ext.getCmp('dataFine'+This.myIndex);
				dataFine.setMinValue(newValue);
				dataFine.validate();
			}
		});
		
		dataF.on('change', function (This, newValue, oldValue) {
			if(This.isValid()) {
				var dataInizio = Ext.getCmp('dataInizio'+This.myIndex);
				dataInizio.setMaxValue(newValue);
				dataInizio.validate();
			}
		});

		var formSx = new Ext.Panel({
			id: 'formSx' + i,
			autoHeight: true,
			bodyStyle: 'paddingBottom: 5px;',
			defaults: {
				msgTarget : 'side'				
	        },
			items: [
				new Ext.Panel({
				    html: '<p style="font-weight: bold; text-align: center;"><s:text name="server.form.titleSX" /></p>'                            
				}),
				new Ext.Panel({
        	    	layout: "table",        	    	
        	    	layoutConfig: {
        	            columns: 2
           	        },
                    items: [
                        new Ext.form.RadioGroup({
							cellCls: "radioGroupCell",
							columns: 1,
							rowspan: 2,
							width: 85,
							defaults: {
								panelIndex: i,
								handler: function(checkbox, checked) {
									if(! checked) return;
									var index = checkbox.panelIndex;									
									toggleForm(checkbox.getGroupValue()==1, index);
									reloadSWF(index);
		        	    		}
		        	    	},							
							items: [ 
								new Ext.form.Radio({
									boxLabel: '<s:text name="server.form.defaultValues" />',
									name: "timeSpan"+i,
									inputValue: 0,
									checked: true
								}),
		                        new Ext.form.Radio({
		                        	boxLabel: '<s:text name="server.form.userDefinedValues" />',
		                        	name: "timeSpan"+i,
		                        	inputValue: 1,
		                        	style: { marginTop: '23px' }    
		                        })
		                    ]
						}),						
						new Ext.Panel({
							layout: "hbox",
        					layoutConfig: {
        						pack: 'start',
        	                    align: 'middle'		                    	                        
                            },
							items: [
								new Ext.Panel({
									width: 70,
									style: { marginTop: '5px' },
        	    					html: '<p><s:text name="server.form.timeSpans.label" /></p>'
								}),
		                        new Ext.form.ComboBox({
			                        id: 'comboTimeSpans'+i,
		                        	width: 170,
		                        	listWidth: 170,
			                        mode: 'local',
			                        value: 10080,
			                        triggerAction: 'all',
			                        forceSelection: true,
			                        editable: false,
			                        displayField: 'name',
			                        valueField: 'value',
			                        store: new Ext.data.JsonStore({
			                        	fields: [ 'name', 'value' ],
			                            data: [
			   	                            { name: '<s:text name="server.form.timeSpans.span1" />', value: 5 },
						   	                { name: '<s:text name="server.form.timeSpans.span2" />', value: 60 },
				   	               		    { name: '<s:text name="server.form.timeSpans.span3" />', value: 1440 },
					   	                    { name: '<s:text name="server.form.timeSpans.span4" />', value: 10080 },
			   	                         	{ name: '<s:text name="server.form.timeSpans.span5" />', value: 43200 },
			   	                         	{ name: '<s:text name="server.form.timeSpans.span6" />', value: 0 }
			   	                        ]
			                        }),
			                        listeners: {
		                            	scope: this,
		                            	'select': function(combo, record, itemIndex) {
	                            			var index = combo.panelIndex;
	                            			var timeSpan = record.get('value');
	                            			if(timeSpan == 0) {
		                            			Ext.Ajax.request({
													url: '<s:url action="MaxMinTimestamp" namespace="/json" encode="false" />',
													success: function (response, opts) {
														var data = JSON.parse(response.responseText);
														if(data.min && data.max) {
															if(data.min == data.max)
																data.max = data.min + 100000; // da sistemare
															startDateTime = new Date(data.min);
															endDateTime = new Date(data.max);																		 
															var startTime = Ext.util.Format.date(startDateTime, "H:i");
															var endTime = Ext.util.Format.date(endDateTime, "H:i");															
															Ext.getCmp("dataInizio" + index).setValue(startDateTime);
															Ext.getCmp("dataFine" + index).setValue(endDateTime);
															Ext.getCmp("oraInizio" + index).setValue(startTime);
															Ext.getCmp("oraFine" + index).setValue(endTime);
															reloadSWF(index);
														} else {
															alert("No data");
														}
		                            				},
			                            			failure: onFailure,
			                    		            scope: this,
			                    		            params: {
				                    		            idServiceInstance: idServices[index]
		                            				}
			                            		});
			                            		return;
	                            			}
											endDateTime = new Date();
											startDateTime = endDateTime.add(Date.MINUTE, -timeSpan);
											var startTime = Ext.util.Format.date(startDateTime, "H:i");
											var endTime = Ext.util.Format.date(endDateTime, "H:i");
											Ext.getCmp("dataInizio" + index).setValue(startDateTime);
											Ext.getCmp("dataFine" + index).setValue(endDateTime);
											Ext.getCmp("oraInizio" + index).setValue(startTime);
											Ext.getCmp("oraFine" + index).setValue(endTime);
											reloadSWF(index);
		                        		}
		                       		},
		                       		panelIndex: i
		                        }) 
		                    ]
						}),                        
                        new Ext.Panel({
                            width: 450,
                            defaults: {
                            	style: { marginTop: '5px' }
                    		},
                            items: [
	                        	new Ext.Panel({
	            					layout: "hbox",
	            					layoutConfig: {
	            						pack: 'start',
	            	                    align: 'middle'		                    	                        
	                                },
	            				   	items: [
	            						new Ext.Panel({
	            	    					width: 70,
	            	    					html: '<p><s:text name="server.form.startDate" /></p>'	            	
	                					}), 
	                					dataI, new Ext.Panel({ width: 20 }), timeI
	                				]
	                    	    }),
	                    	    new Ext.Panel({
	                    	    	layout: "hbox",
	            					layoutConfig: {
	            						pack: 'start',
	            	                    align: 'middle'		                    	                        
	                                },
	                                items: [
	               						new Ext.Panel({
	               	    					width: 70,
	               	    					html: '<p><s:text name="server.form.endDate" /></p>'
	               						}), 
	               						dataF, new Ext.Panel({ width: 20 }), timeF, new Ext.Panel({ width: 22 }),
	               						new Ext.Button({
	               	   						id: "submitForm" + i,
	            					    	text: '<s:text name="server.form.submit" />',
	            					    	panelIndex: i,
	            					    	width: 50,
	            					    	handler: function(button, eventObject) {	            					    		
	            					    		reloadSWF(button.panelIndex);
	            					    	}
	            						})
                					]
                    	   		})
                    	    ]
						})
                	]
				})
			],						
			columnWidth: .6
		});
		
		var formDx = new Ext.Panel({
			autoHeight: true,
			defaults: {
				msgTarget : 'side'
	        },
			items: [
				new Ext.Panel({
					html: '<p style="font-weight: bold; text-align: center;"><s:text name="server.form.titleDX" /></p>'
				}),
			    createFormDxField('<s:text name="server.form.spinnerPie.label" />', '<s:text name="server.form.checkboxShowPie" />', 
					    i, "pie", getPieWidget, 50),
			    createFormDxField('<s:text name="server.form.spinnerPieOff.label" />', '<s:text name="server.form.checkboxShowPieOff" />', 
					    i, "pieOff", getPieOffWidget, 50),
			    createFormDxField('<s:text name="server.form.spinnerActivity.label" />', '<s:text name="server.form.checkboxShowActivity" />', 
					    i, "activity", getActivityWidget, 50),
				createFormDxField('<s:text name="server.form.spinnerRTT.label" />', '<s:text name="server.form.checkboxShowRTT" />', 
					    i, "rtt", getRttWidget, 50),
			    createFormDxField('<s:text name="server.form.spinnerDescription.label" />', '<s:text name="server.form.checkboxShowDescription" />', 
					    i, "description", getDescriptionWidget, 50)
			],			
			columnWidth: .4
		});
		switch(servicesType[i]) {
		}	
		formDx.items.last().add(new Ext.Panel({ width: 20 }));
		formDx.items.last().add(new Ext.Button({
		    text: '<s:text name="server.form.button.refreshDashboard" />',
		    width: 55,
		    handler: function() {
				Ext.getCmp("tabGrafici"+i).setActiveTab(0);
		    	var table = Ext.getCmp("dash_tablePanel" + i);
		    	if(servicesType[i] == 'OPeNDAP')
					document.getElementById("divOpendap"+i).appendChild(document.getElementById("iframe_opendap"+i));
				table.removeAll();
				table.add(createDashboardElements(i));
				table.doLayout(false, true);
			}
		}));
		toggleForm(false, i);
		formSx.on("render", function() {
			Ext.getCmp('formSx' + i).doLayout(false, true);
		});		
		return new Ext.Panel({
			layout: "column",
			items: [ formSx, formDx ]
		});
	}

	function barTypeMenuSpecialized(text, checked, group, value, swfEl, dataGetter, par) {
		return {
			text: text,
			checked: checked,
			group: group,
			value: value,												
			checkHandler: function(item) {
				if(!item.checked) return;
				var swf = document.getElementById(swfEl);
				if(swf.load)
					swf.load(dataGetter(par));													
			}
		}
	}
	function barTypeMenu(group, swf, dataGetter, par, selected) {
		var types = [ "Bar", "Bar Glass", "Bar Filled", "Bar 3D", "Bar Sketch", "Cylinder", "Cylinder 3D", 
		  			  "Bar Round Glass", "Bar Round", "Bar Dome" ];
		var menus = new Array(types.length);
		for(var i = 0; i < types.length; i ++)
			menus[i] = barTypeMenuSpecialized(types[i], i == (selected==null? 6: selected), group, i, swf, dataGetter, par);
		return menus;
	}
	
	function addServiceTab(index, title) {
		function customCheckHandler(item) {
			if(!activityDiagramSettings[index].onOff)
				eval("onActivityCheck" + index + "()");
			else {
				Ext.getCmp("checkboxFailure"+index).setChecked(false, true);
				activityDiagramSettings[index].onOff = false;
				var params = activityDiagramSettings[index].activityInterval[activityDiagramSettings[index].activityInterval.length-1];
				params.onOff = false; 
				loadActivityChart(index, false, params);				 
			}
		}		
		return {
			title: '<span class="titleServiceTab">' + title + '</span>',
			frame: true,
			autoScroll: true, //// formerly true!
	        items: [
			    new Ext.Panel({
	            	margins: '3 0 3 3',
	                cmargins: '3 3 3 3',
	        		frame: true,
	        		title: '<s:text name="server.form.title" />',
	        		labelAlign: 'right',
	        	    labelWidth: 85,
	        	    waitMsgTarget: true,
	        	    collapsible: true,
	        	    titleCollapse: true,
	        	    collapsed: true,	        	    
	        	    items: [ createForm(index) ],
	        	    tools: [ 
	    	        	{
							id: "help",
							qtip: "Help",
							handler: function(event, toolEl, panel, tc) {
	    	        			openHelpWindow(toolEl, '<s:text name="help.server.settingsPanel" />');
	    	        		}
		        		} 
		        	]
		        }),
		        new Ext.TabPanel({
			        id: "tabGrafici"+index,
			        margins: '3 0 3 3',
	                cmargins: '3 3 3 3',
	                enableTabScroll: true,// formerly true!
	             // tabPosition: 'bottom',
		            items: [
						new Ext.Panel({
							id: "myDashboard"+index,
							margins: '3 0 3 3',
						    cmargins: '3 3 3 3',
							frame: true,
							title: '<span class="titleDashboardTab"><s:text name="server.panel.dashboard.title" /></span>',
							layout: "fit",
							items: [ createDashboard(index) ],
							height: 2000,
							panelEl: "dashboard",
							panelIndex: index // custom properties
						}),
						new Ext.Panel({
			            	margins: '3 0 3 3',
			                cmargins: '3 3 3 3',
			        		frame: true,
			        		title: '<span class="titleNoDashboardTab"><s:text name="server.panel.pieChart.title" /></span>',
				        	height: 390,
				        	tbar: new Ext.Toolbar({
								items: [
									new Ext.Button({
										iconCls: 'restoreDashBoardViewIcon',
										tooltip: '<s:text name="server.toolbar.button.dashboardReturn" />',
										handler: onRestore,
										myParams: {
											index: index
										}
									}),
									{
					        			id: "checkboxPieChartAnimation"+index,
					        			text: '<s:text name="server.toolbar.checkbox.animation.label" />',
					        			pressed: true,
					        			enableToggle: true
						        	}, {
							        	id: "pieBarType" + index,
										text: '<s:text name="server.toolbar.checkbox.barType.label" />',										
										hidden: true,
										menu: new Ext.menu.Menu({
											items: barTypeMenu("pieBarType"+index, "graficoTorta"+index, get_data_pie, index)											
										})
							        }, {
								        text: '<s:text name="server.toolbar.checkbox.color.label" />',
								        menu: {
								        	items: [
										        {
											        text: '<s:text name="pieChart.slice.available.text" />',
										        	menu: new Ext.menu.ColorMenu({
											        	value: pieDiagramColor[index].available,
											        	handler: function(cm, color) {
										        			pieDiagramColor[index].available = "#" + color;
										        			ricarica("graficoTorta"+index, get_data_pie(index));
											        	}
											        })
									        	},
									        	{
									        		text: '<s:text name="pieChart.slice.failure.text" />',
										        	menu: new Ext.menu.ColorMenu({
										        		value: pieDiagramColor[index].failure,
											        	handler: function(cm, color) {
										        			pieDiagramColor[index].failure = "#" + color;
										        			ricarica("graficoTorta"+index, get_data_pie(index));
											        	}
											        })
									        	}
									        ]
							        	}
							        }, '->',
						        	{
							        	iconCls: 'buttonPieChartIcon',	        				
					    	    		tooltip: '<span class=\"toolbarItem\">Pie</span>',
					    	    		enableToggle: true,
					    	    		toggleHandler: eval("onPieItemToggle"+index),
					    	            toggleGroup: "togglePie"+index,
					    	            pressed: true,
					    	            tipoGrafico: 0 
					    			},
						        	{
				        				iconCls: 'buttonBarChartIcon',
					    	    		tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.bar" /></span>',
					    	    		enableToggle: true,
					    	    		toggleHandler: eval("onPieItemToggle"+index),
					    	            toggleGroup: "togglePie"+index,
					    	            pressed: false,
					    	            tipoGrafico: 1 
					    			},
					    			new Ext.Button({
										iconCls: "helpButton",
										tooltip: 'Help',
										handler: function(button, eventObject) {
					    					openHelpWindow(button, '<s:text name="help.server.dashboard.totalAvailabilityChart" />');
										}					
						    		})
								]
				        	}),
			        		contentEl: 'divPieChart'+index,
			        		panelEl: "pie",
			        		panelIndex: index // custom properties
			            }),
			            new Ext.Panel({
			            	margins: '3 0 3 3',
			                cmargins: '3 3 3 3',
			        		frame: true,
			        		title: '<span class="titleNoDashboardTab"><s:text name="server.panel.pieChartOff.title" /></span>',
				        	height: 390,
				        	tbar: new Ext.Toolbar({
								items: [
									new Ext.Button({
										iconCls: 'restoreDashBoardViewIcon',
										tooltip: '<s:text name="server.toolbar.button.dashboardReturn" />',
										handler: onRestore,
										myParams: {
											index: index
										}
									}),
									{
					        			id: "checkboxPieChartOffAnimation"+index,
					        			text: '<s:text name="server.toolbar.checkbox.animation.label" />',
					        			pressed: true,
					        			enableToggle: true
						        	}, {
							        	id: "pieOffBarType" + index,
										text: '<s:text name="server.toolbar.checkbox.barType.label" />',										
										hidden: false,
										menu: new Ext.menu.Menu({
											items: barTypeMenu("pieOffBarType"+index, "grafico1Torta"+index, get_data_pie_off, index, 7)											
										})
							        }, {
								        text: '<s:text name="server.toolbar.checkbox.color.label" />',
								        menu: {
								        	items: [
										        {
											        text: '<s:text name="activityChart.element.inactive.text" />',
										        	menu: new Ext.menu.ColorMenu({
														value: pieOffDiagramColor[index].down,
														handler: function(cm, color) {
										        			pieOffDiagramColor[index].down = "#" + color;
										        			ricarica("grafico1Torta"+index, get_data_pie_off(index));
											        	}
											        })
									        	},
									        	{
									        		text: '<s:text name="activityChart.element.timeout.text" />',
										        	menu: new Ext.menu.ColorMenu({
														value: pieOffDiagramColor[index].unreachable,
														handler: function(cm, color) {
										        			pieOffDiagramColor[index].unreachable = "#" + color;
										        			ricarica("grafico1Torta"+index, get_data_pie_off(index));
											        	}
											        })
									        	},
									        	{
									        		text: '<s:text name="activityChart.element.error.text" />',
										        	menu: new Ext.menu.ColorMenu({
														value: pieOffDiagramColor[index].error,
														handler: function(cm, color) {
										        			pieOffDiagramColor[index].error = "#" + color;
										        			ricarica("grafico1Torta"+index, get_data_pie_off(index));
											        	}
											        })
									        	}
									        ]
							        	}											
							        }, '->',
						        	{
							        	iconCls: 'buttonPieChartIcon',
					    	    		tooltip: '<span class=\"toolbarItem\">Pie</span>',
					    	    		enableToggle: true,
					    	    		toggleHandler: eval("onPieOffItemToggle"+index),
					    	            toggleGroup: "togglePieOff"+index,
					    	            pressed: false,
					    	            tipoGrafico: 0 
					    			},
						        	{
				        				iconCls: 'buttonBarChartIcon',
					    	    		tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.bar" /></span>',
					    	    		enableToggle: true,
					    	    		toggleHandler: eval("onPieOffItemToggle"+index),
					    	            toggleGroup: "togglePieOff"+index,
					    	            pressed: true,
					    	            tipoGrafico: 1 
					    			},
					    			new Ext.Button({
										iconCls: "helpButton",
										tooltip: 'Help',
										handler: function(button, eventObject) {
					    					openHelpWindow(button, '<s:text name="help.server.dashboard.failureChart" />');
					    				}					
						    		})
								]
				        	}),
			        		contentEl: 'divPieChartOff'+index,
			        		panelEl: "pieOff",
			        		panelIndex: index // custom properties
			            }),
			            new Ext.Panel({
			            	margins: '3 0 3 3',
			                cmargins: '3 3 3 3',
			        		frame: true,
			        		title: '<span class="titleNoDashboardTab"><s:text name="server.panel.activityChart.title" /></span>',
			        		height: 390,				        	
			        		tbar: new Ext.Toolbar({
			        			items: [
					        		new Ext.Button({
										iconCls: 'restoreDashBoardViewIcon',
										tooltip: '<s:text name="server.toolbar.button.dashboardReturn" />',
						    	    	handler: onRestore,
						    	    	myParams: {
											index: index
						    	    	}
						        	}),
					        		{
						        		id: "menuActivity" + index,
						        		text: '<s:text name="server.toolbar.toggleButton.active.label" />',
					        			menu: {
						        			items: [
												{
													id: "checkboxActivity"+index,
													text: "Show",
													checked: true,
													checkHandler: eval("onActivityCheck"+index)
												}, {
													text: '<s:text name="server.toolbar.checkbox.color.label" />',
													menu: new Ext.menu.ColorMenu({
														value: activityDiagramSettings[index].activeColor,
														handler: function(cm, color) {
															activityDiagramSettings[index].activeColor = "#" + color;
															ricarica("graficoAttivita"+index, get_data_activity(index));
														}
												    })
							        			}
							        		]
					        			}
				        			},
				        			{
					        			id: "menuFailures" + index,
				        				text: '<s:text name="server.toolbar.toggleButton.failure.label" />',
				        				menu: new Ext.menu.Menu({
						        			items: [
												{
													id: "checkboxFailure"+index,
													text: '<s:text name="server.toolbar.checkbox.allFailures.label" />',
													checked: true,
													checkHandler: function(item) {														
														if(activityDiagramSettings[index].onOff)
															eval("onActivityCheck" + index + "()");
														else {
															if(item.checked) {
																Ext.getCmp("checkboxInactivity"+index).setChecked(false, true);
																Ext.getCmp("checkboxTimeout"+index).setChecked(false, true);
																Ext.getCmp("checkboxError"+index).setChecked(false, true);
																activityDiagramSettings[index].onOff = true;
																var params = activityDiagramSettings[index].activityInterval[activityDiagramSettings[index].activityInterval.length-1];
																params.onOff = true;
																loadActivityChart(index, false, params);
															} 
														}
													},
													menu: {
														items: [
															{
																text: '<s:text name="server.toolbar.checkbox.color.label" />',
																menu: new Ext.menu.ColorMenu({
																	value: activityDiagramSettings[index].failureColor,
																	handler: function(cm, color) {
																		activityDiagramSettings[index].failureColor = "#" + color;
																		ricarica("graficoAttivita"+index, get_data_activity(index));
																	}													
															    })
															}
														]
													}
												}, '-', {
													id: "checkboxInactivity"+index,
													text: '<s:text name="server.toolbar.checkbox.inactive.label" />',
													checked: false,													
													checkHandler: customCheckHandler,
													menu: {
														items: [
															{
																text: '<s:text name="server.toolbar.checkbox.color.label" />',
																menu: new Ext.menu.ColorMenu({
																	value: activityDiagramSettings[index].downColor,
																	handler: function(cm, color) {
																		activityDiagramSettings[index].downColor = "#" + color;
																		ricarica("graficoAttivita"+index, get_data_activity(index));
																	}													
															    })
															}
														]
													}
												}, {
													id: "checkboxTimeout"+index,
													text: '<s:text name="server.toolbar.checkbox.timeout.label" />',
													checked: false,
													checkHandler: customCheckHandler,
													menu: {
														items: [
															{
																text: '<s:text name="server.toolbar.checkbox.color.label" />',
																menu: new Ext.menu.ColorMenu({
																	value: activityDiagramSettings[index].unreachableColor,
																	handler: function(cm, color) {
																		activityDiagramSettings[index].unreachableColor = "#" + color;
																		ricarica("graficoAttivita"+index, get_data_activity(index));
																	}													
															    })
															}
														]
													}
												}, {
													id: "checkboxError"+index,
													text: '<s:text name="server.toolbar.checkbox.error.label" />',
													checked: false,
													checkHandler: customCheckHandler,
													menu: {
														items: [
															{
																text: '<s:text name="server.toolbar.checkbox.color.label" />',
																menu: new Ext.menu.ColorMenu({
																	value: activityDiagramSettings[index].errorColor,
																	handler: function(cm, color) {
																		activityDiagramSettings[index].errorColor = "#" + color;
																		ricarica("graficoAttivita"+index, get_data_activity(index));
																	}													
															    })
															}
														]
													}
												}
								        	]
						        		})
					        		}, 
				        			{
				        				id: "checkboxActivityChartGrid"+index,
					        			text: '<s:text name="server.toolbar.checkbox.chartGrid.label" />',
					        			pressed: true,
					        			enableToggle: true					        			
					        		},
					        		{
										text: '<s:text name="server.toolbar.checkbox.animation.label" />',
										enableToggle: true,
										id: "checkboxActivityChartAnimation"+index,
										pressed: true
							        }, {
							        	id: "activityBarType" + index,
										text: '<s:text name="server.toolbar.checkbox.barType.label" />',										
										hidden: false,
										menu: new Ext.menu.Menu({
											items: barTypeMenu("activityBarType"+index, "graficoAttivita"+index, get_data_activity, index)											
										})
							        }, {
								        id: "menuZoomOut" + index,
								        iconCls: "buttonZoomOutIcon",
										tooltip: '<s:text name="server.toolbar.button.zoomOut" />',
										handler: function() {
											zoomActivity(null, true);
							        	}
							        }, '->', 
					        		{
				        				iconCls: 'buttonGridIcon',
					        			tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.table" /></span>',
					    	    		enableToggle: true,
					    	    		toggleHandler: eval("onActItemToggle"+index),
					    	    		toggleGroup: "toggleAct"+index,
					    	            pressed: false,
					    	            tipoGrafico: -1
					        		},					        		
							        {
				        				iconCls: 'buttonBarChartIcon',
					    	    		tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.bar" /></span>',
					    	    		enableToggle: true,
					    	    		toggleHandler: eval("onActItemToggle"+index),
					    	            toggleGroup: "toggleAct"+index,
					    	            pressed: true,
					    	            tipoGrafico: 0 
					    			}, {
						    			iconCls: 'buttonStackChartIcon',
					    	    		tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.stack" /></span>',
					    	    		enableToggle: true,
					    	            toggleHandler: eval("onActItemToggle"+index),
					    	            toggleGroup: "toggleAct"+index,
					    	            pressed: false,
					    	            tipoGrafico: 1
					    			}, {
					    				iconCls: 'buttonLineChartIcon',
					    	    		tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.line" /></span>',
					    	    		enableToggle: true,
					    	            toggleHandler: eval("onActItemToggle"+index),
					    	            toggleGroup: "toggleAct"+index,
					    	            pressed: false,
					    	            tipoGrafico: 2
					    			}, {
					    				iconCls: 'buttonAreaChartIcon',
					    	    		tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.area" /></span>',
					    	    		enableToggle: true,
					    	            toggleHandler: eval("onActItemToggle"+index),
					    	            toggleGroup: "toggleAct"+index,
					    	            pressed: false,
					    	            tipoGrafico: 3
					    			},
					    			new Ext.Button({
										iconCls: "helpButton",
										tooltip: 'Help',
										handler: function(button, eventObject) {
					    					openHelpWindow(button, '<s:text name="help.server.dashboard.availabilityChart" />');
					    				}					
						    		})
				    			]
				    		}),
			        		contentEl: 'divActivityChart'+index,
			        		panelEl: "activity",
			        		panelIndex: index // custom properties
				        }), 
				        new Ext.Panel({
			            	margins: '3 0 3 3',
			                cmargins: '3 3 3 3',
			        		frame: true,
			        		title: '<span class="titleNoDashboardTab"><s:text name="server.panel.rttChart.title" /></span>',
			        		height: 390,				        	
			        		tbar: new Ext.Toolbar({
								items: [
									new Ext.Button({
										iconCls: 'restoreDashBoardViewIcon',
										tooltip: '<s:text name="server.toolbar.button.dashboardReturn" />',
										handler: onRestore,
										myParams: {
											index: index
										}
									}),
									{
				        				id: "checkboxRttChartGrid"+index,
					        			text: '<s:text name="server.toolbar.checkbox.chartGrid.label" />',
					        			pressed: true,
					        			enableToggle: true,
					        			panelIndex: index
				        			},
				        			{
					        			id: "checkboxRttChartAnimation"+index,
					        			text: '<s:text name="server.toolbar.checkbox.animation.label" />',
					        			pressed: true,
					        			enableToggle: true
					        		//	fieldClass: "checkboxAttivita"
						        	}, {
							        	id: "rttBarType" + index,
										text: '<s:text name="server.toolbar.checkbox.barType.label" />',										
										hidden: false,
										menu: new Ext.menu.Menu({
											items: barTypeMenu("rttBarType"+index, "graficoRTT"+index, get_data_rtt, index)											
										})
							        },
						        	{
							        	id: 'colorMenuRTT' + index,
										text: '<s:text name="server.toolbar.checkbox.color.label" />',
										menu: new Ext.menu.ColorMenu({
											value: rttDiagramColor[index],
									        handler: function(cm, color) {
												rttDiagramColor[index] = "#" + color;
												toggleRttChart({panelIndex: index}, null);
									        }
									    })
								    }, '->', 
									{
										iconCls: 'buttonGridIcon',
					        			tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.table" /></span>',
					    	    		enableToggle: true,
					    	    		toggleHandler: eval("onRttItemToggle"+index),
					    	    		toggleGroup: "toggleRtt"+index,
					    	            pressed: false,
					    	            tipoGrafico: -1
					        		},
									{
										iconCls: 'buttonBarChartIcon',
										tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.bar" /></span>',
										enableToggle: true,
										toggleHandler: eval("onRttItemToggle"+index),
									    toggleGroup: "toggleRtt"+index,
									    pressed: true,
									    tipoGrafico: 0 
									}, {
										iconCls: 'buttonLineChartIcon',
										tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.line" /></span>',
										enableToggle: true,
									    toggleHandler: eval("onRttItemToggle"+index),
									    toggleGroup: "toggleRtt"+index,
									    pressed: false,
									    tipoGrafico: 1
									}, {
										iconCls: 'buttonAreaChartIcon',
										tooltip: '<span class=\"toolbarItem\"><s:text name="server.toolbar.button.area" /></span>',
										enableToggle: true,
									    toggleHandler: eval("onRttItemToggle"+index),
									    toggleGroup: "toggleRtt"+index,
									    pressed: false,
									    tipoGrafico: 2
									},
									new Ext.Button({
										iconCls: "helpButton",
										tooltip: 'Help',
										handler: function(button, eventObject) {
					    					openHelpWindow(button, '<s:text name="help.server.dashboard.rttChart" />');
					    				}					
						    		}),
								]
			        		}),
			        		contentEl: 'divRTTChart'+index,
			        		panelEl: "rtt",
			        		panelIndex: index // custom properties
				        }),
			            {
			            	margins: '3 0 3 3',
			                cmargins: '3 3 3 3',
			        		frame: true,
			        		title: '<span class="titleNoDashboardTab"><s:text name="server.panel.serviceDescription.title" /></span>',
			        		tbar: new Ext.Toolbar({
								items: [
									new Ext.Button({
										iconCls: 'restoreDashBoardViewIcon',
										tooltip: '<s:text name="server.toolbar.button.dashboardReturn" />',
										handler: onRestore,
										myParams: {
											index: index
										}
									}), '->',
									new Ext.Button({
										iconCls: "helpButton",
										tooltip: 'Help',
										handler: function(button, eventObject) {
					    					openHelpWindow(button, '<s:text name="help.server.dashboard.summary" />');
					    				}					
						    		})
								]
			        		}),
			        		contentEl: 'divDescription'+index,
			        		panelEl: "description",
			        		panelIndex: index // custom properties
			            }
		            ]
		        })
	        ]
		};
	}
//]]>
</script>
