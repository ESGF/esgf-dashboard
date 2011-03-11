package org.esg.node.beans;

import java.io.Serializable;
import java.util.List;

/**
 * @author University of Salento and CMCC
 */
public class MostActiveProjects implements Serializable {
	private static final long serialVersionUID = 1L;

	private List<ProjectActivity> projects = null;
	private Integer totalCount = null;

	public MostActiveProjects(Integer totalCount) {
		this.totalCount = totalCount;
	}
	
	public void setProjects(List<ProjectActivity> projects) {
		this.projects = projects;
	}
	public List<ProjectActivity> getProjects() {
		return projects;
	}
	public void setTotalCount(Integer totalCount) {
		this.totalCount = totalCount;
	}
	public Integer getTotalCount() {
		return totalCount;
	}	
}
