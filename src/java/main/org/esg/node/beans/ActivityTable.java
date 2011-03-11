package org.esg.node.beans;

import java.io.Serializable;
import java.util.List;

/**
 * @author University of Salento and CMCC
 */
public class ActivityTable implements Serializable {
	private static final long serialVersionUID = 1L;

	private List<ActivityRecord> activityRecords = null;
	private Integer totalCount = null;
	
	public void setTotalCount(Integer totalCount) {
		this.totalCount = totalCount;
	}

	public Integer getTotalCount() {
		return totalCount;
	}

	public void setActivityRecords(List<ActivityRecord> activityRecords) {
		this.activityRecords = activityRecords;
	}

	public List<ActivityRecord> getActivityRecords() {
		return activityRecords;
	}	
}
