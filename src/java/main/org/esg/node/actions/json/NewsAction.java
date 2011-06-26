package org.esg.node.actions.json;

import com.opensymphony.xwork2.ActionSupport;
import java.sql.*;
import java.util.*;
import org.esg.node.utils.*;
import org.esg.node.beans.News;

/**
 * @author University of Salento and CMCC
 */
public class NewsAction extends ActionSupport {
	private static final long serialVersionUID = 1L;

	private Integer start = null;
	private Integer limit = null;
	private List<News> news = null;
	
	@Override
	public void validate() {
		if(start == null || start < 0) start = 0;
		if(limit == null || limit < 0) limit = 5;
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.NEWS.getSql());
			//stmt.setInt(1, start);
			stmt.setInt(1, limit);
			ResultSet rs = stmt.executeQuery();
			news = new LinkedList<News>();
			DateFormat formatter = new DateFormat(getLocale().getLanguage());
			while(rs.next()) {
				Calendar c = Calendar.getInstance();
				c.clear();
				c.setTimeInMillis(rs.getTimestamp("dateNews").getTime());
				news.add(new News(formatter.formatDate(c), rs.getString("news")));
			}
			rs.close();
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}		
		return SUCCESS;
	}

	public void setStart(Integer start) {
		this.start = start;
	}

	public Integer getStart() {
		return start;
	}

	public void setLimit(Integer limit) {
		this.limit = limit;
	}

	public Integer getLimit() {
		return limit;
	}

	public void setNews(List<News> news) {
		this.news = news;
	}

	public List<News> getNews() {
		return news;
	}
	
}
