package org.esg.node.utils;

import java.sql.*;
import java.util.Calendar;

public class AvgProjectActivity {
	//private CallableStatement cStmt = null;
	private PreparedStatement cStmt = null;
	
	public AvgProjectActivity(Connection conn) throws SQLException {
		/* cStmt = conn.prepareCall(SqlQuery.SP_AVG_PROJECT_ACTIVITY.getSql());*/
	}
	
	public Number getProjectActivity(Connection conn, Integer idProject, Calendar startDate, Calendar endDate) throws SQLException {
		return getProjectStatus(conn, idProject, startDate, endDate, ServiceStatus.AVAILABLE.getValue());
	}
	
	public Number getProjectStatus(Connection conn, Integer idProject, Calendar startDate, Calendar endDate, Integer status) throws SQLException {
		ResultSet rs = null;
		try {
			String query;
			query = "SELECT AVG(percentage) AS average FROM (SELECT 100* (SELECT COUNT(*) FROM  esgf_dashboard.service_status WHERE idServiceInstance=s.id AND status=";
			query = query + status;
			query = query + " AND timestamp BETWEEN '";
			
			//cStmt.clearParameters();
			//cStmt.setInt(1, idProject);
			
			Calendar cs = startDate==null? Calendar.getInstance(): startDate;
			//cStmt.setTimestamp(2, new Timestamp(c.getTimeInMillis()));
			query = query + (new Timestamp(cs.getTimeInMillis()));
			query = query + "' AND '";
			
			Calendar ce = endDate==null? Calendar.getInstance(): endDate;
			query = query + (new Timestamp(ce.getTimeInMillis()));
			query = query + "') / (SELECT COUNT(*) FROM  esgf_dashboard.service_status WHERE idServiceInstance=s.id AND timestamp BETWEEN '";
			query = query + (new Timestamp(cs.getTimeInMillis()));
			query = query + "' AND '";
			query = query + (new Timestamp(ce.getTimeInMillis()));
			query = query + "') AS percentage FROM  esgf_dashboard.service_instance s INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id WHERE u.idProject=";
			query = query + idProject;
			query = query + " AND u.endDate IS NULL) t";
			
			//System.out.println("$$ AvgProjectActivity Query = "+query);
			cStmt = conn.prepareStatement(query);
			
			//cStmt.setTimestamp(3, new Timestamp(c.getTimeInMillis()));	
			//cStmt.setInt(4, status); 
			
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
