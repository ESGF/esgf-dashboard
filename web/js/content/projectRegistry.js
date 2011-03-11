/* Author: University of Salento and CMCC */

function buildLayout(title, registryInformation, filters, grid, removeFilters, descriptionPanelHelp, settingsPanelHelp, gridPanelHelp) {
	var tabs = Ext.getCmp('tab-panel');
	var form = createSettingsForm();
	tabs.add({
        title: title,
        frame: true,
		autoScroll: true,
		items: [
		    new Ext.Panel({
		    	margins: '3 0 3 3',
                cmargins: '3 3 3 3',
                title: registryInformation,
        		frame: true,
        		collapsible: true,
        		titleCollapse: true,
        		collapsed: true,
        		contentEl: 'registryInformation',
    			tools: [{        			
					id: "help",
					qtip: "Help",
					handler: function(event, toolEl, panel, tc) {
    					openHelpWindow(toolEl, descriptionPanelHelp);
	        		}
        		}]
		    }),
		    new Ext.Panel({
		    	id: 'settingsPanel',
		    	margins: '3 0 3 3',
                cmargins: '3 3 3 3',
                title: filters,
        		frame: true,
        		collapsible: true,
        		titleCollapse: true,
        		collapsed: true,
        		items: [ form ],
        		tools: [{        			
					id: "help",
					qtip: "Help",
					handler: function(event, toolEl, panel, tc) {
        				openHelpWindow(toolEl, settingsPanelHelp);
	        		}
        		}],
        		listeners: {
		    		'render': function() {
		    			Ext.getCmp('settingsPanel').doLayout(false, true);
		    		}
		    	}
		    }), new Ext.Panel({
		    	margins: '3 0 3 3',
                cmargins: '3 3 3 3',
                title: grid,
                frame: true,
                items: [ createGrid() ],
                tools: [
					new Ext.Button({
						id: "gear",
						qtip: removeFilters,						
						handler: function() {										
							clearFilters();
						}
					}),
					{
						id: "help",
						qtip: "Help",
						handler: function(event, toolEl, panel, tc) {
							openHelpWindow(toolEl, gridPanelHelp);
		        		}
					}
				]
		    })
		]
	}).show();
	var treePanel = Ext.getCmp('tree-panel'); 
	if(treePanel)
		treePanel.loader.on('load', function() {
			var nodo = treePanel.root.findChild('myObject', idProject, false);
			nodo.expand();
			nodo = nodo.findChild('myObject', "projectRegistry", false);
			nodo.select();
			nodo.setText("<b>" + nodo.attributes.text + "</b>");			
		});	
}

function openDetailsTab(id, name, tabTitle) {
	var tabs = Ext.getCmp('tab-panel');
	var panel = tabs.find("panelId", id);
	if(panel && panel.length>0) {
		panel[0].show();
		return;
	}
	
	var node = document.createElement("div");
	node.setAttribute("id", "tagsDiv"+id);
	document.getElementById("containerTags").appendChild(node);
	loadTags(id);
	
	node = document.createElement("div");
	node.setAttribute("id", "rateDetails" + id);
	document.getElementById("containerRates").appendChild(node);
	
	tabs.add({
        title: String.format(tabTitle, name),
        frame: true,
		autoScroll: true,
		closable: true,
		items: [
		    new Ext.Panel({
		    	frame: false,
		    	margins: '3 0 3 3',
                cmargins: '3 3 3 3',
                items: [
                    createInformationPanel(id),
		    	    createCommentsPanel(id)
		    	]
		    /*	tools: [{
					id: "help",
					qtip: "Help",
					handler: function(event, toolEl, panel, tc) {						
	        		}
        		}]*/
		    })
		],
		panelId: id
	}).show();
	Ext.getCmp("commentsGrid" + id).syncSize();	
}

