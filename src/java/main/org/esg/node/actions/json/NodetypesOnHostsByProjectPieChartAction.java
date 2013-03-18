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
public class NodetypesOnHostsByProjectPieChartAction extends ActionSupport {
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
			PreparedStatement stmt = conn.prepareStatement(SqlQuery.NODETYPES_ON_HOSTS_BY_PROJECT.getSql());
			stmt.setInt(1, idProject);
			stmt.setInt(2, idProject);
			stmt.setInt(3, idProject);
			stmt.setInt(4, idProject);
			System.out.println("Query eseguita ="+ SqlQuery.NODETYPES_ON_HOSTS_BY_PROJECT.getSql());
			ResultSet rs = stmt.executeQuery();
			rs.next();
			int count;
			count = rs.getInt("datatype");
			Slice slice = new Slice();
			slice.setLabel("Data " + "[" + count + "]");
			slice.setTooltip("Data " + "\n" + count + " hosts");
			slice.setValue(count);
			slices.add(slice);				

			count = rs.getInt("indextype");
			slice = new Slice();
			slice.setLabel("Index " + "[" + count + "]");
			slice.setTooltip("Index " + "\n" + count + " hosts");
			slice.setValue(count);
			slices.add(slice);				

			count = rs.getInt("idptype");
			slice = new Slice();
			slice.setLabel("Idp " + "[" + count + "]");
			slice.setTooltip("Idp " + "\n" + count + " hosts");
			slice.setValue(count);
			slices.add(slice);				

			count = rs.getInt("computetype");
			slice = new Slice();
			slice.setLabel("Compute " + "[" + count + "]");
			slice.setTooltip("Compute " + "\n" + count + " hosts");
			slice.setValue(count);
			slices.add(slice);				
			
			rs.close();
			stmt.close();
		} catch(SQLException e) {
			return ERROR;
		} finally {
			if(conn != null) conn.close();
		}
		String[] colors = { "ff0000", "FFDA00", "00ff00", "081C94", "ffff00", "ff0000", "ff00ff", "ffffff" };
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
		pieChart.setTitle(new Title("Data Node Types Distribution", "{" + getText("avgProjectActivityPieChart.title.style") + "}"));
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
