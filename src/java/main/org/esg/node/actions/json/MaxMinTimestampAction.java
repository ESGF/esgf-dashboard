package org.esg.node.actions.json;

import java.sql.*;
import org.esg.node.utils.*;
import org.esg.node.beans.MaxMinTimestamp;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class MaxMinTimestampAction extends ActionSupport {
	private static final long serialVersionUID = 1L;

	private Integer idServiceInstance = null;
	private MaxMinTimestamp maxMin = null;
	
	@Override
	public void validate() {
		if(idServiceInstance == null) {
			addActionError("");
			return;
		}		
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			MaxMinTimestampHelper helper = new MaxMinTimestampHelper(conn);
			maxMin = helper.getMaxMinTimestamp(idServiceInstance);
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		return SUCCESS;
	}

	public void setIdServiceInstance(Integer idServiceInstance) {
		this.idServiceInstance = idServiceInstance;
	}

	public Integer getIdServiceInstance() {
		return idServiceInstance;
	}

	public MaxMinTimestamp getMaxMin() {
		return maxMin;
	}

	public void setMaxMin(MaxMinTimestamp maxMin) {
		this.maxMin = maxMin;
	}
}
