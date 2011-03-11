<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<script type="text/javascript">
//<![CDATA[
	function changeStartDate(val, metaData, record, rowIndex, colIndex, store) {
		return val + " " + record.get("startTime");
	}

	function changeEndDate(val, metaData, record, rowIndex, colIndex, store) {
		return val + " " + record.get("endTime");
	}

	function createActivityGrid(x, id, store) {
		return new Ext.grid.GridPanel({
			id: id,
			title: '<s:text name="server.serviceActivityGrid.title" />',
			height: 260,
			boxMinHeight: 250,
			store: store,
	        bodyCssClass: "grid",
	        trackMouseOver: true,
	        disableSelection: false,
	        loadMask: true,
	        columns: [
				{
				    header: '<s:text name="server.serviceActivityGrid.columns.startDateTime" />',
				    dataIndex: "startDate",
				    sortable: true,
				    renderer: changeStartDate
				},
				{
				    header: '<s:text name="server.serviceActivityGrid.columns.endDateTime" />',
				    dataIndex: "endDate",
				    sortable: true,
				    renderer: changeEndDate
				},
				{
				    header: '<s:text name="server.serviceActivityGrid.columns.activity" />',
				    dataIndex: "values[0]",
				    sortable: true,
				    renderer: changePercentage
				},
				{
				    header: '<s:text name="server.serviceActivityGrid.columns.inactivity" />',
				    dataIndex: "values[1]",
				    sortable: true,
				    renderer: changeComplementaryPercentage
				}, {
				    header: '<s:text name="server.serviceActivityGrid.columns.timeout" />',
				    dataIndex: "values[2]",
				    sortable: true,
				    renderer: changeComplementaryPercentage
				}, {
				    header: '<s:text name="server.serviceActivityGrid.columns.errors" />',
				    dataIndex: "values[3]",
				    sortable: true,
				    renderer: changeComplementaryPercentage
				}
		    ],
	        bbar: new Ext.PagingToolbar({
	            pageSize: <s:text name="server.serviceActivityGrid.limit" />,
	            store: store,	            
	            displayInfo: true,
	            displayMsg: '<s:text name="server.serviceActivityGrid.displayMsg" />',
	            emptyMsg: '<s:text name="server.serviceActivityGrid.emptyMsg" />',
	            items:[
	                '-', {
	                	text: '<s:text name="grid.button.exportXML" />',
	                	handler: exportAsXML,
	                	panelURL: 0,
	                	panelIndex: x	                                
	                }, {
	                	text: '<s:text name="grid.button.exportCSV" />',
	                	handler: exportAsCSV,
	                	panelURL: 0,
	                	panelIndex: x 
	                }
	            ]
	        }),
	        stripeRows: true,
	        viewConfig: {
	            forceFit: true
			}
		});		
	}

	function createActivityStore(x) {
		var url = '<s:url action="ActivityTable" namespace="/json" encode="false" />?';
		return new Ext.data.JsonStore({
	        root: 'activityRecords',
	        totalProperty: 'totalCount',
	        fields: [
				'startDate', 'startTime', 'endDate', 'endTime', 
				'values[0]', 'values[1]', 'values[2]', 'values[3]'
	        ],
	        proxy: new Ext.data.HttpProxy({
	            url: url + getParamsString(x)
	        })
	    });
	}

	function loadActivityGrid(x) {		
		swfobject.removeSWF('graficoAttivita'+x);
		var div = document.createElement("div");
		div.setAttribute("id", "graficoAttivita" + x);
		document.getElementById("divActivityChart" + x).appendChild(div);

		var store = createActivityStore(x);
		var grid = createActivityGrid(x, "gridActivity"+x, store);
		grid.render("graficoAttivita"+x);
		grid.syncSize();
		store.load({params:{start:0, limit:<s:text name="server.serviceActivityGrid.limit" />}});		
		if(widgets[x].indexOf(getActivityWidget) != -1) {
			swfobject.removeSWF('dash_graficoAttivita'+x);
			div = document.createElement("div");
			div.setAttribute("id", 'dash_graficoAttivita' + x);
			document.getElementById("dash_divActivityChart" + x).appendChild(div);			
			grid = createActivityGrid(x, "dash_gridActivity"+x, store);
			grid.setWidth((Ext.getCmp("tabGrafici"+x).getWidth()-12)*(Ext.getCmp('spinner_activity'+x).getValue()*1)/100.);
			grid.render('dash_graficoAttivita'+x);
			Ext.getCmp('dash_tablePanel'+x).doLayout(false, true);
		}		
	}

	function updateDashActivityGrid(x) {
		var store = Ext.getCmp("gridActivity"+x).getStore();
		var grid = createActivityGrid(x, "dash_gridActivity"+x, store);
		store.load({params:{start:0, limit:<s:text name="server.serviceActivityGrid.limit" />}});
		grid.setWidth((Ext.getCmp("tabGrafici"+x).getWidth()-12)*(Ext.getCmp('spinner_activity'+x).getValue()*1)/100.);
		grid.render('dash_graficoAttivita'+x);
	}

	function reloadActivityGrid(index) {
		var store = createActivityStore(index);
		var grid = Ext.getCmp("gridActivity"+index);
		grid.reconfigure(store, grid.getColumnModel());
		grid.getBottomToolbar().bindStore(store);
		store.load({params:{start:0, limit:<s:text name="server.serviceRttGrid.limit" />}});
		if(widgets[index].indexOf(getActivityWidget) != -1) {
			grid = Ext.getCmp("dash_gridActivity"+index);
			grid.reconfigure(store, grid.getColumnModel());
			grid.getBottomToolbar().bindStore(store);			
			Ext.getCmp('dash_tablePanel'+index).doLayout(false, true);
		}		
	}
	
	function exportAsCSV(button, eventObject) {
		var url = [ '<s:url action="ActivityTableCSV" namespace="/stream" encode="false" />',
		            '<s:url action="RTTTableCSV" namespace="/stream" encode="false" />' 
		          ];
		window.open(url[button.panelURL]+"?"+getParamsString(button.panelIndex));				
	}
	
	function exportAsXML(button, eventObject) {
		var url = [
           {
               xmlTree: '<s:url action="ActivityTableXMLTree" namespace="/xsl" encode="false" />',
               xml: '<s:url action="ActivityTableXML" namespace="/xsl" encode="false" />'
	       },
	       {
               xmlTree: '<s:url action="RTTTableXMLTree" namespace="/xsl" encode="false" />',
               xml: '<s:url action="RTTTableXML" namespace="/xsl" encode="false" />'
	       }
		]
		window.open(url[button.panelURL].xml + '?' + getParamsString(button.panelIndex));
	}

	function createRttGrid(x, id, store) {
		return new Ext.grid.GridPanel({
			id: id,
			title: '<s:text name="server.serviceRttGrid.title" />',
			height: 260,
			boxMinHeight: 250,
			store: store,
	        bodyCssClass: "grid",
	        trackMouseOver: true,
	        disableSelection: false,
	        loadMask: true,
	        columns: [
				{
				    header: '<s:text name="server.serviceRttGrid.columns.startDateTime" />',
				    dataIndex: "startDate",
				    sortable: true,
				    renderer: changeStartDate
				},
				{
				    header: '<s:text name="server.serviceRttGrid.columns.endDateTime" />',
				    dataIndex: "endDate",
				    sortable: true,
				    renderer: changeEndDate
				},
				{
				    header: '<s:text name="server.serviceRttGrid.columns.value" />',
				    dataIndex: "values[0]",
				    sortable: true
				}
		    ],
	        bbar: new Ext.PagingToolbar({
	            pageSize: <s:text name="server.serviceRttGrid.limit" />,
	            store: store,
	            displayInfo: true,
	            displayMsg: '<s:text name="server.serviceRttGrid.displayMsg" />',
	            emptyMsg: '<s:text name="server.serviceRttGrid.emptyMsg" />',
	            items:[
	                '-', {
	                	text: '<s:text name="grid.button.exportXML" />',
	                	handler: exportAsXML,
	                	panelURL: 1,
	                	panelIndex: x
	                }, {
	                	text: '<s:text name="grid.button.exportCSV" />',
	                	panelURL: 1,
	                	panelIndex: x,
	                	handler: exportAsCSV
	                }
	            ]
	        }),
	        stripeRows: true,	        
	        viewConfig: {
	            forceFit: true
			}
		});
	}

	function createRttStore(x) {
		var url = '<s:url action="RTTTable" namespace="/json" encode="false" />?';
		return new Ext.data.JsonStore({
	        root: 'activityRecords',
	        totalProperty: 'totalCount',
	        fields: [
				'startDate', 'startTime', 'endDate', 'endTime', 'values[0]' 
	        ],
	        proxy: new Ext.data.HttpProxy({
	            url: url + getParamsString(x)
	        })
	    });
	}
	
	function loadRttGrid(x) {
		swfobject.removeSWF('graficoRTT'+x);
		var div = document.createElement("div");
		div.setAttribute("id", 'graficoRTT'+x);
		document.getElementById("divRTTChart"+x).appendChild(div);
		
		var store = createRttStore(x);
		var grid = createRttGrid(x, "gridRTT"+x, store);
		grid.render('graficoRTT'+x);
		grid.syncSize();
		store.load({params:{start:0, limit:<s:text name="server.serviceRttGrid.limit" />}});
		
		if(widgets[x].indexOf(getRttWidget) != -1) {
			swfobject.removeSWF('dash_graficoRTT'+x);
			div = document.createElement("div");
			div.setAttribute("id", "dash_graficoRTT"+x);
			document.getElementById("dash_divRTTChart"+x).appendChild(div);

			grid = createRttGrid(x, "dash_gridRTT"+x, store);
			grid.setWidth((Ext.getCmp("tabGrafici"+x).getWidth()-12)*Ext.getCmp('spinner_rtt'+x).getValue()/100.);
			grid.render('dash_graficoRTT'+x);		
			Ext.getCmp('dash_tablePanel'+x).doLayout(false, true);
		}
	}

	function updateDashRttGrid(x) {
		var store = Ext.getCmp("gridRTT"+x).getStore();
		var grid = createRttGrid(x, "dash_gridRTT"+x, store);
		store.load({params:{start:0, limit:<s:text name="server.serviceRttGrid.limit" />}});
		grid.setWidth((Ext.getCmp("tabGrafici"+x).getWidth()-12)*Ext.getCmp('spinner_rtt'+x).getValue()/100.);
		grid.render('dash_graficoRTT'+x);	
	}
	
	function reloadRttGrid(index) {
		var store = createRttStore(index);
		var grid = Ext.getCmp("gridRTT"+index);
		grid.reconfigure(store, grid.getColumnModel());
		grid.getBottomToolbar().bindStore(store);
		store.load({params:{start:0, limit:<s:text name="server.serviceRttGrid.limit" />}});
		if(widgets[index].indexOf(getRttWidget) != -1) {
			grid = Ext.getCmp("dash_gridRTT"+index);
			grid.reconfigure(store, grid.getColumnModel());
			grid.getBottomToolbar().bindStore(store);		
			Ext.getCmp('dash_tablePanel'+index).doLayout(false, true);
		}	
	}
//]]>
</script>
