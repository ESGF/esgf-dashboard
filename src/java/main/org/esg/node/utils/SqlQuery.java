package org.esg.node.utils;

/**
 * @author University of Salento and CMCC
 */
public enum SqlQuery {
	
	USERNAME_VALIDATION("SELECT id FROM user WHERE username=? LIMIT 1;"), 
	
	USER_DN_VALIDATION("SELECT id FROM user WHERE dn=? LIMIT 1;"),
	
	LOGIN("SELECT id, name, surname FROM user WHERE username=? AND password=? AND accountCertified=1;"),
	
	REGISTRATION("INSERT INTO user(name, surname, idCountry, mail, username, password, dn) VALUES(?, ?, ?, ?, ?, ?, ?);"),
	
	ACCOUNT_ACTIVATION("UPDATE user SET accountCertified=1 WHERE username=? AND MD5(id)=? AND accountCertified=0;"),
	
	COUNTRIES("SELECT id, name FROM country ORDER BY name ASC;"),
	
	PUBLIC_PROJECTS("SELECT id, name FROM project WHERE regPublic=1 ORDER BY name ASC;"),
	
	USER_PROFILE("SELECT name, surname, idCountry, username, mail, dn " +
				 "FROM user " +
				 "WHERE id=?;"),
				 
	EDIT_USER_PROFILE("UPDATE user SET name=?, surname=?, idCountry=?, dn=? WHERE id=?;"),
	EDIT_USER_PASSWORD("UPDATE user SET password=? WHERE id=?;"),
	
	MAIL_ADMIN("SELECT s.mail_admin, g.mailAdmin, g.name, h.ip " +
			   "FROM service_instance s INNER JOIN node n ON n.id=s.id INNER JOIN grid_db g ON g.id_node=n.id INNER JOIN host h ON h.id=s.idHost " +
			   "WHERE g.id=?;"),
	
	USER_DN("SELECT dn FROM user WHERE id=?;"),
			   	
	PROJECT_CONTAINS_OPENDAP_SERVICES("SELECT s.id " +
									  "FROM uses u INNER JOIN service_instance s ON u.idServiceInstance=s.id " +
									  "INNER JOIN opendap_node o ON o.id=s.id " +
									  "WHERE u.endDate IS NULL AND u.idProject=? LIMIT 1;"),
	
	/**
	 * @return PROJECT.id, PROJECT.name
	 */
	ALL_PROJECTS_ID_AND_NAME("SELECT p.id, p.name " +
							 "FROM project p INNER JOIN `join` j ON j.idProject=p.id " +
							 "WHERE j.idUser=?;"),
	
	ALL_PROJECTS_START_DATE("SELECT MIN(startDate) as startDate " +
							"FROM project p INNER JOIN `join` j ON j.idProject=p.id " +
							"WHERE j.idUser=?;"),
	
	ALL_PROJECTS_NUM_HOSTS("SELECT COUNT(DISTINCT s.idHost) as numHosts " + 
						   "FROM uses u INNER JOIN `join` j ON j.idProject=u.idProject INNER JOIN service_instance s ON s.id=u.idServiceInstance " + 
						   "WHERE u.endDate IS NULL AND j.idUser=?;"),
	
	ALL_PROJECTS_NUM_SERVICES("SELECT COUNT(DISTINCT idServiceInstance) as numServices " +
							  "FROM uses u INNER JOIN `join` j ON j.idProject=u.idProject " + 
							  "WHERE u.endDate IS NULL AND j.idUser=?;"),
						   
	USER_CAN_VIEW_PROJECT("SELECT idProject FROM `join` WHERE idUser=? AND idProject=?;"),
	
	/**
	 * @return id, name, endDate
	 */
	PROJECTS("SELECT p.id, p.name, p.endDate " +
			 "FROM project p INNER JOIN `join` j ON j.idProject=p.id " +
			 "WHERE j.idUser=? " +
			 "ORDER BY p.name;"),
	
	/**
	 * @param id
	 * @return name, description, startDate, endDate, hostsNumber, servicesNumber
	 */
	PROJECT_BY_ID("SELECT p.name, p.description, p.startDate, p.endDate, (SELECT COUNT(DISTINCT s.idHost) FROM uses u INNER JOIN service_instance s ON s.id=u.idServiceInstance WHERE u.idProject=p.id) as hostsNumber, " + 
				  	   "(SELECT COUNT(*) FROM uses u WHERE u.idProject=p.id) as servicesNumber " +
				  "FROM project p " +
				  "WHERE p.id=?;"),
	
	/**
	 * @param list of PROJECT.id
	 * @return HOST.id, HOST.name, HOST.ip, HOST.latitude, HOST.longitude
	 */
	PROJECT_HOSTS_POSITION("SELECT h.id, h.name, h.ip, h.latitude, h.longitude, h.city " +
						   "FROM host h INNER JOIN service_instance s ON h.id=s.idHost INNER JOIN uses u ON s.id=u.idServiceInstance " +
						   "WHERE idProject IN (?) AND u.endDate IS NULL " +
						   "GROUP BY h.id " +
						   "ORDER BY h.longitude, h.latitude;"),

    /**
     * @param HOST.id
     * @param list of PROJECT.id
     * @return PROJECT.id
     */
    PROJECTS_ID_FOR_HOST("SELECT DISTINCT p.id " +
    					  "FROM project p INNER JOIN uses u ON u.idProject=p.id INNER JOIN service_instance s ON s.id=u.idServiceInstance " +
    					  "WHERE p.id IN (#) AND s.idHost=? AND u.endDate IS NULL;"),

	/**
	 * @param Project.id 
	 * @return HOST.id, HOST.name, HOST.ip, HOST.numInstances
	 */
	HOSTS_BY_PROJECT("SELECT h.id, h.name, h.ip, COUNT(*) as numInstances " +
					 "FROM host h INNER JOIN service_instance s ON h.id=s.idHost INNER JOIN uses u ON u.idServiceInstance=s.id " +
					 "WHERE u.idProject=? AND u.endDate IS NULL " +
					 "GROUP BY h.id " +
					 "ORDER BY h.name, h.ip;"),
	
	/**
	 * @param PROJECT.id
	 * @param HOST.id
	 * @return HOST.ip, HOST.name, HOST.city, HOST.latitude, HOST.longitude, HOST.numInstances
	 */
	HOST_BY_ID_AND_PROJECT("SELECT h.ip, h.name, h.city, h.latitude, h.longitude, COUNT(*) as numInstances, p.name " +
						   "FROM host h INNER JOIN service_instance s ON h.id=s.idHost INNER JOIN uses u ON u.idServiceInstance=s.id " +
						   "INNER JOIN project p ON u.idProject=p.id " +
						   "WHERE p.id=? AND h.id=? AND u.endDate IS NULL " +
						   "GROUP BY h.id;"),
	
	/**
	 * @param PROJECT.id
	 * @param HOST.id
	 * @return SERVICE.id, SERVICE.port, SERVICE.name, SERVICE.mail_admin, SERVICE.institution, USES.startDate, USES.endDate
	 */
	SERVICE_INSTANCE_FOR_SERVER_IN_PROJECT("SELECT s.id, s.port, s.name, s.mail_admin, s.institution, u.startDate, u.endDate " +
										   "FROM service_instance s INNER JOIN uses u ON s.id=u.idServiceInstance " +
										   "WHERE u.idProject=? AND s.idHost=? AND u.endDate IS NULL " +
										   "ORDER BY s.name, s.port;"),
	/**
	 * @param idHost
	 * @return services_count
	 */
	COUNT_SERVICES_ON_HOST("SELECT COUNT(*) as servicesCount " +
						   "FROM service_instance " +
						   "WHERE idHost=?;"),
						   
   /**
	 * @param idHost
	 * @param idProject
	 * @return services_count
	 */			   
	COUNT_SERVICES_ON_HOST_BY_PROJECT("SELECT COUNT(*) as servicesCount " +
									  "FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id " +
									  "WHERE s.idHost=? AND u.idProject=? AND u.endDate IS NULL;"), // bisogna aggiungere un DISTINCT poiché un servizio potrebbe essere utilizzato più volte in intervalli temporali distinti

    /**
     * @param PROJECT.idProject
     * @return HOST.name, services_count
     */
	SERVICES_ON_HOSTS_BY_PROJECT("SELECT h.name, h.ip, t.conteggio " +
								 "FROM ( " +
								 	"SELECT idHost, COUNT(*) as conteggio " + 
								 	"FROM uses u INNER JOIN service_instance s ON s.id=u.idServiceInstance " + 
								 	"WHERE idProject=? AND u.endDate IS NULL " + 
								 	"GROUP BY idHost) t " +
								 "INNER JOIN host h ON h.id=t.idHost " +
								 "ORDER BY name ASC"),
	
    /**
     * @param idServiceInstance
     * @return MIN timestamp, MAX timestamp
     */
	MIN_MAX_TIMESTAMP("SELECT MIN(timestamp) as minTimestamp, MAX(timestamp) as maxTimestamp FROM service_status WHERE idServiceInstance=?;"),	

	AVG_RTT("SELECT AVG(elapsedTime) as avgRtt FROM service_status WHERE idServiceInstance=? AND timestamp BETWEEN ? AND ? AND status<>3;"),
	
	DEV_STD_RTT("SELECT SQRT(SUM(POW(elapsedTime - ?, 2)) / COUNT(*)) as devStdRtt " +
				"FROM service_status WHERE idServiceInstance=? AND timestamp BETWEEN ? AND ? AND status<>3;"),
	
	/**
	 * @param IdServiceInstance
	 * @param startTimestamp
	 * @param endTimestamp
	 * @return status, count
	 */				
	STATUS_QUERY("SELECT status*1 as status, COUNT(*) as hitCount " +
				 "FROM service_status " +
				 "WHERE IdServiceInstance = ? AND `timestamp` BETWEEN ? AND ? " +
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
	SP_ACTIVITY_CHART("{CALL attivitaServizio(?, ?, ?, ?)}"),

	/**
	 * @param IdServiceInstance
	 * @param startTimestamp
	 * @param endTimestamp
	 * @param step
	 * @return status, count
	 */						   
	SP_ACTIVITY_CHART_ON_OFF("{CALL attivitaServizioOnOff(?, ?, ?, ?)}"),
	
	/**
	 * @param IdServiceInstance
	 * @param startTimestamp
	 * @param endTimestamp
	 * @param step
	 * @return status, avgRTT
	 */	
	SP_RTT_CHART("{CALL rttRilevamentiServizio(?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param IdHost
	 * @param startDate
	 * @param endDate
	 * @return idServiceInstance, name, activity percentage
	 */
	SP_HOST_ACTIVITY_CHART("{CALL hostActivity1(?, ?, ?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param IdHost
	 * @param startDate
	 * @param endDate
	 * @param lowerBound
	 * @param size
	 * @return idServiceInstance, name, port, activity percentage
	 */	
	SP_MOST_ACTIVE_SERVICES("{CALL mostActiveServices(?, ?, ?, ?, ?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param startDate
	 * @param endDate
	 * @param lowerBound
	 * @param size
	 * @return idHost, name, IP, average activity percentage
	 */	
	SP_MOST_ACTIVE_HOST("{CALL mostActiveHost(?, ?, ?, ?, ?, ?, ?)}"),
	
	/**
	 * @param startDate
	 * @param endDate
	 * @param lowerBound
	 * @param size
	 * @return id, name, description, startDate, endDate 
	 */
	SP_MOST_ACTIVE_PROJECTS("{CALL mostActiveProjects(?, ?, ?, ?, ?, ?, ?)}"),
	
	/**
	 * @param idProject
	 * @param startDate
	 * @param endDate
	 * @return average activity 
	 */
	SP_AVG_PROJECT_ACTIVITY("{CALL avgProjectActivity(?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param IdHost
	 * @param startDate
	 * @param endDate
	 * @return percentage
	 */
	SP_AVG_HOST_ACTIVITY("{CALL avgHostActivity(?, ?, ?, ?)}"),
	
	/**
	 * @param IdProject
	 * @param IdService
	 * @param startDate
	 * @param endDate
	 * @return percentage
	 */
	SP_AVG_SERVICE_ACTIVITY("{CALL avgServiceActivity(?, ?, ?, ?)}"),
		
	/**
	 * @param idService
	 * @return url
	 */
	OPENDAP_URL("SELECT url FROM opendap_node WHERE id=?;"),
	
	OPENDAP_REGISTRY("SELECT o.id, o.url, h.name, h.ip, s.port " +
					 "FROM opendap_node o INNER JOIN service_instance s ON s.id=o.id INNER JOIN uses u ON u.idServiceInstance=s.id INNER JOIN host h ON h.id=s.idHost " +
					 "WHERE u.idProject=? " +
					 "ORDER BY h.ip, s.port;"),
	
	
	/**
	 * @param idService
	 * @return idService
	 */
	IS_OPENDAP_SERVICE("SELECT id FROM opendap_node WHERE id=?;"),
	
	NEWS("SELECT news, dateNews FROM news ORDER BY dateNews DESC LIMIT ?, ?;")
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
