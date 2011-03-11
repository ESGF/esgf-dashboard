package org.esg.node.actions.stream;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.sql.*;
import java.util.Calendar;
import org.esg.node.utils.*;
import org.esg.node.actions.base.ChartBaseAction;

public class RTTCSVAction extends ChartBaseAction {
	private static final long serialVersionUID = 1L;

	private InputStream stream = null;
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			Long step = (super.cF.getTimeInMillis() - super.cI.getTimeInMillis())/1000;
			step =  step / super.spansNumber + (step%super.spansNumber == 0L? 0: 1); // arrotondo sempre per eccesso
			
			CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_RTT_CHART.getSql());
			cStmt.setInt(1, super.idServiceInstance);
			cStmt.setTimestamp(2, new Timestamp(super.cI.getTimeInMillis()));
			cStmt.setTimestamp(3, new Timestamp(super.cF.getTimeInMillis()));
			cStmt.setLong(4, step);
			
			DateFormat formatter = new DateFormat(getLocale().getLanguage());
			Calendar c = (Calendar) cI.clone();
			boolean hadResults = cStmt.execute();
			CsvWriter csv = new CsvWriter();
			csv.setDelimiter(';');
			csv.writeRecord(new String[] { "startDate", "startTime", "endDate", "endTime", "RTT" });
			while(hadResults) {
				ResultSet rs = cStmt.getResultSet();
				csv.write(formatter.formatDate(c));
				csv.write(formatter.formatTime(c, step<60));
				c.add(Calendar.SECOND, step.intValue());
				csv.write(formatter.formatDate(c));
				csv.write(formatter.formatTime(c, step<60));
				
				if(rs.next() && rs.getBigDecimal("avgRtt") != null)
					csv.write(String.valueOf(Math.round(rs.getBigDecimal("avgRtt").doubleValue()/10.)/100.));
				else csv.write(null);
				csv.endRecord();
				hadResults = cStmt.getMoreResults(); 
			}
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
