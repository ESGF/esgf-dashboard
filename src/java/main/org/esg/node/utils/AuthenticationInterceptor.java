package org.esg.node.utils;

import java.util.Map;
import com.opensymphony.xwork2.Action;
import com.opensymphony.xwork2.ActionInvocation;
import com.opensymphony.xwork2.interceptor.Interceptor;

public class AuthenticationInterceptor implements Interceptor {
	private static final long serialVersionUID = 1L;
	
	@Override
	public void init() { }
	
	@Override
	public void destroy() { }

	@Override
	public String intercept(ActionInvocation actionInvocation) throws Exception {
		Map<String, Object> session = actionInvocation.getInvocationContext().getSession();
		Object user = session.get(Constants.USER_SESSION);
		if(user == null || !(user instanceof UserSession))
			return Action.LOGIN;
		else {			
			Action action = (Action) actionInvocation.getAction();
			if(action instanceof UserAware)
				((UserAware)action).setUserSession((UserSession) user);			
		}
		return actionInvocation.invoke();
	}
}
