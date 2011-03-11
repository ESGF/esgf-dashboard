package org.esg.node.opendap.actions.beans;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class Node implements Serializable {
	private static final long serialVersionUID = 1L;

	private String url = null;
	
	public Node() { }

	public Node(String url) {
		this.url = url;
	}
	
	public void setUrl(String url) {
		this.url = url;
	}

	public String getUrl() {
		return url;
	}
}
