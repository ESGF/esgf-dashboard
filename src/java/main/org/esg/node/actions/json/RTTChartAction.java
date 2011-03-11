package org.esg.node.actions.json;

import java.sql.*;
import java.util.*;
import org.esg.node.beans.RTTAvgAndStdDev;
import org.esg.node.actions.base.ChartBaseAction;
import org.esg.node.utils.*;
import org.esg.node.chart.*;
import org.esg.node.chart.axis.*;
import org.esg.node.chart.element.*;
import org.esg.node.chart.element.animation.*;
import org.esg.node.chart.util.*;

/**
 * @author University of Salento and CMCC
 */
public class RTTChartAction extends ChartBaseAction {
	private static final long serialVersionUID = 1L;
	private BarChart barChart = null;
	private LineChart lineChart = null;
	private AreaChart areaChart = null;
	private Long step = null;
	private double max = 0.;
	private RTTAvgAndStdDev rttAvgAndStdDev = null;
	
	private void evalMax() {
		if(max == 0) { max = 1000; return; }

		for(int i = Constants.RTT_MAX; i >= 1; i/=10) {
			int val = (int) max/i;
			if(val != 0) {
				max = (val + (max%i==0? 0: 1)) * i / 1000.;
				break;
			}
		}
	}
	
	private RTTAvgAndStdDev getRTTAvgAndStdDev(Connection conn) throws Exception {
		RTTAvgAndStdDevHelper helper = new RTTAvgAndStdDevHelper(conn);
		return helper.compute(cI, cF, idServiceInstance);
	}
	
	// BarChart
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		MyBarsList values = new MyBarsList();
		try {
			conn = Constants.DATASOURCE.getConnection();
			step = (super.cF.getTimeInMillis() - super.cI.getTimeInMillis())/1000;
			step =  step / super.spansNumber + (step%super.spansNumber == 0L? 0: 1); // arrotondo sempre per eccesso
			
			CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_RTT_CHART.getSql());
			cStmt.setInt(1, super.idServiceInstance);
			cStmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			cStmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			cStmt.setLong(4, step);

			boolean hadResults = cStmt.execute();
			while(hadResults) {
				ResultSet rs = cStmt.getResultSet();
				values.add(null);
				if(rs.next() && rs.getBigDecimal("avgRtt") != null) {
					double val = rs.getBigDecimal("avgRtt").doubleValue();
					values.add(new Bar(val/1000., null, null, null));
					if(val > max) max = val;
				}
				else values.add(new Bar(0., null, null, getText("chart.defaultUnknownValue")));			
				hadResults = cStmt.getMoreResults(); 
			}
			cStmt.close();
			rttAvgAndStdDev = getRTTAvgAndStdDev(conn);	
		} catch(Exception e) {e.printStackTrace();
			return ERROR; 
		}
		finally {
			if(conn != null) conn.close();
		}
			
		barChart = new BarChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		barChart.setTitle(new Title(getText("rttChart.title.text", new String[] { formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
			"{" + getText("rttChart.title.style") + "}"));
		
		BarAnimation animation = new BarAnimation(BarAnimation.TYPE_FADE_IN);
		animation.setCascade(2);
		animation.setDelay(0);
		
		List<ElementBar> elements = new LinkedList<ElementBar>();
		ElementBar element = new ElementBar();
		element.setAlpha(0.7f);
		element.setType(ElementBar.TYPE_BAR_CYLINDER);
		element.setColour(getText("rttChart.element.colour"));
		String avg = rttAvgAndStdDev==null || rttAvgAndStdDev.getAvg()==null? "-": String.valueOf(Math.round(rttAvgAndStdDev.getAvg().doubleValue()/10.) / 100.);
		String devStd = rttAvgAndStdDev==null || rttAvgAndStdDev.getStdDev()==null? "-": String.valueOf(Math.round(rttAvgAndStdDev.getStdDev().doubleValue()/10.) / 100.);
		element.setText(getText("rttChart.element.text", new String[] { avg, devStd }));
		element.setFontSize(12);
		element.setValues(values); 
		element.setTooltip("#val# msec");
		element.setOn_show(animation);
		elements.add(element);
		barChart.setElements(elements);
		barChart.setX_legend(new Text(getText("rttChart.xLegend.text"), "{font-size: 10px; color: #000000}"));
		barChart.setY_legend(new Text("", "{font-size: 10px; color: #000000}"));
		XAxis x_axis = new XAxis(0, super.spansNumber*2+1, 1, false);
		x_axis.setColour("#000000");
		//x_axis.setThreed(2);
		//x_axis.setStroke(3);
		x_axis.setTick_height(2);
		x_axis.setGrid_colour(getText("chart.defaultGridColor"));
		evalMax();
		YAxis y_axis = new YAxis(0, max, max/10, false);
		y_axis.setGrid_colour(getText("chart.defaultGridColor"));
		y_axis.setColour("#000000");
		y_axis.setTick_length(2);
		barChart.setY_axis(y_axis);
		Label label = new Label();
		label.setSteps(2l);//step);
		//l.setVisible_steps(8);
		label.setVisible(true);
		String []labels = new String[super.spansNumber.intValue() * 2 + 2];
		Calendar c = Calendar.getInstance();
		c.setTimeInMillis(super.cI.getTimeInMillis());
		for(int index = 0; index <= super.spansNumber; index ++) {
			labels[index*2] = formatter.formatDateTime(c, "\n", step<60);
			labels[index*2+1] = "";
			c.add(Calendar.SECOND, step.intValue());
		}
		label.setLabels(labels);
		label.setRotate(270);
		label.setSize(12);
		x_axis.setLabels(label);
		barChart.setX_axis(x_axis);
		barChart.setBg_colour(getText("chart.defaultBgColor"));
		
		return SUCCESS;
	}
	
	private MyDotsList getDots() throws Exception {
		Connection conn = null;
		MyDotsList values = new MyDotsList();

		try {
			conn = Constants.DATASOURCE.getConnection();
			step = (super.cF.getTimeInMillis() - super.cI.getTimeInMillis())/1000;
			step =  step / super.spansNumber + (step%super.spansNumber == 0L? 0: 1); // arrotondo sempre per eccesso
			
			CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_RTT_CHART.getSql());
			cStmt.setInt(1, super.idServiceInstance);
			cStmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			cStmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			cStmt.setLong(4, step);
			
			Calendar c = Calendar.getInstance();
			c.setTimeInMillis(super.cI.getTimeInMillis());
			c.add(Calendar.SECOND, step.intValue()/2);
			
			boolean hadResults = cStmt.execute();
			while(hadResults) {
				ResultSet rs = cStmt.getResultSet();
				Dot dot = new Dot();
				
				dot.setX(c.getTimeInMillis()/1000);
				if(rs.next() && rs.getBigDecimal("avgRtt") != null) {
					double val = rs.getBigDecimal("avgRtt").doubleValue();
					dot.setY(val/1000.);
					if(val > max) max = val;
				}
				else { dot.setY(0); dot.setTooltip(getText("chart.defaultUnknownValue")); }
				values.add(dot);
				c.add(Calendar.SECOND, step.intValue());
				hadResults = cStmt.getMoreResults(); 
			}
			cStmt.close();
			rttAvgAndStdDev = getRTTAvgAndStdDev(conn);
		} catch(SQLException e) { 
			throw new Exception(); 
		}
		finally {
			if(conn != null) conn.close();
		}
		evalMax();
		return values;
	}

	// LineChart
	public String execute1() throws Exception {
		MyDotsList values;
		try {
			values = getDots();
			if(values == null) return SUCCESS;
		} catch(Exception e) {
			return ERROR;
		}
		
		lineChart = new LineChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		lineChart.setTitle(new Title(getText("rttChart.title.text", new String[] { formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
			"{" + getText("rttChart.title.style") + "}"));
		
		LineStyle lineStyle = new LineStyle();
		lineStyle.setStyle(LineStyle.STYLE_SOLID);
		LineAnimation animation = new LineAnimation(LineAnimation.TYPE_EXPLODE);
		animation.setDelay(0);
		animation.setCascade(2);
		
		List<ElementLine> elements = new LinkedList<ElementLine>();
		DotStyle ds = new DotStyle();
		ds.setType(DotStyle.TYPE_SOLID_DOT);
		ds.setWidth(2);
		ds.setDotSize(2);
		ds.setColour("#000000");
		ds.setTooltip("#val# msec");
		ElementLine element = new ElementLine();
		element.setValues(values);
		element.setDotStyle(ds);
		element.setLineStyle(lineStyle);
		element.setOn_show(animation);
		element.setColour(getText("rttChart.element.colour"));
		String avg = rttAvgAndStdDev==null || rttAvgAndStdDev.getAvg()==null? "-": String.valueOf(Math.round(rttAvgAndStdDev.getAvg().doubleValue()/10.) / 100.);
		String devStd = rttAvgAndStdDev==null || rttAvgAndStdDev.getStdDev()==null? "-": String.valueOf(Math.round(rttAvgAndStdDev.getStdDev().doubleValue()/10.) / 100.);
		element.setText(getText("rttChart.element.text", new String[] { avg, devStd }));
		element.setFontSize(12);
		elements.add(element);
		
		lineChart.setX_legend(new Text(getText("rttChart.xLegend.text"), "{font-size: 10px; color: #000000}"));
		lineChart.setY_legend(new Text("", "{font-size: 10px; color: #000000}"));
		//lineStyle.setOn(2);
		//lineStyle.setOff(3);	
		lineChart.setElements(elements);
		
		XAxis x_axis = new XAxis(super.cI.getTimeInMillis()/1000L, super.cF.getTimeInMillis()/1000L+step/2, step/2, false);
		x_axis.setColour("#000000");
		//x_axis.setStroke(3);
		x_axis.setTick_height(2);
		x_axis.setGrid_colour(getText("chart.defaultGridColor"));
		YAxis y_axis = new YAxis(0, max, max/10, false);
		y_axis.setTick_length(2);
		y_axis.setGrid_colour(getText("chart.defaultGridColor"));
		y_axis.setColour("#000000");
		lineChart.setY_axis(y_axis);
		Label l = new Label();
		l.setSteps(step);
		//l.setVisible_steps(8);
		l.setVisible(true);
		//x_axis.setThreed(1);
		l.setRotate(270);
		l.setSize(12);
		l.setText("#date:" + formatter.getShortDateFormat() + "\n" + formatter.getShortTimeFormat(step<60) + "#");
		x_axis.setLabels(l);
		lineChart.setX_axis(x_axis);
		lineChart.setBg_colour(getText("chart.defaultBgColor"));		
		
		return SUCCESS;
	}
	
	// AreaChart
	public String execute2() throws Exception {
		MyDotsList values;
		try {
			values = getDots();
			if(values == null) return SUCCESS;
		} catch(Exception e) {
			return ERROR;
		}
		
		areaChart = new AreaChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		areaChart.setTitle(new Title(getText("rttChart.title.text", new String[] { formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
			"{" + getText("rttChart.title.style") + "}"));
		
		List<ElementArea> elements = new LinkedList<ElementArea>();
		LineStyle lineStyle = new LineStyle();
		lineStyle.setStyle(LineStyle.STYLE_SOLID);
		LineAnimation animation = new LineAnimation(LineAnimation.TYPE_EXPLODE);
		animation.setDelay(0);
		animation.setCascade(2);
		
		DotStyle ds = new DotStyle();
		ds.setType(DotStyle.TYPE_SOLID_DOT);
		ds.setWidth(2);
		ds.setDotSize(2);
		ds.setColour("#000000");
		ds.setTooltip("#val# msec");
		ElementArea element = new ElementArea();
		element.setValues(values);
		element.setAlpha(0.9);
		element.setFillAlpha(0.5);
		element.setFillColour(getText("rttChart.element.fillColour"));
		element.setDotStyle(ds);
		element.setLineStyle(lineStyle);
		element.setOn_show(animation);
		element.setColour(getText("rttChart.element.colour"));
		String avg = rttAvgAndStdDev==null || rttAvgAndStdDev.getAvg()==null? "-": String.valueOf(Math.round(rttAvgAndStdDev.getAvg().doubleValue()/10.) / 100.);
		String devStd = rttAvgAndStdDev==null || rttAvgAndStdDev.getStdDev()==null? "-": String.valueOf(Math.round(rttAvgAndStdDev.getStdDev().doubleValue()/10.) / 100.);
		element.setText(getText("rttChart.element.text", new String[] { avg, devStd }));
		element.setFontSize(12);
		elements.add(element);
		
		areaChart.setElements(elements);
		areaChart.setX_legend(new Text(getText("rttChart.xLegend.text"), "{font-size: 10px; color: #000000}"));
		areaChart.setY_legend(new Text("", "{font-size: 10px; color: #000000}"));
		//lineStyle.setOn(2);
		//lineStyle.setOff(3);	
		
		XAxis x_axis = new XAxis(super.cI.getTimeInMillis()/1000L, super.cF.getTimeInMillis()/1000L+step/2, step/2, false);
		x_axis.setColour("#000000");
		//x_axis.setStroke(3);
		x_axis.setTick_height(2);
		x_axis.setGrid_colour(getText("chart.defaultGridColor"));
		YAxis y_axis = new YAxis(0, max, max/10, false);
		y_axis.setTick_length(2);
		y_axis.setGrid_colour(getText("chart.defaultGridColor"));
		y_axis.setColour("#000000");
		areaChart.setY_axis(y_axis);
		Label l = new Label();
		l.setSteps(step);
		//l.setVisible_steps(8);
		l.setVisible(true);
		//x_axis.setThreed(1);
		l.setRotate(270);
		l.setSize(12);
		l.setText("#date:" + formatter.getShortDateFormat() + "\n" + formatter.getShortTimeFormat(step<60) + "#");
		x_axis.setLabels(l);
		areaChart.setX_axis(x_axis);
		areaChart.setBg_colour(getText("chart.defaultBgColor"));	
		return SUCCESS;
	}
	
	public BarChart getBarChart() {
		return barChart;
	}

	public void setBarChart(BarChart barChart) {
		this.barChart = barChart;
	}

	public LineChart getLineChart() {
		return lineChart;
	}

	public void setLineChart(LineChart lineChart) {
		this.lineChart = lineChart;
	}

	public AreaChart getAreaChart() {
		return areaChart;
	}

	public void setAreaChart(AreaChart areaChart) {
		this.areaChart = areaChart;
	}
}
