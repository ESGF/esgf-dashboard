package org.esg.node.chart.element;

import java.io.Serializable;
import org.apache.struts2.json.annotations.JSON;

/**
 * @author University of Salento and CMCC
 */
public class ElementArea extends ElementLine implements Serializable {
	private static final long serialVersionUID = 1L;
	public final static String TYPE_AREA = "area";
	
	private String fillColour = null;
	private Number fillAlpha = null;
	
	public ElementArea() {
		this.setType(TYPE_AREA);
	}
	
	@JSON(name="fill")
	public String getFillColour() {
		return fillColour;
	}
	public void setFillColour(String fillColour) {
		this.fillColour = fillColour;
	}
	@JSON(name="fill-alpha")
	public Number getFillAlpha() {
		return fillAlpha;
	}
	public void setFillAlpha(Number fillAlpha) {
		this.fillAlpha = fillAlpha;
	}
}
