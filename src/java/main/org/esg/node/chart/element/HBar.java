package org.esg.node.chart.element;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class HBar implements Serializable {
	private static final long serialVersionUID = 1L;

	private Number left = null;
	private Number right = null;
	private String tooltip = null;
	private String colour = null;
	
	public HBar() {
		super();
	}
	
	public HBar(Number left, Number right) {
		super();
		this.left = left;
		this.right = right;
	}
	
	public Number getLeft() {
		return left;
	}
	public void setLeft(Number left) {
		this.left = left;
	}
	public Number getRight() {
		return right;
	}
	public void setRight(Number right) {
		this.right = right;
	}
	@JSON(name="tip")
	public String getTooltip() {
		return tooltip;
	}
	public void setTooltip(String tooltip) {
		this.tooltip = tooltip;
	}

	public void setColour(String colour) {
		this.colour = colour;
	}

	public String getColour() {
		return colour;
	}	
}
