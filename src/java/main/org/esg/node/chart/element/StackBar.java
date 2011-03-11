package org.esg.node.chart.element;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class StackBar implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Number val = null;
	private String colour = null;
	private String tooltip = null;
	
	public void setVal(Number val) {
		this.val = val;
	}
	public Number getVal() {
		return val;
	}
	public void setColour(String colour) {
		this.colour = colour;
	}
	public String getColour() {
		return colour;
	}
	public void setTooltip(String tooltip) {
		this.tooltip = tooltip;
	}
	@JSON(name="tip")
	public String getTooltip() {
		return tooltip;
	}	
}
