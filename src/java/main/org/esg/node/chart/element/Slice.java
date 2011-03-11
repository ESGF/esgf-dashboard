package org.esg.node.chart.element;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class Slice implements Serializable {
	private static final long serialVersionUID = 1L;

	private String label = null;
	private String tooltip = null;
	private String highlight = "alpha";
	private String text = null;
	private Number value = null;
	private String labelColour = null;
	private String fontSize = null;
	private String onClick = null;
	private String colour = null;

	public String getLabel() {
		return label;
	}
	public void setLabel(String label) {
		this.label = label;
	}
	@JSON(name="tip")
	public String getTooltip() {
		return tooltip;
	}
	public void setTooltip(String tooltip) {
		this.tooltip = tooltip;
	}
	public String getHighlight() {
		return highlight;
	}
	public void setHighlight(String highlight) {
		this.highlight = highlight;
	}
	public String getText() {
		return text;
	}
	public void setText(String text) {
		this.text = text;
	}
	public Number getValue() {
		return value;
	}
	public void setValue(Number value) {
		this.value = value;
	}
	@JSON(name="label-colour")
	public String getLabelColour() {
		return labelColour;
	}
	public void setLabelColour(String labelColour) {
		this.labelColour = labelColour;
	}
	@JSON(name="font-size")
	public String getFontSize() {
		return fontSize;
	}
	public void setFontSize(String fontSize) {
		this.fontSize = fontSize;
	}
	@JSON(name="on-click")
	public String getOnClick() {
		return onClick;
	}
	public void setOnClick(String onClick) {
		this.onClick = onClick;
	}
	public void setColour(String colour) {
		this.colour = colour;
	}
	public String getColour() {
		return colour;
	}
}
