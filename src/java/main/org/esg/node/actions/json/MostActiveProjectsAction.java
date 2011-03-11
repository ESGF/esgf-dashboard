package org.esg.node.actions.json;

import java.util.*;

import org.esg.node.actions.base.GridBaseAction;
import org.esg.node.beans.*;
import java.sql.*;
import org.esg.node.utils.*;

/**
 * @author University of Salento and CMCC
 */
public class MostActiveProjectsAction extends GridBaseAction implements UserAware {
	private static final long serialVersionUID = 1L;

	private MostActiveProjects projectsList = null;
	private Integer numProjects = null;
	private UserSession userSession = null;
	public final static Map<String, String> FIELD_MAP = new HashMap<String, String>();
	
	static {
		FIELD_MAP.put("id", "p.id");
		FIELD_MAP.put("name", "p.name");
		FIELD_MAP.put("startDate", "p.startDate");
		FIELD_MAP.put("endDate", "p.endDate");
		FIELD_MAP.put("average", "average");
	}
	
	@Override
	public void validate() {
		super.validate();
		if(numProjects == null) {
			this.addActionError("");
			return;
		}
		if(super.limit == null)
			super.limit = numProjects;
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			int pivot = GridBaseAction.MAP.containsKey(super.sort)? GridBaseAction.MAP.get(super.sort): 0;
			conn = Constants.DATASOURCE.getConnection();
			CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_MOST_ACTIVE_PROJECTS.getSql());
			Calendar c = (Calendar) super.endDateTime.clone();
			cStmt.setTimestamp(2, new Timestamp(c.getTimeInMillis()));
			c.add(Calendar.MINUTE, -Constants.TIME_SPANS[pivot]);
			cStmt.setTimestamp(1, new Timestamp(c.getTimeInMillis()));
			cStmt.setInt(3, super.start);
			cStmt.setInt(4, super.limit);
			cStmt.setString(5, FIELD_MAP.containsKey(sort)? FIELD_MAP.get(sort): "average");
			cStmt.setBoolean(6, super.getDir());
			cStmt.setInt(7, userSession.getId());
			ResultSet rs = cStmt.executeQuery();
			projectsList = new MostActiveProjects(numProjects);			
			List<ProjectActivity> projects = new LinkedList<ProjectActivity>();
			AvgProjectActivity avgProjectActivity = new AvgProjectActivity(conn);
			while(rs.next()) {
				ProjectActivity project = new ProjectActivity();
				project.setId(rs.getInt("p.id"));
				project.setName(rs.getString("p.name"));
				DateFormat formatter = new DateFormat(getLocale().getLanguage());
				c.clear();
				c.setTimeInMillis(rs.getTimestamp("p.startDate").getTime());
				project.setStartDate(formatter.formatDate(c));
				project.setStartTime(formatter.formatTime(c));
								
				String date, time;
				if(rs.getTimestamp("p.endDate") == null) date = time = null;
				else {
					c.clear();
					c.setTimeInMillis(rs.getTimestamp("p.endDate").getTime());
					date = formatter.formatDate(c);
					time = formatter.formatTime(c);
				}
				project.setEndDate(date);
				project.setEndTime(time);
				List<Number> percentages = new ArrayList<Number>(Constants.TIME_SPANS.length);
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					percentages.add(null);
				percentages.set(pivot, rs.getBigDecimal("average"));
				if(percentages.get(pivot) != null)
					percentages.set(pivot, Math.round(percentages.get(pivot).doubleValue()*100.)/100.);
				 
				Number temp = null;
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++) {					
					c = (Calendar) super.endDateTime.clone();
					c.add(Calendar.MINUTE, -Constants.TIME_SPANS[index]);
					if(index != pivot)
						if((temp = avgProjectActivity.getProjectActivity(project.getId(), c, super.endDateTime)) != null)
							percentages.set(index, Math.round(temp.doubleValue()*100.)/100.);
				}
				project.setPercentages(percentages);
				projects.add(project);
			}
			rs.close();
			cStmt.close();
			avgProjectActivity.close();
			projectsList.setProjects(projects);
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		
		return SUCCESS;
	}

	public MostActiveProjects getProjectsList() {
		return projectsList;
	}

	public void setProjectsList(MostActiveProjects projectsList) {
		this.projectsList = projectsList;
	}

	public Integer getNumProjects() {
		return numProjects;
	}

	public void setNumProjects(Integer numProjects) {
		this.numProjects = numProjects;
	}

	@Override
	public void setUserSession(UserSession userSession) {
		this.userSession = userSession;
	}
}
