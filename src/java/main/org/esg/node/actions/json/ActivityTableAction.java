package org.esg.node.actions.json;

import java.sql.*;
import java.util.*;

import org.esg.node.utils.*;
import org.esg.node.actions.base.ChartBaseAction;
import org.esg.node.beans.*;

/**
 * @author University of Salento and CMCC
 */
public class ActivityTableAction extends ChartBaseAction {
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
		PreparedStatement cStmt = null;
		String query;
		//boolean hadResults;
		//System.out.println("Activity Table Action start!!!!");
		try {
			conn = Constants.DATASOURCE.getConnection();
			Long step = (super.cF.getTimeInMillis() - super.cI.getTimeInMillis()) / 1000;
			step =  step / super.spansNumber + (step%super.spansNumber == 0L? 0: 1); // round up 
			
			cI.add(Calendar.SECOND, (int) (start*step));
			Calendar c = null;
			if(limit != null && limit <= super.spansNumber)
				(c = (Calendar) cI.clone()).add(Calendar.SECOND, (int) (limit*step));
			else
				c = (Calendar) cF.clone();
			if(c.before(cF))
				cF = c;
			DateFormat formatter = new DateFormat(getLocale().getLanguage());
			table = new ActivityTable();
			
			List<ActivityRecord> activityRecords = new LinkedList<ActivityRecord>();
			
			//CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_ACTIVITY_CHART.getSql());
			
			//cStmt.setInt(1, super.idServiceInstance);
			//cStmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			//cStmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			//cStmt.setLong(4, step);
						
			/*`attivitaServizio`(IN id INTEGER, IN startDate DATETIME, IN endDate DATETIME, IN step LONG)
			
			DECLARE position DATETIME;

	        SET position = startDate;

	        IF step>0 THEN
	                WHILE position < endDate DO
	                        SELECT `status`*1 as status, count(*) as hitCount FROM service_status WHERE idServiceInstance=id AND `timestamp` between position AND position+INTERVAL step second GROUP BY `status`;
	                SET position = position + INTERVAL step second;
	                END WHILE;
	        END IF;*/
			
			c = (Calendar) cI.clone();
			
			boolean fullTime = step < 60;
			//while(hadResults) {
			if (step>0) {
				
			  while (c.getTimeInMillis() < cF.getTimeInMillis()) {
				
				query = "SELECT status*1 as status, count(*) as hitCount FROM service_status WHERE idServiceInstance=";
				query = query + super.idServiceInstance;
				query = query + " AND timestamp between '";
			    query = query + new Timestamp(c.getTimeInMillis()) + "' AND '";
			   	//System.out.println("c.getTimeMillis = " + c.getTimeInMillis());		
				//hadResults = cStmt.execute();
				//ResultSet rs = cStmt.getResultSet();
				long totale = 0L;				
				ActivityRecord record = new ActivityRecord();
				List<Number> percentages = new ArrayList<Number>(Constants.STATI_LENGTH);
				for(int i = 0; i < Constants.STATI_LENGTH; i ++)
					percentages.add(0);
				record.setStartDate(formatter.formatDate(c));
				record.setStartTime(formatter.formatTime(c, fullTime));
				c.add(Calendar.SECOND, step.intValue());
				record.setEndDate(formatter.formatDate(c));
				record.setEndTime(formatter.formatTime(c, fullTime));
				
			    query = query + new Timestamp(c.getTimeInMillis()) + "' GROUP BY status";
			    //System.out.println("|||->>> ActivityTableAction Query = "+query);

			    cStmt = conn.prepareStatement(query);
				ResultSet rs = cStmt.executeQuery();
			    
				while(rs.next()) {
					totale += rs.getLong("hitCount");
					for(int i = 0; i < Constants.STATI_LENGTH; i ++)
						if(Constants.STATI[i].getValue() == rs.getInt("status")) {
							percentages.set(i, rs.getLong("hitCount"));
							break;
						}
				}
				for(int i = 0; i < Constants.STATI_LENGTH; i ++) 
					if(totale != 0)
						percentages.set(i, Math.round(percentages.get(i).doubleValue()*10000./totale)/100.);
					else percentages.set(i, null);
				record.setValues(percentages);
				activityRecords.add(record);
				//hadResults = cStmt.getMoreResults(); 				
			  }
			}
			
			cStmt.close();
			table.setActivityRecords(activityRecords);
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
