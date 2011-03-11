package org.esg.node.chart.util;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class Text implements Serializable {
	private static final long serialVersionUID = 1L;
	public static final String TEXT_ALIGN_CENTER = "center";
	public static final String TEXT_ALIGN_LEFT = "left";
	public static final String TEXT_ALIGN_RIGHT = "right";
	
	private String text = null;
	/** Set the font-size, color or text-align */
	private String style = null;
	
	public Text() {
		super();
	}
	
	public Text(String text, String style) {
		super();
		this.text = text;
		this.style = style;
	}
	
	public void setText(String text) {
		this.text = text;
	}
	public String getText() {
		return text;
	}
	public void setStyle(String style) {
		this.style = style;
	}
	public String getStyle() {
		return style;
	}
}
