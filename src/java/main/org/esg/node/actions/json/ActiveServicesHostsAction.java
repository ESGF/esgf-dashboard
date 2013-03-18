package org.esg.node.actions.json;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.LinkedList;
import java.util.List;

import org.esg.node.beans.HostBean;
import org.esg.node.utils.Constants;
import org.esg.node.utils.SqlQuery;
import org.esg.node.utils.UserAware;
import org.esg.node.utils.UserSession;

import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class ActiveServicesHostsAction extends ActionSupport implements UserAware {
	
	private static final long serialVersionUID = 1L;
	
	private String project = null;
	private List<HostBean> activehosts = null;
	private UserSession userSession = null;

	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt;
			if (project==null)
				stmt = conn.prepareStatement(SqlQuery.GET_ALL_HOSTS_WITH_ACTIVE_SERVICES.getSql());
			else {
				stmt = conn.prepareStatement(SqlQuery.GET_ALL_HOSTS_OF_ONE_PROJECT_WITH_ACTIVE_SERVICES.getSql());
				stmt.setString(1,project);
			}
			ResultSet rs = stmt.executeQuery();
			
			activehosts = new LinkedList<HostBean>();
			
			while(rs.next()) {
				HostBean host = new HostBean();
				host.setId(rs.getInt("id"));
				host.setIp(rs.getString("ip"));
				host.setName(rs.getString("name"));
				host.setCity(rs.getString("city"));
				host.setLatitude(rs.getBigDecimal("latitude"));
				host.setLongitude(rs.getBigDecimal("longitude"));
				
				activehosts.add(host);
		    }
			rs.close();
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;		
	}

	public String getProject() {
		return project;
	}

	public void setProject(String project) {
		this.project = project;
	}

	public List<HostBean> getActivehosts() {
		return activehosts;
	}

	public void setActivehosts(List<HostBean> activehosts) {
		this.activehosts = activehosts;
	}

	@Override
	public void setUserSession(UserSession userSession) {
		this.userSession  = userSession;
	}
}
