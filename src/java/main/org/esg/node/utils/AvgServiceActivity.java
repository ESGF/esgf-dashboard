package org.esg.node.utils;

import java.sql.*;
import java.util.Calendar;

/**
 * @author University of Salento and CMCC
 */
public class AvgServiceActivity {

	//private CallableStatement cStmt = null;
	private PreparedStatement cStmt = null;
	
	public AvgServiceActivity(Connection conn) throws SQLException {
		//cStmt = conn.prepareCall(SqlQuery.SP_AVG_SERVICE_ACTIVITY.getSql());
	}
	
	public Number getServiceActivity(Connection conn, Integer idProject, Integer idService, Integer timeSpan, Calendar endDate) throws SQLException{
		ResultSet rs = null;
		try {
			String query;
			//cStmt.clearParameters();
			//cStmt.setInt(1, idProject);
			//cStmt.setInt(2, idService);
			Calendar c = (Calendar) (endDate==null? Calendar.getInstance(): endDate.clone());
			//cStmt.setTimestamp(4, new Timestamp(c.getTimeInMillis()));
			
			Calendar cs = (Calendar) (endDate==null? Calendar.getInstance(): endDate.clone());
			cs.add(Calendar.MINUTE, -timeSpan);
			//cStmt.setTimestamp(3, new Timestamp(cs.getTimeInMillis()));
			
			query = "SELECT 100* (SELECT COUNT(*) FROM service_status WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "') / (SELECT COUNT(*) FROM service_status WHERE idServiceInstance=s.id AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis()); 
			query = query + "') AS percentage FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id WHERE s.id=";
			query = query + idService;
			query = query + " AND u.endDate IS NULL AND u.idProject=";
			query = query + idProject;
			//System.out.println("%%% AVG SERVICE ACTIVITY ="+ query);
			cStmt = conn.prepareStatement(query);
			
			try {
				rs = cStmt.executeQuery();
			} catch(SQLException e) {
				//System.out.println("%%% AVG SERVICE ACTIVITY ERROR ="+ e.getMessage());
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
