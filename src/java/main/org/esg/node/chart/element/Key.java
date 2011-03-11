package org.esg.node.chart.element;

import java.io.Serializable;

import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class Key implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private String text = null;
	private String colour = null;
	private Integer fontSize = null;
	
	public String getText() {
		return text;
	}
	public void setText(String text) {
		this.text = text;
	}
	public String getColour() {
		return colour;
	}
	public void setColour(String colour) {
		this.colour = colour;
	}
	@JSON(name="font-size")
	public Integer getFontSize() {
		return fontSize;
	}
	public void setFontSize(Integer fontSize) {
		this.fontSize = fontSize;
	}
}
