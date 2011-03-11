package org.esg.node.chart.element.animation;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class LineAnimation extends Animation implements Serializable {
	private static final long serialVersionUID = 1L;
	
	public static final String TYPE_POP_UP = "pop-up";
	public static final String TYPE_EXPLODE = "explode";
	public static final String TYPE_MID_SLIDE = "mid-slide";
	public static final String TYPE_DROP = "drop";
	public static final String TYPE_FADE_IN = "fade-in";
	public static final String TYPE_SHRINK_IN = "shrink-in";
	
	public LineAnimation(String type) {
		super(type);
	}
}
