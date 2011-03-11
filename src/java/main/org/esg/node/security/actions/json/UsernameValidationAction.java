package org.esg.node.security.actions.json;

import com.opensymphony.xwork2.ActionSupport;
import java.sql.*;
import org.esg.node.utils.*;

/**
 * @author University of Salento and CMCC
 */
public class UsernameValidationAction extends ActionSupport {
	private static final long serialVersionUID = 1L;
	
	private String username = null;
	private Boolean valid = null;
	
	@Override
	public void validate() {
		if(username == null) {
			addActionError("");
			return;
		}
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.USERNAME_VALIDATION.getSql());
			stmt.setString(1, username);
			ResultSet rs = stmt.executeQuery();
			valid = !rs.next();
			rs.close();
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		return SUCCESS;
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public String getUsername() {
		return username;
	}

	public void setValid(Boolean valid) {
		this.valid = valid;
	}

	public Boolean getValid() {
		return valid;
	}
}
