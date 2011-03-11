package org.esg.node.utils;

import java.sql.*;
import java.util.Calendar;

public class AvgHostActivity {
	
	private CallableStatement cStmt = null;
	
	public AvgHostActivity(Connection conn) throws SQLException {
		 cStmt = conn.prepareCall(SqlQuery.SP_AVG_HOST_ACTIVITY.getSql());		 
	}
	
	/**
	 * 
	 * @param idProject
	 * @param idHost
	 * @param timeSpan - as minutes
	 * @return last timeSpan minutes host activity 
	 * @throws SQLException
	 */
	public Number getHostActivity(Integer idProject, Integer idHost, Integer timeSpan, Calendar endDate) throws SQLException {
		ResultSet rs = null;
		try {
			cStmt.clearParameters();
			if(idProject == null)
				cStmt.setNull(1, Types.INTEGER);
			else
				cStmt.setInt(1, idProject);
			cStmt.setInt(2, idHost);
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
				return rs.getBigDecimal("average");
			return null;
		} finally {
			if(rs != null) rs.close();
		}
	}
	
	public void finalize() {
		try {
			close();
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	public void close() throws SQLException {
		if(cStmt != null) cStmt.close();
	}
}
