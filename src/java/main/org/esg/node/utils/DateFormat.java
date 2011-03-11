package org.esg.node.utils;

import java.util.*;

/**
 * @author University of Salento and CMCC
 */
public class DateFormat {
	public final static Map<String, String> DATE_FORMAT = new HashMap<String, String>();
	public final static String TIME_FORMAT = "HH:ii";
	public final static String FULL_TIME_FORMAT = "HH:ii:ss";
	static {
		DATE_FORMAT.put("IT", "dd/mm/YYYY");
		DATE_FORMAT.put("EN", "mm/dd/YYYY");		
	}
	private String locale = null; 
	
	public DateFormat(String locale) throws Exception {
		this.locale = locale.toUpperCase();
		if(!DATE_FORMAT.containsKey(this.locale))
			throw new Exception("Not supported locale");
	}

	public String formatDate(Calendar c) {
		if(c == null) return null;
		String date = new String(DATE_FORMAT.get(locale));
		int day = c.get(Calendar.DAY_OF_MONTH);
		int month = c.get(Calendar.MONTH) + 1;
		int year = c.get(Calendar.YEAR);
		date = date.replace("dd", (day<10? "0": "") + day);
		date = date.replace("mm", (month<10? "0": "") + month);
		date = date.replace("YYYY", (year<1000? year<100? year<10? "000" : "00" : "0" : "") + year);
		return date;
	}
	
	public String formatTime(Calendar c) {
		return formatTime(c, false);
	}
	
	public String formatTime(Calendar c, boolean fullTime) {
		if(c == null) return null;
		String time = fullTime? FULL_TIME_FORMAT: TIME_FORMAT;
		int hour = c.get(Calendar.HOUR_OF_DAY);
		int minutes = c.get(Calendar.MINUTE);
		time = time.replace("HH", (hour<10? "0": "") + hour);
		time = time.replace("ii", (minutes<10? "0": "") + minutes);
		if(fullTime) {
			int seconds = c.get(Calendar.SECOND);
			time = time.replace("ss", (seconds<10? "0": "") + seconds);
		}
		//System.out.println("Format Time call");
		return time;
	}
	
	public String formatDateTime(Calendar c) {
		return formatDateTime(c, false);
	}
	
	/**
	 * @param c - Calendar
	 * @return <strong>&lt;date&gt; &lt;time&gt; </strong><br />divided by a space character
	 */
	public String formatDateTime(Calendar c, boolean fullTime) {
		return formatDateTime(c, " ", fullTime);
	}
	
	public String formatDateTime(Calendar c, String separator, boolean fullTime) {
		if(c == null) return null;
		return formatDate(c) + separator + formatTime(c, fullTime);
	}
	
	public Calendar toStandard(String date, String time, boolean fullTime) throws Exception {
		Calendar c = Calendar.getInstance();
		String format = new String(DATE_FORMAT.get(locale));
		c.clear();
		if(date != null) {
			int year = Integer.parseInt(date.substring(format.indexOf("Y"), format.lastIndexOf("Y")+1));
			int month = Integer.parseInt(date.substring(format.indexOf("m"), format.lastIndexOf("m")+1));
			int day = Integer.parseInt(date.substring(format.indexOf("d"), format.lastIndexOf("d")+1));
			c.set(year, month-1, day);
		}
		if(time != null) {
			int hour = Integer.parseInt(time.substring(TIME_FORMAT.indexOf("H"), TIME_FORMAT.lastIndexOf("H")+1));
			int minutes = Integer.parseInt(time.substring(TIME_FORMAT.indexOf("i"), TIME_FORMAT.lastIndexOf("i")+1));
			c.set(Calendar.HOUR_OF_DAY, hour);
			c.set(Calendar.MINUTE, minutes);
			int seconds = fullTime? Integer.parseInt(time.substring(FULL_TIME_FORMAT.indexOf("s"), FULL_TIME_FORMAT.lastIndexOf("s")+1)): 0;
			c.set(Calendar.SECOND, seconds);
		}
		return c;
	}
	
	public Calendar toStandard(String date, String time) throws Exception {
		return toStandard(date, time, false);
	}
	
	public String getShortDateFormat() {
		return DATE_FORMAT.get(locale).replace("dd", "d").replace("mm", "m").replace("YYYY", "Y");
	}
	
	public String getShortTimeFormat(boolean fullTime) {
		return (fullTime? FULL_TIME_FORMAT.replace("ss", "s"): TIME_FORMAT).replace("HH", "H").replace("ii", "i");
	}
}
