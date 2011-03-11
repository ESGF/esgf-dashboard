/* Author: University of Salento and CMCC */

Ext.onReady(function() {
	Ext.QuickTips.init();
	Ext.form.Field.prototype.msgTarget = 'side';
	
	var viewport = new Ext.Viewport({
		layout: 'border',
		items: [
		    new Ext.Panel({
		    	region: 'north',
		    	frame: true,
		    	height: 38,
		    	margins: '1 0 1 0',
		    	contentEl: 'loggedInContainer'
		    }),
		    new Ext.TabPanel({
				id: 'tab-panel',
		        region: 'center',
		        margins: '3 3 3 0', 
		        activeTab: 0,
				defaults: { autoScroll:true },
				enableTabScroll: true,
		       	frame: true
		    })
		]
	});
});
