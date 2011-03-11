package org.esg.node.actions.stream;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.sql.*;
import java.util.*;
import org.esg.node.utils.*;
import org.esg.node.actions.base.GridBaseAction;
import org.esg.node.actions.json.MostActiveHostsAction;

/**
 * @author University of Salento and CMCC
 */
public class MostActiveHostsCSVAction extends GridBaseAction {
	private static final long serialVersionUID = 1L;

	private Integer idProject = null;
	private Integer numHosts = null;
	private InputStream stream = null;
	
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
		try {
			int pivot = GridBaseAction.MAP.containsKey(super.sort)? GridBaseAction.MAP.get(super.sort): 0;
			conn = Constants.DATASOURCE.getConnection();
			CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_MOST_ACTIVE_HOST.getSql());
			cStmt.setInt(1, idProject);
			Calendar c = (Calendar) super.endDateTime.clone();
			cStmt.setTimestamp(3, new Timestamp(c.getTimeInMillis()));
			c.add(Calendar.MINUTE, -Constants.TIME_SPANS[pivot]);
			cStmt.setTimestamp(2, new Timestamp(c.getTimeInMillis()));
			cStmt.setInt(4, super.start);
			cStmt.setInt(5, super.limit);
			cStmt.setString(6, MostActiveHostsAction.FIELD_MAP.containsKey(sort)? MostActiveHostsAction.FIELD_MAP.get(sort): "average");
			cStmt.setBoolean(7, super.getDir());
			
			ResultSet rs = cStmt.executeQuery();
			
			CsvWriter csv = new CsvWriter();
			csv.setDelimiter(';');
			csv.writeRecord(new String[] { "id", "name", "ip", "percentages0", "percentages1", "percentages2", "percentages3", "percentages4" });
			AvgHostActivity avgHostActivity = new AvgHostActivity(conn);
			while(rs.next()) {
				Integer idHost = rs.getInt("t.idHost");
				csv.write(rs.getString("t.idHost"));
				csv.write(rs.getString("h.name"));
				csv.write(rs.getString("h.ip"));
				Number[] percentages = new Number[Constants.TIME_SPANS.length];
				Arrays.fill(percentages, null);
				
				percentages[pivot] = rs.getBigDecimal("average");
				if(percentages[pivot] != null)
					percentages[pivot] = Math.round(percentages[pivot].doubleValue()*100.)/100.;
				Number temp = null;
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					if(index != pivot)
						if((temp=avgHostActivity.getHostActivity(idProject, idHost, Constants.TIME_SPANS[index], super.endDateTime)) != null)
							percentages[index] = Math.round(temp.doubleValue()*100.)/100.;
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					csv.write(percentages[index]==null? null: percentages[index].toString());
				csv.endRecord();				
			}
			rs.close();
			cStmt.close();
			avgHostActivity.close();
			stream = new ByteArrayInputStream(csv.getOutputStream().toByteArray());
			csv.close();
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

	public Integer getNumHosts() {
		return numHosts;
	}

	public void setNumHosts(Integer numHosts) {
		this.numHosts = numHosts;
	}

	public void setStream(InputStream stream) {
		this.stream = stream;
	}

	public InputStream getStream() {
		return stream;
	}
}
