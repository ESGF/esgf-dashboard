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
			System.out.println("PROJECT_HOST_POSITION(0) ="+ stmt.toString());
			
			ResultSet rs = stmt.executeQuery();
			hosts = new LinkedList<HostPosition>();
			AvgHostActivity avgHostActivity = new AvgHostActivity(conn);
			CountServicesOnHost countServicesOnHost = new CountServicesOnHost(conn, true);			
			while(rs.next()) { 
				if(rs.getBigDecimal("latitude") == null || rs.getBigDecimal("longitude") == null) continue;
				HostPosition host = new HostPosition();
				host.setId(rs.getInt("id"));
				host.setRegUsers(rs.getInt("regusers"));
				host.setName(rs.getString("name"));
				host.setIp(rs.getString("ip"));
				host.setLatitude(rs.getBigDecimal("latitude"));
				host.setLongitude(rs.getBigDecimal("longitude"));
				host.setCity(rs.getString("city"));
				host.setRegUsers(rs.getInt("regusers"));
				Number activity = avgHostActivity.getHostActivity(conn,idProjects.get(0), host.getId(), timeSpan, endDateTime);
				host.setActivity(activity == null? null: Math.round(activity.doubleValue()*100.)/100.);
				host.setIdProjects(idProjects);
				host.setNumInstances(countServicesOnHost.getCount(idProjects.get(0), host.getId()));
				hosts.add(host);
				//System.out.println("HostPositionAction execute Un nuovo host � stato aggiunto");
			}
			rs.close();
			stmt.close();
			avgHostActivity.close();
			countServicesOnHost.close();
		} catch(SQLException e) {
			//System.out.println("Error Message HostPositionAction execute " + e.getMessage());
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}

	
	
	public String execute1() throws Exception {
		Connection conn = null;		
		try {
			//System.out.println("HOST POSITION ACTION = Start execute1");
			
			conn = Constants.DATASOURCE.getConnection();			
			String arg = "?";
			for(int index = 1; index < idProjects.size(); index ++)
				arg += ", ?"; 
			String query = SqlQuery.PROJECT_HOSTS_POSITION.getSql().replace("?", arg);
			System.out.println("PROJECT_HOSTS_POSITION query stmt1 = "+ query);
			
			PreparedStatement stmt1 = conn.prepareStatement(query);
			
			query = SqlQuery.PROJECTS_ID_FOR_HOST.getSql().replace("#", arg);
			//System.out.println("PROJECTS_ID_FOR_HOST query stmt2 = "+ query);
			
			PreparedStatement stmt2 = conn.prepareStatement(query);			
			
			for(int index = 0; index < idProjects.size(); index ++) 
				stmt1.setInt(index+1, idProjects.get(index));
			//System.out.println("PROJECT_HOSTS_POSITION query stmt1 = " + stmt1.toString());
			ResultSet rs1 = stmt1.executeQuery();
			
			AvgHostActivity avgHostActivity = new AvgHostActivity(conn);
			//System.out.println("HostPosition = AvgHostActivity eseguita ");
			
			CountServicesOnHost countServicesOnHost = new CountServicesOnHost(conn, false);
			//System.out.println("HostPosition = CountServicesOnHost eseguita ");
			
			hosts = new LinkedList<HostPosition>();
			
			while(rs1.next()) {
				//System.out.println("HostPositionAction execute1 iterazione");
				if(rs1.getBigDecimal("latitude") == null || rs1.getBigDecimal("longitude") == null) continue;
				HostPosition host = new HostPosition();
				host.setId(rs1.getInt("id"));
				host.setName(rs1.getString("name"));
				host.setIp(rs1.getString("ip"));
				host.setRegUsers(rs1.getInt("regusers"));
				host.setLatitude(rs1.getBigDecimal("latitude"));
				host.setLongitude(rs1.getBigDecimal("longitude"));
				host.setCity(rs1.getString("city"));
				Number activity = avgHostActivity.getHostActivity(conn,null, host.getId(), timeSpan, endDateTime);
				host.setActivity(activity == null? null: Math.round(activity.doubleValue()*100.)/100.);
				List<Integer> idProjectsForHost = new LinkedList<Integer>();
				stmt2.clearParameters();
				for(int index = 0; index < idProjects.size(); index ++) 
					stmt2.setInt(index+1, idProjects.get(index));
				stmt2.setInt(idProjects.size()+1, host.getId());
				
				//System.out.println("Query stmt2 " + stmt2.toString());
				ResultSet rs2 = stmt2.executeQuery();
				while(rs2.next())
					idProjectsForHost.add(rs2.getInt("id"));
				rs2.close();
				host.setIdProjects(idProjectsForHost);
				host.setNumInstances(countServicesOnHost.getCount(host.getId()));
				//System.out.println("HostPositionAction execute1 nuovo host aggiunto");
				hosts.add(host);
			}		
			rs1.close();
			stmt1.close();
			stmt2.close();
			avgHostActivity.close();
			countServicesOnHost.close();
		} catch(SQLException e) {
			//System.out.println("Error Message HostPositionAction execute1 " + e.getMessage());
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		return SUCCESS;		
	}
	
	
	/* Start */
	
	public String execute2() throws Exception {
		Connection conn = null;
		try {
			
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.PROJECT_HOSTS_POSITION_REG_USERS.getSql());
			stmt.setInt(1, idProjects.get(0));
			System.out.println("PROJECT_HOST_POSITION(0) ="+ stmt.toString());
			
			ResultSet rs = stmt.executeQuery();
			hosts = new LinkedList<HostPosition>();
			AvgHostActivity avgHostActivity = new AvgHostActivity(conn);
			CountServicesOnHost countServicesOnHost = new CountServicesOnHost(conn, true);			
			while(rs.next()) { 
				if(rs.getBigDecimal("latitude") == null || rs.getBigDecimal("longitude") == null) continue;
				HostPosition host = new HostPosition();
				host.setId(rs.getInt("id"));
				host.setRegUsers(rs.getInt("regusers"));
				host.setName(rs.getString("name"));
				host.setIp(rs.getString("ip"));
				host.setLatitude(rs.getBigDecimal("latitude"));
				host.setLongitude(rs.getBigDecimal("longitude"));
				host.setCity(rs.getString("city"));
				host.setRegUsers(rs.getInt("regusers"));
				Number activity = avgHostActivity.getHostActivity(conn,idProjects.get(0), host.getId(), timeSpan, endDateTime);
				host.setActivity(activity == null? null: Math.round(activity.doubleValue()*100.)/100.);
				host.setIdProjects(idProjects);
				host.setNumInstances(countServicesOnHost.getCount(idProjects.get(0), host.getId()));
				hosts.add(host);
				//System.out.println("HostPositionAction execute Un nuovo host � stato aggiunto");
			}
			rs.close();
			stmt.close();
			avgHostActivity.close();
			countServicesOnHost.close();
		} catch(SQLException e) {
			//System.out.println("Error Message HostPositionAction execute " + e.getMessage());
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}

	
	/* end */
	
	
	
	
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
