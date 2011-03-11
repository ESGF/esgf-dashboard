package org.esg.node.utils;

import java.math.BigDecimal;
import java.sql.*;
import java.util.Calendar;
import org.esg.node.beans.RTTAvgAndStdDev;

/**
 * @author University of Salento and CMCC
 */
public class RTTAvgAndStdDevHelper {

	private Connection conn = null;
		
	public RTTAvgAndStdDevHelper(Connection conn) {
		this.conn = conn;
	}

	public RTTAvgAndStdDev compute(Calendar cI, Calendar cF, Integer idServiceInstance) throws Exception {
		if(idServiceInstance == null || cI == null || cF == null) return null;
		RTTAvgAndStdDev avgStdDev = new RTTAvgAndStdDev();
		PreparedStatement stmt = conn.prepareStatement(SqlQuery.AVG_RTT.getSql());
		stmt.setInt(1, idServiceInstance);
		stmt.setTimestamp(2, new Timestamp(cI.getTimeInMillis()));
		stmt.setTimestamp(3, new Timestamp(cF.getTimeInMillis()));
		ResultSet rs = stmt.executeQuery();
		if(rs.next())			
			avgStdDev.setAvg(rs.getBigDecimal("avgRtt"));
		rs.close();
		stmt.close();
		if(avgStdDev.getAvg() == null) return avgStdDev;
		stmt = conn.prepareStatement(SqlQuery.DEV_STD_RTT.getSql());
		stmt.setBigDecimal(1, (BigDecimal) avgStdDev.getAvg());
		stmt.setInt(2, idServiceInstance);
		stmt.setTimestamp(3, new Timestamp(cI.getTimeInMillis()));
		stmt.setTimestamp(4, new Timestamp(cF.getTimeInMillis()));
		rs = stmt.executeQuery();
		if(rs.next())
			avgStdDev.setStdDev(rs.getBigDecimal("devStdRtt"));
		rs.close();
		stmt.close();
		return avgStdDev;
	}	
}
