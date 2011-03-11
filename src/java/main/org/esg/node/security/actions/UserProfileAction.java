package org.esg.node.security.actions;

import com.opensymphony.xwork2.ActionSupport;
import java.sql.*;
import java.util.*;
import org.esg.node.utils.*;
import org.esg.node.beans.*;

/**
 * @author University of Salento and CMCC
 */
public class UserProfileAction extends ActionSupport implements UserAware {
	private static final long serialVersionUID = 1L;

	private UserSession userSession = null;
	private User user = null;
	private List<Country> countries = null;
	private Boolean success = false;
	
	@Override
	public void validate() {
		if(userSession.getId() == 1)
			addActionError("");
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.USER_PROFILE.getSql());
			stmt.setInt(1, userSession.getId());
			ResultSet rs = stmt.executeQuery();
			if(rs.next()) {
				user = new User();
				user.setName(rs.getString("name"));
				user.setSurname(rs.getString("surname"));
				if(rs.getObject("idCountry") != null)
					user.setCountry(rs.getShort("idCountry"));
				user.setUsername(rs.getString("username"));
				user.setMail(rs.getString("mail"));
				user.setDn(rs.getString("dn"));
			}
			rs.close();
			stmt.close();
			stmt = conn.prepareStatement(SqlQuery.COUNTRIES.getSql());
			rs = stmt.executeQuery();
			countries = new LinkedList<Country>();
			while(rs.next())
				countries.add(new Country(rs.getInt("id"), rs.getString("name")));
			rs.close();
			stmt.close();
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
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.EDIT_USER_PROFILE.getSql());
			stmt.setString(1, user.getName());
			stmt.setString(2, user.getSurname());
			if(user.getCountry() == null)
				stmt.setNull(3, Types.SMALLINT);
			else
				stmt.setShort(3, user.getCountry());
			if(user.getDn() == null)
				stmt.setNull(4, Types.VARCHAR);
			else 
				stmt.setString(4, user.getDn());
			stmt.setInt(5, userSession.getId());
			Integer rowCount = stmt.executeUpdate();
			success = rowCount==1;
			stmt.close();
			if(user.getPassword() != null && !user.getPassword().trim().isEmpty()) {
				stmt = conn.prepareStatement(SqlQuery.EDIT_USER_PASSWORD.getSql());
				CheckSum checkSum = new CheckSum();
				stmt.setString(1, checkSum.getDigest(user.getPassword()));
				stmt.setInt(2, userSession.getId());
				rowCount = stmt.executeUpdate();
				success &= rowCount==1;
				stmt.close();
			}			
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}
	
	@Override
	public void setUserSession(UserSession userSession) {
		this.userSession = userSession;
	}

	public void setUser(User user) {
		this.user = user;
	}

	public User getUser() {
		return user;
	}

	public void setCountries(List<Country> countries) {
		this.countries = countries;
	}

	public List<Country> getCountries() {
		return countries;
	}

	public void setSuccess(Boolean success) {
		this.success = success;
	}

	public Boolean getSuccess() {
		return success;
	}
}
