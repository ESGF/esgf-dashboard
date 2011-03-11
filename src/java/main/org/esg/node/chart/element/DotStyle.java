package org.esg.node.chart.element;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class DotStyle implements Serializable {
	private static final long serialVersionUID = 1L;
	public final static String TYPE_STAR = "star";
	public final static String TYPE_ANCHOR = "anchor";
	public final static String TYPE_BOW = "bow";
	public final static String TYPE_SOLID_DOT = "solid-dot";
    public final static String TYPE_HOLLOW_DOT = "hollow-dot";
    public final static String TYPE_DOT = "dot";
    
	private String type = TYPE_DOT;
	private Integer sides = null;
	private Integer width = null;
	private String tooltip = null;
	private String colour = null;
	private Integer dotSize = null;
	private Integer haloSize = null;
	
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public Integer getSides() {
		return sides;
	}
	public void setSides(Integer sides) {
		this.sides = sides;
	}
	public Integer getWidth() {
		return width;
	}
	public void setWidth(Integer width) {
		this.width = width;
	}
	@JSON(name="tip")
	public String getTooltip() {
		return tooltip;
	}
	public void setTooltip(String tooltip) {
		this.tooltip = tooltip;
	}
	public String getColour() {
		return colour;
	}
	public void setColour(String colour) {
		this.colour = colour;
	}
	@JSON(name="dot-size")
	public Integer getDotSize() {
		return dotSize;
	}	
	public void setDotSize(Integer dotSize) {
		this.dotSize = dotSize;
	}
	public void setHaloSize(Integer haloSize) {
		this.haloSize = haloSize;
	}
	@JSON(name="halo-size")
	public Integer getHaloSize() {
		return haloSize;
	}
}
