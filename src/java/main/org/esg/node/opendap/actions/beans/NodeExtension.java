package org.esg.node.opendap.actions.beans;

import java.io.Serializable;

/**
 * @author University of Salento and CMCC
 */
public class NodeExtension extends Node implements Serializable {
	private static final long serialVersionUID = 1L;

	private Integer idService = null;
	private String hostName = null;
	private String hostIP = null;
	private Integer portNumber = null;
	
	public NodeExtension() { }
	
	public NodeExtension(Integer idService, String hostName, String hostIP,
			Integer portNumber, String url) {
		super(url);
		this.idService = idService;
		this.hostName = hostName;
		this.hostIP = hostIP;
		this.portNumber = portNumber;
	}
	public Integer getIdService() {
		return idService;
	}
	public void setIdService(Integer idService) {
		this.idService = idService;
	}
	public String getHostName() {
		return hostName;
	}
	public void setHostName(String hostName) {
		this.hostName = hostName;
	}
	public String getHostIP() {
		return hostIP;
	}
	public void setHostIP(String hostIP) {
		this.hostIP = hostIP;
	}
	public Integer getPortNumber() {
		return portNumber;
	}
	public void setPortNumber(Integer portNumber) {
		this.portNumber = portNumber;
	}	
}
