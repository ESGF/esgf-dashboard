package org.esg.node.chart.element;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class Dot implements Serializable {
	private static final long serialVersionUID = 1L;

	private Integer haloSize = null;
	private Integer dotSize = null;
	private Number value = null;
	private String colour = null;
	private String onClick = null;
	private String tooltip = null;
	private String type = null;
	private Number x = null;
	private Number y = null;
	
	public Dot() {
		super();
	}
	
	public Dot(Integer haloSize, Integer dotSize, Number value, String colour,
			String onClick) {
		super();
		this.haloSize = haloSize;
		this.dotSize = dotSize;
		this.value = value;
		this.colour = colour;
		this.onClick = onClick;
	}
	
	@JSON(name="halo-size")
	public Integer getHaloSize() {
		return haloSize;
	}
	public void setHaloSize(Integer haloSize) {
		this.haloSize = haloSize;
	}
	@JSON(name="dot-size")
	public Integer getDotSize() {
		return dotSize;
	}
	public void setDotSize(Integer dotSize) {
		this.dotSize = dotSize;
	}
	public Number getValue() {
		return value;
	}
	public void setValue(Number value) {
		this.value = value;
	}
	public String getColour() {
		return colour;
	}
	public void setColour(String colour) {
		this.colour = colour;
	}
	@JSON(name="on-click")
	public String getOnClick() {
		return onClick;
	}
	public void setOnClick(String onClick) {
		this.onClick = onClick;
	}
	public void setTooltip(String tooltip) {
		this.tooltip = tooltip;
	}
	@JSON(name="tip")
	public String getTooltip() {
		return tooltip;
	}
	public void setType(String type) {
		this.type = type;
	}
	public String getType() {
		return type;
	}
	public Number getX() {
		return x;
	}
	public void setX(Number x) {
		this.x = x;
	}
	public Number getY() {
		return y;
	}
	public void setY(Number y) {
		this.y = y;
	}
}
