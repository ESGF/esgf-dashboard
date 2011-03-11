package org.esg.node.chart.element.animation;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class Animation implements Serializable {
	private static final long serialVersionUID = 1L;

	private String type = null;
	/** As seconds */
	private Number cascade = null; 
	/** As seconds */
	private Number delay = null;
	
	public Animation(String type) {
		this.type = type;
	}
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public Number getCascade() {
		return cascade;
	}
	public void setCascade(Number cascade) {
		this.cascade = cascade;
	}
	public Number getDelay() {
		return delay;
	}
	public void setDelay(Number delay) {
		this.delay = delay;
	}	
}
