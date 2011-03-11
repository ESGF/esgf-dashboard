package org.esg.node.beans;

import java.io.Serializable;

public class News implements Serializable {
	private static final long serialVersionUID = 1L;

	private String date = null;
	private String text = null;
	
	public News(String date, String text) {
		super();
		this.date = date;
		this.text = text;
	}
	
	public String getDate() {
		return date;
	}
	public void setDate(String date) {
		this.date = date;
	}
	public String getText() {
		return text;
	}
	public void setText(String text) {
		this.text = text;
	}
	
}
