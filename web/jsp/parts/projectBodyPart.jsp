<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<%@ taglib prefix="s" uri="/struts-tags" %>

<%
	String parameter = request.getParameter("mini"); 
	boolean mini = parameter!=null && parameter.equals("1");
	String iframe = request.getParameter("iframe");
%>

<div id="containerAvailabilityMap" class="x-hide-display">
	<div id="map_canvas_availability" style="height: 500;"></div>
</div>

<div id="containerUsersMap" class="x-hide-display">
	<div id="map_canvas_users" style="height: 500;"></div>
</div>

<div id="containerNodeTypeMap" class="x-hide-display">
	<div id="map_canvas_nodetype" style="height: 500;"></div>
</div>



<div id="projectContent" class="x-hide-display">
	<table align="left" cellspacing="5" width="100%" height="214">
		<tbody>
			<tr>
				<td style="text-align: left">
					<p>
						<span style="font-weight: bold;"><s:text name="project.startDate" /></span>
					</p>
				</td>								
				<td>
					<p>
						<s:property value="project.startDate"/> <s:property value="project.startTime" />
					</p>
				</td>
				<td style="text-align: left;">
					<p>
						<span style="font-weight: bold;"><s:text name="project.endDate" /> </span>
					</p>
				</td>
				<td>
					<p>
						<s:if test="project.endDate==null">
							<s:text name="project.endDate.defaultValue" />
						</s:if>
						<s:else>
							<s:property value="project.endDate" />&nbsp;<s:property value="project.endTime" default="" />
						</s:else>						
					</p>
				</td>
			</tr>
			<tr>						
				<td><p style="font-weight: bold;"><s:text name="project.hostsNumber" /></p></td>
				<td><p><s:property value="project.hostsNumber" /></p></td>
				<td><p style="font-weight: bold;"><s:text name="project.servicesNumber" /></p></td>
				<td><p><s:property value="project.servicesNumber" /></p></td>
			</tr>
			<tr>
				<td><p style="font-weight: bold;"><s:text name="project.usersNumber" /></p></td>
				<td><p><s:property value="project.regUserNumber" /></p></td>
				<td style="text-align: left"><p style="font-weight: bold;">Time Interval</p></td>
				<td>
					<select onchange="reloadAvgProjectActivityCharts(this.value);">
						<option value="5">5 minutes</option>
						<option value="60">1 hour</option>
						<option value="1440">1 day</option>
						<option value="10080">1 week</option>
						<option value="43200">1 month</option>
					</select>
				</td>
			</tr>
			<tr>
				<td style="text-align: left">
					<p style="font-weight: bold;">
						<s:text name="project.permLink" />	
					</p>
				</td>
				<td>
					<p>
						<a href="#" onclick="showPermLink(); return false;">
							code
						</a>
					</p>
				</td>
			</tr>
			<tr>
				<td height="25px" colspan="4"></td>
			</tr>
			<tr>
				<td valign="top"><p style="font-weight: bold;"><s:text name="project.description" /></p></td>
				<td style="text-align: left; border: 1px solid #B5B8C8; height: 100%;" colspan="3" valign="top">
					<p style="text-align: justify; padding: 3px;">
						<script type="text/javascript">
						//<![CDATA[
							document.write('<s:property value="project.description" escape="false" escapeJavaScript="true" />');
						//]]>
						</script>
					</p>
				</td>
			</tr>		
		</tbody>
	</table>
</div>

<div id="containerWindowChart" class="x-hide-display">

	<div id="containerServicesDistributionPieChart">
		<div id="servicesDistributionPieChart"></div>						
	</div>

	<div id="containerUsersDistributionPieChart">
		<div id="usersDistributionPieChart"></div>						
	</div>
	
	<div id="containerAvgProjectActivityPieChart">
		<div id="avgProjectActivityPieChart"></div>
	</div>

	<div id="containerNodetypeDistributionPieChart">
		<div id="nodetypeDistributionPieChart"></div>						
	</div>
	
	<div id="containerAvgProjectActivityBarChartOff">
		<div id="avgProjectActivityBarChartOff"></div>
	</div>
		
</div>
<div id="infoWindow" style="display: none;"></div>

<div style="display: none;">
	<form method="post" id="serverForm" action='<%if(mini) { %><s:url action="ServerMini" namespace="/monitoring" encode="false"/> <% } else { %><s:url action="Server" namespace="/monitoring" encode="false"/><% } %>'>
		<s:hidden id="idServerClicked" name="idServer" />
		<s:hidden name="idProject" />
		<input type="hidden" name="iframe" value="<%=iframe %>" />
	</form>
</div>
