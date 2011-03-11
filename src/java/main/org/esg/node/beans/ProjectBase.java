package org.esg.node.beans;

import java.io.Serializable;

public class ProjectBase implements Serializable {
	private static final long serialVersionUID = 1L;

	private Integer id = null;
	private String name = null;
	
	public ProjectBase() {}
	
	public ProjectBase(Integer id, String name) {
		super();
		this.id = id;
		this.name = name;
	}
	
	public void setId(Integer id) {
		this.id = id;
	}
	public Integer getId() {
		return id;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}	
}
