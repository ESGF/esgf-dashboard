package org.esg.node.utils;

import java.sql.*;
import java.util.Calendar;

public class AvgHostActivity {
	
	//private CallableStatement cStmt = null;
	private PreparedStatement cStmt = null;
	
	public AvgHostActivity(Connection conn) throws SQLException {
		//System.out.println("Start Constructor - AvgHostActivity");		
		// try {
			 //cStmt = conn.prepareCall(SqlQuery.SP_AVG_HOST_ACTIVITY.getSql());
	    //} catch(SQLException e) {
		//		System.out.println("Error Message AvgHostActivity " + e.getMessage());
		//	}
	   //System.out.println("End Constructor - AvgHostActivity");
	}
	
	/**
	 * 
	 * @param idProject
	 * @param idHost
	 * @param timeSpan - as minutes
	 * @return last timeSpan minutes host activity 
	 * @throws SQLException
	 */
	public Number getHostActivity(Connection conn, Integer idProject, Integer idHost, Integer timeSpan, Calendar endDate) throws SQLException {
		ResultSet rs = null;
		
		try {
			String query;
			//System.out.println("getHostActivity1");
			//cStmt.clearParameters();
			//if(idProject == null)
			//	cStmt.setNull(1, Types.INTEGER);
			//else
			//	cStmt.setInt(1, idProject);
			//cStmt.setInt(2, idHost);
			Calendar c = (Calendar) (endDate==null? Calendar.getInstance(): endDate.clone());
			//cStmt.setTimestamp(4, new Timestamp(c.getTimeInMillis()));
		
			Calendar cs = (Calendar) (endDate==null? Calendar.getInstance(): endDate.clone());
			cs.add(Calendar.MINUTE, -timeSpan);
			
			//cStmt.setTimestamp(3, new Timestamp(c.getTimeInMillis()));
			query = "SELECT AVG(percentage) as average FROM (SELECT 100* (SELECT COUNT(*) FROM  esgf_dashboard.service_status WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "' ) / (SELECT COUNT(*) FROM  esgf_dashboard.service_status WHERE idServiceInstance=s.id AND timestamp BETWEEN '" ;
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "')  AS percentage FROM  esgf_dashboard.service_instance s INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id WHERE s.idHost=";
			query = query + idHost;
			if (idProject != null)
		    {
				query = query + " AND u.idProject=";
				query = query + idProject;
			}	
			query = query + " AND u.endDate IS NULL) t";
			//System.out.println("%%% AVG HOST ACTIVITY ="+ query);
			cStmt = conn.prepareStatement(query);
			
			try {
				rs = cStmt.executeQuery();
			} catch(SQLException e) {
				//System.out.println("Error Message getHostActivity " + e.getMessage());
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
