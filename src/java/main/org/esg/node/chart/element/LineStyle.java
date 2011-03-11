package org.esg.node.chart.element;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class LineStyle implements Serializable {
	private static final long serialVersionUID = 1L;
	public final static String STYLE_DASHED = "dash";
	public final static String STYLE_SOLID = "solid";
	
	private String style = null; 
	private Integer on = null;
	private Integer off = null;
	
	public String getStyle() {
		return style;
	}
	public void setStyle(String style) {
		this.style = style;
	}
	public Integer getOn() {
		return on;
	}
	public void setOn(Integer on) {
		this.on = on;
	}
	public Integer getOff() {
		return off;
	}
	public void setOff(Integer off) {
		this.off = off;
	}	
}
