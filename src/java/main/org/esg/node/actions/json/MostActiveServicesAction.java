package org.esg.node.actions.json;

import org.esg.node.utils.*;

import java.sql.*;
import java.util.*;

import org.esg.node.actions.base.GridBaseAction;
import org.esg.node.beans.*;

/**
 * @author University of Salento and CMCC
 */
public class MostActiveServicesAction extends GridBaseAction {
	private static final long serialVersionUID = 1L;

	private Integer idProject = null;
	private Integer idServer = null;
	private Integer numInstances = null;
	private MostActiveServices servicesList = null;
	public final static Map<String, String> FIELD_MAP = new HashMap<String, String>();
	
	static {
		FIELD_MAP.put("id", "s.id");
		FIELD_MAP.put("name", "s.name");
		FIELD_MAP.put("port", "s.port");
		FIELD_MAP.put("average", "percentage");
	}	
	
	@Override
	public void validate() {
		super.validate();
		if(idProject == null || idServer == null || numInstances == null) {
			this.addActionError("");
			return;
		}
		if(super.limit == null)
			super.limit = numInstances;
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		String query;
		PreparedStatement cStmt = null;
		try {
			//int pivot = GridBaseAction.MAP.containsKey(super.sort)? GridBaseAction.MAP.get(super.sort): 0;
			int pivot = 4;
			conn = Constants.DATASOURCE.getConnection();
			//CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_MOST_ACTIVE_SERVICES.getSql());
			//cStmt.setInt(1, idProject);
			//cStmt.setInt(2, idServer);
			Calendar c = (Calendar) super.endDateTime.clone();
			Calendar cs = (Calendar) super.endDateTime.clone();
			//cStmt.setTimestamp(4, new Timestamp(c.getTimeInMillis()));
			cs.add(Calendar.MINUTE, -Constants.TIME_SPANS[pivot]);
			//cStmt.setTimestamp(3, new Timestamp(c.getTimeInMillis()));
			//cStmt.setInt(5, start);
			//cStmt.setInt(6, limit);
			//cStmt.setString(7, FIELD_MAP.containsKey(sort)? FIELD_MAP.get(sort): "percentage");
			//cStmt.setBoolean(8, super.getDir());
			
			query = "SELECT s.id, s.name, s.port, 100* ( SELECT COUNT(*) FROM service_status WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "') / (SELECT COUNT(*) FROM service_status WHERE idServiceInstance=s.id AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "') AS percentage FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id WHERE s.idHost=";
			query = query + idServer;
			query = query + " AND u.idProject=";
			query = query + idProject;
			query = query + " AND u.endDate IS NULL ORDER BY " + (FIELD_MAP.containsKey(sort)? FIELD_MAP.get(sort): "percentage");
						
			if (super.getDir()) 
				query = query + " ASC LIMIT ";
			else 
				query = query + " DESC LIMIT ";
						
			query = query + super.limit;
			
			//System.out.println("Start: "+ super.start + " ||| size: "+ super.limit);
			//System.out.println("!!! MostActiveService Query = "+query);
			cStmt = conn.prepareStatement(query);
			
			ResultSet rs = cStmt.executeQuery();
			
			servicesList = new MostActiveServices(numInstances);
			List<ServiceInstanceActivity> services = new LinkedList<ServiceInstanceActivity>(); 
			AvgServiceActivity avgServiceActivity = new AvgServiceActivity(conn);
			
			while(rs.next()) {
				ServiceInstanceActivity service = new ServiceInstanceActivity();
				service.setId(rs.getInt("id"));
				service.setName(rs.getString("name"));
				service.setPort(rs.getInt("port"));
				List<Number> percentages = new ArrayList<Number>(Constants.TIME_SPANS.length);
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					percentages.add(null);
				percentages.set(pivot, rs.getBigDecimal("percentage"));
				if(percentages.get(pivot) != null)
					percentages.set(pivot, Math.round(percentages.get(pivot).doubleValue()*100.)/100.);
				
				Number temp = null;
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					if(index != pivot)
						if((temp=avgServiceActivity.getServiceActivity(conn, idProject, service.getId(), Constants.TIME_SPANS[index], super.endDateTime)) != null)
							percentages.set(index, Math.round(temp.doubleValue()*100.)/100.);
				service.setPercentages(percentages);
				services.add(service);				
			}
			rs.close();
			cStmt.close();
			avgServiceActivity.close();
			servicesList.setServices(services);
		} catch(SQLException e) {
			//System.out.println("%%% MOST ACTIVE SERVICE ERROR ="+ e.getMessage());
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		
		return SUCCESS;
	}

	public Integer getIdProject() {
		return idProject;
	}

	public void setIdProject(Integer idProject) {
		this.idProject = idProject;
	}

	public Integer getIdServer() {
		return idServer;
	}

	public void setIdServer(Integer idServer) {
		this.idServer = idServer;
	}

	public MostActiveServices getServicesList() {
		return servicesList;
	}

	public void setServicesList(MostActiveServices servicesList) {
		this.servicesList = servicesList;
	}

	public void setNumInstances(Integer numInstances) {
		this.numInstances = numInstances;
	}

	public Integer getNumInstances() {
		return numInstances;
	}
}
