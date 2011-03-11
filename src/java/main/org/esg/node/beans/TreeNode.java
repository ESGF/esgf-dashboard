package org.esg.node.beans;

import java.io.Serializable;
import java.util.List;

/**
 * @author University of Salento and CMCC
 */
public class TreeNode implements Serializable {
	private static final long serialVersionUID = 1L;

	private Boolean allowChildren = null;
	private Boolean allowDrag = null;
	private Boolean allowDrop = null;
	private Boolean checked = null;
	private String cls = null;
	private List<TreeNode> children = null;
	private Boolean disabled = null; 
	private Boolean draggable = null;
	private Boolean editable = null;
	private Boolean expandable = null;
	private Boolean expanded = null;
	private Boolean hidden = null;
	private String href = null;
	private String hrefTarget = null;
	private String icon = null;
	private String iconCls = null;
	private String id = null;
	private Boolean isTarget = null;
	private Boolean leaf = null;
	private Object listeners = null;
	private String qtip = null;
	private String qtipCfg = null;
	private Boolean singleClickExpand = null;
	private String text = null;
	private String uiProvider = null;
	/**
	 * Attributo personalizzato
	 */
	private Object myObject = null;
	
	public void setMyObject(Object myObject) {
		this.myObject = myObject;
	}
	public Object getMyObject() {
		return myObject;
	}
	public Boolean getAllowChildren() {
		return allowChildren;
	}
	public void setAllowChildren(Boolean allowChildren) {
		this.allowChildren = allowChildren;
	}
	public Boolean getAllowDrag() {
		return allowDrag;
	}
	public void setAllowDrag(Boolean allowDrag) {
		this.allowDrag = allowDrag;
	}
	public Boolean getAllowDrop() {
		return allowDrop;
	}
	public void setAllowDrop(Boolean allowDrop) {
		this.allowDrop = allowDrop;
	}
	public Boolean getChecked() {
		return checked;
	}
	public void setChecked(Boolean checked) {
		this.checked = checked;
	}
	public String getCls() {
		return cls;
	}
	public void setCls(String cls) {
		this.cls = cls;
	}
	public void setChildren(List<TreeNode> children) {
		this.children = children;
	}
	public List<TreeNode> getChildren() {
		return children;
	}
	public Boolean getDisabled() {
		return disabled;
	}
	public void setDisabled(Boolean disabled) {
		this.disabled = disabled;
	}
	public Boolean getDraggable() {
		return draggable;
	}
	public void setDraggable(Boolean draggable) {
		this.draggable = draggable;
	}
	public Boolean getEditable() {
		return editable;
	}
	public void setEditable(Boolean editable) {
		this.editable = editable;
	}
	public Boolean getExpandable() {
		return expandable;
	}
	public void setExpandable(Boolean expandable) {
		this.expandable = expandable;
	}
	public Boolean getExpanded() {
		return expanded;
	}
	public void setExpanded(Boolean expanded) {
		this.expanded = expanded;
	}
	public Boolean getHidden() {
		return hidden;
	}
	public void setHidden(Boolean hidden) {
		this.hidden = hidden;
	}
	public String getHref() {
		return href;
	}
	public void setHref(String href) {
		this.href = href;
	}
	public String getHrefTarget() {
		return hrefTarget;
	}
	public void setHrefTarget(String hrefTarget) {
		this.hrefTarget = hrefTarget;
	}
	public String getIcon() {
		return icon;
	}
	public void setIcon(String icon) {
		this.icon = icon;
	}
	public String getIconCls() {
		return iconCls;
	}
	public void setIconCls(String iconCls) {
		this.iconCls = iconCls;
	}
	public String getId() {
		return id;
	}
	public void setId(String id) {
		this.id = id;
	}
	public Boolean getIsTarget() {
		return isTarget;
	}
	public void setIsTarget(Boolean isTarget) {
		this.isTarget = isTarget;
	}
	public Boolean getLeaf() {
		return leaf;
	}
	public void setLeaf(Boolean leaf) {
		this.leaf = leaf;
	}
	public Object getListeners() {
		return listeners;
	}
	public void setListeners(Object listeners) {
		this.listeners = listeners;
	}
	public String getQtip() {
		return qtip;
	}
	public void setQtip(String qtip) {
		this.qtip = qtip;
	}
	public String getQtipCfg() {
		return qtipCfg;
	}
	public void setQtipCfg(String qtipCfg) {
		this.qtipCfg = qtipCfg;
	}
	public Boolean getSingleClickExpand() {
		return singleClickExpand;
	}
	public void setSingleClickExpand(Boolean singleClickExpand) {
		this.singleClickExpand = singleClickExpand;
	}
	public String getText() {
		return text;
	}
	public void setText(String text) {
		this.text = text;
	}
	public String getUiProvider() {
		return uiProvider;
	}
	public void setUiProvider(String uiProvider) {
		this.uiProvider = uiProvider;
	}
}
