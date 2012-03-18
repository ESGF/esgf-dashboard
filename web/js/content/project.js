/* Author: University of Salento and CMCC */

var gridloaded=0;

function buildProjectTab(projectName, availabilityPanelTitle, registeredUsersPanelTitle, hostTypePanelTitle, mapsTitle, descriptionPanelTitle, projectAvailability, projectFailure, serviceDistribution, userDistribution, descriptionPanelHelp, mapPanelHelp, gridPanelHelp) {
	var tabs = Ext.getCmp('tab-panel');	
	tabs.add({
		id: "tabProject",
        title: '<span class="titleProjectTab">' + projectName + '</span>',
        frame: true,
		autoScroll: true,
	    items: [
            new Ext.Panel({
        		frame: true,
        		collapsible: true,
        		titleCollapse: true,
        		collapsed: true,
        		layout: "column",
        		margins: '3 0 3 3',
                cmargins: '3 3 3 3',
        		title: descriptionPanelTitle,
        		items: [
        		    new Ext.Panel({        		    	
        		    	columnWidth: ".5",
        		    	frame: false,
        		    	height: 210,
        		    	contentEl: 'projectContent'
        		    }),
        		    new Ext.TabPanel({
        		    	columnWidth: ".5",
        		    	frame: false,
        		    	border: true,
        		    	height: 210,
        		    	enableTabScroll: true,
        		    	activeTab: 0,
        		    	items: [
							new Ext.Panel({
								title: projectAvailability,
								frame: true,
								contentEl: 'containerAvgProjectActivityPieChart'
							}),
							new Ext.Panel({
								title: projectFailure,
								frame: true,
								contentEl: 'containerAvgProjectActivityBarChartOff'
							}),
        		    	    new Ext.Panel({
        		    	    	title: serviceDistribution,
        		    	    	frame: true,
        		    	    	contentEl: 'containerServicesDistributionPieChart'
        		    	    }),
							new Ext.Panel({
        		    	    	title: userDistribution,
        		    	    	frame: true,
        		    	    	contentEl: 'containerUsersDistributionPieChart'
        		    	    })
        		    	]
        		    })
        		],
        		tools: [{        			
					id: "help",
					qtip: "Help",
					handler: function(event, toolEl, panel, tc) {
        				openHelpWindow(toolEl, descriptionPanelHelp);
	        		}        			
        		}]
            }),
            /*new Ext.Panel({
            	id: "projectMapPanel",
            	margins: '3 0 3 3',
                cmargins: '3 3 3 3',
        		frame: true,
        		title: mapPanelTitle,
        		collapsible: true,
        		titleCollapse: true,
        		contentEl: 'containerAvailabilityMap',
        		tools: [{        			
					id: "help",
					qtip: "Help",
					handler: function(event, toolEl, panel, tc) {
        				openHelpWindow(toolEl, mapPanelHelp);
	        		}        			
        		}]
            }),*/
            /*new Ext.TabPanel({
		    	collapsible: true,
		    	//margins: '3 0 3 3',
		    	frame: true,
		    	enableTabScroll: true,
		    	activeTab: 0,
		    	tools: [{
					id: "help",
					qtip: "Help",
					handler: function(event, toolEl, panel, tc) {
       				openHelpWindow(toolEl, mapPanelHelp);
	        		}
       		}],
		    	items: [
					new Ext.Panel({
					    id: 'availabilityMapPanel',
						title: availabilityPanelTitle,
						frame: true,
						listeners: {activate: loadProjectMap},
						contentEl: 'containerAvailabilityMap'
					}),
		    	    new Ext.Panel({
		    	    	id: 'registeredUsersPanel',
		    	    	title: registeredUsersPanelTitle,
		    	    	frame: true,
		    	    	listeners: {activate: loadRegisteredUsersMap},
						contentEl: 'containerUsersMap'
		    	    }),
					new Ext.Panel({
						id: 'hostTypePanel',
						title: hostTypePanelTitle,
						frame: true,
						listeners: {activate: loadNodeTypeMap},
						contentEl: 'containerNodeTypeMap'
					})
		    	]
		    }),*/
            new Ext.Panel({
               	id: "projectMapPanel",
               	margins: '3 0 3 3',
                   cmargins: '3 3 3 3',
           		frame: true,
           		title: mapsTitle,
           		collapsible: true,
           		titleCollapse: true,
           		tools: [{
    					id: "help",
    					qtip: "Help",
    					handler: function(event, toolEl, panel, tc) {
           				openHelpWindow(toolEl, mapPanelHelp);
    	        		}
           		}],
           		items: [
    					new Ext.TabPanel({
    						frame: true,
    						enableTabScroll: true,
    						activeTab: 0,
    						items: [
    							new Ext.Panel({
    							    id: 'availabilityMapPanel',
    								title: availabilityPanelTitle,
    								frame: true,
    								listeners: {activate: loadProjectMap},
    								contentEl: 'containerAvailabilityMap'
    							}),
    						    new Ext.Panel({
    						    	id: 'registeredUsersPanel',
    						    	title: registeredUsersPanelTitle,
    						    	frame: true,
    						    	listeners: {activate: loadRegisteredUsersMap},
    								contentEl: 'containerUsersMap'
    						    })/*,
    							new Ext.Panel({
    								id: 'hostTypePanel',
    								title: hostTypePanelTitle,
    								frame: true,
    								listeners: {activate: loadNodeTypeMap},
    								contentEl: 'containerNodeTypeMap'
    							})*/
    						]
    					}),
           		]
               }),
            new Ext.Panel({
            	id: "gridPanel",
            	margins: '3 0 3 3',
                cmargins: '3 3 3 3',
        		frame: true,
        		height: 280,
        		title: ' ',
        		tools: [{        			
					id: "help",
					qtip: "Help",
					handler: function(event, toolEl, panel, tc) {
        				openHelpWindow(toolEl, gridPanelHelp);
	        		}        			
        		}]
            })
        ]
    }).show();	
	var treePanel = Ext.getCmp('tree-panel'); 
	if(treePanel)
		treePanel.loader.on('load', selectProject);
}

function selectProject() {
	var nodo = Ext.getCmp('tree-panel').root.findChild('myObject', idProject, false);
	nodo.expand();
	nodo.select();
	nodo.setText("<b>" + nodo.attributes.text + "</b>");
}
