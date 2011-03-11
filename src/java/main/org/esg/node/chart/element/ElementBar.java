package org.esg.node.chart.element;

import java.io.Serializable;

import org.apache.struts2.json.annotations.JSON;

import org.esg.node.chart.element.animation.BarAnimation;

/**
 * @author University of Salento and CMCC
 */
public class ElementBar extends Element<Bar> implements Serializable {
	private static final long serialVersionUID = 1L;
	
	public static final String TYPE_BAR = "bar";
	public static final String TYPE_BAR_GLASS = "bar_glass";
	public static final String TYPE_BAR_FILLED = "bar_filled";
	public static final String TYPE_BAR_3D = "bar_3d";
	public static final String TYPE_BAR_SKETCH = "bar_sketch";
	public static final String TYPE_BAR_CYLINDER = "bar_cylinder";
	public static final String TYPE_BAR_CYLINDER_OUTLINE = "bar_cylinder_outline";
	public static final String TYPE_BAR_ROUNDED_GLASS = "bar_round_glass";
	public static final String TYPE_BAR_ROUND = "bar_round";
	public static final String TYPE_BAR_DOME = "bar_dome";
	
	private BarAnimation on_show = null; 
	
	public ElementBar() {
		super(TYPE_BAR);
	}
	public ElementBar(String type) {
		super(type);
	}
	
	public void setOn_show(BarAnimation on_show) {
		this.on_show = on_show;
	}
	@JSON(name="on-show")
	public BarAnimation getOn_show() {
		return on_show;
	}
}
