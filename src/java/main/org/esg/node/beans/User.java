package org.esg.node.beans;

import java.io.Serializable;

public class User implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private String name = null;
	private String surname = null;
	private Short country = null;
	private String mail = null;
	private String username = null;
	private String password = null;
	private String dn = null;
	
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getSurname() {
		return surname;
	}
	public void setSurname(String surname) {
		this.surname = surname;
	}
	public String getMail() {
		return mail;
	}
	public void setMail(String mail) {
		this.mail = mail;
	}
	public String getUsername() {
		return username;
	}
	public void setUsername(String username) {
		this.username = username;
	}
	public String getPassword() {
		return password;
	}
	public void setPassword(String password) {
		this.password = password;
	}
	public void setCountry(Short country) {
		this.country = country;
	}
	public Short getCountry() {
		return country;
	}
	public void setDn(String dn) {
		this.dn = dn;
	}
	public String getDn() {
		return dn;
	}
}
