package org.esg.node.beans;

import java.io.Serializable;
import java.util.List;

/**
 * @author University of Salento and CMCC
 */
public class MostActiveServices implements Serializable {
	private static final long serialVersionUID = 1L;

	private Integer totalCount = null; 
	private List<ServiceInstanceActivity> services = null;
	
	public MostActiveServices(Integer totalCount) {
		super();
		this.totalCount = totalCount;
	}
	
	public void setTotalCount(Integer totalCount) {
		this.totalCount = totalCount;
	}

	public Integer getTotalCount() {
		return totalCount;
	}

	public void setServices(List<ServiceInstanceActivity> services) {
		this.services = services;
	}

	public List<ServiceInstanceActivity> getServices() {
		return services;
	}
}
