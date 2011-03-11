package org.esg.node.chart.util;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class Label implements Serializable {
	
	private static final long serialVersionUID = 1L;
	
	private Object[] labels = null;
	private Long steps = null;
	private Long visible_steps = null;
	/** As degree */
	private Integer rotate = null;
	private String colour = null; 
	private String text = null;
	private Integer size = null;
	private Boolean visible = null;
	
	public Object[] getLabels() {
		return labels;
	}
	public void setLabels(Object[] labels) {
		this.labels = labels;
	}
	public Long getSteps() {
		return steps;
	}
	public void setSteps(Long steps) {
		this.steps = steps;
	}
	public Integer getRotate() {
		return rotate;
	}
	public void setRotate(Integer rotate) {
		this.rotate = rotate;
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
	public Boolean getVisible() {
		return visible;
	}
	public void setVisible(Boolean visible) {
		this.visible = visible;
	}
	public Integer getSize() {
		return size;
	}
	public void setSize(Integer size) {
		this.size = size;
	}
	public void setVisible_steps(Long visible_steps) {
		this.visible_steps = visible_steps;
	}
	@JSON(name="visible-steps") 
	public Long getVisible_steps() {
		return visible_steps;
	}
}
