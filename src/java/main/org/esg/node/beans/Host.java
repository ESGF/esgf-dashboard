package org.esg.node.beans;

import java.io.Serializable;
import java.util.List;

/**
 * @author University of Salento and CMCC
 */
public class Host extends HostBase implements Serializable {
	private static final long serialVersionUID = 1L;

	private String city = null;
	private String projectName = null;
	private Number latitude = null;
	private Number longitude = null;
	private Integer numInstances = null;
	private List<ServiceInstance> services = null;
	
	public String getCity() {
		return city;
	}
	public void setCity(String city) {
		this.city = city;
	}
	public void setProjectName(String projectName) {
		this.projectName = projectName;
	}
	public String getProjectName() {
		return projectName;
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
	public void setNumInstances(Integer numInstances) {
		this.numInstances = numInstances;
	}
	public Integer getNumInstances() {
		return numInstances;
	}
	public void setServices(List<ServiceInstance> services) {
		this.services = services;
	}
	public List<ServiceInstance> getServices() {
		return services;
	}
}
