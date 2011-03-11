package org.esg.node.chart;

import java.io.Serializable;
import org.esg.node.chart.element.ElementBar;

/**
 * @author University of Salento and CMCC
 */
public class BarChart extends Chart<ElementBar> implements Serializable {
	private static final long serialVersionUID = 1L;

	private String colour = null;

	public void setColour(String colour) {
		this.colour = colour;
	}
	public String getColour() {
		return colour;
	}	
}
