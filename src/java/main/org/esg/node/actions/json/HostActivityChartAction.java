package org.esg.node.actions.json;

import org.esg.node.utils.*;
import java.sql.*;

import com.opensymphony.xwork2.ActionSupport;
import org.esg.node.chart.*;
import java.util.*;

import org.esg.node.chart.axis.*;
import org.esg.node.chart.element.*;
import org.esg.node.chart.util.*;

/**
 * @author University of Salento and CMCC
 */
public class HostActivityChartAction extends ActionSupport {
	private static final long serialVersionUID = 1L;

	private HorizontalBarChart horizontalBarChart = null;
	private Integer idServer = null;
	private Integer idProject = null;
	private Integer start = null;
	private Integer limit = null;
	private Integer timeSpan = null;
	private Integer numServices = null;
	private Long endDate = null;
	private Calendar endDateTime = null;
	
	@Override
	public void validate() {
		if(idServer == null || numServices == null) { 
			this.addActionError("");
			return;
		}
		if(timeSpan == null || timeSpan <= 0)
			timeSpan = Integer.parseInt(getText("default.timespan"));
		if(start == null)
			start = 0;
		if(limit == null)
			limit = numServices;
		endDateTime = Calendar.getInstance();
		if(endDate == null)		
			return;	
		try {
			endDateTime.clear();
			endDateTime.setTimeInMillis(endDate*1000l);
		} catch(Exception e) {
			this.addActionError("");
			return;
		}
	}
	
	@Override
	public String execute() throws Exception{
		Connection conn = null;
		PreparedStatement cStmt = null;
		String query;
		
		String[] date = new String[2];
		try {
			conn = Constants.DATASOURCE.getConnection();
			//CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_HOST_ACTIVITY_CHART.getSql());
			//if(idProject == null)
			//	cStmt.setNull(1, Types.INTEGER);
			//else
			//	cStmt.setInt(1, idProject);
			//cStmt.setInt(2, idServer);
			DateFormat formatter = new DateFormat(getLocale().getLanguage());
			Calendar c = (Calendar) (endDateTime==null? Calendar.getInstance(): endDateTime.clone());
			Calendar cs =(Calendar) (endDateTime==null? Calendar.getInstance(): endDateTime.clone());
			cs.add(Calendar.MINUTE, -timeSpan);
			//System.out.println("Timespan Minutes: " + timeSpan);
			//System.out.println("GettimeMillis "+ cs.getTimeInMillis() + " " + c.getTimeInMillis());
			
			date[1] = formatter.formatDateTime(c);
			
			//cStmt.setTimestamp(4, new Timestamp(c.getTimeInMillis()));
			
			
			date[0] = formatter.formatDateTime(cs);
			//cStmt.setTimestamp(3, new Timestamp(c.getTimeInMillis()));
			
			//cStmt.setInt(5, start);
			//cStmt.setInt(6, limit);
			
			query = "SELECT s.id, s.name, 100* (  SELECT COUNT(*)  FROM service_status WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "') / ( SELECT COUNT(*) FROM service_status WHERE idServiceInstance=s.id AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis()); 
			
			if(idProject == null)
				query = query + "') AS percentage FROM service_instance s WHERE s.idHost=";
			else
				query = query + "') AS percentage FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id WHERE s.idHost=";
			
			query = query + idServer;
			
			if (idProject == null)
				query = query + " ORDER BY s.name LIMIT " + limit;
			else
				query = query + " AND u.idProject=" + idProject + " AND u.endDate IS NULL ORDER BY s.name LIMIT " + limit;
				
			//System.out.println("HostActivityChartAction ="+ query);
			cStmt = conn.prepareStatement(query);
			
			//boolean hadResults = cStmt.execute();
			ResultSet rs = cStmt.executeQuery();
			
			horizontalBarChart = new HorizontalBarChart();
			List<ElementHBar> elements = new LinkedList<ElementHBar>();
			ElementHBar element = new ElementHBar();
			List<HBar> values = new LinkedList<HBar>();
			YAxis yAxis = new YAxis(null, null, 1, true);
			yAxis.setGrid_colour(getText("chart.defaultGridColor"));
			ArrayDeque<String> labels = new ArrayDeque<String>();
			//while(hadResults) {
			//	ResultSet rs = cStmt.getResultSet();
			while(rs.next()) {
					labels.addFirst(rs.getString("name"));
					HBar hBar = new HBar(0, rs.getBigDecimal("percentage"));
					values.add(hBar);
					if(rs.getBigDecimal("percentage") == null) {
						hBar.setRight(0);
						hBar.setTooltip(getText("hostActivityHChart.element.tooltip.unknown"));
						continue; 
					}
					double x = rs.getBigDecimal("percentage").doubleValue();
					String colour = null;
					int value = (int) (5.1*x);
					String hex;
					if(x > 50) {
						hex = Integer.toHexString(510-value);
						if(hex.length() == 1) hex = "0" + hex;
						colour = "#" + hex + "ff";
					}
					else {
						hex = Integer.toHexString(value);
						if(hex.length() == 1) hex = "0" + hex;
						colour = "#ff" + hex;
					}
					hBar.setColour(colour + "00");
				} //endwhile
			//	hadResults = cStmt.getMoreResults();
			//}
			cStmt.close();
			element.setTooltip("#val#%");
			element.setAlpha(1);
			element.setColour("#00ff00");
			yAxis.setLabels(labels);
			horizontalBarChart.setY_axis(yAxis);
			element.setValues(values);
			elements.add(element);
			horizontalBarChart.setElements(elements);
		} catch(SQLException e) {
			//System.out.println("Error Message HostActivityCharAction  " + e.getMessage());
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		horizontalBarChart.setTitle(new Title(getText("hostActivityHChart.title.text", date), 
				"{" + getText("hostActivityHChart.title.style") + "}"));
		horizontalBarChart.setX_legend(new Text(getText("hostActivityHChart.xLegend.text"), "{font-size: 10px; color: #000000}"));
		XAxis xAxis = new XAxis(0, 100, 10, true);
		xAxis.setGrid_colour(getText("chart.defaultGridColor"));
		xAxis.setTick_height(2);
		xAxis.setColour("#000000");
		horizontalBarChart.setX_axis(xAxis);
		// xAxis.setLabels(labels);
		horizontalBarChart.setBg_colour(getText("chart.defaultBgColor"));
		
		return SUCCESS;
	}

	public HorizontalBarChart getHorizontalBarChart() {
		return horizontalBarChart;
	}

	public void setHorizontalBarChart(HorizontalBarChart horizontalBarChart) {
		this.horizontalBarChart = horizontalBarChart;
	}

	public Integer getIdServer() {
		return idServer;
	}

	public void setIdServer(Integer idServer) {
		this.idServer = idServer;
	}

	public void setIdProject(Integer idProject) {
		this.idProject = idProject;
	}

	public Integer getIdProject() {
		return idProject;
	}

	public Integer getStart() {
		return start;
	}

	public void setStart(Integer start) {
		this.start = start;
	}

	public Integer getLimit() {
		return limit;
	}

	public void setLimit(Integer limit) {
		this.limit = limit;
	}

	public Integer getTimeSpan() {
		return timeSpan;
	}

	public void setTimeSpan(Integer timeSpan) {
		this.timeSpan = timeSpan;
	}

	public void setNumServices(Integer numServices) {
		this.numServices = numServices;
	}

	public Integer getNumServices() {
		return numServices;
	}

	public void setEndDate(Long endDate) {
		this.endDate = endDate;
	}

	public Long getEndDate() {
		return endDate;
	}
	
}
