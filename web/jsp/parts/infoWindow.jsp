<%-- AUTHOR: University of Salento and CMCC --%>
<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>


<script type="text/javascript">
//<![CDATA[
    function buildWindowContent(prefix) {
		var infoWindow = document.getElementById(prefix+"infoWindow");
		var infoWindowContent = document.createElement("div");
		infoWindowContent.setAttribute("id", prefix+"infoWindowContent");
		infoWindow.appendChild(infoWindowContent);
		var graficoAttivita = document.createElement("div");
		graficoAttivita.setAttribute("id", prefix+"graficoAttivita");
		var divActivityChart = document.createElement("div");
		divActivityChart.setAttribute("id", prefix+"divActivityChart");
		divActivityChart.appendChild(graficoAttivita);
		infoWindowContent.appendChild(divActivityChart);
	}
 	
	function openWindow(prefix, pages, idServer, numServices, title) {
		var html = prefix==""? String.format('<select id="{0}pageSelector" onchange="reloadChart(this.value, {1}, {2});">', prefix, idServer, numServices):
			String.format('<select id="{0}pageSelector" onchange="reloadHostActivityChart(this.value, {1});">', prefix, numServices);
		for(var i = 0; i < pages; i ++)
			html += String.format('<option value="{0}">{1}</option>', i, i+1);
		html += '</select>';

		var select = prefix==""? String.format('<select id="{0}radio_timeSpan" onchange="reloadChart(document.getElementById(\'{1}\').value, {2}, {3});">', prefix, prefix+"pageSelector", idServer, numServices):
			String.format('<select id="{0}radio_timeSpan" onchange="reloadHostActivityChart(document.getElementById(\'{1}\').value, {2});">', prefix, prefix+"pageSelector", numServices);
		var spans = [ 5, 60, 1440, 10080, 43200 ];
		var span_labels = [ '<s:text name="server.form.timeSpans.span1" />', 
		                     '<s:text name="server.form.timeSpans.span2" />',
		                     '<s:text name="server.form.timeSpans.span3" />',
		                     '<s:text name="server.form.timeSpans.span4" />',
		                     '<s:text name="server.form.timeSpans.span5" />' ];
		for(var i = 0; i < spans.length; i ++)
			select += String.format('<option value="{0}" {1}>{2}</option>', spans[i], i==0? "checked": "", span_labels[i]);
		select += '</select>';
		
		return new Ext.Window({
			title: String.format('<s:text name="infoWindow.window.title" />', title),
			autoHeight: true,
			width: 600,
			resizable: true,
			autoscroll: true,
			items: [
		        new Ext.Panel({
		        	frame: true,
		        	border: false,
		        	contentEl: prefix+"infoWindowContent"
		        }),
		        new Ext.Panel({
		        	layout: "hbox",		        	
					layoutConfig: {
						pack: 'start',
	                    align: 'top'		                    	                        
	                },
	                frame: true,
		        	items: [
				        new Ext.Panel({
				        	width: 450,			        	
					        frame: false,
					        items: [
							    new Ext.Panel({
							    	width: 200,
									height: 35,	
									html: '<p>Time Interval: ' + select + '</p>'
								})
								/*,
								new Ext.form.RadioGroup({									
									id: prefix+"radio_timeSpan",
									columns: 2,
									defaults: {
										handler: function(checkbox, checked) {
											if(prefix=="")
												reloadChart(document.getElementById(prefix+"pageSelector").value, idServer, numServices);
											else
												reloadHostActivityChart(document.getElementById(prefix+"pageSelector").value, numServices);
										}
									},
									autoWidth: true,
									//autoHeight: true,							
									items: [
										new Ext.form.Radio({									
											boxLabel: '<s:text name="server.form.timeSpans.span1" />',
											name: "timeSpan",
											inputValue: 5,
											checked: true
										}),
										new Ext.form.Radio({
											boxLabel: '<s:text name="server.form.timeSpans.span2" />',
											name: "timeSpan",
											inputValue: 60							
										}),new Ext.form.Radio({
											boxLabel: '<s:text name="server.form.timeSpans.span3" />',
											name: "timeSpan",
											inputValue: 1440							
										}),
										new Ext.form.Radio({
											boxLabel: '<s:text name="server.form.timeSpans.span4" />',
											name: "timeSpan",
											inputValue: 10080							
										}),
										new Ext.form.Radio({
											boxLabel: '<s:text name="server.form.timeSpans.span5" />',
											name: "timeSpan",
											inputValue: 43200							
										})						
									]
								}),*/	
							]
				        }),
						new Ext.Panel({
							width: 140,
							height: 35,							
							html: '<p><s:text name="infoWindow.pageSelector.label" />: ' + html + '</p>'
						})
		        	]	        	
		        })
			]		
		});
	}

	function HomeControl(controlDiv, map, bounds) {
		controlDiv.style.padding = '5px';
		
		var controlUI = document.createElement('DIV');
		controlUI.style.backgroundColor = 'white';
		controlUI.style.borderStyle = 'solid';
		controlUI.style.borderWidth = '2px';
		controlUI.style.cursor = 'pointer';
		controlUI.style.textAlign = 'center';
		controlUI.title = '<s:text name="infoWindow.homeControl.controlUI.title" />';
		controlDiv.appendChild(controlUI);
		
		// Set CSS for the control interior
		var controlText = document.createElement('DIV');
		controlText.style.fontFamily = 'Arial,sans-serif';
		controlText.style.fontSize = '12px';
		controlText.style.paddingLeft = '4px';
		controlText.style.paddingRight = '4px';
		controlText.innerHTML = '<s:text name="infoWindow.homeControl.controlText" />';
		controlUI.appendChild(controlText);
		
		google.maps.event.addDomListener(controlUI, 'click', function() {
			map.fitBounds(bounds);
		});
	}
//]]>
</script>
