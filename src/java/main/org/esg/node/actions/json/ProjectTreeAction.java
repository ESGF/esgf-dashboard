package org.esg.node.actions.json;

import java.sql.*;
import java.util.*;
import org.esg.node.utils.*;
import org.esg.node.beans.TreeNode;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class ProjectTreeAction extends ActionSupport implements UserAware {
	private static final long serialVersionUID = 1L;
	private List<TreeNode> projects = null;
	/*private Integer timeSpan = null;
	private Long endDate = null;
	private Calendar endDateTime = null;*/
	private UserSession userSession = null;

	@Override
	public void validate() {
		/*if(timeSpan == null || timeSpan <= 0)
			timeSpan = Integer.parseInt(getText("default.timespan"));
		endDateTime = Calendar.getInstance();
		if(endDate == null)
			return;
		try {
			endDateTime.clear();
			endDateTime.setTimeInMillis(endDate*1000l);
		} catch(Exception e) {
			addActionError("");
			return;
		}*/
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt1 = conn.prepareStatement(SqlQuery.PROJECTS.getSql());
			stmt1.setInt(1, userSession.getId());
			ResultSet rs1 = stmt1.executeQuery();
			PreparedStatement stmt2 = conn.prepareStatement(SqlQuery.HOSTS_BY_PROJECT.getSql());
			PreparedStatement stmt4 = conn.prepareStatement(SqlQuery.PROJECT_CONTAINS_OPENDAP_SERVICES.getSql());
			projects = new LinkedList<TreeNode>();
			// AvgHostActivity avgHostActivity = new AvgHostActivity(conn);
			DateFormat formatter = new DateFormat(getLocale().getLanguage());
			while(rs1.next()) {
				TreeNode project = new TreeNode();
				project.setMyObject(rs1.getInt("id"));
				project.setText(rs1.getString("name"));
				project.setExpanded(false);
				project.setCls("treeProject");
				Timestamp d = rs1.getTimestamp("endDate");
				if(d == null) 
					project.setIconCls("treeProjectYesIcon");
				else {
					Calendar endDate = Calendar.getInstance();
					endDate.clear();
					endDate.setTimeInMillis(d.getTime());
					project.setIconCls("treeProjectNoIcon");					
					project.setQtip(getText("treeNode.project.quickTip.text", "End Date: {0}", new String[] { formatter.formatDate(endDate) }));
				}
				stmt2.clearParameters();
				stmt2.setInt(1, rs1.getInt("id"));
				ResultSet rs2 = stmt2.executeQuery();
				List<TreeNode> servers = new LinkedList<TreeNode>();				
				while(rs2.next()) {
					TreeNode server = new TreeNode();
					server.setMyObject(rs2.getInt("id"));
					server.setLeaf(true);
					String name = rs2.getString("name")==null? rs2.getString("ip"): rs2.getString("name");
					/*
					Number activity = avgHostActivity.getHostActivity((Integer)project.getMyObject(), (Integer)server.getMyObject(), timeSpan, endDateTime);
					String value = getText("treeNode.server.activity.text.unknown", "Unknown");
					if(activity != null)
						value = Math.round(activity.doubleValue() * 100.) / 100. + "%";		*/			
					server.setText(name);
					server.setCls("treeServer");
					server.setIconCls("treeServerIcon");
					server.setQtip(getText("treeNode.server.quickTip.text", "Available Services: {0}", new String[] { rs2.getString("numInstances")/*, value*/ }));
					servers.add(server);
				}
				rs2.close();
				stmt4.clearParameters();
				stmt4.setInt(1, rs1.getInt("id"));
				ResultSet rs4 = stmt4.executeQuery();
				if(rs4.next()) {
					TreeNode registry = new TreeNode();
					registry.setLeaf(true);
					registry.setText(getText("treeNode.registry.opendap"));
					registry.setCls("treeServer");
					registry.setMyObject("opendapRegistry");
					servers.add(registry);
				}
				rs4.close();
				project.setChildren(servers);
				projects.add(project);
			}
			stmt2.close();
			stmt4.close();
			rs1.close();
			stmt1.close();
			// avgHostActivity.close();			
		} catch(SQLException e) {
			//System.out.println("Error MessageProjectTreeAction " + e.getMessage());
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;		
	}

	public void setProjects(List<TreeNode> projects) {
		this.projects = projects;
	}
	public List<TreeNode> getProjects() {
		return projects;
	}	
	/*public Integer getTimeSpan() {
		return timeSpan;
	}

	public void setTimeSpan(Integer timeSpan) {
		this.timeSpan = timeSpan;
	}

	public void setEndDate(Long endDate) {
		this.endDate = endDate;
	}

	public Long getEndDate() {
		return endDate;
	}*/

	@Override
	public void setUserSession(UserSession userSession) {
		this.userSession  = userSession;
	}
}
