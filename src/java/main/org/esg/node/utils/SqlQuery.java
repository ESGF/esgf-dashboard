package org.esg.node.utils;

/**
 * @author University of Salento and CMCC
 */
public enum SqlQuery {
	
	USERNAME_VALIDATION("SELECT id FROM  esgf_dashboard.user1 WHERE username=? LIMIT 1;"), 
	
	USER_DN_VALIDATION("SELECT id FROM  esgf_dashboard.user1 WHERE dn=? LIMIT 1;"),
	
	// new queries 
	GET_ALL_HOSTS_WITH_ACTIVE_SERVICES("SELECT h.id, h.ip, h.name, h.city, h.latitude, h.longitude " + 
	  "FROM esgf_dashboard.host h " +
	  "INNER JOIN esgf_dashboard.service_instance s " +
	  "ON h.id=s.idHost " +
	  "INNER JOIN esgf_dashboard.uses u " +
	  "ON u.idServiceInstance=s.id " +
	  "INNER JOIN esgf_dashboard.project_dash p " +
	  "ON p.id=u.idProject " +
	  "WHERE u.endDate IS NULL " +
	  "AND p.endDate IS NULL " +
	  "GROUP BY h.id, h.ip, h.name, h.city, h.latitude, h.longitude " +
	  "ORDER BY h.ip;"),

	   
	GET_ALL_HOSTS_OF_ONE_PROJECT_WITH_ACTIVE_SERVICES("SELECT h.id, h.ip, h.name, h.city, h.latitude, h.longitude " + 
	  "FROM esgf_dashboard.host h " +
	  "INNER JOIN esgf_dashboard.service_instance s " +
	  "ON h.id=s.idHost " +
	  "INNER JOIN esgf_dashboard.uses u " +
	  "ON u.idServiceInstance=s.id " +
	  "INNER JOIN esgf_dashboard.project_dash p " +
	  "ON p.id=u.idProject " +
	  "WHERE u.endDate IS NULL " +
	  "AND p.endDate IS NULL " +
	      "AND p.name=? " + 
	      "GROUP BY h.id,h.ip,h.name,h.city,h.latitude,h.longitude " +
	  "ORDER BY h.ip;"),
	
	// small change 
	LOGIN("SELECT id, name, surname FROM  esgf_dashboard.user1 WHERE username=? AND password=? AND accountCertified=1;"),
//	LOGIN("SELECT id, name, surname FROM user1 WHERE username=? AND accountCertified=1;"),
	
	REGISTRATION("INSERT INTO  esgf_dashboard.user1(name, surname, idCountry, mail, username, password, dn) VALUES(?, ?, ?, ?, ?, ?, ?);"),
	
	ACCOUNT_ACTIVATION("UPDATE  esgf_dashboard.user1 SET accountCertified=1 WHERE username=? AND MD5(id)=? AND accountCertified=0;"),
	
	COUNTRIES("SELECT id, name FROM  esgf_dashboard.country ORDER BY name ASC;"),
	
	PUBLIC_PROJECTS("SELECT id, name FROM  esgf_dashboard.project_dash WHERE regPublic=1 ORDER BY name ASC;"),
	
	USER_PROFILE("SELECT name, surname, idCountry, username, mail, dn " +
				 "FROM  esgf_dashboard.user1 " +
				 "WHERE id=?;"),
				 
	EDIT_USER_PROFILE("UPDATE  esgf_dashboard.user1 SET name=?, surname=?, idCountry=?, dn=? WHERE id=?;"),
	EDIT_USER_PASSWORD("UPDATE  esgf_dashboard.user1 SET password=? WHERE id=?;"),
	
	MAIL_ADMIN("SELECT s.mail_admin, g.mailAdmin, g.name, h.ip " +
			   "FROM  esgf_dashboard.service_instance s INNER JOIN  esgf_dashboard.node n ON n.id=s.id INNER JOIN  esgf_dashboard.grid_db g ON g.id_node=n.id INNER JOIN  esgf_dashboard.host h ON h.id=s.idHost " +
			   "WHERE g.id=?;"),
	
	USER_DN("SELECT dn FROM  esgf_dashboard.user1 WHERE id=?;"),
			   	
	PROJECT_CONTAINS_OPENDAP_SERVICES("SELECT s.id " +
									  "FROM  esgf_dashboard.uses u INNER JOIN  esgf_dashboard.service_instance s ON u.idServiceInstance=s.id " +
									  "INNER JOIN  esgf_dashboard.opendap_node o ON o.id=s.id " +
									  "WHERE u.endDate IS NULL AND u.idProject=? LIMIT 1;"),
	
	/**
	 * @return PROJECT.id, PROJECT.name
	 */
									  
	/** ok **/								  
	ALL_PROJECTS_ID_AND_NAME("SELECT p.id, p.name " +
							 "FROM  esgf_dashboard.project_dash p INNER JOIN  esgf_dashboard.join1 j ON j.idProject=p.id " +
							 "WHERE j.idUser=?;"),
							 
	/** ok **/						 
	ALL_PROJECTS_START_DATE("SELECT MIN(startDate) as startDate " +
							"FROM  esgf_dashboard.project_dash p INNER JOIN  esgf_dashboard.join1 j ON j.idProject=p.id " +
							"WHERE j.idUser=?;"),
	
	/** ok **/							
	ALL_PROJECTS_NUM_HOSTS("SELECT COUNT(DISTINCT s.idHost) as numHosts " + 
						   "FROM  esgf_dashboard.uses u INNER JOIN  esgf_dashboard.join1 j ON j.idProject=u.idProject INNER JOIN  esgf_dashboard.service_instance s ON s.id=u.idServiceInstance " + 
						   "WHERE u.endDate IS NULL AND j.idUser=?;"),
	
	/** ok **/
	ALL_PROJECTS_NUM_SERVICES("SELECT COUNT(DISTINCT idServiceInstance) as numServices " +
							  "FROM  esgf_dashboard.uses u INNER JOIN  esgf_dashboard.join1 j ON j.idProject=u.idProject " + 
							  "WHERE u.endDate IS NULL AND j.idUser=?;"),

	ALL_PROJECTS_NUM_USERS("SELECT SUM(regusers) as numUsers FROM esgf_dashboard.host;"),
							  
	/** ok **/							  
	USER_CAN_VIEW_PROJECT("SELECT idProject FROM  esgf_dashboard.join1 WHERE idUser=? AND idProject=?;"),
	
	/**
	 * @return id, name, endDate
	 */
	PROJECTS("SELECT p.id, p.name, p.endDate " +
			 "FROM  esgf_dashboard.project_dash p INNER JOIN  esgf_dashboard.join1 j ON j.idProject=p.id " +
			 "WHERE j.idUser=? " +
			 "ORDER BY p.name;"),
	
	/**
	 * @param id
	 * @return name, description, startDate, endDate, hostsNumber, servicesNumber
	 */
	// ok. Query validated. 		 
	PROJECT_BY_ID("SELECT p.name, p.description, p.startDate, p.endDate, " + 
				  "(SELECT COUNT(DISTINCT s.idHost) FROM  esgf_dashboard.uses u INNER JOIN  esgf_dashboard.service_instance s ON s.id=u.idServiceInstance WHERE u.idProject=p.id) as hostsNumber, " + 
				  "(SELECT COUNT(*) FROM  esgf_dashboard.uses u WHERE u.idProject=p.id) as servicesNumber, " +
				  "(SELECT sum(regusers) from esgf_dashboard.host h where h.id in (SELECT distinct(idHost) FROM esgf_dashboard.uses u INNER JOIN  esgf_dashboard.service_instance s ON s.id=u.idServiceInstance WHERE u.idProject=? AND u.endDate IS NULL and ((nodetype & '10000') > 0))) as totusers " +
				  "FROM  esgf_dashboard.project_dash p " +
				  "WHERE p.id=?;"),
	
	/**
	 * @param list of PROJECT.id
	 * @return HOST.id, HOST.name, HOST.ip, HOST.latitude, HOST.longitude
	 */
	// ok. Query validated. Changed adding "h.name, h.ip, h.latitude, h.longitude, h.city" in the group by clause
				  
	PROJECT_HOSTS_POSITION("SELECT h.id, h.name, h.ip, h.latitude, h.longitude, h.city, h.regusers " +
						   "FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON s.id=u.idServiceInstance " +
						   "WHERE idProject IN (?) AND u.endDate IS NULL " +
						   "GROUP BY h.id, h.name, h.ip, h.latitude, h.longitude, h.city, h.regusers " +
						   "ORDER BY h.longitude, h.latitude;"),

	// RegisteredUsers Map					   
	PROJECT_HOSTS_POSITION_REG_USERS("SELECT h.id, h.name, h.ip, h.latitude, h.longitude, h.city, h.regusers " +
						   "FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON s.id=u.idServiceInstance " +
						   "WHERE idProject IN (?) AND u.endDate IS NULL and ((h.nodetype & '10000')>0)" +
						   "GROUP BY h.id, h.name, h.ip, h.latitude, h.longitude, h.city, h.regusers " +
						   "ORDER BY h.regusers DESC;"),
						   
	// RegisteredUsers Map2					   
	PROJECT_HOSTS_POSITION_REG_USERS2("SELECT h.id, h.name, h.ip, h.latitude, h.longitude, h.city, h.regusers " +
						"FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON s.id=u.idServiceInstance " +
						"WHERE idProject IN (?) AND u.endDate IS NULL and ((h.regusers)>0)" +
						"GROUP BY h.id, h.name, h.ip, h.latitude, h.longitude, h.city, h.regusers " +
						"ORDER BY h.regusers DESC;"),					   
		
	// NodeType Map					   
	PROJECT_HOSTS_POSITION_NODE_TYPE("SELECT h.id, h.name, h.ip, h.latitude, h.longitude, h.city, h.nodetype " +
								   "FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON s.id=u.idServiceInstance " +
								   "WHERE idProject IN (?) AND u.endDate IS NULL " +
								   "GROUP BY h.id, h.name, h.ip, h.latitude, h.longitude, h.city, h.nodetype " +
								   "ORDER BY h.longitude, h.latitude;"),						   						   
						   
    /**
     * @param HOST.id
     * @param list of PROJECT.id
     * @return PROJECT.id
     */
						   
	// ok. query validated.					   
    PROJECTS_ID_FOR_HOST("SELECT DISTINCT p.id " +
    					  "FROM  esgf_dashboard.project_dash p INNER JOIN  esgf_dashboard.uses u ON u.idProject=p.id INNER JOIN  esgf_dashboard.service_instance s ON s.id=u.idServiceInstance " +
    					  "WHERE p.id IN (#) AND s.idHost=? AND u.endDate IS NULL;"),

	/**
	 * @param Project.id 
	 * @return HOST.id, HOST.name, HOST.ip, HOST.numInstances
	 */
    // ok. query validated. The query has been changed in the group by clause					  
	HOSTS_BY_PROJECT("SELECT h.id, h.name, h.ip, COUNT(*) as numInstances " +
					 "FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id " +
					 "WHERE u.idProject=? AND u.endDate IS NULL " +
					 "GROUP BY h.id, h.name, h.ip " +
					 "ORDER BY h.name, h.ip;"),
	
	/**
	 * @param PROJECT.id
	 * @param HOST.id
	 * @return HOST.ip, HOST.name, HOST.city, HOST.latitude, HOST.longitude, HOST.numInstances
	 */
					
	// ok. query validated. The query has been changed in the group by clause
	HOST_BY_ID_AND_PROJECT("SELECT h.ip, h.name, h.city, h.latitude, h.longitude, COUNT(*) as numInstances, p.name " +
						   "FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id " +
						   "INNER JOIN  esgf_dashboard.project_dash p ON u.idProject=p.id " +
						   "WHERE p.id=? AND h.id=? AND u.endDate IS NULL " +
						   "GROUP BY h.id, h.ip, h.name, h.city, h.latitude, h.longitude,p.name;"),
	
	/**
	 * @param PROJECT.id
	 * @param HOST.id
	 * @return SERVICE.id, SERVICE.port, SERVICE.name, SERVICE.mail_admin, SERVICE.institution, USES.startDate, USES.endDate
	 */
						   
	// ok. query validated.					   
	SERVICE_INSTANCE_FOR_SERVER_IN_PROJECT("SELECT s.id, s.port, s.name, s.mail_admin, s.institution, u.startDate, u.endDate " +
										   "FROM  esgf_dashboard.service_instance s INNER JOIN  esgf_dashboard.uses u ON s.id=u.idServiceInstance " +
										   "WHERE u.idProject=? AND s.idHost=? AND u.endDate IS NULL " +
										   "ORDER BY s.name, s.port;"),
	/**
	 * @param idHost
	 * @return services_count
	 */
	// ok. query validated.									   
	COUNT_SERVICES_ON_HOST("SELECT COUNT(*) as servicesCount " +
						   "FROM  esgf_dashboard.service_instance " +
						   "WHERE idHost=?;"),
						   
   /**
	 * @param idHost
	 * @param idProject
	 * @return services_count
	 */			   
    // ok. query validated.					   
	COUNT_SERVICES_ON_HOST_BY_PROJECT("SELECT COUNT(*) as servicesCount " +
									  "FROM  esgf_dashboard.service_instance s INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id " +
									  "WHERE s.idHost=? AND u.idProject=? AND u.endDate IS NULL;"), // add a DISTINCT clause?

    /**
     * @param PROJECT.idProject
     * @return HOST.name, services_count
     */
    // ok. query validated.									  
	SERVICES_ON_HOSTS_BY_PROJECT("SELECT h.name, h.ip, t.conteggio " +
								 "FROM ( " +
								 	"SELECT idHost, COUNT(*) as conteggio " + 
								 	"FROM  esgf_dashboard.uses u INNER JOIN  esgf_dashboard.service_instance s ON s.id=u.idServiceInstance " + 
								 	"WHERE idProject=? AND u.endDate IS NULL " + 
								 	"GROUP BY idHost) t " +
								 "INNER JOIN  esgf_dashboard.host h ON h.id=t.idHost " +
								 "ORDER BY t.conteggio ASC"),
								 
/**
    * @param PROJECT.idProject
    * @return HOST.name, services_count
   */
   // ok. query validated.									  
	USERS_ON_HOSTS_BY_PROJECT("SELECT h.name, h.ip, h.regusers FROM esgf_dashboard.host h where h.id in " +
									"(SELECT distinct(idHost) FROM esgf_dashboard.uses u INNER JOIN  esgf_dashboard.service_instance s ON s.id=u.idServiceInstance " + 
									"WHERE u.idProject=? AND u.endDate IS NULL and ((h.nodetype & '10000') > 0))" +
							  "ORDER BY h.regusers; "),	
							  
							  
	NODETYPES_ON_HOSTS_BY_PROJECT("SELECT (select count(*) as datatype from (SELECT h.id, h.nodetype FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id WHERE u.idProject=? AND u.endDate IS NULL GROUP BY h.id, h.nodetype) t where ((t.nodetype & '100') > 0)) as datatype, (select count(*) as indextype from (SELECT h.id, h.nodetype FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id WHERE u.idProject=? AND u.endDate IS NULL GROUP BY h.id, h.nodetype) t where ((t.nodetype & '1000') > 0)) as indextype,(select count(*) as idptype from (SELECT h.id, h.nodetype FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id WHERE u.idProject=? AND u.endDate IS NULL GROUP BY h.id, h.nodetype) t where ((t.nodetype & '10000') > 0)) as idptype,(select count(*) as computetype from (SELECT h.id, h.nodetype FROM  esgf_dashboard.host h INNER JOIN  esgf_dashboard.service_instance s ON h.id=s.idHost INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id WHERE u.idProject=? AND u.endDate IS NULL GROUP BY h.id, h.nodetype) t where ((t.nodetype & '100000') > 0)) as computetype;"),							  
    /**
     * @param idServiceInstance
     * @return MIN timestamp, MAX timestamp
     */
	MIN_MAX_TIMESTAMP("SELECT MIN(timestamp) as minTimestamp, MAX(timestamp) as maxTimestamp FROM  esgf_dashboard.service_status WHERE idServiceInstance=?;"),	

	AVG_RTT("SELECT AVG(elapsedTime) as avgRtt FROM  esgf_dashboard.service_status WHERE idServiceInstance=? AND timestamp BETWEEN ? AND ? AND status<>3;"),
	
	DEV_STD_RTT("SELECT SQRT(SUM(POW(elapsedTime - ?, 2)) / COUNT(*)) as devStdRtt " +
				"FROM  esgf_dashboard.service_status WHERE idServiceInstance=? AND timestamp BETWEEN ? AND ? AND status<>3;"),
	
	/**
	 * @param IdServiceInstance
	 * @param startTimestamp
	 * @param endTimestamp
	 * @return status, count
	 */				
	// Query ok.  			
	STATUS_QUERY("SELECT status*1 as status, COUNT(*) as hitCount " +
				 "FROM  esgf_dashboard.service_status " +
				 "WHERE IdServiceInstance = ? AND timestamp BETWEEN ? AND ? " +
				 "GROUP BY status;"),
	
	/*
	 * @param IdServiceInstance
	 * @param startTimestamp
	 * @param endTimestamp
	 * @return count
	 *
	 *TAKE_OVERS_NUMBER_IN_INTERVAL("SELECT COUNT(*) " +
							   "FROM service_status " +
							   "WHERE IdServiceInstance = ? AND `timestamp` BETWEEN ? AND ?;"),*/
					   
    /**
	 * @param IdServiceInstance
	 * @param startTimestamp
	 * @param endTimestamp
	 * @param step
	 * @return status, count
	 */						   
//	SP_ACTIVITY_CHART("{CALL attivitaServizio(?, ?, ?, ?)}"),

	/**
	 * @param IdServiceInstance
	 * @param startTimestamp
	 * @param endTimestamp
	 * @param step
	 * @return status, count
	 */						   
//	SP_ACTIVITY_CHART_ON_OFF("{CALL attivitaServizioOnOff(?, ?, ?, ?)}"),
	
	/**
	 * @param IdServiceInstance
	 * @param startTimestamp
	 * @param endTimestamp
	 * @param step
	 * @return status, avgRTT
	 */	
	
//	SP_RTT_CHART("{CALL rttRilevamentiServizio(?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param IdHost
	 * @param startDate
	 * @param endDate
	 * @return idServiceInstance, name, activity percentage
	 */
	// ok. moved to an SQL query 
	//SP_HOST_ACTIVITY_CHART("{CALL hostActivity1(?, ?, ?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param IdHost
	 * @param startDate
	 * @param endDate
	 * @param lowerBound
	 * @param size
	 * @return idServiceInstance, name, port, activity percentage
	 */	
	// ok. moved to an SQL query
	//SP_MOST_ACTIVE_SERVICES("{CALL mostActiveServices(?, ?, ?, ?, ?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param startDate
	 * @param endDate
	 * @param lowerBound
	 * @param size
	 * @return idHost, name, IP, average activity percentage
	 */	
	//SP_MOST_ACTIVE_HOST("{CALL mostActiveHost(?, ?, ?, ?, ?, ?, ?)}"),
	
	/**
	 * @param startDate
	 * @param endDate
	 * @param lowerBound
	 * @param size
	 * @return id, name, description, startDate, endDate 
	 */
	//SP_MOST_ACTIVE_PROJECTS("{CALL mostActiveProjects(?, ?, ?, ?, ?, ?, ?)}"),
	
	/**
	 * @param idProject
	 * @param startDate
	 * @param endDate
	 * @return average activity 
	 */
	//SP_AVG_PROJECT_ACTIVITY("{CALL avgProjectActivity(?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param IdHost
	 * @param startDate
	 * @param endDate
	 * @return percentage
	 */
	//SP_AVG_HOST_ACTIVITY("{CALL avgHostActivity(?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param IdService
	 * @param startDate
	 * @param endDate
	 * @return percentage
	 */
	//SP_AVG_SERVICE_ACTIVITY("{CALL avgServiceActivity(?, ?, ?, ?)}"),
		
	/**
	 * @param idService
	 * @return url
	 */
	// ok. Query validated
	OPENDAP_URL("SELECT url FROM  esgf_dashboard.opendap_node WHERE id=?;"),
	
	// ok. Query validated.
	OPENDAP_REGISTRY("SELECT o.id, o.url, h.name, h.ip, s.port " +
					 "FROM  esgf_dashboard.opendap_node o INNER JOIN  esgf_dashboard.service_instance s ON s.id=o.id INNER JOIN  esgf_dashboard.uses u ON u.idServiceInstance=s.id INNER JOIN  esgf_dashboard.host h ON h.id=s.idHost " +
					 "WHERE u.idProject=? " +
					 "ORDER BY h.ip, s.port;"),
	
	
	/**
	 * @param idService
	 * @return idService
	 */
	// ok. Query validated.
	IS_OPENDAP_SERVICE("SELECT id FROM  esgf_dashboard.opendap_node WHERE id=?;"),
	
	// ok. Query validated.
	NEWS("SELECT news, dateNews FROM  esgf_dashboard.news ORDER BY dateNews DESC LIMIT ?;")
	;
	
	private final String sql;
	
	SqlQuery(final String sql) {
		this.sql = sql;
	}

	public String getSql() {
		return sql;
	}
	
	@Override
	public String toString() {
		return getSql();
	}
}
