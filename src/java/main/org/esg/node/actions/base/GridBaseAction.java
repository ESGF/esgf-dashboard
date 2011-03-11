package org.esg.node.actions.base;

import java.util.HashMap;
import java.util.Map;
import java.util.Calendar;
import org.esg.node.utils.Constants;
import org.esg.node.utils.DateFormat;

import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public abstract class GridBaseAction extends ActionSupport  {
	private static final long serialVersionUID = 1L;

	private Long endDate = null;
	protected Calendar endDateTime = null;
	protected Integer start = null;
	protected Integer limit = null;
	protected String sort = null;
	private String dir = null;
	protected final static Map<String, Integer> MAP = new HashMap<String, Integer>();
	
	static {
		for(byte index = 0; index < Constants.TIME_SPANS.length; index ++)
			MAP.put("percentages[" + index + "]", index*1);
	}
	
	@Override
	public void validate() {
		if(start == null)
			start = 0;
		if(sort == null)
			sort = "percentages[0]";
		if(dir == null)
			dir = Constants.DIR_DESCENDANT;
		endDateTime = Calendar.getInstance();
		if(endDate == null)		
			return;		
		try {
			endDateTime.clear();
			endDateTime.setTimeInMillis(endDate*1000);
		} catch(Exception e) {
			this.addActionError("");
			return;
		}
	}
	
	@Override
	public abstract String execute() throws Exception;
	
	public String getFormattedDate() throws Exception {
		DateFormat formatter = new DateFormat(getLocale().getLanguage());
		return formatter.formatDateTime(endDateTime).replace('/', '_').replace(':', '_');
	}
	
	public void setEndDate(Long endDate) {
		this.endDate = endDate;
	}

	public Long getEndDate() {
		return endDate;
	}

	public Integer getStart() {
		return start;
	}
	public void setStart(Integer start) {
		this.start = start;
	}
	public Integer getLimit() {
		return limit;
	}
	public void setLimit(Integer limit) {
		this.limit = limit;
	}
	public String getSort() {
		return sort;
	}
	public void setSort(String sort) {
		this.sort = sort;
	}
	public Boolean getDir() {
		if(dir == null) return null;
		return dir.equals(Constants.DIR_ASCENDANT);
	}
	public void setDir(String dir) {
		this.dir = dir;
	}
	
}
