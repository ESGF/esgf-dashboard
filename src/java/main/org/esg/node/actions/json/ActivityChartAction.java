package org.esg.node.actions.json;

import java.sql.*;
import java.util.*;

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

public class ActivityChartAction extends ChartBaseAction {
	private static final long serialVersionUID = 1L;
	
	private BarChart barChart = null;
	private StackChart stackChart = null;
	private LineChart lineChart = null;
	private AreaChart areaChart = null;
	private Long step = null;
	private Long labelStep = null;
	private Boolean onOff = null;
	private ServiceStatus[] STATI = null;
	//private String QUERY = null;
	private String TYPEQUERY = null;
	
	@Override
	public void validate() {
		super.validate();
		if(onOff != null && onOff) {
			STATI = Constants.STATI_ON_OFF;
			TYPEQUERY = "SP_ACTIVITY_CHART_ON_OFF";
		} else {
			STATI = Constants.STATI;
			TYPEQUERY = "SP_ACTIVITY_CHART";
		}
	}
	
	// build a bar chart
	@Override
	public String execute() throws Exception {		
		Connection conn = null;
		PreparedStatement cStmt = null;
		String query;
		String query2;
		
		MyBarsList values[] = new MyBarsList[STATI.length];
		for(byte i = 0; i < STATI.length; i ++)
			values[i] = new MyBarsList();
		try {
			conn = Constants.DATASOURCE.getConnection();	
			step = (super.cF.getTimeInMillis() - super.cI.getTimeInMillis())/1000;
			step =  step / super.spansNumber + (step%super.spansNumber == 0L? 0: 1); // round up
			//CallableStatement cStmt = conn.prepareCall(QUERY);
			
			//cStmt.setInt(1, super.idServiceInstance);
			//cStmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			//cStmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			//cStmt.setLong(4, step);

			//boolean hadResults = cStmt.execute();
			//while(hadResults) 
			Calendar c = (Calendar) (super.cI.clone());
			
		if (step>0) {		
			while ((c.getTimeInMillis()) < (super.cF.getTimeInMillis())) {
					  
				//ResultSet rs = cStmt.getResultSet();
				query="";
				query2="";
				if (TYPEQUERY=="SP_ACTIVITY_CHART") {
					query = "SELECT status*1 as status, count(*) as hitCount FROM  esgf_dashboard.service_status WHERE idServiceInstance=";
					query = query + super.idServiceInstance;
					query = query + " AND timestamp between '";
					query = query + new Timestamp(c.getTimeInMillis()) + "' AND '";
					c.add(Calendar.SECOND, step.intValue());
					query = query + new Timestamp(c.getTimeInMillis()) + "' GROUP BY status";
				}
				if (TYPEQUERY=="SP_ACTIVITY_CHART_ON_OFF") {
					query = "(SELECT 1 as status, count(*) as hitCount FROM  esgf_dashboard.service_status WHERE idServiceInstance=";
					query2= "(SELECT -1 as status, count(*) as hitCount FROM  esgf_dashboard.service_status WHERE idServiceInstance=";
					query = query + super.idServiceInstance;
					query2 = query2 + super.idServiceInstance;
					query = query + " AND timestamp between '";
					query2= query2 + " AND timestamp between '";
					query = query + new Timestamp(c.getTimeInMillis()) + "' AND '";
					query2 = query2 + new Timestamp(c.getTimeInMillis()) + "' AND '";
					c.add(Calendar.SECOND, step.intValue());
					query = query + new Timestamp(c.getTimeInMillis()) + "' AND status=1) UNION ";
					query2 = query2 + new Timestamp(c.getTimeInMillis()) + "' AND status<>1)";
					query = query+query2;
				}
				//System.out.println(TYPEQUERY + "|-> ActivityChartAction Query = "+ query);
				
			   	//SELECT `status`*1 as status, count(*) as hitCount FROM service_status WHERE idServiceInstance=id AND `timestamp` between position AND position+INTERVAL step second GROUP BY `status`;
			   	
			   	     /* (
                        SELECT 1 as status, count(*) as hitCount FROM service_status
                        WHERE idServiceInstance=id AND `timestamp` between position AND position+INTERVAL step second
                        AND status=1
                      )
                      UNION
                      (
                        SELECT -1 as status, count(*) as hitCount FROM service_status
                        WHERE idServiceInstance=id AND `timestamp` between position AND position+INTERVAL step second
                        AND status<>1
                      );*/
			   			
				cStmt = conn.prepareStatement(query);
				ResultSet rs = cStmt.executeQuery();
				
				long totale = 0L;
				Bar[] barre = new Bar[STATI.length];
				for(byte i = 0; i < STATI.length; i ++) 
					barre[i] = new Bar(0L, null, null, null);

				while(rs.next()) {
					totale += rs.getLong("hitCount");
					for(byte i = 0; i < STATI.length; i ++)
						if(STATI[i].getValue() == rs.getInt("status")) {
							barre[i].setTop(rs.getLong("hitCount"));
							break;
						}
				}
				for(byte i = 0; i < STATI.length; i ++) {
					if(totale != 0)
						barre[i].setTop(Math.round(barre[i].getTop().longValue()*10000./totale)/100.);
					else
						barre[i].setTooltip(getText("chart.defaultUnknownValue"));
					values[i].add(null);
					values[i].add(barre[i]);
				}
				//hadResults = cStmt.getMoreResults(); 
			} //endwhile
		} //endif
			cStmt.close();
		} catch(SQLException e) { 
			return ERROR; 
		} finally {
			if(conn != null) conn.close();
		}
		
		barChart = new BarChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		barChart.setTitle(new Title(getText("activityChart.title.text", new String[] { formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
			"{" + getText("activityChart.title.style") + "}"));
		
		BarAnimation animation = new BarAnimation(BarAnimation.TYPE_GROW_UP);
		animation.setCascade(3);
		animation.setDelay(0);
		
		List<ElementBar> elements = new LinkedList<ElementBar>();
		for(byte i = 0; i < STATI.length; i ++) {
			ElementBar element = new ElementBar();
			element.setAlpha(0.7f);
			element.setColour(getText(STATI[i].getColor()));
			element.setType(ElementBar.TYPE_BAR_CYLINDER_OUTLINE);
			element.setText(getText(STATI[i].getText()));
			element.setFontSize(12);
			element.setValues(values[i]); 
			element.setTooltip(getText(STATI[i].getText())+"<br>#val#%");
			element.setOn_show(animation);
			if(step >= 60)
				element.setOn_click("zoomActivity");
			elements.add(element);
		}
		barChart.setElements(elements);
		barChart.setX_legend(new Text(getText("activityChart.xLegend.text"), "{font-size: 10px; color: #000000}"));
		barChart.setY_legend(new Text("", "{font-size: 10px; color: #000000}"));
		XAxis x_axis = new XAxis(0, super.spansNumber*2+1, 1, false);
		x_axis.setColour("#000000");
		//x_axis.setThreed(2);
		//x_axis.setStroke(3);
		x_axis.setTick_height(2);
		x_axis.setGrid_colour(getText("chart.defaultGridColor"));
		
		barChart.setY_axis(super.getDefaultYAxis());
		Label label = new Label();
		label.setSteps(2l);
		//l.setVisible_steps(8);
		label.setVisible(true);
		String []labels = new String[super.spansNumber * 2 + 2];
		Calendar c = (Calendar) cI.clone();		
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

	private MyDotsList[] getDots() throws Exception {
		Connection conn = null;
		PreparedStatement cStmt = null;
		String query;
		String query2;
		MyDotsList[] values = new MyDotsList[STATI.length];
		try {
			conn = Constants.DATASOURCE.getConnection();
			
			step = (super.cF.getTimeInMillis() - super.cI.getTimeInMillis())/1000;
			step =  step / super.spansNumber + (step%super.spansNumber == 0L? 0: 1); // arrotondo sempre per eccesso
			// labelStep = step * dots_number / MAX_LABEL;
			labelStep = step;
			
			for(byte i = 0; i < STATI.length; i ++)
				values[i] = new MyDotsList();

			//CallableStatement cStmt = conn.prepareCall(QUERY);
			//cStmt.setInt(1, super.idServiceInstance);
			//cStmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			//cStmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			//cStmt.setLong(4, step);
			
			Calendar c = (Calendar) cI.clone();
			c.add(Calendar.SECOND, step.intValue()/2);
			
			//boolean hadResults = cStmt.execute();
			int index = 1;
			
			if (step>0) {		
				while (c.getTimeInMillis() < super.cF.getTimeInMillis()) {
						  
					//ResultSet rs = cStmt.getResultSet();
					query="";
					query2="";
					if (TYPEQUERY=="SP_ACTIVITY_CHART") {
						query = "SELECT status*1 as status, count(*) as hitCount FROM  esgf_dashboard.service_status WHERE idServiceInstance=";
						query = query + super.idServiceInstance;
						query = query + " AND timestamp between '";
						query = query + new Timestamp(c.getTimeInMillis()) + "' AND '";
						c.add(Calendar.SECOND, step.intValue());
						query = query + new Timestamp(c.getTimeInMillis()) + "' GROUP BY status";
					}
					if (TYPEQUERY=="SP_ACTIVITY_CHART_ON_OFF") {
						query = "(SELECT 1 as status, count(*) as hitCount FROM  esgf_dashboard.service_status WHERE idServiceInstance=";
						query2= "(SELECT -1 as status, count(*) as hitCount FROM  esgf_dashboard.service_status WHERE idServiceInstance=";
						query = query + super.idServiceInstance;
						query2 = query2 + super.idServiceInstance;
						query = query + " AND timestamp between '";
						query2= query2 + " AND timestamp between '";
						query = query + new Timestamp(c.getTimeInMillis()) + "' AND '";
						query2 = query2 + new Timestamp(c.getTimeInMillis()) + "' AND '";
						c.add(Calendar.SECOND, step.intValue());
						query = query + new Timestamp(c.getTimeInMillis()) + "' AND status=1) UNION ";
						query2 = query2 + new Timestamp(c.getTimeInMillis()) + "' AND status<>1)";
						query = query+query2;
					}
					//System.out.println(TYPEQUERY + "|-> ActivityChartAction Query = "+ query);
				   			
					cStmt = conn.prepareStatement(query);
					ResultSet rs = cStmt.executeQuery();
			
			
			//while(hadResults) {
				//ResultSet rs = cStmt.getResultSet();
				long totale = 0L;
				
				Dot[] punti = new Dot[STATI.length];
				for(byte i = 0; i < STATI.length; i ++) { 
					punti[i] = new Dot();
					punti[i].setY(0L);
					if(step >= 60)
						punti[i].setOnClick("zoomActivity(" + index + ")");
					punti[i].setX(c.getTimeInMillis()/1000);
				}
				index += 2;
//				c.add(Calendar.SECOND, step.intValue());
				
				while(rs.next()) {
					totale += rs.getLong("hitCount");
					for(byte i = 0; i < STATI.length; i ++)
						if(STATI[i].getValue() == rs.getInt("status")) {
							punti[i].setY(rs.getLong("hitCount"));
							break;
						}
				}		
				for(byte i = 0; i < STATI.length; i ++) {
					if(totale != 0)
						punti[i].setY(Math.round(punti[i].getY().longValue()*10000./totale)/100.);
					else
						punti[i].setTooltip(getText("chart.defaultUnknownValue"));
					values[i].add(punti[i]);
				}
				//hadResults = cStmt.getMoreResults(); 
			} //end while
			}// endif	
			cStmt.close();
		} catch(SQLException e) { 
			throw new Exception(); 
		} finally {
			if(conn != null) conn.close();
		}		
		return values;
	}
	
	// build a line chart
	public String execute1() throws Exception {
		MyDotsList[] values;
		try {
			values = getDots();
			if(values == null) return SUCCESS;
		} catch(Exception e) {
			return ERROR;
		}
		lineChart = new LineChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		lineChart.setTitle(new Title(getText("activityChart.title.text", new String[] { formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
			"{" + getText("activityChart.title.style") + "}"));
		
		LineStyle lineStyle = new LineStyle();
		lineStyle.setStyle(LineStyle.STYLE_SOLID);
		LineAnimation animation = new LineAnimation(LineAnimation.TYPE_EXPLODE);
		animation.setDelay(0);
		animation.setCascade(2);
		
		List<ElementLine> elements = new LinkedList<ElementLine>();
		for(byte i = 0; i < STATI.length; i ++) {
			DotStyle ds = new DotStyle();
			ds.setType(DotStyle.TYPE_SOLID_DOT);
			ds.setWidth(2);
			ds.setDotSize(2);
			ds.setColour("#000000");
			ds.setTooltip("#val#%");
			ElementLine element = new ElementLine();
			element.setValues(values[i]);
			element.setDotStyle(ds);
			element.setLineStyle(lineStyle);
			element.setOn_show(animation);
			element.setColour(getText(STATI[i].getColor()));
			element.setText(getText(STATI[i].getText()));
			element.setFontSize(12);
			elements.add(element);
		}
		
		lineChart.setX_legend(new Text(getText("activityChart.xLegend.text"), "{font-size: 10px; color: #000000}"));
		lineChart.setY_legend(new Text("", "{font-size: 10px; color: #000000}"));
		//lineStyle.setOn(2);
		//lineStyle.setOff(3);	
		lineChart.setElements(elements);

		XAxis x_axis = new XAxis(super.cI.getTimeInMillis()/1000L, super.cF.getTimeInMillis()/1000L+step/2, step/2, false);
		x_axis.setColour("#000000");
		//x_axis.setStroke(3);
		x_axis.setTick_height(2);
		x_axis.setGrid_colour(getText("chart.defaultGridColor"));
		lineChart.setY_axis(super.getDefaultYAxis());
		Label l = new Label();
		l.setSteps(labelStep);
		//l.setVisible_steps(8);
		l.setVisible(true);
		//x_axis.setThreed(1);
		l.setRotate(270);
		l.setSize(12);
		l.setText("#date:" + formatter.getShortDateFormat() + "\n" + 
				formatter.getShortTimeFormat(step<60) + "#");
		x_axis.setLabels(l);
		lineChart.setX_axis(x_axis);
		lineChart.setBg_colour(getText("chart.defaultBgColor"));		
		
		return SUCCESS;
	}
	
	// build an area chart
	public String execute2() throws Exception {
		MyDotsList[] values;
		try {
			values = getDots();
			if(values == null) return SUCCESS;
		} catch(Exception e) {
			return ERROR;
		}
		
		areaChart = new AreaChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		areaChart.setTitle(new Title(getText("activityChart.title.text", new String[] { formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
			"{" + getText("activityChart.title.style") + "}"));
		
		List<ElementArea> elements = new LinkedList<ElementArea>();
		LineStyle lineStyle = new LineStyle();
		lineStyle.setStyle(LineStyle.STYLE_SOLID);
		LineAnimation animation = new LineAnimation(LineAnimation.TYPE_EXPLODE);
		animation.setDelay(0);
		animation.setCascade(2);
		for(byte i = 0; i < STATI.length; i ++) {
			DotStyle ds = new DotStyle();
			ds.setType(DotStyle.TYPE_SOLID_DOT);
			ds.setWidth(2);
			ds.setDotSize(2);
			ds.setColour("#000000");
			ds.setTooltip("#val#%");
			ElementArea element = new ElementArea();
			element.setValues(values[i]);
			element.setAlpha(0.9);
			element.setFillAlpha(0.5);
			element.setFillColour(getText(STATI[i].getColor()));
			element.setDotStyle(ds);
			element.setLineStyle(lineStyle);
			element.setOn_show(animation);
			element.setColour(getText(STATI[i].getColor()));
			element.setText(getText(STATI[i].getText()));
			element.setFontSize(12);
			elements.add(element);
		}
		areaChart.setElements(elements);
		areaChart.setX_legend(new Text(getText("activityChart.xLegend.text"), "{font-size: 10px; color: #000000}"));
		areaChart.setY_legend(new Text("", "{font-size: 10px; color: #000000}"));
		//lineStyle.setOn(2);
		//lineStyle.setOff(3);	
		
		XAxis x_axis = new XAxis(super.cI.getTimeInMillis()/1000L, super.cF.getTimeInMillis()/1000L+step/2, step/2, false);
		x_axis.setColour("#000000");
		//x_axis.setStroke(3);
		x_axis.setTick_height(2);
		x_axis.setGrid_colour(getText("chart.defaultGridColor"));
		areaChart.setY_axis(super.getDefaultYAxis());
		Label l = new Label();
		l.setSteps(labelStep);
		//l.setVisible_steps(8);
		l.setVisible(true);
		//x_axis.setThreed(1);
		l.setRotate(270);
		l.setSize(12);
		l.setText("#date:" + formatter.getShortDateFormat() + "\n" + 
			formatter.getShortTimeFormat(step<60) + "#");
		x_axis.setLabels(l);
		areaChart.setX_axis(x_axis);
		areaChart.setBg_colour(getText("chart.defaultBgColor"));		
		return SUCCESS;
	}
	
	// build a stack chart
	public String execute3() throws Exception {
		Connection conn = null;
		String query;
		String query2;
		PreparedStatement cStmt = null;
		
		List<StackBar[]> values = new LinkedList<StackBar[]>();
		try {
			conn = Constants.DATASOURCE.getConnection();
			step = (cF.getTimeInMillis() - cI.getTimeInMillis())/1000;
			step =  step / super.spansNumber + (step%super.spansNumber == 0L? 0: 1); // arrotondo sempre per eccesso
			
			
			//CallableStatement cStmt = conn.prepareCall(QUERY);
			//cStmt.setInt(1, super.idServiceInstance);
			//cStmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			//cStmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			//cStmt.setLong(4, step);
			//boolean hadResults = cStmt.execute();
			
			//while(hadResults) {
				//ResultSet rs = cStmt.getResultSet();
			Calendar c = (Calendar) (super.cI.clone());
			
			if (step>0) {		
				while (c.getTimeInMillis() < super.cF.getTimeInMillis()) {
						  
					//ResultSet rs = cStmt.getResultSet();
					query="";
					query2="";
					if (TYPEQUERY=="SP_ACTIVITY_CHART") {
						query = "SELECT status*1 as status, count(*) as hitCount FROM  esgf_dashboard.service_status WHERE idServiceInstance=";
						query = query + super.idServiceInstance;
						query = query + " AND timestamp between '";
						query = query + new Timestamp(c.getTimeInMillis()) + "' AND '";
						c.add(Calendar.SECOND, step.intValue());
						query = query + new Timestamp(c.getTimeInMillis()) + "' GROUP BY status";
					}
					if (TYPEQUERY=="SP_ACTIVITY_CHART_ON_OFF") {
						query = "(SELECT 1 as status, count(*) as hitCount FROM  esgf_dashboard.service_status WHERE idServiceInstance=";
						query2= "(SELECT -1 as status, count(*) as hitCount FROM  esgf_dashboard.service_status WHERE idServiceInstance=";
						query = query + super.idServiceInstance;
						query2 = query2 + super.idServiceInstance;
						query = query + " AND timestamp between '";
						query2= query2 + " AND timestamp between '";
						query = query + new Timestamp(c.getTimeInMillis()) + "' AND '";
						query2 = query2 + new Timestamp(c.getTimeInMillis()) + "' AND '";
						c.add(Calendar.SECOND, step.intValue());
						query = query + new Timestamp(c.getTimeInMillis()) + "' AND status=1) UNION ";
						query2 = query2 + new Timestamp(c.getTimeInMillis()) + "' AND status<>1)";
						query = query+query2;
					}
					//System.out.println(TYPEQUERY + "|-> ActivityChartAction Query = "+ query);
				   			
					cStmt = conn.prepareStatement(query);
					ResultSet rs = cStmt.executeQuery();
			
			
				long totale = 0L;
				StackBar[] barre = new StackBar[STATI.length];
				for(byte i = 0; i < STATI.length; i ++) {
					barre[i] = new StackBar();
					barre[i].setVal(0);
					barre[i].setTooltip("#val#%");
				}
			
				while(rs.next()) {
					totale += rs.getLong("hitCount");
					for(byte i = 0; i < STATI.length; i ++)
						if(STATI[i].getValue() == rs.getInt("status")) {
							barre[i].setVal(rs.getLong("hitCount"));
							break;
						}
				}
				
				for(byte i = 0; i < STATI.length; i ++) {
					if(totale != 0)
						barre[i].setVal(Math.round(barre[i].getVal().longValue()*10000./totale)/100.);
					else
						barre[i].setTooltip(getText("chart.defaultUnknownValue"));
				}
				values.add(null);
				values.add(barre);
				//hadResults = cStmt.getMoreResults(); 
			} //endwhile
			} //endif	
			cStmt.close();
		} catch(SQLException e) { 
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		
		stackChart = new StackChart();
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		stackChart.setTitle(new Title(getText("activityChart.title.text", new String[] { formatter.formatDateTime(cI), formatter.formatDateTime(cF) }), 
			"{" + getText("activityChart.title.style") + "}"));
		
		List<ElementStack> elements = new LinkedList<ElementStack>();
		ElementStack element = new ElementStack();
		element.setValues(values);
		element.setAlpha(0.8);		
		List<String> colours = new LinkedList<String>();
		List<Key> chiavi = new LinkedList<Key>();
		for(byte i = 0; i < STATI.length; i ++) {
			Key k = new Key();
			k.setText(getText(STATI[i].getText()));
			k.setColour(getText(STATI[i].getColor()));
			k.setFontSize(12);
			chiavi.add(k);
			colours.add(getText(STATI[i].getColor()));
		}
		element.setColours(colours);
		element.setKeys(chiavi);
		if(step >= 60)
			element.setOn_click("zoomActivity");
		elements.add(element);
		stackChart.setElements(elements);
		BarAnimation animation = new BarAnimation(BarAnimation.TYPE_POP);
		animation.setCascade(3);
		animation.setDelay(1);
		element.setOn_show(animation);
		stackChart.setX_legend(new Text(getText("activityChart.xLegend.text"), "{font-size: 10px; color: #000000}"));
		stackChart.setY_legend(new Text("", "{font-size: 10px; color: #000000}"));
		XAxis x_axis = new XAxis(0, super.spansNumber*2+1, 1, false);
		x_axis.setColour("#000000");
		//x_axis.setThreed(2);
		//x_axis.setStroke(3);
		x_axis.setTick_height(2);
		x_axis.setGrid_colour(getText("chart.defaultGridColor"));
		stackChart.setY_axis(super.getDefaultYAxis());
		Label label = new Label();
		label.setSteps(2l);
		//l.setVisible_steps(8);
		label.setVisible(true);
		label.setRotate(270);
		label.setSize(12);
		String []labels = new String[super.spansNumber * 2 + 2];
		Calendar c = Calendar.getInstance();
		c.setTimeInMillis(super.cI.getTimeInMillis());		
		/*for(int index = 0; index <= super.spansNumber; index ++) {
			labels[index*2] = formatter.formatDateTime(c, "\n", step<60);
			c.add(Calendar.SECOND, step.intValue()/2);
			labels[index*2+1] = formatter.formatDateTime(c, "\n", step<60);
			c.add(Calendar.SECOND, -step.intValue()/2);
			c.add(Calendar.SECOND, step.intValue());
		}*/
		for(int index = 0; index <= super.spansNumber; index ++) {
			labels[index*2] = formatter.formatDateTime(c, "\n", step<60);
			labels[index*2+1] = "";
			c.add(Calendar.SECOND, step.intValue());
		}
		label.setLabels(labels);
		x_axis.setLabels(label);
		stackChart.setX_axis(x_axis);
		stackChart.setBg_colour(getText("chart.defaultBgColor"));
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

	public void setStackChart(StackChart stackChart) {
		this.stackChart = stackChart;
	}

	public StackChart getStackChart() {
		return stackChart;
	}

	public Boolean getOnOff() {
		return onOff;
	}

	public void setOnOff(Boolean onOff) {
		this.onOff = onOff;
	}
}
