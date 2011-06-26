package org.esg.node.actions.stream;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.sql.*;
import java.util.*;

import org.esg.node.utils.*;
import org.esg.node.actions.base.GridBaseAction;
import org.esg.node.actions.json.MostActiveProjectsAction;

public class MostActiveProjectsCSVAction extends GridBaseAction implements UserAware {
	private static final long serialVersionUID = 1L;
	
	private InputStream stream = null;
	private Integer numProjects = null;
	private UserSession userSession = null;
	
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
		PreparedStatement cStmt = null;
		String query;
		try {
			int pivot = GridBaseAction.MAP.containsKey(super.sort)? GridBaseAction.MAP.get(super.sort): 0;
			conn = Constants.DATASOURCE.getConnection();
			
			//CallableStatement cStmt = conn.prepareCall(SqlQuery.SP_MOST_ACTIVE_PROJECTS.getSql());
			
			Calendar c = (Calendar) super.endDateTime.clone();
			//cStmt.setTimestamp(2, new Timestamp(c.getTimeInMillis()));
		
			Calendar cs = (Calendar) super.endDateTime.clone(); 
			
			//cStmt.setTimestamp(2, new Timestamp(c.getTimeInMillis()));
			cs.add(Calendar.MINUTE, -Constants.TIME_SPANS[pivot]);
			query = "SELECT p.id, p.name, p.startDate, p.endDate, AVG(percentage) AS average FROM (SELECT u.idProject, 100* ( SELECT COUNT(*) FROM service_status WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "') / (SELECT COUNT(*) FROM service_status WHERE idServiceInstance=s.id AND timestamp BETWEEN '";
			query = query + new Timestamp(cs.getTimeInMillis());
			query = query + "' AND '";
			query = query + new Timestamp(c.getTimeInMillis());
			query = query + "') AS percentage FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id INNER JOIN join1 j ON j.idProject=u.idProject WHERE u.endDate IS NULL AND j.idUser=";
			query = query + userSession.getId();
			query = query + ") t INNER JOIN project_dash p ON p.id=t.idProject GROUP BY p.id, p.name, p.startDate, p.endDate ORDER BY ";
			query = query + (MostActiveProjectsAction.FIELD_MAP.containsKey(sort)? MostActiveProjectsAction.FIELD_MAP.get(sort): "average");
			
			if (super.getDir()) 
				query = query + " ASC LIMIT ";
			else 
				query = query + " DESC LIMIT ";
			
			query = query + super.limit;
			
			//System.out.println("Start: "+ super.start + " ||| size: "+ super.limit);
			//System.out.println("!!! MostActiveProjectsCSV Query = "+query);
			cStmt = conn.prepareStatement(query);
			
			//cStmt.setTimestamp(1, new Timestamp(c.getTimeInMillis()));
			//cStmt.setInt(3, super.start);
			//cStmt.setInt(4, super.limit);
			//cStmt.setString(5, MostActiveProjectsAction.FIELD_MAP.containsKey(sort)? MostActiveProjectsAction.FIELD_MAP.get(sort): "average");
			//cStmt.setBoolean(6, super.getDir());
			//cStmt.setInt(7, userSession.getId());
			
			ResultSet rs = cStmt.executeQuery();
			
			CsvWriter csv = new CsvWriter();
			csv.setDelimiter(';');
			csv.writeRecord(new String[] { "id", "name", "startDate", "startTime", "endDate", "endTime", "percentages0", "percentages1", "percentages2", "percentages3", "percentages4" });
			AvgProjectActivity avgProjectActivity = new AvgProjectActivity(conn);
			while(rs.next()) {
				Integer id = rs.getInt("id");
				csv.write(rs.getString("id"));
				csv.write(rs.getString("name"));				
				DateFormat formatter = new DateFormat(getLocale().getLanguage());
				c.clear();
				c.setTimeInMillis(rs.getTimestamp("startDate").getTime());
				csv.write(formatter.formatDate(c));
				csv.write(formatter.formatTime(c));
								
				String date, time;
				if(rs.getTimestamp("endDate") == null) date = time = null;
				else {
					c.clear();
					c.setTimeInMillis(rs.getTimestamp("endDate").getTime());
					date = formatter.formatDate(c);
					time = formatter.formatTime(c);
				}
				csv.write(date);
				csv.write(time);
				Number[] percentages = new Number[Constants.TIME_SPANS.length];
				Arrays.fill(percentages, null);
				
				percentages[pivot] = rs.getBigDecimal("average");
				if(percentages[pivot] != null)
					percentages[pivot] = Math.round(percentages[pivot].doubleValue()*100.)/100.;
				 
				Number temp = null;
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++) {
					c = (Calendar) super.endDateTime.clone();
					c.add(Calendar.MINUTE, -Constants.TIME_SPANS[index]);
					if(index != pivot)
						if((temp = avgProjectActivity.getProjectActivity(conn, id, c, super.endDateTime)) != null)
							percentages[index] = Math.round(temp.doubleValue()*100.)/100.;
				}
				for(int index = 0; index < Constants.TIME_SPANS.length; index ++)
					csv.write(percentages[index]==null? null: percentages[index].toString());
				csv.endRecord();
			}
			rs.close();
			cStmt.close();
			avgProjectActivity.close();
			stream = new ByteArrayInputStream(csv.getOutputStream().toByteArray());
			csv.close();
		} catch(Exception e) {e.printStackTrace();
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		

		return SUCCESS;
	}
	
	public void setStream(InputStream stream) {
		this.stream = stream;
	}

	public InputStream getStream() {
		return stream;
	}

	public Integer getNumProjects() {
		return numProjects;
	}

	public void setNumProjects(Integer numProjects) {
		this.numProjects = numProjects;
	}

	@Override
	public void setUserSession(UserSession userSession) {
		this.userSession  = userSession;
	}
}
