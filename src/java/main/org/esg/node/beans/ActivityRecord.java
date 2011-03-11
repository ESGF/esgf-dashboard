package org.esg.node.beans;

import java.io.Serializable;
import java.util.List;

/**
 * @author University of Salento and CMCC
 */
public class ActivityRecord implements Serializable {
	private static final long serialVersionUID = 1L;

	private String startDate = null;
	private String startTime = null;
	private String endDate = null;
	private String endTime = null;
	private List<Number> values = null;
	
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
	public void setEndTime(String endTime) {
		this.endTime = endTime;
	}
	public String getEndTime() {
		return endTime;
	}
	public void setStartTime(String startTime) {
		this.startTime = startTime;
	}
	public String getStartTime() {
		return startTime;
	}
	public void setValues(List<Number> values) {
		this.values = values;
	}
	public List<Number> getValues() {
		return values;
	}	
}
