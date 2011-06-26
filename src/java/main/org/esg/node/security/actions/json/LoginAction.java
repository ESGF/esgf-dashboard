package org.esg.node.security.actions.json;

import java.util.Map;
import org.apache.struts2.interceptor.SessionAware;
import org.esg.node.utils.*;
import java.sql.*;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class LoginAction extends ActionSupport implements SessionAware {
	private static final long serialVersionUID = 1L;
	
	private String username = null;
	private String password = null;
	private Map<String, Object> session = null;
	private UserSession userSession = null;
	
	@Override
	public void validate() {
		if(username == null || password == null || username.isEmpty())
			addActionError("");
	}

	@Override
	public String execute() throws Exception {
		Connection conn = null;
		if(username.toLowerCase().equals("guest")) {
			session.put(Constants.USER_SESSION, userSession=new UserSession(1, "guest", ""));
			//System.out.println("guest user connection");
			return SUCCESS;
		}
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.LOGIN.getSql());
			//System.out.println("Other user connection");
			stmt.setString(1, username);
			CheckSum checkSum = new CheckSum(); // commented for test
			stmt.setString(2, checkSum.getDigest(password)); // commented for test
			//System.out.println("Username ="+ username);
			//System.out.println("Password ="+password + " - CheckSum ="+checkSum.getDigest(password));
			ResultSet rs = stmt.executeQuery();
			if(rs.next()) {
				//System.out.println(rs.getInt("id"));
				session.put(Constants.USER_SESSION, userSession=new UserSession(rs.getInt("id"), rs.getString("name"), rs.getString("surname")));
			} else {
				stmt.close();
				return LOGIN;
			}
			rs.close();
			stmt.close();
			return SUCCESS;			
		} catch(SQLException e) {
			//System.out.println("Query failed");
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public String getUsername() {
		return username;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public String getPassword() {
		return password;
	}

	@Override
	public void setSession(Map<String, Object> session) {
		this.session = session;
	}

	public void setUserSession(UserSession userSession) {
		this.userSession = userSession;
	}

	public UserSession getUserSession() {
		return userSession;
	}
}
