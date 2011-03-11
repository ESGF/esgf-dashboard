package org.esg.node.chart;

import java.io.Serializable;
import java.util.List;
import org.esg.node.chart.axis.XAxis;
import org.esg.node.chart.axis.YAxis;
import org.esg.node.chart.util.*;

/**
 * @author University of Salento and CMCC
 */
public class Chart <T> implements Serializable {
	private static final long serialVersionUID = 1L;
	
	private Title title = null;
	private XAxis x_axis = null;
	private YAxis y_axis = null;
	private String bg_colour = null;
	private Text x_legend = null;
	private Text y_legend = null;	
	private int is_decimal_separator_comma = 1;
	private int is_fixed_num_decimals_forced = 1;
	private int is_thousand_separator_disabled = 0;
	private int num_decimals = 2;
	private List<T> elements = null;

	public void setElements(List<T> elements) {
		this.elements = elements;
	}
	public List<T> getElements() {
		return elements;
	}
	public Title getTitle() {
		return title;
	}
	public void setTitle(Title title) {
		this.title = title;
	}
	public XAxis getX_axis() {
		return x_axis;
	}
	public void setX_axis(XAxis xAxis) {
		x_axis = xAxis;
	}
	public YAxis getY_axis() {
		return y_axis;
	}
	public void setY_axis(YAxis yAxis) {
		y_axis = yAxis;
	}
	public void setBg_colour(String bg_colour) {
		this.bg_colour = bg_colour;
	}
	public String getBg_colour() {
		return bg_colour;
	}
	public int getIs_decimal_separator_comma() {
		return is_decimal_separator_comma;
	}
	public void setIs_decimal_separator_comma(int isDecimalSeparatorComma) {
		is_decimal_separator_comma = isDecimalSeparatorComma;
	}
	public int getIs_fixed_num_decimals_forced() {
		return is_fixed_num_decimals_forced;
	}
	public void setIs_fixed_num_decimals_forced(int isFixedNumDecimalsForced) {
		is_fixed_num_decimals_forced = isFixedNumDecimalsForced;
	}
	public int getIs_thousand_separator_disabled() {
		return is_thousand_separator_disabled;
	}
	public void setIs_thousand_separator_disabled(int isThousandSeparatorDisabled) {
		is_thousand_separator_disabled = isThousandSeparatorDisabled;
	}
	public int getNum_decimals() {
		return num_decimals;
	}
	public void setNum_decimals(int numDecimals) {
		num_decimals = numDecimals;
	}
	
	public Text getX_legend() {
		return x_legend;
	}
	public void setX_legend(Text xLegend) {
		x_legend = xLegend;
	}
	public Text getY_legend() {
		return y_legend;
	}
	public void setY_legend(Text yLegend) {
		y_legend = yLegend;
	}
}
