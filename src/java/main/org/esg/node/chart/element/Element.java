package org.esg.node.chart.element;

import java.io.Serializable;
import java.util.List;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class Element <T> implements Serializable {
	private static final long serialVersionUID = 1L;

	private String type = null;
	private Number alpha = null;
	private String colour = null;
	private String text = null;
	/* Size of text properties font */
	private Integer fontSize = null;
	private String tooltip = null;
	private String on_click = null;
	private List<T> values = null;
	
	public Element(String type) {
		super();
		this.type = type;
	}
	
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public Number getAlpha() {
		return alpha;
	}
	public void setAlpha(Number alpha) {
		this.alpha = alpha;
	}
	public String getColour() {
		return colour;
	}
	public void setColour(String colour) {
		this.colour = colour;
	}
	public String getText() {
		return text;
	}
	public void setText(String text) {
		this.text = text;
	}
	@JSON(name="font-size")
	public Integer getFontSize() {
		return fontSize;
	}
	public void setFontSize(Integer fontSize) {
		this.fontSize = fontSize;
	}
	public List<T> getValues() {
		return values;
	}
	public void setValues(List<T> values) {
		this.values = values;
	}
	@JSON(name="tip")
	public String getTooltip() {
		return tooltip;
	}
	public void setTooltip(String tooltip) {
		this.tooltip = tooltip;
	}
	public void setOn_click(String on_click) {
		this.on_click = on_click;
	}
	@JSON(name="on-click")
	public String getOn_click() {
		return on_click;
	}
}

