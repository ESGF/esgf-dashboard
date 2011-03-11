/*	Author: University of Salento and CMCC */

function buildLayout(centerRegion) {
	Ext.QuickTips.init();
	Ext.form.Field.prototype.msgTarget = 'side';
	
	var viewport = new Ext.Viewport({
		layout: 'border',
		items: [
	        {
	        	layout: 'border',
	        	region: 'center',
	        	autoScroll: true,
	        	width: '88%',
	        	frame: true,
	        	items: [ northPanel, centerRegion ]
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
	
	viewport.on('resize', function() { viewport.doLayout(false, true); });
}
