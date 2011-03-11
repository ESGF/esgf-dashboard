package org.esg.node.chart.element;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;
import org.esg.node.chart.element.animation.LineAnimation;

/**
 * @author University of Salento and CMCC
 */
public class ElementLine extends Element<Dot> implements Serializable {
	private static final long serialVersionUID = 1L;
	public static final String TYPE_LINE = "line";
	
	private Integer width = null;
	private DotStyle dotStyle = null;
	private LineStyle lineStyle = null;
	private LineAnimation on_show = null;
	
	public ElementLine() {
		super(TYPE_LINE);
	}
	
	public Integer getWidth() {
		return width;
	}
	public void setWidth(Integer width) {
		this.width = width;
	}
	@JSON(name="dot-style")
	public DotStyle getDotStyle() {
		return dotStyle;
	}
	public void setDotStyle(DotStyle dotStyle) {
		this.dotStyle = dotStyle;
	}
	@JSON(name="line-style")
	public LineStyle getLineStyle() {
		return lineStyle;
	}
	public void setLineStyle(LineStyle lineStyle) {
		this.lineStyle = lineStyle;
	}
	public void setOn_show(LineAnimation on_show) {
		this.on_show = on_show;
	}
	@JSON(name="on-show")
	public LineAnimation getOn_show() {
		return on_show;
	}
}
