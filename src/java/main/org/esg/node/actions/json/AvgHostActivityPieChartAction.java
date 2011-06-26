package org.esg.node.actions.json;

import org.esg.node.beans.AvgHostActivityBean;
import org.esg.node.chart.PieChart;
import org.esg.node.chart.element.ElementPie;
import org.esg.node.chart.element.Slice;
import org.esg.node.chart.util.Title;
import java.sql.*;
import org.esg.node.utils.*;
import java.util.*;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class AvgHostActivityPieChartAction extends ActionSupport {
	private static final long serialVersionUID = 1L;

	private AvgHostActivityBean bean = null;
	private Integer idServer = null;
	private Integer idProject = null;
	private Integer timeSpan = null;
	private Integer endDateTime = null;
	private Calendar cF = null;
	
	@Override
	public void validate() {
		if(idProject == null || idServer == null || endDateTime == null) {
			addActionError("");
			return;
		}
		try {
			cF = Calendar.getInstance();
			cF.clear();
			cF.setTimeInMillis(endDateTime*1000l);
		} catch(Exception e) {
			addActionError("");
			return;
		}
		
		if(timeSpan == null || timeSpan <= 0)
			timeSpan = Integer.parseInt(getText("default.timespan"));
		//System.out.println("NEW CONF --> Timespan "+ timeSpan);
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		Number activity = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			AvgHostActivity avgHostActivity = new AvgHostActivity(conn);
			activity = avgHostActivity.getHostActivity(conn,idProject, idServer, timeSpan, cF);
			avgHostActivity.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		bean = new AvgHostActivityBean();
		PieChart pieChart = new PieChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		Calendar cI = (Calendar) cF.clone();
		cI.add(Calendar.MINUTE, -timeSpan);
		pieChart.setTitle(new Title(getText("avgHostActivityPieChart.title.text", new String[] { formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
				"{" + getText("avgHostActivityPieChart.title.style") + "}"));
	
		List<ElementPie> elements = new LinkedList<ElementPie>();
		ElementPie element = new ElementPie();
		element.setAlpha(0.7f);
		List<Slice> slices = new LinkedList<Slice>();
		if(activity != null) {
			double value = Math.round(activity.doubleValue()*100.)/100.;
			bean.setActivity(value);
			if(value != 0) {
				Slice slice = new Slice();				
				slice.setLabel(getText("avgHostActivityPieChart.slice.available.text") + "\n" + value + "%");		
				slice.setValue(value);
				slice.setColour(getText("avgHostActivityPieChart.slice.available.colour"));
				slice.setTooltip(getText("avgHostActivityPieChart.slice.available.text") + "\n" + value + "%");
				slices.add(slice);
			}
			value = Math.round((100.-activity.doubleValue())*100.)/100.;
			if(value != 0) {
				Slice slice = new Slice();				
				slice.setLabel(getText("avgHostActivityPieChart.slice.failure.text") + "\n" + value + "%");
				slice.setValue(value);
				slice.setColour(getText("avgHostActivityPieChart.slice.failure.colour"));
				slice.setTooltip(getText("avgHostActivityPieChart.slice.failure.text") + "\n" + value + "%");
				slices.add(slice);
			}
		}
		element.setValues(slices);
		element.setGradientFill(true);
		element.setStartAngle(0);
		element.setRadius(50);
		elements.add(element);
		pieChart.setBg_colour(getText("chart.defaultBgColor"));
		pieChart.setBorder(1);
		pieChart.setAnimate(true);
		pieChart.setElements(elements);
		bean.setPieChart(pieChart);
		return SUCCESS;
	}
	
	public Integer getIdServer() {
		return idServer;
	}

	public void setIdServer(Integer idServer) {
		this.idServer = idServer;
	}

	public Integer getIdProject() {
		return idProject;
	}

	public void setIdProject(Integer idProject) {
		this.idProject = idProject;
	}

	public Integer getTimeSpan() {
		return timeSpan;
	}

	public void setTimeSpan(Integer timeSpan) {
		this.timeSpan = timeSpan;
	}

	public Integer getEndDateTime() {
		return endDateTime;
	}

	public void setEndDateTime(Integer endDateTime) {
		this.endDateTime = endDateTime;
	}

	public void setBean(AvgHostActivityBean bean) {
		this.bean = bean;
	}

	public AvgHostActivityBean getBean() {
		return bean;
	}	
}
