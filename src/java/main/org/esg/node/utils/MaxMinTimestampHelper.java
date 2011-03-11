package org.esg.node.utils;

import java.sql.*;
import org.esg.node.beans.MaxMinTimestamp;

/**
 * @author University of Salento and CMCC
 */
public class MaxMinTimestampHelper {

	private Connection conn = null;
		
	public MaxMinTimestampHelper(Connection conn) {
		this.conn = conn;
	}
	
	public MaxMinTimestamp getMaxMinTimestamp(Integer idServiceInstance) throws Exception {
		PreparedStatement stmt = conn.prepareStatement(SqlQuery.MIN_MAX_TIMESTAMP.getSql());
		stmt.setInt(1, idServiceInstance);
		ResultSet rs = stmt.executeQuery();
		MaxMinTimestamp maxMin = null;
		if(rs.next()) {
			maxMin = new MaxMinTimestamp();
			if(rs.getTimestamp("minTimestamp") != null)
				maxMin.setMin(rs.getTimestamp("minTimestamp").getTime());
			if(rs.getTimestamp("maxTimestamp") != null) {
				long max = rs.getTimestamp("maxTimestamp").getTime();
				long mod = max % 60000;
				if(mod != 0)
					max = max + 60000 - mod;
				maxMin.setMax(max);
			}
		}
		rs.close();
		stmt.close();
		return maxMin;		
	}
	
}
