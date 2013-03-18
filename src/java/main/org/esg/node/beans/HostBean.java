package org.esg.node.beans;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class HostBean extends HostBase implements Serializable {
	private static final long serialVersionUID = 1L;

	private Integer id = null;
	private String name = null;
	private String ip = null;
	private String city = null;
	private Number latitude = null;
	private Number longitude = null;
	
	
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
	public String getCity() {
		return city;
	}
	public void setCity(String city) {
		this.city = city;
	}
	public Number getLatitude() {
		return latitude;
	}
	public void setLatitude(Number latitude) {
		this.latitude = latitude;
	}
	public Number getLongitude() {
		return longitude;
	}
	public void setLongitude(Number longitude) {
		this.longitude = longitude;
	}
}
