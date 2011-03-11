package org.esg.node.chart.element;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class Bar implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Number top = null;
	private Number bottom = null;
	private String colour = null;
	private String tooltip = null;
	
	public Bar() {
		super();
	}
	
	public Bar(Number top, Number bottom, String colour, String tooltip) {
		super();
		this.top = top;
		this.bottom = bottom;
		this.colour = colour;
		this.tooltip = tooltip;
	}
	
	public Number getTop() {
		return top;
	}
	public void setTop(Number top) {
		this.top = top;
	}
	public Number getBottom() {
		return bottom;
	}
	public void setBottom(Number bottom) {
		this.bottom = bottom;
	}
	public String getColour() {
		return colour;
	}
	public void setColour(String colour) {
		this.colour = colour;
	}
	@JSON(name="tip")
	public String getTooltip() {
		return tooltip;
	}
	public void setTooltip(String tooltip) {
		this.tooltip = tooltip;
	}
}
