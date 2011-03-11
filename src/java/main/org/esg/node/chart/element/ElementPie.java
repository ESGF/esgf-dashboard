package org.esg.node.chart.element;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

import org.esg.node.chart.element.animation.SliceAnimation;

/**
 * @author University of Salento and CMCC
 */
public class ElementPie extends Element<Slice> implements Serializable {
	private static final long serialVersionUID = 1L;
	public static final String TYPE_PIE = "pie";

	private Boolean noLabels = null;
	private Integer radius = null;
	private Integer startAngle = null;
	private Boolean gradientFill = null;
	private SliceAnimation[] animate = null;
	
	public ElementPie() {
		super(TYPE_PIE);
	}
	
	@JSON(name="no-labels")
	public Boolean getNoLabels() {
		return noLabels;
	}
	public void setNoLabels(Boolean noLabels) {
		this.noLabels = noLabels;
	}
	public void setRadius(Integer radius) {
		this.radius = radius;
	}
	public Integer getRadius() {
		return radius;
	}
	@JSON(name="start-angle")
	public Integer getStartAngle() {
		return startAngle;
	}
	public void setStartAngle(Integer startAngle) {
		this.startAngle = startAngle;
	}
	@JSON(name="gradient-fill")
	public Boolean getGradientFill() {
		return gradientFill;
	}
	public void setGradientFill(Boolean gradientFill) {
		this.gradientFill = gradientFill;
	}
	public void setAnimate(SliceAnimation[] animate) {
		this.animate = animate;
	}
	public SliceAnimation[] getAnimate() {
		return animate;
	}
}
