package org.esg.node.beans;

import java.io.Serializable;

public class HostBase implements Serializable {
	private static final long serialVersionUID = 1L;

	private Integer id = null;
	private String name = null;
	private String ip = null;
	
	public Integer getId() {
		return id;
	}
	public void setId(Integer id) {
		this.id = id;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getIp() {
		return ip;
	}
	public void setIp(String ip) {
		this.ip = ip;
	}
}
