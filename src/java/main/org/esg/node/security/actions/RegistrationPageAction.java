package org.esg.node.security.actions;

import java.sql.*;
import java.util.*;
import org.esg.node.utils.*;
import org.esg.node.beans.*;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class RegistrationPageAction extends ActionSupport {
	private static final long serialVersionUID = 1L;

	private List<Country> countries = null;
	private List<ProjectBase> publicProjects = null;
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.COUNTRIES.getSql());
			ResultSet rs = stmt.executeQuery();
			countries = new LinkedList<Country>();
			while(rs.next())
				countries.add(new Country(rs.getInt("id"), rs.getString("name")));
			rs.close();
			stmt.close();
			publicProjects = new LinkedList<ProjectBase>();
			stmt = conn.prepareStatement(SqlQuery.PUBLIC_PROJECTS.getSql());
			rs = stmt.executeQuery();
			while(rs.next()) 
				publicProjects.add(new ProjectBase(rs.getInt("id"), rs.getString("name")));
			rs.close();
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}

	public void setCountries(List<Country> countries) {
		this.countries = countries;
	}

	public List<Country> getCountries() {
		return countries;
	}

	public void setPublicProjects(List<ProjectBase> publicProjects) {
		this.publicProjects = publicProjects;
	}

	public List<ProjectBase> getPublicProjects() {
		return publicProjects;
	}
	
}
