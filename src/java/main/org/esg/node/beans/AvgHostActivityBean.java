package org.esg.node.beans;

import java.io.Serializable;
import org.esg.node.chart.PieChart;

public class AvgHostActivityBean implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private PieChart pieChart = null;
	private Number activity = null;
	
	public void setPieChart(PieChart pieChart) {
		this.pieChart = pieChart;
	}
	public PieChart getPieChart() {
		return pieChart;
	}
	public void setActivity(Number activity) {
		this.activity = activity;
	}
	public Number getActivity() {
		return activity;
	}
}
