package org.esg.node.chart.util;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class Title implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private String text = null;
	/** Set the font-size, color or text-align */
	private String style = null;
	
	public Title() {
		super();
	}
	public Title(String text, String style) {
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
