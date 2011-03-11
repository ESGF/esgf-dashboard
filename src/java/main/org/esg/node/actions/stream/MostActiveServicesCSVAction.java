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
		try {
			int pivot = GridBaseAction.MAP.containsKey(super.sort)? GridBaseAction.MAP.get(super.sort): 0;
			conn = Constants.DATASOURCE.getConnection();
			CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_MOST_ACTIVE_SERVICES.getSql());
			cStmt.setInt(1, idProject);
			cStmt.setInt(2, idServer);
			Calendar c = (Calendar) super.endDateTime.clone();
			cStmt.setTimestamp(4, new Timestamp(c.getTimeInMillis()));
			c.add(Calendar.MINUTE, -Constants.TIME_SPANS[pivot]);
			cStmt.setTimestamp(3, new Timestamp(c.getTimeInMillis()));
			cStmt.setInt(5, start);
			cStmt.setInt(6, limit);
			cStmt.setString(7, MostActiveServicesAction.FIELD_MAP.containsKey(sort)? MostActiveServicesAction.FIELD_MAP.get(sort): "percentage");
			cStmt.setBoolean(8, super.getDir());
			
			ResultSet rs = cStmt.executeQuery();
			CsvWriter csv = new CsvWriter();
			csv.setDelimiter(';');
			csv.writeRecord(new String[] { "id", "name", "port", "percentages0", "percentages1", "percentages2", "percentages3", "percentages4" });
			AvgServiceActivity avgServiceActivity = new AvgServiceActivity(conn);			
			while(rs.next()) {
				Integer idService = rs.getInt("s.id");
				csv.write(rs.getString("s.id"));
				csv.write(rs.getString("s.name"));
				csv.write(rs.getString("s.port"));
				Number[] percentages = new Number[Constants.TIME_SPANS.length];
				Arrays.fill(percentages, null);
				percentages[pivot] = rs.getBigDecimal("percentage");
				if(percentages[pivot] != null)
					percentages[pivot] = Math.round(percentages[pivot].doubleValue()*100.)/100.;
				
				Number temp = null;
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					if(index != pivot)
						if((temp=avgServiceActivity.getServiceActivity(idProject, idService, Constants.TIME_SPANS[index], super.endDateTime)) != null)
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
