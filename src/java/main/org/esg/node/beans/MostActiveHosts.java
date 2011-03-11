package org.esg.node.beans;

import java.io.Serializable;
import java.util.List;

/**
 * @author University of Salento and CMCC
 */
public class MostActiveHosts implements Serializable {
	private static final long serialVersionUID = 1L;

	private Integer totalCount = null;
	private List<HostActivity> hosts = null;
	
	public MostActiveHosts(Integer totalCount) {
		super();
		this.totalCount = totalCount;
	}
	
	public void setTotalCount(Integer totalCount) {
		this.totalCount = totalCount;
	}

	public Integer getTotalCount() {
		return totalCount;
	}

	public void setHosts(List<HostActivity> hosts) {
		this.hosts = hosts;
	}

	public List<HostActivity> getHosts() {
		return hosts;
	} 
	
}
