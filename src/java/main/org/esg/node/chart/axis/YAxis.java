package org.esg.node.chart.axis;

import java.io.Serializable;
import java.util.Collection;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class YAxis extends Axis implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Integer tick_length = null;
	private Collection<String> labels = null;
	
	public YAxis() {
		super();
	}
	
	public YAxis(Number min, Number max, Number steps, Boolean offset) {
		super(min, max, steps, offset);
	}
	
	@JSON(name="tick-length")
	public Integer getTick_length() {
		return tick_length;
	}
	
	public void setTick_length(Integer tickLength) {
		tick_length = tickLength;
	}

	public void setLabels(Collection<String> labels) {
		this.labels = labels;
	}

	public Collection<String> getLabels() {
		return labels;
	}
	
}
