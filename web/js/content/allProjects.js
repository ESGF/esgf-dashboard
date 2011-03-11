/* Author: University of Salento and CMCC */

function buildAllProjectsTab(mainPanelTitle, mapPanelTitle, descriptionPanelTitle, descriptionPanelHelp, mapPanelHelp, gridPanelHelp) {
	var tabs = Ext.getCmp('tab-panel');	
	tabs.add({
        title: mainPanelTitle,
        frame: true,
		autoScroll: true,
		items: [
		    new Ext.Panel({
		    	margins: '3 0 3 3',
                cmargins: '3 3 3 3',
        		frame: true,
        		title: descriptionPanelTitle,
        		collapsible: true,
        		titleCollapse: true,
        		collapsed: true,
        		contentEl: "descriptionContent",
        		tools: [{        			
					id: "help",
					qtip: "Help",
					handler: function(event, toolEl, panel, tc) {
        				openHelpWindow(toolEl, descriptionPanelHelp);
	        		}        			
        		}]
		    }),
            new Ext.Panel({
            	margins: '3 0 3 3',
                cmargins: '3 3 3 3',
        		frame: true,
        		title: mapPanelTitle,
        		collapsible: true,
        		titleCollapse: true,
        		contentEl: 'container',
        		tools: [{        			
					id: "help",
					qtip: "Help",
					handler: function(event, toolEl, panel, tc) {
        				openHelpWindow(toolEl, mapPanelHelp);
	        		}        			
        		}]
            }),
            new Ext.Panel({
            	id: 'gridPanel',
        		margins: '3 0 3 3',
                cmargins: '3 3 3 3',
        		frame: true,
        		title: ' ',
        		height: 280,
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
		treePanel.loader.on('load', function() {
			var nodo = Ext.getCmp('tree-panel').root;
			nodo.select();
			nodo.setText("<b>" + nodo.attributes.text + "</b>");
		});
}
