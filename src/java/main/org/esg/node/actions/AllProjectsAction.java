package org.esg.node.actions;

import org.esg.node.beans.ProjectBase;
import com.opensymphony.xwork2.ActionSupport;
import java.util.*;
import java.sql.*;
import org.esg.node.utils.*;

/**
 * @author University of Salento and CMCC
 */
public class AllProjectsAction extends ActionSupport implements UserAware {
	private static final long serialVersionUID = 1L;
	private List<ProjectBase> projects = null;
	private String startDate = null;
	private Integer numServices = null;
	private Integer numHosts = null;
	private Integer numUsers = null;
	private UserSession userSession = null;
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			//System.out.println("ALLProjects "+SqlQuery.ALL_PROJECTS_ID_AND_NAME.getSql());
			//System.out.println("ALLProjects UserID=" + userSession.getId());
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.ALL_PROJECTS_ID_AND_NAME.getSql());
			stmt.setInt(1, userSession.getId());
			//System.out.println("ALLProjects Query=" + stmt.toString());
			
			ResultSet rs = stmt.executeQuery();
			projects = new LinkedList<ProjectBase>();
			while(rs.next()) {
				projects.add(new ProjectBase(rs.getInt("id"), rs.getString("name")));
				//System.out.println("inside the first query cycle!");
			}	
			rs.close();
			stmt.close();
			
			//System.out.println("ALL_PROJECTS_START_DATE ="+ SqlQuery.ALL_PROJECTS_START_DATE.getSql());
			
			stmt = conn.prepareStatement(SqlQuery.ALL_PROJECTS_START_DATE.getSql());
			stmt.setInt(1, userSession.getId());
			rs = stmt.executeQuery();
			if(rs.next() && rs.getTimestamp("startDate") != null) {
				DateFormat formatter = new DateFormat(getLocale().getLanguage());
				Calendar c = Calendar.getInstance();
				c.setTimeInMillis(rs.getTimestamp("startDate").getTime());
				startDate = formatter.formatDateTime(c);
				//System.out.println("inside the second query");
			}
			rs.close();
			stmt.close();
	
			//System.out.println("ALL_PROJECTS_NUM_HOSTS ="+ SqlQuery.ALL_PROJECTS_NUM_HOSTS.getSql());

			stmt = conn.prepareStatement(SqlQuery.ALL_PROJECTS_NUM_HOSTS.getSql());
			stmt.setInt(1, userSession.getId());
			rs = stmt.executeQuery();
			if(rs.next()) 
				numHosts = rs.getInt("numHosts");
			rs.close();
			stmt.close();
			
			//System.out.println("ALL_PROJECTS_NUM_SERVICES ="+ SqlQuery.ALL_PROJECTS_NUM_SERVICES.getSql());
			
			stmt = conn.prepareStatement(SqlQuery.ALL_PROJECTS_NUM_SERVICES.getSql());
			stmt.setInt(1, userSession.getId());
			rs = stmt.executeQuery();			
			if(rs.next()) 
				numServices = rs.getInt("numServices");
			rs.close();
			stmt.close();	
			
			// Get the total number of users
			stmt = conn.prepareStatement(SqlQuery.ALL_PROJECTS_NUM_USERS.getSql());
			//stmt.setInt(1, userSession.getId());
			rs = stmt.executeQuery();			
			if(rs.next()) 
				numUsers = rs.getInt("numUsers");
			rs.close();
			stmt.close();	
			
		} catch(SQLException e) {
			//System.out.println("Error Message AllProjectsAction " + e.getMessage());
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		return SUCCESS;
	}	
	
	public void setProjects(List<ProjectBase> projects) {
		this.projects = projects;
	}

	public List<ProjectBase> getProjects() {
		return projects;
	}

	public String getStartDate() {
		return startDate;
	}

	public void setStartDate(String startDate) {
		this.startDate = startDate;
	}

	public Integer getNumServices() {
		return numServices;
	}

	public void setNumServices(Integer numServices) {
		this.numServices = numServices;
	}

	public Integer getNumHosts() {
		return numHosts;
	}

	public void setNumHosts(Integer numHosts) {
		this.numHosts = numHosts;
	}

	@Override
	public void setUserSession(UserSession userSession) {
		this.userSession  = userSession;
	}

	/**
	 * @return the numUsers
	 */
	public Integer getNumUsers() {
		return numUsers;
	}

	/**
	 * @param numUsers the numUsers to set
	 */
	public void setNumUsers(Integer numUsers) {
		this.numUsers = numUsers;
	}		
}
