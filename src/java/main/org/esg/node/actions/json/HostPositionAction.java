package org.esg.node.actions.json;

import org.esg.node.beans.HostPosition;
import com.opensymphony.xwork2.ActionSupport;
import java.sql.*;
import java.util.*;

import org.esg.node.utils.*;

/**
 * @author University of Salento and CMCC
 */
public class HostPositionAction extends ActionSupport {
	private static final long serialVersionUID = 1L;
	
	private List<HostPosition> hosts = null;
	private List<Integer> idProjects = null;
	private Integer timeSpan = null;
	private Long endDate = null;
	private Calendar endDateTime = null;
	
	@Override
	public void validate() {
		if(idProjects == null)
			this.addActionError("");
		if(timeSpan == null || timeSpan <= 0)
			timeSpan = Integer.parseInt(getText("default.timespan"));
		endDateTime = Calendar.getInstance();
		if(endDate == null)
			return;
		try {
			endDateTime.clear();
			endDateTime.setTimeInMillis(endDate*1000l);
		} catch(Exception e) {
			this.addActionError("");
			return;
		}
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.PROJECT_HOSTS_POSITION.getSql());
			stmt.setInt(1, idProjects.get(0));
			ResultSet rs = stmt.executeQuery();
			hosts = new LinkedList<HostPosition>();
			AvgHostActivity avgHostActivity = new AvgHostActivity(conn);
			CountServicesOnHost countServicesOnHost = new CountServicesOnHost(conn, true);			
			while(rs.next()) { 
				if(rs.getBigDecimal("h.latitude") == null || rs.getBigDecimal("h.longitude") == null) continue;
				HostPosition host = new HostPosition();
				host.setId(rs.getInt("h.id"));
				host.setName(rs.getString("h.name"));
				host.setIp(rs.getString("h.ip"));
				host.setLatitude(rs.getBigDecimal("h.latitude"));
				host.setLongitude(rs.getBigDecimal("h.longitude"));
				host.setCity(rs.getString("h.city"));
				Number activity = avgHostActivity.getHostActivity(idProjects.get(0), host.getId(), timeSpan, endDateTime);
				host.setActivity(activity == null? null: Math.round(activity.doubleValue()*100.)/100.);
				host.setIdProjects(idProjects);
				host.setNumInstances(countServicesOnHost.getCount(idProjects.get(0), host.getId()));
				hosts.add(host);
			}
			rs.close();
			stmt.close();
			avgHostActivity.close();
			countServicesOnHost.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}

	public String execute1() throws Exception {
		Connection conn = null;		
		try {
			conn = Constants.DATASOURCE.getConnection();			
			String arg = "?";
			for(int index = 1; index < idProjects.size(); index ++)
				arg += ", ?"; 
			String query = SqlQuery.PROJECT_HOSTS_POSITION.getSql().replace("?", arg);
			PreparedStatement stmt1 = conn.prepareStatement(query);
			query = SqlQuery.PROJECTS_ID_FOR_HOST.getSql().replace("#", arg);
			PreparedStatement stmt2 = conn.prepareStatement(query);
			for(int index = 0; index < idProjects.size(); index ++) 
				stmt1.setInt(index+1, idProjects.get(index));
			
			ResultSet rs1 = stmt1.executeQuery();
			AvgHostActivity avgHostActivity = new AvgHostActivity(conn);
			CountServicesOnHost countServicesOnHost = new CountServicesOnHost(conn, false);
			hosts = new LinkedList<HostPosition>();
			
			while(rs1.next()) {
				if(rs1.getBigDecimal("h.latitude") == null || rs1.getBigDecimal("h.longitude") == null) continue;
				HostPosition host = new HostPosition();
				host.setId(rs1.getInt("h.id"));
				host.setName(rs1.getString("h.name"));
				host.setIp(rs1.getString("h.ip"));
				host.setLatitude(rs1.getBigDecimal("h.latitude"));
				host.setLongitude(rs1.getBigDecimal("h.longitude"));
				host.setCity(rs1.getString("h.city"));
				Number activity = avgHostActivity.getHostActivity(null, host.getId(), timeSpan, endDateTime);
				host.setActivity(activity == null? null: Math.round(activity.doubleValue()*100.)/100.);
				List<Integer> idProjectsForHost = new LinkedList<Integer>();
				stmt2.clearParameters();
				for(int index = 0; index < idProjects.size(); index ++) 
					stmt2.setInt(index+1, idProjects.get(index));
				stmt2.setInt(idProjects.size()+1, host.getId());
				ResultSet rs2 = stmt2.executeQuery();
				while(rs2.next())
					idProjectsForHost.add(rs2.getInt("p.id"));
				rs2.close();
				host.setIdProjects(idProjectsForHost);
				host.setNumInstances(countServicesOnHost.getCount(host.getId()));
				hosts.add(host);
			}		
			rs1.close();
			stmt1.close();
			stmt2.close();
			avgHostActivity.close();
			countServicesOnHost.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		return SUCCESS;		
	}
	
	public List<HostPosition> getHosts() {
		return hosts;
	}

	public void setHosts(List<HostPosition> hosts) {
		this.hosts = hosts;
	}

	public List<Integer> getIdProjects() {
		return idProjects;
	}

	public void setIdProjects(List<Integer> idProjects) {
		this.idProjects = idProjects;
	}

	public void setTimeSpan(Integer timeSpan) {
		this.timeSpan = timeSpan;
	}

	public Integer getTimeSpan() {
		return timeSpan;
	}

	public void setEndDate(Long endDate) {
		this.endDate = endDate;
	}

	public Long getEndDate() {
		return endDate;
	}
}
