package org.esg.node.actions;

import java.sql.*;

import org.esg.node.utils.*;
import org.esg.node.beans.*;

import com.opensymphony.xwork2.ActionSupport;
import java.util.*;

/**
 * @author University of Salento and CMCC
 */
public class ServerAction extends ActionSupport implements UserAware {
	private static final long serialVersionUID = 1L;

	private Integer idServer = null;
	private Integer idProject = null;
	private Host server = null;
	private UserSession userSession = null;
	
	@Override
	public void validate() {
		if(idProject == null || idServer == null)
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
			if(!rs.next()) 
				return INPUT;
			rs.close();
			stmt.close();			
			stmt = conn.prepareStatement(SqlQuery.HOST_BY_ID_AND_PROJECT.getSql());
			stmt.setInt(1, idProject);
			stmt.setInt(2, idServer);
			rs = stmt.executeQuery();	
			if(rs.next()) {
				server = new Host();
				server.setId(idServer);
				server.setIp(rs.getString("ip"));
				server.setName(rs.getString("name"));
				server.setCity(rs.getString("city"));
				server.setLatitude(rs.getBigDecimal("latitude"));
				server.setLongitude(rs.getBigDecimal("longitude"));
				server.setNumInstances(rs.getInt("numInstances"));
				server.setProjectName(rs.getString("name"));
			}
			else return ERROR;
			rs.close();
			stmt.close();
			
			stmt = conn.prepareStatement(SqlQuery.SERVICE_INSTANCE_FOR_SERVER_IN_PROJECT.getSql());
			stmt.setInt(1, idProject);
			stmt.setInt(2, idServer);
			rs = stmt.executeQuery();
			List<ServiceInstance> services = new LinkedList<ServiceInstance>();
			while(rs.next()) {
				ServiceInstance service = new ServiceInstance();
				service.setId(rs.getInt("id"));
				service.setPort(rs.getInt("port"));
				service.setName(rs.getString("name"));
				service.setMailAdmin(rs.getString("mail_admin"));
				service.setInstitution(rs.getString("institution"));
				DateFormat formatter = new DateFormat(getLocale().getLanguage());
				Calendar c = Calendar.getInstance();
				c.clear();
				c.setTimeInMillis(rs.getTimestamp("startDate").getTime());
				service.setStartDate(formatter.formatDate(c));
				service.setStartTime(formatter.formatTime(c));
				if(rs.getTimestamp("endDate") != null) {
					c.clear();
					c.setTimeInMillis(rs.getTimestamp("endDate").getTime());
					service.setEndDate(formatter.formatDate(c));
					service.setEndTime(formatter.formatTime(c));
				}
				
				for(ServiceType serviceType: ServiceType.values()) {
					PreparedStatement pStmt = conn.prepareStatement(serviceType.getQuery().getSql());
					try {
						pStmt.setInt(1, service.getId());
						if(pStmt.executeQuery().next()) {					
							service.setType(serviceType);
							break;
						}
					} finally {
						pStmt.close();
					}
				}				
				services.add(service);
			}
			rs.close();
			stmt.close();
			server.setServices(services);
		} catch(SQLException e) {
			//System.out.println("Error Message ServerAction " + e.getMessage());
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}

	public void setIdServer(Integer idServer) {
		this.idServer = idServer;
	}
	public Integer getIdServer() {
		return idServer;
	}
	public void setIdProject(Integer idProject) {
		this.idProject = idProject;
	}
	public Integer getIdProject() {
		return idProject;
	}
	public void setServer(Host server) {
		this.server = server;
	}
	public Host getServer() {
		return server;
	}

	@Override
	public void setUserSession(UserSession userSession) {
		this.userSession = userSession;
	}
}
