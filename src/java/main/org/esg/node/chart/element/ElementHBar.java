package org.esg.node.chart.element;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class ElementHBar extends Element<HBar> implements Serializable {
	private static final long serialVersionUID = 1L;
	public final static String TYPE_HORIZONTAL_BAR = "hbar";
	
	public ElementHBar() {
		super(TYPE_HORIZONTAL_BAR);		
	}
	
	public ElementHBar(String type) {
		super(type);
	}

}
