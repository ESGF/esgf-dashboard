package org.esg.node.chart;

import java.io.Serializable;
import org.esg.node.chart.element.ElementPie;
import org.esg.node.chart.util.Legend;

/**
 * @author University of Salento and CMCC
 */
public class PieChart extends Chart<ElementPie> implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Boolean animate = null;
	private Integer border = null;
	private Legend legend = null;

	public Boolean getAnimate() {
		return animate;
	}
	public void setAnimate(Boolean animate) {
		this.animate = animate;
	}
	public Integer getBorder() {
		return border;
	}
	public void setBorder(Integer border) {
		this.border = border;
	}
	public void setLegend(Legend legend) {
		this.legend = legend;
	}
	public Legend getLegend() {
		return legend;
	}
}
