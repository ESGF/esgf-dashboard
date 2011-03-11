package org.esg.node.utils;

import java.sql.*;
import java.util.Calendar;

public class AvgProjectActivity {
	private CallableStatement cStmt = null;
	
	public AvgProjectActivity(Connection conn) throws SQLException {
		cStmt = conn.prepareCall(SqlQuery.SP_AVG_PROJECT_ACTIVITY.getSql());
	}
	
	public Number getProjectActivity(Integer idProject, Calendar startDate, Calendar endDate) throws SQLException {
		return getProjectStatus(idProject, startDate, endDate, ServiceStatus.AVAILABLE.getValue());
	}
	
	public Number getProjectStatus(Integer idProject, Calendar startDate, Calendar endDate, Integer status) throws SQLException {
		ResultSet rs = null;
		try {
			cStmt.clearParameters();
			cStmt.setInt(1, idProject);
			Calendar c = startDate==null? Calendar.getInstance(): startDate;
			cStmt.setTimestamp(2, new Timestamp(c.getTimeInMillis()));
			c = endDate==null? Calendar.getInstance(): endDate;
			cStmt.setTimestamp(3, new Timestamp(c.getTimeInMillis()));	
			cStmt.setInt(4, status);
			try {
				rs = cStmt.executeQuery();
			} catch(SQLException e) {
				return null;
			}
			if(rs.next())
				return rs.getBigDecimal("average");
			return null;
		} finally {
			if(rs != null) rs.close();
		}
	}
	
	public void close() throws SQLException {
		if(cStmt != null) cStmt.close();
	}
}
