package org.esg.node.chart.axis;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;
import org.esg.node.chart.util.Label;

/**
 * @author University of Salento and CMCC
 */
public class XAxis extends Axis implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Integer tick_height = null;
	private Label labels = null;
	
	public XAxis() {
		super();
	}
	
	public XAxis(Number min, Number max, Number steps, Boolean offset) {
		super(min, max, steps, offset);
	}
	
	@JSON(name="tick-height")
	public Integer getTick_height() {
		return tick_height;
	}
	public void setTick_height(Integer tickHeight) {
		tick_height = tickHeight;
	}
	public Label getLabels() {
		return labels;
	}
	public void setLabels(Label labels) {
		this.labels = labels;
	}
}
