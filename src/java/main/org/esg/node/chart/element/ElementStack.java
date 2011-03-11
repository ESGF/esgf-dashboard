package org.esg.node.chart.element;

import java.util.List;

import org.apache.struts2.json.annotations.JSON;

import org.esg.node.chart.element.animation.BarAnimation;

/**
 * @author University of Salento and CMCC
 */
public class ElementStack extends Element<StackBar[]> {
	private static final long serialVersionUID = 1L;
	public static final String TYPE_STACK = "bar_stack";
	
	private List<Key> keys = null;
	private List<String> colours = null;
	private BarAnimation on_show = null; 

	public ElementStack() {
		super(TYPE_STACK);
	}
	
	public ElementStack(String type) {
		super(type);
	}
	
	public void setKeys(List<Key> keys) {
		this.keys = keys;
	}

	public List<Key> getKeys() {
		return keys;
	}

	public void setColours(List<String> colours) {
		this.colours = colours;
	}

	public List<String> getColours() {
		return colours;
	}

	public void setOn_show(BarAnimation on_show) {
		this.on_show = on_show;
	}
	@JSON(name="on-show")
	public BarAnimation getOn_show() {
		return on_show;
	}	
}
