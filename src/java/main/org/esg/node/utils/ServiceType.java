package org.esg.node.utils;

public enum ServiceType {

	OPeNDAP(SqlQuery.IS_OPENDAP_SERVICE)
	;
	private final SqlQuery query;
	
	ServiceType(SqlQuery query) {
		this.query = query;
	}

	public SqlQuery getQuery() {
		return query;
	}	
}
