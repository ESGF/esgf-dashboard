package org.esg.node.beans;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class MaxMinTimestamp implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Long min = null;
	private Long max = null;
	
	public Long getMin() {
		return min;
	}

	public void setMin(Long min) {
		this.min = min;
	}

	public Long getMax() {
		return max;
	}

	public void setMax(Long max) {
		this.max = max;
	}
}
