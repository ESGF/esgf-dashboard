package org.esg.node.actions.json;

import java.sql.*;
import java.util.*;

import org.esg.node.utils.*;
import org.esg.node.chart.*;
import org.esg.node.chart.axis.*;
import org.esg.node.chart.element.*;
import org.esg.node.chart.util.*;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class AvgProjectActivityPieChartAction extends ActionSupport {
	private static final long serialVersionUID = 1L;

	private Integer idProject = null;
	private Integer endDateTime = null;
	private Integer timeSpan = null;
	private Calendar cF = null;
	private PieChart pieChart = null;
	private BarChart barChart = null; 
	
	@Override
	public void validate() {
		if(idProject == null || endDateTime == null) {
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
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		Number activity = null;
		Calendar cI = (Calendar) cF.clone();
		cI.add(Calendar.MINUTE, -timeSpan);
		try {
			conn = Constants.DATASOURCE.getConnection();
			AvgProjectActivity avgProjectActivity = new AvgProjectActivity(conn);			
			activity = avgProjectActivity.getProjectActivity(conn,idProject, cI, cF);
			avgProjectActivity.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		
		
		pieChart = new PieChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		pieChart.setTitle(new Title(getText("avgProjectActivityPieChart.title.text", new String[] { formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
				"{" + getText("avgProjectActivityPieChart.title.style") + "}"));
	
		List<ElementPie> elements = new LinkedList<ElementPie>();
		ElementPie element = new ElementPie();
		element.setAlpha(0.7f);
		List<Slice> slices = new LinkedList<Slice>();
		if(activity != null) {
			double value = Math.round(activity.doubleValue()*100.)/100.;
			if(value != 0) {
				Slice slice = new Slice();				
				slice.setLabel(getText("avgProjectActivityPieChart.slice.available.text") + "\n" + value + "%");		
				slice.setValue(value);
				slice.setColour(getText("avgProjectActivityPieChart.slice.available.colour"));
				slice.setTooltip(getText("avgProjectActivityPieChart.slice.available.text") + "\n" + value + "%");
				slices.add(slice);
			}
			value = Math.round((100.-activity.doubleValue())*100.)/100.;
			if(value != 0) {
				Slice slice = new Slice();				
				slice.setLabel(getText("avgProjectActivityPieChart.slice.failure.text") + "\n" + value + "%");
				slice.setValue(value);
				slice.setColour(getText("avgProjectActivityPieChart.slice.failure.colour"));
				slice.setTooltip(getText("avgProjectActivityPieChart.slice.failure.text") + "\n" + value + "%");
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
		
		return SUCCESS;
	}

	public String execute1() throws Exception {
		Connection conn = null;
		Calendar cI = (Calendar) cF.clone();
		cI.add(Calendar.MINUTE, -timeSpan);
		ServiceStatus[] status = { ServiceStatus.INACTIVE, ServiceStatus.TIMEOUT, ServiceStatus.ERROR };
		Number[] values = new Number[status.length];
		Double total = 0.; 
		try {
			conn = Constants.DATASOURCE.getConnection();
			AvgProjectActivity avgProjectActivity = new AvgProjectActivity(conn);
			for(byte i = 0; i < status.length; i ++)
				if((values[i] = avgProjectActivity.getProjectStatus(conn,idProject, cI, cF, status[i].getValue())) != null) {
					total += values[i].doubleValue();
					//System.out.println("status "+ status[i].getValue() + "value "+ values[i].doubleValue());
				}
			avgProjectActivity.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		
		barChart = new BarChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		String absoluteTotal = String.valueOf(Math.round(total*100.)/100.);
		barChart.setTitle(new Title(getText("avgProjectActivityBarChartOff.title.text", new String[] { absoluteTotal, formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
			"{" + getText("avgProjectActivityBarChartOff.title.style") + "}"));
		
		String prefix = "avgProjectActivityBarChartOff.bar.";
		List<ElementBar> elements = new LinkedList<ElementBar>();
		List<Bar> bars = new LinkedList<Bar>();		
		for(byte i = 0; i < status.length; i ++) {
			Double absoluteValue = new Double(Math.round(100.*values[i].doubleValue())/100.);
			Double relativeValue = new Double(Math.round(10000.*values[i].doubleValue()/total)/100.);
			Bar bar = new Bar(relativeValue, 0, getText(status[i].getColor(prefix)), getText(status[i].getText(prefix)) + "\n" + relativeValue + "% [" + absoluteValue + "%]");
			bars.add(bar);
		}
		for(byte i = 0; i < status.length; i ++) {
			ElementBar element = new ElementBar();
			element.setValues(bars);
			element.setType(ElementBar.TYPE_BAR_ROUNDED_GLASS);
			element.setText(getText(status[i].getText(prefix)));
			element.setColour(getText(status[i].getColor(prefix)));
			elements.add(element);
			bars = null;
		}
		XAxis xAxis = new XAxis();
		xAxis.setGrid_colour(getText("chart.defaultBgColor"));
		Label labels = new Label();
		labels.setLabels(new String[] { "", "", "", "" });
		xAxis.setLabels(labels);
		xAxis.setTick_height(0);
		xAxis.setColour("#000000");
		barChart.setX_axis(xAxis);
		
		barChart.setElements(elements);
		barChart.setX_legend(new Text(" ", "{font-size: 20px;}"));
		barChart.setY_axis(getDefaultYAxis());
		
		barChart.setBg_colour(getText("chart.defaultBgColor"));		
		return SUCCESS;
	}
	
	private YAxis getDefaultYAxis() {
		YAxis y_axis = new YAxis(0, 100., 10., false);
		y_axis.setTick_length(2);
		y_axis.setGrid_colour(getText("chart.defaultGridColor"));
		y_axis.setColour("#000000");
		List<String> labels = new LinkedList<String>();
		for(byte i = 0; i <= 100; i++)
			if(i % 10 == 0) labels.add(String.valueOf(i));
			else labels.add("");
		y_axis.setLabels(labels);
		return y_axis;
	}
	
	public void setIdProject(Integer idProject) {
		this.idProject = idProject;
	}

	public Integer getIdProject() {
		return idProject;
	}

	public void setTimeSpan(Integer timeSpan) {
		this.timeSpan = timeSpan;
	}

	public Integer getTimeSpan() {
		return timeSpan;
	}

	public void setEndDateTime(Integer endDateTime) {
		this.endDateTime = endDateTime;
	}

	public Integer getEndDateTime() {
		return endDateTime;
	}

	public void setPieChart(PieChart pieChart) {
		this.pieChart = pieChart;
	}

	public PieChart getPieChart() {
		return pieChart;
	}

	public void setBarChart(BarChart barChart) {
		this.barChart = barChart;
	}

	public BarChart getBarChart() {
		return barChart;
	}
	
}
