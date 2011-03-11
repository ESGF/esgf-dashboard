package org.esg.node.actions.json;

import org.esg.node.utils.*;

import java.sql.*;
import java.util.Calendar;
import org.esg.node.beans.*;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class SummaryAction extends ActionSupport {
	private static final long serialVersionUID = 1L;

	private Integer idServiceInstance = null;
	private String startDate = null;
	private String endDate = null;
	private String startTime = null;
	private String endTime = null;
	private Calendar cI = null;
	private Calendar cF = null;
	private Summary summary = null;
	
	@Override
	public void validate() {
		if(idServiceInstance == null) {
			addActionError("");
			return;
		}
		if(startDate == null || startTime == null || endDate == null || endTime == null)
			return;
		try {
			DateFormat formatter = new DateFormat(getLocale().getLanguage());
			cI = formatter.toStandard(startDate, startTime);
			cF = formatter.toStandard(endDate, endTime);
			if(!cI.before(cF))
				this.addActionError("");
		} catch(Exception e) {
			this.addActionError("");
			return;
		}		
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			if(cI == null || cF == null) {
				MaxMinTimestampHelper helper = new MaxMinTimestampHelper(conn);
				MaxMinTimestamp maxMin = helper.getMaxMinTimestamp(idServiceInstance);
				if(maxMin == null || maxMin.getMax() == null || maxMin.getMin() == null)
					return SUCCESS;
				cI = Calendar.getInstance(); 
				cI.clear();
				cF = Calendar.getInstance();
				cF.clear();
				cI.setTimeInMillis(maxMin.getMin());
				cF.setTimeInMillis(maxMin.getMax());
			}
			summary = new Summary();
			RTTAvgAndStdDevHelper helper = new RTTAvgAndStdDevHelper(conn);
			summary.setAvgStdDev(helper.compute(cI, cF, idServiceInstance));
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.STATUS_QUERY.getSql());
			stmt.setInt(1, idServiceInstance);
			stmt.setTimestamp(2, new Timestamp(cI.getTimeInMillis()));
			stmt.setTimestamp(3, new Timestamp(cF.getTimeInMillis()));
			ResultSet rs = stmt.executeQuery();
			long absoluteTotal = 0l;
			long activity = 0l;
			while(rs.next()) {
				absoluteTotal += rs.getLong("hitCount");
				if(rs.getInt("status") == ServiceStatus.ACTIVE.getValue())
					activity = rs.getLong("hitCount");
			}
			if(absoluteTotal != 0)
				summary.setAvailability(Math.round(10000.*activity/absoluteTotal) / 100.);
			rs.close();
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		return SUCCESS;
	}

	public Integer getIdServiceInstance() {
		return idServiceInstance;
	}

	public void setIdServiceInstance(Integer idServiceInstance) {
		this.idServiceInstance = idServiceInstance;
	}

	public void setSummary(Summary summary) {
		this.summary = summary;
	}

	public Summary getSummary() {
		return summary;
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
}
