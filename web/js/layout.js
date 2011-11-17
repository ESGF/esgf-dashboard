/* Author: University of Salento and CMCC */

Ext.onReady(function() {
	Ext.QuickTips.init();
	Ext.form.Field.prototype.msgTarget = 'side';
	// var params = Ext.urlDecode(window.location.search.substring(1));
	
	/*var themeData = new Ext.data.SimpleStore({
		fields: ['display', 'value'],
		data: [
			['Default (Blue)', ''], 
			['Gray', grayCssPath]
		]
	});

	var comboTheme = new Ext.form.ComboBox({
		store: themeData,
		renderTo: 'exttheme',
		displayField: 'display',
		typeAhead: true, 
		mode: 'local', 
		triggerAction: 'all', 
		selectOnFocus: true, 
		resizable: false, 
		listWidth: 230,  
		width: 230, 
		valueField: 'value', 
		value: ''
	}); 

	comboTheme.on('select', 
		function(combo){ 
			Ext.util.CSS.swapStyleSheet('theme', combo.getValue()); 
		}
		, this); 
*/
	var tabs = new Ext.TabPanel({
		id: 'tab-panel',
        region: 'center',
        margins: '3 3 3 0', 
        activeTab: 0,
		defaults: { autoScroll:true },
		enableTabScroll: true,
        frame: true,
        items: []
    });

    // Panel for the west
    var nav = new Ext.tree.TreePanel({
    	id: 'tree-panel',
    	title: navigatorTitle,
		region: 'west',
		frame: true,
        split: true,
        width: '18%',
        minSize: 150,
        maxSize: 400,
		collapsible: true,
		titleCollapse: true,
		autoScroll: true,
        margins: '3 0 3 3',
        cmargins: '3 3 3 3',
        root: new Ext.tree.AsyncTreeNode({
        	cls: "treeRoot",
    		expanded: true,
    		text: globalViewText
    	}),
        rootVisible: true,
        lines: false,
        //singleExpand: true,
        animate: true,
        useArrows: true,
        loader: new Ext.tree.TreeLoader({
            dataUrl : projectTreePath // + "?endDate=" + new Date().format("U")
        })
    });
	
    nav.on('click', function(n){
    	var sn = this.selModel.selNode || {}; // selNode is null on initial selection
    	// if(n.id == sn.id) return; // ignore clicks on currently selected node
    	if(n.id == this.root.id) {
    		document.location=pathURL+"monitoring/AllProjects.action";
    	} else if(n.leaf) {
    		switch(n.attributes.myObject) {
    			case "opendapRegistry":
    				document.location=pathURL+"opendap/OpendapRegistry.action?idProject=" + n.parentNode.attributes.myObject;
    				break;
    			default: 
		    		document.getElementById('idServer').value = n.attributes.myObject;
		    		document.getElementById('idProjectServer').value = n.parentNode.attributes.myObject;
		    		document.getElementById('formServer').submit();
    		}
    	} else {
    		document.getElementById('idProject').value = n.attributes.myObject;
    		document.getElementById('formProject').submit();
    	}
    });
    /*
    var nord = new Ext.Panel({
        region: 'north',
		frame: true,
		height: 75,
		// maxWidth:260, split: true
        margins: '3 0 3 3',
		contentEl: 'northEl'
    });*/
    
    var viewport = new Ext.Viewport({
		layout: 'border',
		items: [
	        {
	        	region: 'center',
	        	layout: 'border',
	        	width: '88%',
	        	frame: true,
	        	items: [northPanel, nav, tabs]
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
    viewport.on('resize', function() { viewport.doLayout(false, true);});
});
