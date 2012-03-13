/* Author: University of Salento and CMCC */

// Only for IE
function ricarica(element, data) {
	var swf = document.getElementById(element);
	swf.load(data);
}

function buildServiceTab(tabTitle, loadingMsg, descriptionPanelTitle, mapPanelTitle, servicesGridPanelTitle, mapPanelHelp, descriptionPanelHelp, gridPanelHelp) {
	var tabs = Ext.getCmp('tab-panel');
	Ext.form.Field.prototype.msgTarget = 'side';
	if(tabProjectOn)
		loadAllProject();
	tabs.add({
		title: '<span class="titleHostTab">' + tabTitle + '</span>',
		//iconCls: 'treeProjectYesIcon',
        frame: true,
        autoScroll: true,
        items: [
            new Ext.Panel({
            	layout: "border",
            	width: "100%",
            	height: 360,
            	items:[
		    		new Ext.Panel({			
						frame: true,
						width: "64%",
						title: mapPanelTitle,
						region: "center",
						height: 500,
						layout: "fit",
						contentEl: 'containerMap',
						tools: [{        			
							id: "help",
							qtip: "Help",
							handler: function(event, toolEl, panel, tc) {
								openHelpWindow(toolEl, mapPanelHelp);
			        		}        			
		        		}]
					}), new Ext.Panel({
						frame: true,
		    			width: "36%",
		    			//height: 400,
		    			title: descriptionPanelTitle,
		    			region: "east",
		    			layout: "fit",
		    			contentEl: 'content',
		    			tools: [{        			
							id: "help",
							qtip: "Help",
							handler: function(event, toolEl, panel, tc) {
		    					openHelpWindow(toolEl, descriptionPanelHelp);
			        		}        			
		        		}]
		    		})
		    	]
		    }), 
		    new Ext.Panel({
    			id: "serverGridPanel",
    			region: "south",
            	height: 260,
            	margins: '3 0 3 3',
			    cmargins: '3 3 3 3',
	        	frame: true,	        	
	            title: '', //servicesGridPanelTitle,
	        	collapsible: false,	        	
	        	contentEl: 'pagingGrid',
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
	
	var tabProject = Ext.getCmp("tabProject");
	tabs.on('tabchange', function tabChangeListener(tabPanel, tab) {
		if(tabProject) { 
			if(tab==tabProject && Ext.getCmp("gridPanel")) {
				tabProject = null;
				Ext.getCmp("gridPanel").doLayout(false, true);
				setTimeout('loadProjectMap(); Ext.getCmp("projectMapPanel").doLayout(false, true);', 500);
			}
		}
		if(tab.get(1).getActiveTab && tab.get(1).getActiveTab() == null) {
			tab.get(1).setActiveTab(0);		
		}
	});
	
	for(var i = 0, j=2; i < services.length; i ++) {		
		tabs.add(services[i]);
		
		Ext.getCmp('checkboxActivityChartGrid' + i).on("toggle", eval("onActivityCheck"+i));
		Ext.getCmp('checkboxRttChartGrid' + i).on("toggle", toggleRttChart);
				
		swfobject.embedSWF("../flash/ofc.swf", "graficoTorta"+i, "100%", "350", "9.0.0", "../flash/expressInstall.swf",  
			{ "get-data": "get_data_pie", "id": i, "loading": loadingMsg }, { 
				allowScriptAccess: "always",
	            bgcolor: "#ffffff",
	            wmode: "transparent" // opaque
	        }, false
		);
		swfobject.embedSWF("../flash/ofc.swf", "grafico1Torta"+i, "100%", "350", "9.0.0", "../flash/expressInstall.swf",  
			{ "get-data": "get_data_pie_off", "id": i, "loading": loadingMsg }, { 
				allowScriptAccess: "always",
	            bgcolor: "#ffffff",
	            wmode: "transparent" // opaque
	        }, false
		);
		swfobject.embedSWF("../flash/ofc.swf", "graficoAttivita"+i, "100%", "350", "9.0.0", "../flash/expressInstall.swf",
			{ "get-data": "get_data_activity", "id": i, "loading": loadingMsg }, { 
				allowScriptAccess: "always",
	            bgcolor: "#ffffff",
	            wmode: "transparent" // opaque
	        }, false
		);	
		swfobject.embedSWF("../flash/ofc.swf", "graficoRTT"+i, "100%", "350", "9.0.0", "../flash/expressInstall.swf",
			{ "get-data": "get_data_rtt", "id": i, "loading": loadingMsg }, { 
				allowScriptAccess: "always",
	            bgcolor: "#ffffff",
	            wmode: "transparent" // opaque
	        }, false
		);		
		services[i].items[1].on('tabchange', function(tabPanel, tab) {
			var index = tab.panelIndex;
			switch(tab.panelEl) {
				case "dashboard":
				{
					if(Ext.getCmp("dash_gridActivity"+index))
						Ext.getCmp("dash_gridActivity"+index).syncSize();
					if(Ext.getCmp("dash_gridRTT"+index))
						Ext.getCmp("dash_gridRTT"+index).syncSize();
					Ext.getCmp("tabGrafici"+index).get(0).doLayout(true, true);
					break;					
				}				
				case "pie":
				{
					if(Ext.isIE)
						var t = setTimeout("ricarica('graficoTorta" + index + "', '" + get_data_pie(index) + "')", 50);
					Ext.getCmp("tabGrafici"+index).get(1).doLayout(false, true);
					break;
				}
				case "pieOff":
				{
					if(Ext.isIE)
						var t = setTimeout("ricarica('grafico1Torta" + index + "', '" + get_data_pie_off(index) + "')", 50);
					Ext.getCmp("tabGrafici"+index).get(2).doLayout(false, true);
					break;
				}
				case "activity":
				{
					if(Ext.isIE)
						var t = setTimeout("ricarica('graficoAttivita" + index + "', '" + get_data_activity(index) + "')", 50);
					Ext.getCmp("tabGrafici"+index).get(3).doLayout(false, true);
					break;
				}
				case "rtt":
				{
					if(Ext.isIE)
						var t = setTimeout("ricarica('graficoRTT" + index + "', '" + get_data_rtt(index) + "')", 50);
					Ext.getCmp("tabGrafici"+index).get(4).doLayout(false, true);
					break;
				}
			}
		});	
	}
	var treePanel = Ext.getCmp('tree-panel');
	if(treePanel)
		treePanel.loader.on('load', selectHost);
}

function redirectToHost(idHost) {
	if(idHost != idServer*1) {
		document.getElementById('idServerClicked').value = idHost;
		document.getElementById('serverForm').submit();
	} else 
		Ext.getCmp("tab-panel").setActiveTab(1);	
}

function selectHost(This, node, response) {
	var parent = Ext.getCmp('tree-panel').root.findChild('myObject', idProject, false);
	parent.expand();
	var nodo = parent.findChild('myObject', idServer, false);
	nodo.select();
	nodo.setText("<b>" + nodo.attributes.text + "</b>");
}

var serverMap;
function createHostMap(lat, lng, name, activity) {
	var latlng = new google.maps.LatLng(lat, lng);
	var myOptions = {
		zoom: 12,
		center: latlng,
		scrollwheel: false,
		mapTypeId: google.maps.MapTypeId.ROADMAP
	};
	serverMap = new google.maps.Map(document.getElementById("server_map_canvas"), myOptions);
	var marker = new google.maps.Marker({
	    position: latlng,
	    map: serverMap,
		title: name,
	    icon: chooseIcon(activity)
	});
	
	google.maps.event.addListener(marker, 'click', function() {
		loadHostActivityChart(name);
	});		
}

/** START **/
/*function createEarthHostMap(lat, lng, name, activity) {
	initGE(lat, lng, name, activity);
}*/

function createEarthHostMap(lat, lng, name, activity) {
	ret=initGE(lat, lng, name, activity);
	return ret;
}
/** END **/

// return true if there are not valid controls
function checkForm(x) {
	var controlli = [ 'dataInizio', 'oraInizio', 'dataFine', 'oraFine' ];
	for(var i = 0; i < controlli.length; i ++)
		if(! Ext.getCmp(controlli[i]+x).isValid())
			return true;
	return false;
}

function getParams(x) {
	return {
		idServiceInstance : idServices[x],
		startDate : Ext.util.Format.date(Ext.getCmp("dataInizio" + x).getValue()), // Ext.util.Format.date(startDateTime), 
		startTime: Ext.getCmp("oraInizio" + x).getValue(), // Ext.util.Format.date(startDateTime, "H:i"),
		endDate: Ext.util.Format.date(Ext.getCmp("dataFine" + x).getValue()), // Ext.util.Format.date(endDateTime), 
		endTime: Ext.getCmp("oraFine" + x).getValue(), // Ext.util.Format.date(endDateTime, "H:i"),
		spansNumber: 13,
		onOff: activityDiagramSettings[x].onOff
	};
}

function getParamsString(x) {
	return Ext.urlEncode(getParams(x)); 
}
