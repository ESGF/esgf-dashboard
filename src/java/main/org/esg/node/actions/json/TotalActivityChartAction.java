package org.esg.node.actions.json;

import java.sql.*;
import java.util.*;
import org.esg.node.actions.base.ChartBaseAction;
import org.esg.node.utils.*;
import org.esg.node.chart.*;
import org.esg.node.chart.axis.XAxis;
import org.esg.node.chart.element.*;
import org.esg.node.chart.element.animation.SliceAnimation;
import org.esg.node.chart.util.*;

/**
 * @author University of Salento and CMCC
 */
public class TotalActivityChartAction extends ChartBaseAction {
	private static final long serialVersionUID = 1L;
	private PieChart pieChart = null;
	private BarChart barChart = null;
	private long absoluteTotal = 0L;
	
	private long[] getValues() throws Exception {
		Connection conn = null;
		long[] values = new long[Constants.STATI_LENGTH];
		Arrays.fill(values, 0L);
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.STATUS_QUERY.getSql());
			stmt.setInt(1, super.idServiceInstance);
			stmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			stmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			
			//System.out.println("STATUS_QUERY ="+stmt.toString());
			ResultSet rs = stmt.executeQuery();
			while(rs.next()) {
				for(byte i = 0; i < Constants.STATI_LENGTH; i ++)
					if(Constants.STATI[i].getValue() == rs.getInt("status")) {
						absoluteTotal += (values[i] = rs.getLong("hitCount"));
						break;
					}
			}
			rs.close();
			stmt.close();
		} catch(SQLException e) { 
			return null; 
		}
		finally {
			if(conn != null) conn.close();
		}
		return values;
	}
	
	@Override
	public String execute() throws Exception {
		long[] values = getValues();
		if(values == null) return ERROR;
		if(absoluteTotal == 0L) absoluteTotal = 1; // this avoid division by zero
		
		pieChart = new PieChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		pieChart.setTitle(new Title(getText("pieChart.title.text", new String[] { formatter.formatDateTime(super.cI), formatter.formatDateTime(super.cF) }), 
				"{" + getText("pieChart.title.style") + "}"));
	
		List<ElementPie> elements = new LinkedList<ElementPie>();
		ElementPie element = new ElementPie();
		element.setAlpha(0.7f);
		List<Slice> slices = new LinkedList<Slice>();
		Long sum = 0l;
		for(byte i = 0; i < Constants.STATI_LENGTH; i ++) {
			if(Constants.STATI[i] == ServiceStatus.ACTIVE) {
				if(values[i] == 0) continue;
				Slice slice = new Slice();
				Double n = Math.round(10000.*values[i]/absoluteTotal)/100.;
				slice.setValue(values[i]);
				slice.setLabel(getText("pieChart.slice.available.text") + "\n" + n + "%");
				slice.setTooltip(getText("pieChart.slice.available.text") + "\n" + n + "%");
				slice.setColour(getText("pieChart.slice.available.colour"));				
				slices.add(slice);
			} else sum += values[i];
		}
		if(sum != 0) {
			Slice slice = new Slice();
			slice.setValue(sum);
			Double n = new Double(Math.round(10000.*sum/absoluteTotal)/100.);
			slice.setLabel(getText("pieChart.slice.failure.text", "Service Failure") + "\n" + n + "%");
			slice.setTooltip(getText("pieChart.slice.failure.text", "Service Failure") + "\n" + n + "%");
			slice.setColour(getText("pieChart.slice.failure.colour"));
			// slice.setOnClick("gotoPieChartOffTab()");
			slices.add(slice);
		}
		element.setValues(slices);
		element.setStartAngle(0);
		pieChart.setAnimate(true);
		SliceAnimation[] animations = null;
		if(slices.size() > 1) {
			animations = new SliceAnimation[2];
			animations[1] = new SliceAnimation();
			animations[1].setType(SliceAnimation.TYPE_BOUNCE);
			animations[1].setDistance(30);
		}
		else
			animations = new SliceAnimation[1];
		animations[0] = new SliceAnimation();
		animations[0].setType(SliceAnimation.TYPE_FADE);
		element.setAnimate(animations);
		elements.add(element);
		pieChart.setBg_colour(getText("chart.defaultBgColor"));
		element.setGradientFill(true);
		pieChart.setBorder(1);
		// element.setNoLabels(true);
		pieChart.setElements(elements);
		return SUCCESS;
	}

	public String execute1() throws Exception {
		long[] values = getValues();
		if(values == null) return ERROR;
		if(absoluteTotal == 0L) absoluteTotal = 1; // this avoid division by zero
		
		barChart = new BarChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		barChart.setTitle(new Title(getText("barChart.title.text", new String[] { formatter.formatDateTime(super.cI), formatter.formatDateTime(super.cF) }), 
				"{" + getText("barChart.title.style") + "}"));
		List<ElementBar> elements = new LinkedList<ElementBar>();
		ElementBar element = new ElementBar();
		List<Bar> bars = new LinkedList<Bar>();
		Long sum = 0l;
		for(byte i = 0; i < Constants.STATI_LENGTH; i ++) {
			if(Constants.STATI[i] == ServiceStatus.ACTIVE) {
				Double n = Math.round(10000.*values[i]/absoluteTotal)/100.;
				Bar bar = new Bar(n, null, getText("barChart.element.available.colour"), getText("barChart.element.available.text") + "\n" + n + "%");
				bars.add(bar);
				element.setText(getText("barChart.element.available.text"));
				element.setType(ElementBar.TYPE_BAR_CYLINDER);
				element.setAlpha(.7);
				element.setColour(getText("barChart.element.available.colour"));
				element.setValues(bars);
				
			} else sum += values[i];
		}
		Double n = new Double(Math.round(10000.*sum/absoluteTotal)/100.);
		Bar bar = new Bar(n, null, getText("barChart.element.failure.colour"), getText("barChart.element.failure.text") + "\n" + n + "%");
		bars.add(null);
		bars.add(bar);
		bars.add(null);
		elements.add(element);
		element = new ElementBar();
		bars = new LinkedList<Bar>();
		element.setText(getText("barChart.element.failure.text"));
		element.setAlpha(.7);
		element.setColour(getText("barChart.element.failure.colour"));
		element.setType(ElementBar.TYPE_BAR_CYLINDER);
		element.setValues(bars);
		elements.add(element);
		barChart.setElements(elements);
		XAxis xAxis = new XAxis();
		xAxis.setGrid_colour(getText("chart.defaultBgColor"));
		Label labels = new Label();
		labels.setLabels(new String[] { "", "" });
		xAxis.setLabels(labels);
		xAxis.setTick_height(0);
		xAxis.setColour("#000000");
		barChart.setX_axis(xAxis);
		barChart.setY_axis(super.getDefaultYAxis());
		barChart.setX_legend(new Text(" ", "{font-size: 20px;}"));
		barChart.setBg_colour(getText("chart.defaultBgColor"));		
		return SUCCESS;
	}
	
	public String execute2() throws Exception {
		long[] values = getValues();
		if(values == null) return ERROR;
		if(absoluteTotal == 0L) absoluteTotal = 1; // this avoid division by zero
		long relativeTotal = absoluteTotal;
		for(byte i = 0; i < Constants.STATI_LENGTH; i ++)
			if(Constants.STATI[i] == ServiceStatus.ACTIVE) {
				relativeTotal -= values[i];
				break;
			}
		pieChart = new PieChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());		
		pieChart.setTitle(new Title(getText("pieChartOff.title.text", new String[] { String.valueOf(Math.round(10000.*relativeTotal/absoluteTotal)/100.), formatter.formatDateTime(super.cI), formatter.formatDateTime(super.cF) }), 
				"{" + getText("pieChartOff.title.style") + "}"));
	
		List<ElementPie> elements = new LinkedList<ElementPie>();
		ElementPie element = new ElementPie();
		element.setAlpha(0.7f);
		List<Slice> slices = new LinkedList<Slice>();		
		for(byte i = 0; i < Constants.STATI_LENGTH; i ++) {
			if(Constants.STATI[i] == ServiceStatus.ACTIVE || values[i] == 0) continue;
			Slice slice = new Slice();
			slice.setValue(values[i]);
			Double absoluteValue = new Double(Math.round(10000.*values[i]/absoluteTotal)/100.);
			Double relativeValue = new Double(Math.round(10000.*values[i]/relativeTotal)/100.);
			slice.setLabel(getText(Constants.STATI[i].getText()) + "\n" + relativeValue + "% [" + absoluteValue + "%]");
			slice.setTooltip(getText(Constants.STATI[i].getText()) + "\n" + relativeValue + "% [" + absoluteValue + "%]");//relativeValue + "%");
			slice.setColour(getText(Constants.STATI[i].getColor()));
			slices.add(slice);	
		}		
		element.setValues(slices);		
		element.setStartAngle(0);
		pieChart.setAnimate(true);
		SliceAnimation[] animations = null;
		if(slices.size() > 1) {
			animations = new SliceAnimation[2];
			animations[1] = new SliceAnimation();
			animations[1].setType(SliceAnimation.TYPE_BOUNCE);
			animations[1].setDistance(30);
		}
		else
			animations = new SliceAnimation[1];
		animations[0] = new SliceAnimation();
		animations[0].setType(SliceAnimation.TYPE_FADE);
		element.setAnimate(animations);
		elements.add(element);
		pieChart.setBg_colour(getText("chart.defaultBgColor"));
		element.setGradientFill(true);
		pieChart.setBorder(1);
		element.setNoLabels(false);
		pieChart.setElements(elements);
		return SUCCESS;
	}
	
	public String execute3() throws Exception {
		long[] values = getValues();
		if(values == null) return ERROR;
		if(absoluteTotal == 0L) absoluteTotal = 1; // this avoid division by zero
		
		long relativeTotal = absoluteTotal;
		for(byte i = 0; i < Constants.STATI_LENGTH; i ++)
			if(Constants.STATI[i] == ServiceStatus.ACTIVE) {
				relativeTotal -= values[i];
				break;
			}
		
		barChart = new BarChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		barChart.setTitle(new Title(getText("barChartOff.title.text", new String[] { String.valueOf(Math.round(10000.*relativeTotal/absoluteTotal)/100.), formatter.formatDateTime(super.cI), formatter.formatDateTime(super.cF) }), 
				"{" + getText("barChartOff.title.style") + "}"));
		List<ElementBar> elements = new LinkedList<ElementBar>();
		List<Bar> bars = new LinkedList<Bar>();		
		for(byte i = 0; i < Constants.STATI_LENGTH; i ++) {
			if(Constants.STATI[i] == ServiceStatus.ACTIVE) continue;
			Double absoluteValue = new Double(Math.round(10000.*values[i]/absoluteTotal)/100.);
			Double relativeValue = new Double(Math.round(10000.*values[i]/relativeTotal)/100.);
			Bar bar = new Bar(relativeValue, 0, getText(Constants.STATI[i].getColor()), getText(Constants.STATI[i].getText()) + "\n" + relativeValue + "% [" + absoluteValue + "%]");
			bars.add(bar);
		}		
		for(byte i = 0; i < Constants.STATI_LENGTH; i ++) {
			if(Constants.STATI[i] == ServiceStatus.ACTIVE) continue;
			ElementBar element = new ElementBar();
			element.setValues(bars);
			element.setType(ElementBar.TYPE_BAR_CYLINDER_OUTLINE);
			element.setText(getText(Constants.STATI[i].getText()));
			element.setColour(getText(Constants.STATI[i].getColor()));
			elements.add(element);
			bars = null;
		}
		barChart.setElements(elements);
		XAxis xAxis = new XAxis();
		xAxis.setGrid_colour(getText("chart.defaultBgColor"));
		Label labels = new Label();
		labels.setLabels(new String[] { "", "", "", "" });
		xAxis.setLabels(labels);
		xAxis.setTick_height(0);
		xAxis.setColour("#000000");
		barChart.setX_axis(xAxis);
		barChart.setX_legend(new Text(" ", "{font-size: 20px;}"));
		barChart.setY_axis(super.getDefaultYAxis());
		barChart.setBg_colour(getText("chart.defaultBgColor"));
		return SUCCESS;
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
