package org.esg.node.beans;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class Project extends ProjectBase implements Serializable {
	private static final long serialVersionUID = 1L;

	private String description = null;
	private String startDate = null;
	private String startTime = null;
	private String endDate = null;
	private String endTime = null;
	private Integer hostsNumber = null;
	private Integer servicesNumber = null;
	private Integer regUserNumber = null;
	
	public String getDescription() {
		return description;
	}
	public void setDescription(String description) {
		this.description = description;
	}
	public String getStartDate() {
		return startDate;
	}
	public void setStartDate(String startDate) {
		this.startDate = startDate;
	}
	public String getEndDate() {
		return endDate;
	}
	public void setEndDate(String endDate) {
		this.endDate = endDate;
	}
	public String getStartTime() {
		return startTime;
	}
	public void setStartTime(String startTime) {
		this.startTime = startTime;
	}
	public String getEndTime() {
		return endTime;
	}
	public void setEndTime(String endTime) {
		this.endTime = endTime;
	}
	public void setHostsNumber(Integer hostsNumber) {
		this.hostsNumber = hostsNumber;
	}
	public Integer getHostsNumber() {
		return hostsNumber;
	}
	public void setServicesNumber(Integer servicesNumber) {
		this.servicesNumber = servicesNumber;
	}
	public Integer getServicesNumber() {
		return servicesNumber;
	}
	/**
	 * @return the regUserNumber
	 */
	public Integer getRegUserNumber() {
		return regUserNumber;
	}
	/**
	 * @param regUserNumber the regUserNumber to set
	 */
	public void setRegUserNumber(Integer regUserNumber) {
		this.regUserNumber = regUserNumber;
	}
}
