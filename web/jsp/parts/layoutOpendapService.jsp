<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<script type="text/javascript">
//<![CDATA[
	function buildOpendapLayout(index) {
		Ext.Ajax.request({
			url: '<s:url action="URL" namespace="/opendap" encode="false" />',
			success: function(response, opts) {
				var data = JSON.parse(response.responseText);
				var containerCharts = document.getElementById('containerCharts');
		    	var extDiv = document.createElement("div");
				extDiv.setAttribute("id", "divOpendap"+index);
				containerCharts.appendChild(extDiv);
				extDiv.innerHTML = '<iframe id="iframe_opendap'+index+'" width="100%" height="100%" frameborder="0" marginheight="0" marginwidth="0" src="' + data.url + '"></iframe>';

				Ext.getCmp("tabGrafici"+index).add({
					title: '<span class="titleNoDashboardSpecialTab">' + 'Data Browser' + '</span>',
					frame: true,
			        autoScroll: true,
			        margins: '3 0 3 3',
		            cmargins: '3 3 3 3',
		            height: 600,
		            contentEl: "divOpendap"+index,
		            tbar: new Ext.Toolbar({
						items: [
							new Ext.Button({
								iconCls: 'restoreDashBoardViewIcon',
								tooltip: '<s:text name="server.toolbar.button.dashboardReturn" />',
								handler: onRestore,
								myParams: {
									index: index
								}
							}),	new Ext.Button({
								iconCls: 'homeIcon',
								tooltip: 'Home',
								handler: function() {
									document.getElementById("iframe_opendap"+index).setAttribute("src", data.url);					
								}	
							}), '->',
							new Ext.Button({
								iconCls: "helpButton",
								tooltip: 'Help',
								handler: function(button, eventObject) {
			    					openHelpWindow(button, '<s:text name="help.server.dashboard.opendapBrowser" />');									
								}					
				    		})
						]
		            })
				});
				/*
				Ext.getCmp("tabGrafici"+index).on("tabchange", function(tabPanel, tab) {
					if(tab == Ext.getCmp("tabGrafici"+index).get(6))
						document.getElementById("divOpendap"+index).appendChild(document.getElementById("iframe_opendap"+index));
					else if(tab == Ext.getCmp("tabGrafici"+index).get(0) && widgets[index].indexOf(getOpendapWidget) != -1)
						document.getElementById("dash_divIntOpendap"+index).appendChild(document.getElementById("iframe_opendap"+index));
				});	*/
			},
			failure: onFailure,
			params: {
				idNode: idServices[index]
			}
		});	
	}
	/*
	function getOpendapWidget(index) {
		var prefix = "dash_";
		var span = Ext.getCmp('spinner_opendap'+index).getValue()*1;

		if(document.getElementById(prefix + "divExtOpendap" + index) == null) {
			createContainerChartsElement(prefix + "divExtOpendap" + index, prefix + "divIntOpendap" + index);
			
			if(document.getElementById("iframe_opendap"+index) != null) {
				document.getElementById(prefix + "divIntOpendap" + index).appendChild(document.getElementById("iframe_opendap"+index));			
			}
		}
		return {
			id: prefix+"panel_opendap"+index,
			span: span,
			autoScroll: false,
			contentEl: prefix+"divExtOpendap"+index,
			panelEl: 6
		};
	}*/
//]]>
</script>

