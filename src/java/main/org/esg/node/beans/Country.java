package org.esg.node.beans;

import java.io.Serializable;

public class Country implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Integer id = null;
	private String name = null;
	
	public Country() {}
	
	public Country(Integer id, String name) {
		this.id = id;
		this.name = name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getName() {
		return name;
	}

	public void setId(Integer id) {
		this.id = id;
	}

	public Integer getId() {
		return id;
	}
	
}
