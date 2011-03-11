package org.esg.node.chart.element.animation;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class BarAnimation extends Animation implements Serializable {
	private static final long serialVersionUID = 1L;
	
	public static final String TYPE_POP_UP = "pop-up";
	public static final String TYPE_DROP = "drop";
	public static final String TYPE_FADE_IN = "fade-in";
	public static final String TYPE_GROW_UP = "grow-up";
	public static final String TYPE_GROW_DOWN = "grow-down";
	public static final String TYPE_POP = "pop";
	
	public BarAnimation(String type) {
		super(type);
	}
}
