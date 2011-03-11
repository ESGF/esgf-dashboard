package org.esg.node.beans;

import java.io.Serializable;
import org.esg.node.utils.ServiceType;

/**
 * @author University of Salento and CMCC
 */
public class ServiceInstance extends ServiceInstanceBase implements Serializable {
	private static final long serialVersionUID = 1L;

	private String mailAdmin = null;
	private String institution = null;
	private String startDate = null;
	private String startTime = null;
	private String endDate = null;
	private String endTime = null;
	private ServiceType type = null;
	
	public String getMailAdmin() {
		return mailAdmin;
	}
	public void setMailAdmin(String mailAdmin) {
		this.mailAdmin = mailAdmin;
	}
	public String getInstitution() {
		return institution;
	}
	public void setInstitution(String institution) {
		this.institution = institution;
	}
	public String getStartDate() {
		return startDate;
	}
	public void setStartDate(String startDate) {
		this.startDate = startDate;
	}
	public String getStartTime() {
		return startTime;
	}
	public void setStartTime(String startTime) {
		this.startTime = startTime;
	}
	public String getEndDate() {
		return endDate;
	}
	public void setEndDate(String endDate) {
		this.endDate = endDate;
	}
	public String getEndTime() {
		return endTime;
	}
	public void setEndTime(String endTime) {
		this.endTime = endTime;
	}
	public void setType(ServiceType type) {
		this.type = type;
	}
	public ServiceType getType() {
		return type;
	}
}
