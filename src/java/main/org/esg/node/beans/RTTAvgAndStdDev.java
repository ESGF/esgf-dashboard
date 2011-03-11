package org.esg.node.beans;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class RTTAvgAndStdDev implements Serializable {
	private static final long serialVersionUID = 1L;

	private Number avg = null;
	private Number stdDev = null;

	public Number getAvg() {
		return avg;
	}

	public void setAvg(Number avg) {
		this.avg = avg;
	}

	public Number getStdDev() {
		return stdDev;
	}

	public void setStdDev(Number stdDev) {
		this.stdDev = stdDev;
	}
}
