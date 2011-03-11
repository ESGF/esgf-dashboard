package org.esg.node.utils;

/**
 * @author University of Salento and CMCC
 */
public enum ServiceStatus {
	ACTIVE	(1, "active.text", "active.colour"), 
	INACTIVE(2, "inactive.text", "inactive.colour"), 
	TIMEOUT	(3, "timeout.text", "timeout.colour"), 
	ERROR	(4, "error.text", "error.colour"),
	AVAILABLE(1, "available.text", "available.colour"),
	FAILURE(-1, "failure.text", "failure.colour")
	;
	
	private final Integer value;
	private final String text;
	private final String color;
	
	ServiceStatus(final Integer value, final String text, final String color) {
		this.value = value;
		this.text  = text;
		this.color = color;
	}

	public Integer getValue() {
		return value;
	}

	public String getText() {
		return "activityChart.element." + text;
	}
	
	public String getText(String prefix) {
		return prefix + text;
	}

	public String getColor() {
		return "activityChart.element." + color;
	}
	
	public String getColor(String prefix) {
		return prefix + color;
	}
}
