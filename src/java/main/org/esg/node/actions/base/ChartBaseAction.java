package org.esg.node.actions.base;

import java.util.Calendar;
import java.util.LinkedList;
import java.util.List;

import org.esg.node.utils.*;
import org.esg.node.chart.axis.YAxis;

import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public abstract class ChartBaseAction extends ActionSupport {
	private static final long serialVersionUID = 1L;
	
	protected Integer idServiceInstance = null;
	private String startDate = null;
	private String endDate = null;
	private String startTime = null;
	private String endTime = null;
	protected Integer spansNumber = null;
	protected Calendar cI = null, cF = null;
	
	@Override
	public void validate() {
		if(idServiceInstance == null || startDate == null || startTime == null || endDate == null || endTime == null) { 
			this.addActionError("");
			return;
		}		
		if(spansNumber == null)
			spansNumber = 20;
		try {
			DateFormat formatter = new DateFormat(getLocale().getLanguage());
			cI = formatter.toStandard(startDate, startTime);
			cF = formatter.toStandard(endDate, endTime);
			if(!cI.before(cF))
				this.addActionError("");
		} catch(Exception e) {
			addActionError("");
			return;
		}		
	}

	@Override
	public abstract String execute() throws Exception;
	
	protected YAxis getDefaultYAxis() {
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
	
	/*
	protected Long getTakeOversNumber(Connection conn) throws SQLException {
		long dots_number;
		PreparedStatement stmt = conn.prepareStatement(SqlQuery.TAKE_OVERS_NUMBER_IN_INTERVAL.getSql());
		stmt.setInt(1, idServiceInstance);
		stmt.setTimestamp(2, new Timestamp(cI.getTimeInMillis()));
		stmt.setTimestamp(3, new Timestamp(cF.getTimeInMillis()));
		ResultSet rs = stmt.executeQuery();
		if(! rs.next()) return null;
		dots_number = rs.getLong(1);
		rs.close();
		stmt.close();
		return dots_number;
	}*/
		
	public Integer getIdServiceInstance() {
		return idServiceInstance;
	}

	public void setIdServiceInstance(Integer idServiceInstance) {
		this.idServiceInstance = idServiceInstance;
	}
	
	public String getStartDate() {
		return startDate;
	}

	public void setStartDate(String startDate) {
		this.startDate = startDate;
	}

	public String getEndDate() {
		return endDate;
	}

	public void setEndDate(String endDate) {
		this.endDate = endDate;
	}

	public String getStartTime() {
		return startTime;
	}

	public void setStartTime(String startTime) {
		this.startTime = startTime;
	}

	public String getEndTime() {
		return endTime;
	}

	public void setEndTime(String endTime) {
		this.endTime = endTime;
	}

	public void setSpansNumber(Integer spansNumber) {
		this.spansNumber = spansNumber;
	}

	public Integer getSpansNumber() {
		return spansNumber;
	}
}
