package org.esg.node.actions.json;

import org.esg.node.chart.PieChart;
import org.esg.node.chart.element.ElementPie;
import org.esg.node.chart.element.Slice;
import org.esg.node.chart.util.Title;

import com.opensymphony.xwork2.ActionSupport;
import java.sql.*;
import java.util.*;
import org.esg.node.utils.*;

/**
 * @author University of Salento and CMCC
 */
public class UsersOnHostsByProjectPieChartAction extends ActionSupport {
	private static final long serialVersionUID = 1L;

	private Integer idProject = null;
	private PieChart pieChart = null;
	
	@Override
	public void validate() {
		if(idProject == null) {
			this.addActionError("");
			return;
		}
	}
	
	@Override
	public String execute() throws Exception {
		Connection conn = null;
		List<Slice> slices = new LinkedList<Slice>();
		try {
			conn = Constants.DATASOURCE.getConnection();
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.USERS_ON_HOSTS_BY_PROJECT.getSql());
			stmt.setInt(1, idProject);
			System.out.println("Query eseguita ="+ SqlQuery.USERS_ON_HOSTS_BY_PROJECT.getSql());
			ResultSet rs = stmt.executeQuery();
			
			while(rs.next()) {
				String name = rs.getString("name")==null? rs.getString("ip"): rs.getString("name");
				int count = rs.getInt("regusers");
				Slice slice = new Slice();
				slice.setLabel(name + "[" + count + "]");
				slice.setTooltip(name + "\n" + count + " registered users");
				slice.setValue(count);
				slices.add(slice);				
			}
			rs.close();
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		String[] colors = { "000000", "0000ff", "00ffff", "00ff00", "ffff00", "ff0000", "ff00ff", "ffffff" };
		if(slices.size() != 0) {
			int increment = 0;
			for(Slice slice : slices) {				
				slice.setColour("#" + colors[increment]);
				increment = (increment + 1) % colors.length;
			}
			if(slices.size() % colors.length == 1) slices.get(slices.size()-1).setColour(colors[1]);
		}
		pieChart = new PieChart();
		List<ElementPie> elements = new LinkedList<ElementPie>();
		ElementPie element = new ElementPie();
		element.setAlpha(0.9f);
		pieChart.setTitle(new Title("Users Distribution", "{" + getText("avgProjectActivityPieChart.title.style") + "}"));
		element.setValues(slices);
		element.setGradientFill(true);
		element.setRadius(50);
		element.setStartAngle(0);
		elements.add(element);
		pieChart.setBg_colour(getText("chart.defaultBgColor"));
		pieChart.setBorder(1);
		pieChart.setAnimate(true);
		pieChart.setElements(elements);		
		
		return SUCCESS;
	}

	public void setIdProject(Integer idProject) {
		this.idProject = idProject;
	}

	public Integer getIdProject() {
		return idProject;
	}

	public void setPieChart(PieChart pieChart) {
		this.pieChart = pieChart;
	}

	public PieChart getPieChart() {
		return pieChart;
	}
}
