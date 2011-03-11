package org.esg.node.utils;

import java.sql.*;
import java.util.Calendar;

/**
 * @author University of Salento and CMCC
 */
public class AvgServiceActivity {

	private CallableStatement cStmt = null;
	
	public AvgServiceActivity(Connection conn) throws SQLException {
		cStmt = conn.prepareCall(SqlQuery.SP_AVG_SERVICE_ACTIVITY.getSql());
	}
	
	public Number getServiceActivity(Integer idProject, Integer idService, Integer timeSpan, Calendar endDate) throws SQLException{
		ResultSet rs = null;
		try {
			cStmt.clearParameters();
			cStmt.setInt(1, idProject);
			cStmt.setInt(2, idService);
			Calendar c = (Calendar) (endDate==null? Calendar.getInstance(): endDate.clone());
			cStmt.setTimestamp(4, new Timestamp(c.getTimeInMillis()));
			c.add(Calendar.MINUTE, -timeSpan);
			cStmt.setTimestamp(3, new Timestamp(c.getTimeInMillis()));
			try {
				rs = cStmt.executeQuery();
			} catch(SQLException e) {
				return null;
			}
			if(rs.next())
				return rs.getBigDecimal("percentage");
			return null;
		} finally {
			if(rs != null) rs.close();
		}		
	}
	
	public void close() throws SQLException {
		if(cStmt != null) cStmt.close();
	}
}
