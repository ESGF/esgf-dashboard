package org.esg.node.security.actions.json;

import org.esg.node.beans.User;
import java.sql.*;
import java.util.*;
import org.apache.struts2.interceptor.SessionAware;
import nl.captcha.Captcha;
import org.esg.node.utils.*;
import com.opensymphony.xwork2.ActionSupport;

/**
 * @author University of Salento and CMCC
 */
public class RegistrationAction extends ActionSupport implements SessionAware {
	private static final long serialVersionUID = 1L;
	
	private User user = null;
	private String captcha = null;
	private Boolean success = false;
	private String activationUrl = null;
	private Map<String, Object> session = null;
	private String idProjects = null;
	
	@Override
	public void validate() {		
		Captcha captchaGen = (Captcha) session.get(Captcha.NAME);
		if(user == null ||
		user.getName() == null || user.getName().isEmpty() ||
		user.getSurname() == null || user.getSurname().isEmpty() ||
		user.getMail() == null || user.getMail().isEmpty() ||
		user.getUsername() == null || user.getUsername().isEmpty() ||
		user.getPassword() == null || user.getPassword().isEmpty() ||
		captcha == null || !captchaGen.isCorrect(captcha) ||
		activationUrl == null || activationUrl.isEmpty()) { 
			addActionError(""); 
			success = false;
			return; 
		}
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.REGISTRATION.getSql(), Statement.RETURN_GENERATED_KEYS);
			stmt.setString(1, user.getName());
			stmt.setString(2, user.getSurname());
			if(user.getCountry() == null)
				stmt.setNull(3, Types.SMALLINT);
			else
				stmt.setShort(3, user.getCountry());
			stmt.setString(4, user.getMail());
			stmt.setString(5, user.getUsername());
			CheckSum checkSum = new CheckSum();
			stmt.setString(6, checkSum.getDigest(user.getPassword()));
			if(user.getDn() == null || user.getDn().isEmpty())
				stmt.setNull(7, Types.VARCHAR);
			else 
				stmt.setString(7, user.getDn());
			stmt.executeUpdate();
			ResultSet rs = stmt.getGeneratedKeys();
			Integer id = null;
			if(rs.next())
				id = rs.getInt(1);
			else { 
				stmt.close();
				success = false;
				return ERROR; 
			}
			rs.close();
			stmt.close();
			
			MailSender mailSender = new MailSender(getText("mailProvider.smtp.hostname"), Integer.parseInt(getText("mailProvider.smtp.port")), 
				getText("mailProvider.smtp.username"), getText("mailProvider.smtp.password"), getText("mailProvider.smtp.senderAddress"));			
			
			activationUrl += "?user=" + user.getUsername() + "&code=" + checkSum.getDigest(id.toString());
			mailSender.sendMail(user.getMail(), getText("mails.subscription.subject"), 
				getText("mails.subscription.text", new String[] { user.getName(), activationUrl}));			
			
			mailSender.sendMail(getText("adminUser.mailAddress"), getText("mails.subscription.adminNotification.subject"),
				getText("mails.subscription.adminNotification.text", new String[] { user.getName(), user.getSurname(), user.getDn(), idProjects }));
			success = true;
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		return SUCCESS;
	}

	public void setUser(User user) {
		this.user = user;
	}

	public User getUser() {
		return user;
	}

	public void setSuccess(Boolean success) {
		this.success = success;
	}

	public Boolean getSuccess() {
		return success;
	}

	@Override
	public void setSession(Map<String, Object> session) {
		this.session = session;
	}

	public String getCaptcha() {
		return captcha;
	}

	public void setCaptcha(String captcha) {
		this.captcha = captcha;
	}

	public void setActivationUrl(String activationUrl) {
		this.activationUrl = activationUrl;
	}

	public String getActivationUrl() {
		return activationUrl;
	}

	public void setIdProjects(String idProjects) {
		this.idProjects = idProjects;
	}

	public String getIdProjects() {
		return idProjects;
	}
}
