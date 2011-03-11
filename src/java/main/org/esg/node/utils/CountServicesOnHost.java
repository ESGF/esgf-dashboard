package org.esg.node.utils;

import java.sql.*;

/**
 * @author University of Salento and CMCC
 */
public class CountServicesOnHost {
	private static final long serialVersionUID = 1L;

	private PreparedStatement stmt = null;
	
	public CountServicesOnHost(Connection conn, boolean byProject) throws SQLException {
		this.stmt = conn.prepareStatement((byProject? SqlQuery.COUNT_SERVICES_ON_HOST_BY_PROJECT: SqlQuery.COUNT_SERVICES_ON_HOST).getSql());
	}
	
	public Integer getCount(Integer idHost) throws Exception {
		return getCount(null, idHost);
	}
	
	public Integer getCount(Integer idProject, Integer idHost) throws Exception {
		stmt.clearParameters();
		stmt.setInt(1, idHost);
		if(idProject != null)
			stmt.setInt(2, idProject);
		ResultSet rs = stmt.executeQuery();
		Integer numServices = rs.next()? rs.getInt("servicesCount"): null;
		rs.close();
		return numServices;
	}
	
	public void finalize() {
		try {
			close();
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	public void close() throws SQLException {
		if(stmt != null) stmt.close();
	}
}
