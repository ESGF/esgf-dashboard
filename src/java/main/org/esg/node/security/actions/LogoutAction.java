package org.esg.node.security.actions;

import java.util.Map;
import org.apache.struts2.interceptor.SessionAware;
import org.esg.node.utils.Constants;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class LogoutAction extends ActionSupport implements SessionAware {
	private static final long serialVersionUID = 1L;
	
	private Map<String, Object> session = null;
	
	@Override
	public String execute() {
		session.remove(Constants.USER_SESSION);
		return SUCCESS;
	}
	
	@Override
	public void setSession(Map<String, Object> session) {
		this.session = session;
	}

}
