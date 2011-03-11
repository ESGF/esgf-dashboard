package org.esg.node.beans;

import java.io.Serializable;
import java.util.List;

/**
 * @author University of Salento and CMCC
 */
public class ServiceInstanceActivity extends ServiceInstanceBase implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private List<Number> percentages = null;

	public void setPercentages(List<Number> percentages) {
		this.percentages = percentages;
	}

	public List<Number> getPercentages() {
		return percentages;
	}
	
}
