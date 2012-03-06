package org.esg.node.actions.json;

import java.sql.*;
import java.util.*;

import org.esg.node.utils.*;
import org.esg.node.actions.base.GridBaseAction;
import org.esg.node.beans.*;

/**
 * @author University of Salento and CMCC
 */
public class MostActiveHostsAction extends GridBaseAction {
	private static final long serialVersionUID = 1L;

	private Integer idProject = null;
	private Integer numHosts = null;
	private MostActiveHosts hostsList = null;
	public final static Map<String, String> FIELD_MAP = new HashMap<String, String>();
	
	static {
		FIELD_MAP.put("id", "idHost");
		FIELD_MAP.put("name", "h.name");
		FIELD_MAP.put("ip", "h.ip");
		FIELD_MAP.put("average", "average");
	}
	
	@Override
	public void validate() {
		super.validate();
		if(idProject == null || numHosts == null) {
			this.addActionError("");
			return;
		}
		if(super.limit == null)
			super.limit = numHosts;
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;	
		String query;
		PreparedStatement cStmt = null;
		try {
			//int pivot = GridBaseAction.MAP.containsKey(super.sort)? GridBaseAction.MAP.get(super.sort): 0;
			int pivot= 4;
			
			conn = Constants.DATASOURCE.getConnection();
			//CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_MOST_ACTIVE_HOST.getSql());
						
			//cStmt.setInt(1, idProject);
			Calendar c = (Calendar) super.endDateTime.clone();
			Calendar cs = (Calendar) super.endDateTime.clone();
			
			//cStmt.setTimestamp(3, new Timestamp(c.getTimeInMillis()));
			cs.add(Calendar.MINUTE, -Constants.TIME_SPANS[pivot]);
			//cStmt.setTimestamp(2, new Timestamp(c.getTimeInMillis()));
			//cStmt.setInt(4, super.start);
			//cStmt.setInt(5, super.limit);
			//cStmt.setString(6, FIELD_MAP.containsKey(sort)? FIELD_MAP.get(sort): "average");
			//cStmt.setBoolean(7, super.getDir());
			
			query = "SELECT t.idHost, h.name, h.ip, AVG(percentage) AS average FROM (SELECT s.idHost, 100* ( SELECT COUNT(*) FROM  esgf_dashboard.service_status WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "') / (SELECT COUNT(*) FROM  esgf_dashboard.service_status WHERE idServiceInstance=s.id AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "') AS percentage FROM  esgf_dashboard.service_instance s INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id WHERE u.idProject=";
			query = query + idProject;
			query = query + " AND u.endDate IS NULL) t INNER JOIN  esgf_dashboard.host h ON h.id=t.idHost GROUP BY t.idHost, h.name, h.ip ORDER BY ";
			query = query + (FIELD_MAP.containsKey(sort)? FIELD_MAP.get(sort): "average");
						
			if (super.getDir()) 
				query = query + " ASC OFFSET " + super.start + " LIMIT ";
			else 
				query = query + " DESC OFFSET " + super.start + " LIMIT ";
						
			query = query + super.limit;
			
			//System.out.println("Start: "+ super.start + " ||| size: "+ super.limit);
			//System.out.println("!!! MostActiveHost Query = "+query);
			cStmt = conn.prepareStatement(query);
			
			ResultSet rs = cStmt.executeQuery();
			
			hostsList = new MostActiveHosts(numHosts);
			List<HostActivity> hosts = new LinkedList<HostActivity>();
			AvgHostActivity avgHostActivity = new AvgHostActivity(conn);
			while(rs.next()) {
				HostActivity host = new HostActivity();
				host.setId(rs.getInt("idHost"));
				host.setName(rs.getString("name"));
				host.setIp(rs.getString("ip"));
				List<Number> percentages = new ArrayList<Number>(Constants.TIME_SPANS.length);
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					percentages.add(null);
				
				percentages.set(pivot, rs.getBigDecimal("average"));
				if(percentages.get(pivot) != null)
					percentages.set(pivot, Math.round(percentages.get(pivot).doubleValue()*100.)/100.);
				Number temp = null;
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					if(index != pivot)
						if((temp=avgHostActivity.getHostActivity(conn, idProject, host.getId(), Constants.TIME_SPANS[index], super.endDateTime)) != null)
							percentages.set(index, Math.round(temp.doubleValue()*100.)/100.);
				host.setPercentages(percentages);				
				hosts.add(host);
			}
			rs.close();
			cStmt.close();
			avgHostActivity.close();
			hostsList.setHosts(hosts);
		} catch(SQLException e) {
			//System.out.println("%%% MOST ACTIVE HOST ERROR ="+ e.getMessage());
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

	public void setHostsList(MostActiveHosts hostsList) {
		this.hostsList = hostsList;
	}

	public MostActiveHosts getHostsList() {
		return hostsList;
	}

	public Integer getNumHosts() {
		return numHosts;
	}

	public void setNumHosts(Integer numHosts) {
		this.numHosts = numHosts;
	}
}
