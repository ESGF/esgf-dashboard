package org.esg.node.utils;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class UserSession implements Serializable {

	private static final long serialVersionUID = 1L;
	private Integer id = null;
	private String name = null;
	private String surname = null;
	
	public UserSession(Integer id, String name, String surname) {
		this.id = id;
		this.name = name;
		this.surname = surname;
	}
	
	public void setId(Integer id) {
		this.id = id;
	}
	
	public Integer getId() {
		return id;
	}

	public void setSurname(String surname) {
		this.surname = surname;
	}

	public String getSurname() {
		return surname;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getName() {
		return name;
	}		
}
