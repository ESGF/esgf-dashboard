package org.esg.node.chart.util;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class Legend implements Serializable {

	private static final long serialVersionUID = 1L;
	
	private Boolean visible = null;
	private String bg_colour = null;
	private String position = null;
	private Boolean border = null;
	private Boolean shadow = true;
	private String border_color = null;
	private Integer margin = null;
	private Float alpha = null;
	private Integer padding = null;
	private Integer stroke = null;
	
	public String getBorder_color() {
		return border_color;
	}
	public void setBorder_color(String borderColor) {
		border_color = borderColor;
	}
	public Integer getMargin() {
		return margin;
	}
	public void setMargin(Integer margin) {
		this.margin = margin;
	}
	public Float getAlpha() {
		return alpha;
	}
	public void setAlpha(Float alpha) {
		this.alpha = alpha;
	}
	public Integer getPadding() {
		return padding;
	}
	public void setPadding(Integer padding) {
		this.padding = padding;
	}
	public Integer getStroke() {
		return stroke;
	}
	public void setStroke(Integer stroke) {
		this.stroke = stroke;
	}
	public Boolean getVisible() {
		return visible;
	}
	public Boolean getBorder() {
		return border;
	}
	public Boolean getShadow() {
		return shadow;
	}
	public Boolean isVisible() {
		return visible;
	}
	public void setVisible(Boolean visible) {
		this.visible = visible;
	}
	public String getBg_colour() {
		return bg_colour;
	}
	public void setBg_colour(String bgColour) {
		bg_colour = bgColour;
	}
	public String getPosition() {
		return position;
	}
	public void setPosition(String position) {
		this.position = position;
	}
	public Boolean isBorder() {
		return border;
	}
	public void setBorder(Boolean border) {
		this.border = border;
	}
	public Boolean isShadow() {
		return shadow;
	}
	public void setShadow(Boolean shadow) {
		this.shadow = shadow;
	}	
}
