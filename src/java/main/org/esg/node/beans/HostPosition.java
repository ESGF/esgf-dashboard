package org.esg.node.beans;

import java.io.Serializable;
import java.util.List;

/**
 * @author University of Salento and CMCC
 */
public class HostPosition extends HostBase implements Serializable {
	private static final long serialVersionUID = 1L;

	private String city = null;
	private Integer numInstances = null;
	private Number latitude = null;
	private Number longitude = null;
	private Number activity = null;
	private Integer regUsers = null;
	private Integer nodeType = null;
	//private String esgfVersion = null;
	//private String esgfRelease = null;
	
	private List<Integer> idProjects = null;
	
	public String getCity() {
		return city;
	}
	public void setCity(String city) {
		this.city = city;
	}
	public Integer getNumInstances() {
		return numInstances;
	}
	public void setNumInstances(Integer numInstances) {
		this.numInstances = numInstances;
	}
	public Number getLatitude() {
		return latitude;
	}
	public void setLatitude(Number latitude) {
		this.latitude = latitude;
	}
	public Number getLongitude() {
		return longitude;
	}
	public void setLongitude(Number longitude) {
		this.longitude = longitude;
	}
	public void setActivity(Number activity) {
		this.activity = activity;
	}
	public Number getActivity() {
		return activity;
	}
	public void setIdProjects(List<Integer> idProjects) {
		this.idProjects = idProjects;
	}
	public List<Integer> getIdProjects() {
		return idProjects;
	}

	/**
	 * @return the regUsers
	 */
	public Integer getRegUsers() {
		return regUsers;
	}
	/**
	 * @param regUsers the regUsers to set
	 */
	public void setRegUsers(Integer regUsers) {
		this.regUsers = regUsers;
	}
	/**
	 * @return the nodeType
	 */
	public Integer getNodeType() {
		return nodeType;
	}
	/**
	 * @param nodeType the nodeType to set
	 */
	public void setNodeType(Integer nodeType) {
		this.nodeType = nodeType;
	}
	/**
	 * @return the esgfVersion
	 */
	/*public String getEsgfVersion() {
		return esgfVersion;
	}*/
	/**
	 * @param esgfVersion the esgfVersion to set
	 */
	/*public void setEsgfVersion(String esgfVersion) {
		this.esgfVersion = esgfVersion;
	}*/
	/**
	 * @return the esgfRelease
	 */
	/*public String getEsgfRelease() {
		return esgfRelease;
	}*/
	/**
	 * @param esgfRelease the esgfRelease to set
	 */
	/*public void setEsgfRelease(String esgfRelease) {
		this.esgfRelease = esgfRelease;
	}*/
}
