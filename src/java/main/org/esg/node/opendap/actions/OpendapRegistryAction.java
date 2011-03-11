package org.esg.node.opendap.actions;

import java.sql.*;
import org.esg.node.utils.*;
import java.util.*;
import org.esg.node.opendap.actions.beans.NodeExtension;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class OpendapRegistryAction extends ActionSupport implements UserAware {
	private static final long serialVersionUID = 1L;

	private Integer idProject = null;
	private UserSession userSession = null;
	private List<NodeExtension> services = null;
	
	@Override
	public void validate() {
		if(idProject == null)
			addActionError("");
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.USER_CAN_VIEW_PROJECT.getSql());
			stmt.setInt(1, userSession.getId());
			stmt.setInt(2, idProject);
			ResultSet rs = stmt.executeQuery();
			if(!rs.next()) {
				stmt.close();
				return INPUT;
			}
			rs.close();
			stmt.close();
			
			stmt = conn.prepareStatement(SqlQuery.OPENDAP_REGISTRY.getSql());
			stmt.setInt(1, idProject);
			rs = stmt.executeQuery();
			services = new LinkedList<NodeExtension>();
			while(rs.next()) 
				services.add(new NodeExtension(rs.getInt("o.id"), rs.getString("h.name"), rs.getString("h.ip"), rs.getInt("s.port"), rs.getString("o.url")));
			
			rs.close();
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}

	public void setIdProject(Integer idProject) {
		this.idProject = idProject;
	}

	public Integer getIdProject() {
		return idProject;
	}

	@Override
	public void setUserSession(UserSession userSession) {
		this.userSession = userSession;
	}

	public void setServices(List<NodeExtension> services) {
		this.services = services;
	}

	public List<NodeExtension> getServices() {
		return services;
	}
	
}
