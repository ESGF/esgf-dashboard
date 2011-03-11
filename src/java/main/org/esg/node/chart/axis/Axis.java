package org.esg.node.chart.axis;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class Axis implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Integer stroke = null;
    private String colour = null;
    private String grid_colour = null;
    private Number steps = null;
    private Boolean offset = null;
    private Integer threed = null;
    private Number min = null;
    private Number max = null;
    
    public Axis() {
    	super();
    }
    
    public Axis(Number min, Number max, Number steps, Boolean offset) {
    	this.min = min;
    	this.max = max;
    	this.steps = steps;
    	this.offset = offset;
    }
    
	public Integer getStroke() {
		return stroke;
	}
	public void setStroke(Integer stroke) {
		this.stroke = stroke;
	}
	public String getColour() {
		return colour;
	}
	public void setColour(String colour) {
		this.colour = colour;
	}
	@JSON(name="grid-colour")
	public String getGrid_colour() {
		return grid_colour;
	}
	public void setGrid_colour(String gridColour) {
		grid_colour = gridColour;
	}
	public Number getSteps() {
		return steps;
	}
	public void setSteps(Number steps) {
		this.steps = steps;
	}
	public Boolean getOffset() {
		return offset;
	}
	public void setOffset(Boolean offset) {
		this.offset = offset;
	}
	@JSON(name="3d")
	public Integer getThreed() {
		return threed;
	}
	public void setThreed(Integer threed) {
		this.threed = threed;
	}
	public Number getMin() {
		return min;
	}
	public void setMin(Number min) {
		this.min = min;
	}
	public Number getMax() {
		return max;
	}
	public void setMax(Number max) {
		this.max = max;
	}    
}
