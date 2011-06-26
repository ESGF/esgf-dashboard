package org.esg.node.actions.stream;

import org.esg.node.utils.*;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.sql.*;
import java.util.*;
import org.esg.node.actions.base.GridBaseAction;
import org.esg.node.actions.json.MostActiveServicesAction;

/**
 * @author University of Salento and CMCC
 */
public class MostActiveServicesCSVAction extends GridBaseAction {
	private static final long serialVersionUID = 1L;

	private Integer idProject = null;
	private Integer idServer = null;
	private Integer numInstances = null;
	private InputStream stream = null;
	
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
			int pivot = GridBaseAction.MAP.containsKey(super.sort)? GridBaseAction.MAP.get(super.sort): 0;
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
			//cStmt.setString(7, MostActiveServicesAction.FIELD_MAP.containsKey(sort)? MostActiveServicesAction.FIELD_MAP.get(sort): "percentage");
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
			query = query + " AND u.endDate IS NULL ORDER BY " + (MostActiveServicesAction.FIELD_MAP.containsKey(sort)? MostActiveServicesAction.FIELD_MAP.get(sort): "percentage");
						
			if (super.getDir()) 
				query = query + " ASC LIMIT ";
			else 
				query = query + " DESC LIMIT ";
						
			query = query + super.limit;
			
			//System.out.println("Start: "+ super.start + " ||| size: "+ super.limit);
			//System.out.println("!!! MostActiveService Query = "+query);
			cStmt = conn.prepareStatement(query);
			
			ResultSet rs = cStmt.executeQuery();
			CsvWriter csv = new CsvWriter();
			csv.setDelimiter(';');
			csv.writeRecord(new String[] { "id", "name", "port", "percentages0", "percentages1", "percentages2", "percentages3", "percentages4" });
			AvgServiceActivity avgServiceActivity = new AvgServiceActivity(conn);			
			while(rs.next()) {
				Integer idService = rs.getInt("id");
				csv.write(rs.getString("id"));
				csv.write(rs.getString("name"));
				csv.write(rs.getString("port"));
				Number[] percentages = new Number[Constants.TIME_SPANS.length];
				Arrays.fill(percentages, null);
				percentages[pivot] = rs.getBigDecimal("percentage");
				if(percentages[pivot] != null)
					percentages[pivot] = Math.round(percentages[pivot].doubleValue()*100.)/100.;
				
				Number temp = null;
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					if(index != pivot)
						if((temp=avgServiceActivity.getServiceActivity(conn, idProject, idService, Constants.TIME_SPANS[index], super.endDateTime)) != null)
							percentages[index] = Math.round(temp.doubleValue()*100.)/100.;
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					csv.write(percentages[index]==null? null: percentages[index].toString());
				csv.endRecord();
			}
			rs.close();
			cStmt.close();
			avgServiceActivity.close();
			stream = new ByteArrayInputStream(csv.getOutputStream().toByteArray());
			csv.close();
		} catch(SQLException e) {
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

	public void setNumInstances(Integer numInstances) {
		this.numInstances = numInstances;
	}

	public Integer getNumInstances() {
		return numInstances;
	}

	public void setStream(InputStream stream) {
		this.stream = stream;
	}

	public InputStream getStream() {
		return stream;
	}
}
