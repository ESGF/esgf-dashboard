package org.esg.node.opendap.actions.json;

import com.opensymphony.xwork2.ActionSupport;
import java.sql.*;

import org.esg.node.opendap.actions.beans.Node;
import org.esg.node.utils.*;

/**
 * @author University of Salento and CMCC
 */
public class URLAction extends ActionSupport {
	private static final long serialVersionUID = 1L;
	private Integer idNode = null;
	private Node node = null;
	
	@Override
	public void validate() {
		if(idNode == null) {
			addActionError("");
			return;
		}
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.OPENDAP_URL.getSql());
			stmt.setInt(1, idNode);
			ResultSet rs = stmt.executeQuery();
			if(rs.next()) {
				node = new Node();
				node.setUrl(rs.getString("url"));
			}
			rs.close();
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		return SUCCESS;
	}

	public void setIdNode(Integer idNode) {
		this.idNode = idNode;
	}

	public Integer getIdNode() {
		return idNode;
	}

	public void setNode(Node node) {
		this.node = node;
	}

	public Node getNode() {
		return node;
	}	
}
