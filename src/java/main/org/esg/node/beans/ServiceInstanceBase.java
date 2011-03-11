package org.esg.node.beans;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public abstract class ServiceInstanceBase implements Serializable {
	private static final long serialVersionUID = 1L;

	private Integer id = null;
	private Integer port = null;
	private String name = null;
	
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public Integer getPort() {
		return port;
	}
	public void setPort(Integer port) {
		this.port = port;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
}
