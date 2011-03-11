package org.esg.node.security.actions;

import com.opensymphony.xwork2.ActionSupport;
import java.sql.*;
import org.esg.node.utils.*;

/**
 * @author University of Salento and CMCC
 */
public class AccountActivationAction extends ActionSupport {
	private static final long serialVersionUID = 1L;
	
	private String user = null;
	private String code = null;
	private Boolean success = null; 
	
	@Override
	public void validate() {
		if(user == null || code == null) { 
			addActionError(""); return; 
		}		
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.ACCOUNT_ACTIVATION.getSql());
			stmt.setString(1, user);
			stmt.setString(2, code);
			int rowCount = stmt.executeUpdate();
			success = rowCount==1;
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}

	public String getUser() {
		return user;
	}

	public void setUser(String user) {
		this.user = user;
	}

	public String getCode() {
		return code;
	}

	public void setCode(String code) {
		this.code = code;
	}

	public void setSuccess(Boolean success) {
		this.success = success;
	}

	public Boolean getSuccess() {
		return success;
	}
}
