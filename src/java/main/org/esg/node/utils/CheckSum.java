package org.esg.node.utils;

import java.nio.charset.Charset;
import java.security.MessageDigest;

/**
 * @author University of Salento and CMCC
 */
public class CheckSum {
	private Charset charset = null;
	private String algorithm = "MD5";
	
	private final static char []digit = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	
	public CheckSum() { 
		setCharset(Charset.defaultCharset());
	}
	
	public CheckSum(String charsetName) {
		setCharset(Charset.forName(charsetName));
	}
	
	public CheckSum(Charset charset) {
		setCharset(charset);
	}
	
	public String getDigest(String text) throws Exception {
		byte[] testo = text.getBytes(charset);
		
		MessageDigest messageDigest = MessageDigest.getInstance(algorithm);		
		byte[] sum = messageDigest.digest(testo);
		
		String hexSum = "";		
		for(int index = 0; index < sum.length; index ++) {
			int value = sum[index]<0? sum[index]+256: sum[index];
			hexSum += digit[ value/16 ] + "" + digit[ value%16 ];
		}
		return hexSum;		
	}

	public void setCharset(Charset charset) {
		this.charset = charset;
	}

	public Charset getCharset() {
		return charset;
	}
	
	public String getAlgorithm() {
		return algorithm;
	}

	public void setAlgorithm(String algorithm) {
		this.algorithm = algorithm;
	}
}
