package org.esg.node.beans;

import java.io.Serializable;

public class Summary implements Serializable {
	private static final long serialVersionUID = 1L;

	private RTTAvgAndStdDev avgStdDev = null;
	private Number availability = null;
	
	public void setAvgStdDev(RTTAvgAndStdDev avgStdDev) {
		this.avgStdDev = avgStdDev;
	}

	public RTTAvgAndStdDev getAvgStdDev() {
		return avgStdDev;
	}

	public void setAvailability(Number availability) {
		this.availability = availability;
	}

	public Number getAvailability() {
		return availability;
	}	
}
