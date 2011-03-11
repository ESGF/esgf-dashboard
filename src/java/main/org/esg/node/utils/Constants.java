package org.esg.node.utils;

import javax.naming.InitialContext;
import javax.naming.NamingException;
import javax.sql.DataSource;

/**
 * @author University of Salento and CMCC
 */
public abstract class Constants { 
	
	private final static String DATASOURCE_NAME = "java:/comp/env/jdbc/registry";
	/**
	 * as microseconds
	 */
	public final static int RTT_MAX = 1000000;
	public final static ServiceStatus[] STATI = { ServiceStatus.ACTIVE, ServiceStatus.INACTIVE, ServiceStatus.TIMEOUT, ServiceStatus.ERROR };
	public final static ServiceStatus[] STATI_ON_OFF = { ServiceStatus.AVAILABLE, ServiceStatus.FAILURE }; 
	public final static Integer STATI_LENGTH = STATI.length;
	/**
	 * as minutes
	 */	
	public final static Integer[] TIME_SPANS = new Integer[] { 5, 60, 1440, 10080, 43200 };
	public final static String DIR_ASCENDANT = "ASC";
	public final static String DIR_DESCENDANT = "DESC";
	public final static String USER_SESSION = "user_session";
	public static DataSource DATASOURCE = null;
	
	static {
		try {
			DATASOURCE = ((DataSource)(new InitialContext()).lookup(Constants.DATASOURCE_NAME));
		} catch (NamingException e) {
			e.printStackTrace();
		}
	}
}
