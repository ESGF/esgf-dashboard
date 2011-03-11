package org.esg.node.actions.json;

import org.esg.node.actions.base.ChartBaseAction;
import org.esg.node.beans.*;
import java.sql.*;
import org.esg.node.utils.*;
import java.util.*;

/**
 * @author University of Salento and CMCC
 */
public class RTTTableAction extends ChartBaseAction {
	private static final long serialVersionUID = 1L;

	private Integer start = null;
	private Integer limit = null;
	private ActivityTable table = null;
	
	@Override
	public void validate() {
		super.validate();
		if(start == null || start <= 0)
			start = 0;
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			Long step = (super.cF.getTimeInMillis() - super.cI.getTimeInMillis())/1000;
			step =  step / super.spansNumber + (step%super.spansNumber == 0L? 0: 1); // arrotondo sempre per eccesso
			
			cI.add(Calendar.SECOND, (int) (start*step));
			Calendar c = null;
			if(limit != null && limit <= super.spansNumber)
				(c = (Calendar) cI.clone()).add(Calendar.SECOND, (int) (limit*step));
			else
				c = (Calendar) cF.clone();
			if(c.before(cF))
				cF = c;
			
			CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_RTT_CHART.getSql());
			cStmt.setInt(1, super.idServiceInstance);
			cStmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			cStmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			cStmt.setLong(4, step);

			table = new ActivityTable();
			List<ActivityRecord> records = new LinkedList<ActivityRecord>();
			DateFormat formatter = new DateFormat(getLocale().getLanguage());
			c = (Calendar) cI.clone();
			boolean hadResults = cStmt.execute();
			boolean fullTime = step < 60;
			while(hadResults) {
				ResultSet rs = cStmt.getResultSet();
				ActivityRecord record = new ActivityRecord();
				record.setStartDate(formatter.formatDate(c));
				record.setStartTime(formatter.formatTime(c, fullTime));
				c.add(Calendar.SECOND, step.intValue());
				record.setEndDate(formatter.formatDate(c));
				record.setEndTime(formatter.formatTime(c, fullTime));
				record.setValues(new LinkedList<Number>());
				if(rs.next() && rs.getBigDecimal("avgRtt") != null)
					record.getValues().add(Math.round(rs.getBigDecimal("avgRtt").doubleValue()/10.)/100.);
				else record.getValues().add(null);
				records.add(record);
				hadResults = cStmt.getMoreResults(); 
			}
			cStmt.close();
			table.setActivityRecords(records);
			table.setTotalCount(super.spansNumber);
		} catch(SQLException e) {
			return ERROR; 
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}

	public void setStart(Integer start) {
		this.start = start;
	}

	public Integer getStart() {
		return start;
	}

	public void setLimit(Integer limit) {
		this.limit = limit;
	}

	public Integer getLimit() {
		return limit;
	}

	public void setTable(ActivityTable table) {
		this.table = table;
	}

	public ActivityTable getTable() {
		return table;
	}

}
