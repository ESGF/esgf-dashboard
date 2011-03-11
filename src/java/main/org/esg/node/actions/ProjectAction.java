package org.esg.node.actions;

import java.sql.*;
import java.util.Calendar;
import org.esg.node.utils.*;

import com.opensymphony.xwork2.ActionSupport;
import org.esg.node.beans.Project; 

/**
 * @author University of Salento and CMCC
 */
public class ProjectAction extends ActionSupport implements UserAware {
	private static final long serialVersionUID = 1L;

	private Integer idProject = null;
	private Project project = null;
	private UserSession userSession = null;
	
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
			if(!rs.next()) 
				return INPUT;
			rs.close();
			stmt.close();
			stmt = conn.prepareStatement(SqlQuery.PROJECT_BY_ID.getSql());
			stmt.setInt(1, idProject);
			rs = stmt.executeQuery();
			if(rs.next()) {
				project = new Project();
				project.setId(idProject);
				project.setName(rs.getString("name"));
				project.setDescription(rs.getString("description"));
				Calendar c = Calendar.getInstance();
				c.clear();
				c.setTimeInMillis(rs.getTimestamp("startDate").getTime());
				DateFormat formatter = new DateFormat(getLocale().getLanguage());
				project.setStartDate(formatter.formatDate(c));
				project.setStartTime(formatter.formatTime(c));
				String date, time;
				if(rs.getTimestamp("endDate") == null) date = time = null;
				else {
					c.clear();
					c.setTimeInMillis(rs.getTimestamp("endDate").getTime());
					date = formatter.formatDate(c);
					time = formatter.formatTime(c);
				}
				project.setEndDate(date);
				project.setEndTime(time);
				project.setHostsNumber(rs.getInt("hostsNumber"));
				project.setServicesNumber(rs.getInt("servicesNumber"));
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

	public void setIdProject(Integer idProject) {
		this.idProject = idProject;
	}
	public Integer getIdProject() {
		return idProject;
	}
	public void setProject(Project project) {
		this.project = project;
	}
	public Project getProject() {
		return project;
	}

	@Override
	public void setUserSession(UserSession userSession) {
		this.userSession  = userSession;
	}
}
