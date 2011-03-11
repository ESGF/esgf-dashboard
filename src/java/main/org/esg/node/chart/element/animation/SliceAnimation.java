package org.esg.node.chart.element.animation;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class SliceAnimation implements Serializable {
	private static final long serialVersionUID = 1L;
	public final static String TYPE_FADE = "fade";
	public final static String TYPE_BOUNCE = "bounce";
	
	private String type = null;
	private Number distance = null;
	
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public Number getDistance() {
		return distance;
	}
	public void setDistance(Number distance) {
		this.distance = distance;
	}
}
