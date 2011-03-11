Ext.namespace('Ext.ux');

/**
 * @class Ext.ux.TagCloud
 * @extends Ext.Component
 * This class represents a tag cloud with support for remote loading and tag selection
 * @constructor
 * Create a new TagCloud
 * @param {Object} config A configuration object that sets properties for this tag cloud
 * The configuration object supports these properties:
         * <ul style="padding:5px;padding-left:16px;">
	     * <li>{Ext.data.Store} store - The data store for the cloud</li>
	     * <li>{String} displayField - The field from the data store to display</li>
		 * <li>{String} weightField - The field from the data store used to determine the ranking or "weight"</li>
		 * <li>{Boolean} displayWeight - (Optional) When true, the items in the cloud will also visually show their weight
	     * </ul>
 */
Ext.ux.TagCloud = function(config){

    // Call the parent class constructor 
	Ext.ux.TagCloud.superclass.constructor.call(this, config);

	if(this.store)
		this.setStore(this.store);
        
    this.displayField = this.displayField || 'tag';
    this.weightField = this.weightField || 'count';
		
    // private
    this.addEvents({
    
        /**
         * @event tagselect
         * Fires when a tag is selected (clicked on)
         * @param {Ext.ux.TagCloud}     cloud This tag cloud
         * @param {Ext.data.Record}     record The data record returned from the underlying store
         * @param {int}                 index The index of the selected item in the tag cloud
         */
        'tagselect' : true
    
    });
    
    this.nodes = [];
    
}	
 
Ext.extend(Ext.ux.TagCloud, Ext.Component, {
	
	/* private */
    defaultAutoCreate : {tag: "div", cls: "x-cloud"},
    
    getDataSource : function(){
        return this.store;
    },
    
    /*
	 * Sets the store for this cloud
	 * @param {Ext.data.Store} store the store to set
	 */
	setStore : function(store){
        // when the store is loaded, automatically refresh the cloud
		store.on('load', this.refresh, this);
		this.store = store;
        // sort alphabetically
        this.store.remoteSort = false;
        this.store.sort(this.displayField, 'ASC');
	},

	// private
	onRender : function(ct, position){	
        
        this.container = ct;
	
		if(this.el){
            this.el = Ext.get(this.el);
            if(!this.target){
                ct.dom.appendChild(this.el.dom);
            }
        }else {
            var cfg = this.getAutoCreate();
            if(!cfg.name){
                cfg.name = this.name || this.id;
            }
            this.el = ct.createChild(cfg, position);
        }
        
		this.list = this.el.createChild({tag: "ol", cls: "x-cloud-ordered-list"});
        
	},

	// private
	refresh : function(){
		
		/* First, remove all children */
		this.clearNodes();
        
		/* Determine the spread values */
		this.getWeightDistribution();
        
		var records = this.store.getRange();
		for(var i=0; i < records.length; i++){

            var count = records[i].data[this.weightField];
            
			var child = this.list.createChild({
                tag: "li", 
                cls: "x-cloud-item "+this.getWeightClassification(count),
                html: '<a href="#">'+records[i].data[this.displayField]+(this.displayWeight ? ' ('+count+')' : '')+'</a>'
                });
			
			child.on('click', this.onSelect, this);
			
		}
		
		/* Fade the list in */
        this.list.fadeIn(
            {
            duration:0.5,
            block:true
            }
        );
		
		/* Store a list of all child nodes */
        this.nodes = this.list.dom.childNodes;
		
	},
	
	// private
	clearNodes : function(){
        while (this.list.dom.firstChild){
            this.list.dom.removeChild(this.list.dom.firstChild);
        }
	},

    // private
    onSelect : function(e, t){
    
        var item = t.parentNode;
        var index = this.indexOf(item);
        
        /* Remove from selection from any selected children */
        var selected = this.list.query('.x-cloud-item-selected');
        if(selected.length > 0)
            Ext.get(selected[0]).removeClass('x-cloud-item-selected');
        
        this.fireEvent('tagselect', this, this.getDataSource().getAt(index), index);
        
        // Prevent the link href from being followed
        Ext.EventObject.stopEvent(e);
    },
    
    // private
    indexOf : function(node){
        var ns = this.nodes;
        for(var i = 0, len = ns.length; i < len; i++){
            if(ns[i] == node){
                return i;
            }
        }
        return -1;
    },

	// private
	getWeightClassification : function(weight){

		if(weight == this.max)
			return 'largest';
		if(weight == this.min)
			return 'smallest';
		if(weight > (this.min + (this.distribution*2)))
			return 'large';
		if(weight > (this.min + this.distribution))
			return 'medium';

		return 'small';
	},

	// private
	getWeightDistribution : function(){
	
		var records = this.store.getRange();
        if(records.length==0){
            this.max = this.min = 0;
            return;
        }
		
        this.max = records[0].data.count;
        this.min = records[0].data.count;

		for(var i=0; i < records.length; i++){
			var count = records[i].data[this.weightField];
			if(count > this.max){
				this.max = count;
			}
			if(count < this.min){
				this.min = count;
			}
		}

		if(!this.distribution)
			this.distribution = (this.max - this.min) / 5;

	}
	
});