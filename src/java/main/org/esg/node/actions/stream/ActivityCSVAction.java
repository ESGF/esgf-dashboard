package org.esg.node.actions.stream;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.math.BigDecimal;
import java.sql.*;
import java.util.*;
import org.esg.node.actions.base.ChartBaseAction;
import org.esg.node.utils.*;

public class ActivityCSVAction extends ChartBaseAction {
	private static final long serialVersionUID = 1L;

	private InputStream stream = null;
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		PreparedStatement cStmt = null;
		String query;
		try {
			conn = Constants.DATASOURCE.getConnection();
			Long step = (super.cF.getTimeInMillis() - super.cI.getTimeInMillis()) / 1000;
			step =  step / super.spansNumber + (step%super.spansNumber == 0L? 0: 1); // arrotondo sempre per eccesso
			//CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_ACTIVITY_CHART.getSql());
			//cStmt.setInt(1, super.idServiceInstance);
			//cStmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			//cStmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			//cStmt.setLong(4, step);
			
			DateFormat formatter = new DateFormat(getLocale().getLanguage());
			Calendar c = Calendar.getInstance();
			c.setTimeInMillis(super.cI.getTimeInMillis());
			CsvWriter csv = new CsvWriter();
			csv.setDelimiter(';');
			csv.writeRecord(new String[] { "startDate", "startTime", "endDate", "endTime", 
				"Service Available", "Service Down", "Host Unreachable", "Network Errors" });
			//boolean hadResults = cStmt.execute();
			if (step>0) {
				  while (c.getTimeInMillis() < cF.getTimeInMillis()) {
			//while(hadResults) {
				//ResultSet rs = cStmt.getResultSet();
				
				query = "SELECT status*1 as status, count(*) as hitCount FROM service_status WHERE idServiceInstance=";
				query = query + super.idServiceInstance;
				query = query + " AND timestamp between '";
				query = query + new Timestamp(c.getTimeInMillis()) + "' AND '";
					    
				long totale = 0L;
				csv.write(formatter.formatDate(c));
				csv.write(formatter.formatTime(c, step<60));
				c.add(Calendar.SECOND, step.intValue());
				csv.write(formatter.formatDate(c));
				csv.write(formatter.formatTime(c, step<60));
				
				query = query + new Timestamp(c.getTimeInMillis()) + "' GROUP BY status";
			    //System.out.println("|||->>> ActivityCVSAction Query = "+query);

			    cStmt = conn.prepareStatement(query);
				ResultSet rs = cStmt.executeQuery();

				Number[] percentages = new Number[Constants.STATI_LENGTH];
				Arrays.fill(percentages, new BigDecimal(0));
				while(rs.next()) {
					totale += rs.getLong("hitCount");
					for(byte i = 0; i < Constants.STATI_LENGTH; i ++)
						if(Constants.STATI[i].getValue() == rs.getInt("status")) {
							percentages[i] = rs.getLong("hitCount");
							break;
						}
				}
				for(byte i = 0; i < Constants.STATI_LENGTH; i ++) {
					if(totale != 0) {
						percentages[i] = Math.round(percentages[i].longValue()*10000./totale)/100.;
						csv.write(String.valueOf(percentages[i]));
					}
					else csv.write(null);					
				}
				csv.endRecord();
				//hadResults = cStmt.getMoreResults(); 
			} //endwhile
			} //endif
			cStmt.close();
			stream = new ByteArrayInputStream(csv.getOutputStream().toByteArray());
			csv.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		return SUCCESS;
	}

	public void setStream(InputStream stream) {
		this.stream = stream;
	}

	public InputStream getStream() {
		return stream;
	}

}
