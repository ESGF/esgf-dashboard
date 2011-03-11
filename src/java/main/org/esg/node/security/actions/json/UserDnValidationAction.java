package org.esg.node.security.actions.json;

import com.opensymphony.xwork2.ActionSupport;
import java.sql.*;
import org.esg.node.utils.*;

public class UserDnValidationAction extends ActionSupport {
	private static final long serialVersionUID = 1L;

	private String dn = null;
	private Boolean valid = null;
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.USER_DN_VALIDATION.getSql());
			stmt.setString(1, dn);
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
	
	public void setDn(String dn) {
		this.dn = dn;
	}

	public String getDn() {
		return dn;
	}

	public void setValid(Boolean valid) {
		this.valid = valid;
	}

	public Boolean getValid() {
		return valid;
	}
	
}
