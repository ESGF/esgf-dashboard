package org.esg.node.utils;

import java.util.Properties;
import javax.mail.internet.*;
import javax.mail.*;

/**
 * @author University of Salento and CMCC
 */
public class MailSender {
	private String smtp = null; // your SMTP server IP or domain name
	private Integer smtpPort = null;
	private String username = null;
	private String password = null;
	private String sender = null;
		
	public MailSender() { }
	
	public MailSender(String smtp, Integer smtpPort, String username, String password, String sender) {
		this.smtp = smtp;
		this.smtpPort = smtpPort;
		this.username = username;
		this.password = password;
		this.sender = sender;
	}
	
	public void sendMail(String recipient, String recipientCC, String subject, String text) throws MessagingException {
		// Creazione di una mail session
		Properties props = new Properties();
		props.put("mail.smtp.host", smtp);
		props.put("mail.smtp.port", smtpPort);
		Session session = null;
		if(username != null && !username.isEmpty() && password != null && !password.isEmpty()) {
			props.put("mail.smtp.auth", "true");
			Authenticator authenticator = new Authenticator () {
				public PasswordAuthentication getPasswordAuthentication() {
					return new PasswordAuthentication(username, password);
				}	    	
			};
			session = Session.getDefaultInstance(props, authenticator);
		} else {
			props.put("mail.smtp.auth", "false");
			session = Session.getDefaultInstance(props);
		}
		
		// Creazione del messaggio da inviare
		MimeMessage message = new MimeMessage(session);
		message.setSubject(subject);
		message.setText(text);
		// Aggiunta degli indirizzi del mittente e del destinatario
		InternetAddress fromAddress = new InternetAddress(sender);
		InternetAddress toAddress = new InternetAddress(recipient);
		message.setFrom(fromAddress);
		message.setRecipient(Message.RecipientType.TO, toAddress);
		if(recipientCC != null) {
			InternetAddress ccAddress = new InternetAddress(recipientCC);
			message.setRecipient(Message.RecipientType.CC, ccAddress);
		}
		// Invio del messaggio
		Transport.send(message);
	}
	
	public void sendMail(String recipient, String subject, String text) throws MessagingException {
		sendMail(recipient, null, subject, text);
	}
	
	public String getSmtp() {
		return smtp;
	}

	public void setSmtp(String smtp) {
		this.smtp = smtp;
	}

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public String getSender() {
		return sender;
	}

	public void setSender(String sender) {
		this.sender = sender;
	}

	public void setSmtpPort(Integer smtpPort) {
		this.smtpPort = smtpPort;
	}

	public Integer getSmtpPort() {
		return smtpPort;
	}
}
