/* Author: University of Salento and CMCC */

/* Author: University of Salento and CMCC */

//var map_availability=null;
//var map_registered_users=null;
//var map_node_type=null;

var margin_map =0.5;
//var map1;
//var map2;
//var map3;
//var markers1 = new Array();
//var markers2 = new Array();
//var markers3 = new Array();

var open_map1 = 0;
var open_map2 = 0;
var open_map3 = 0;
var map1;
var map2;
var map3;
var map_project;

var markers=null; 

// callback for allproject availability
function onSuccess(response, opts) {
	
	var hosts = eval( response.responseText );

	markers = new Array();
	loadGrid(hosts);

	var latMax = -90, lngMax = -180, latMin = 90, lngMin = 180;

	for(var i = 0; i < hosts.length; i ++) {
		if(hosts[i].latitude < latMin)
			latMin = hosts[i].latitude;
		if(hosts[i].longitude < lngMin)
			lngMin = hosts[i].longitude;
		if(hosts[i].latitude > latMax)
			latMax = hosts[i].latitude;
		if(hosts[i].longitude > lngMax)
			lngMax = hosts[i].longitude;
	}
	
//	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax, lngMin);
//	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin, lngMax);
	//createMap(bounds, 'map_canvas',0);
	//if (open_map1==0){
	//	open_map1=1;
	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax-margin_map, lngMin-margin_map);// considering a margin for scaledSize images
	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin+margin_map, lngMax+margin_map); // considering a margin for scaledSize images	
	var bounds = new google.maps.LatLngBounds(latlngSW, latlngNE);
	map_project = createMap(bounds, 'map_canvas',0);	
	for(var i = 0; i < hosts.length; i ++)
		markers.push(addMarker(map_project,hosts[i]));
	//}
}

function onSuccessAvailability(response, opts) {
	
	//alert("onSuccessAvailability "+ open_map1);
	//alert("Map1 "+ map1);
	var hosts = eval( response.responseText );
	
	if (open_map1==1) 
		return;
	open_map1=1;
	
	//if (open_map1==0) {
	
	markers = new Array();
	
	loadGrid(hosts); //to be added after	
	
	//}	
	
	var latMax = -90, lngMax = -180, latMin = 90, lngMin = 180;
	for(var i = 0; i < hosts.length; i ++) {
		//vertices[i] = new google.maps.LatLng(hosts[i].latitude,hosts[i].longitude);
		if(hosts[i].latitude < latMin)
			latMin = hosts[i].latitude;
		if(hosts[i].longitude < lngMin)
			lngMin = hosts[i].longitude;
		if(hosts[i].latitude > latMax)
			latMax = hosts[i].latitude;
		if(hosts[i].longitude > lngMax)
			lngMax = hosts[i].longitude;
	}
	
//	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax, lngMin);
//	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin, lngMax);
	//createMap(bounds, 'map_canvas',0);
	//if (open_map1==0){
	

	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax-margin_map, lngMin-margin_map);// considering a margin for scaledSize images
	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin+margin_map, lngMax+margin_map); // considering a margin for scaledSize images	
	var bounds = new google.maps.LatLngBounds(latlngSW, latlngNE);
	map1 = createMap(bounds, 'map_canvas_availability',0);	
	for(var i = 0; i < hosts.length; i ++)
			markers.push(addMarker(map1,hosts[i]));
	
	//}
}


function onSuccessUsers(response, opts) {
	var hosts = eval( response.responseText );
	
	//alert("onSuccessUsers: "+ open_map2 + " Map2: "+ map2);
	
	if (open_map2==1) 
		return;
	open_map2=1;
	
	//var vertices = new Array();
	var latMax = -90, lngMax = -180, latMin = 90, lngMin = 180;

	var scale_type= 1; // 0 linear ; 1 logarithmic
	//alert("Scale type = "+scale_type);

	for(var i = 0; i < hosts.length; i ++) {
		//vertices[i] = new google.maps.LatLng(hosts[i].latitude,hosts[i].longitude);
		if(hosts[i].latitude < latMin)
			latMin = hosts[i].latitude;
		if(hosts[i].longitude < lngMin)
			lngMin = hosts[i].longitude;
		if(hosts[i].latitude > latMax)
			latMax = hosts[i].latitude;
		if(hosts[i].longitude > lngMax)
			lngMax = hosts[i].longitude;
	}
	//createMap(bounds, 'map_canvas_users',0);
	//if (open_map2==0) {

	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax-margin_map, lngMin-margin_map);// considering a margin for scaledSize images
	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin+margin_map, lngMax+margin_map); // considering a margin for scaledSize images
	var bounds = new google.maps.LatLngBounds(latlngSW, latlngNE);
	map2 = createMap(bounds, 'map_canvas_users',0);
	for(var i = 0; i < hosts.length; i ++)
		if (hosts[i].regUsers>0) // only idp with at least 1 registered user
			addMarkerRegUser(map2, hosts[i],scale_type,i+1);
	//}
}

function onSuccessNodeType(response, opts) {
	var hosts = eval( response.responseText );
	
	//alert("onSuccessNodeType: "+ open_map3 + " Map3: "+ map3);	
	
	if (open_map3==1) 
		return;
	open_map3=1;
	
	//var vertices = new Array();
	var latMax = -90, lngMax = -180, latMin = 90, lngMin = 180;

	for(var i = 0; i < hosts.length; i ++) {
		//vertices[i] = new google.maps.LatLng(hosts[i].latitude, hosts[i].longitude);
		if(hosts[i].latitude < latMin)
			latMin = hosts[i].latitude;
		if(hosts[i].longitude < lngMin)
			lngMin = hosts[i].longitude;
		if(hosts[i].latitude > latMax)
			latMax = hosts[i].latitude;
		if(hosts[i].longitude > lngMax)
			lngMax = hosts[i].longitude;
	}

//	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax, lngMin);
//	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin, lngMax);
	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax-margin_map, lngMin-margin_map);// considering a margin for scaledSize images
	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin+margin_map, lngMax+margin_map); // considering a margin for scaledSize images
	var bounds = new google.maps.LatLngBounds(latlngSW, latlngNE);
	map3 = createMap(bounds, 'map_canvas_nodetype',0);
	for(var i = 0; i < hosts.length; i ++)
			addMarkerNodeType(map3, hosts[i]);
}

//var map;
function createMap(bounds, idDiv,typemap) {
   var myOptions = {
		mapTypeId: google.maps.MapTypeId.ROADMAP,
		scrollwheel: false
	};
	var map = new google.maps.Map(document.getElementById(idDiv), myOptions);
	map.fitBounds(bounds);

	var homeControlDiv = document.createElement('DIV');
	var homeControl = new HomeControl(homeControlDiv, map, bounds);
	homeControlDiv.index = 1;
	map.controls[google.maps.ControlPosition.TOP_RIGHT].push(homeControlDiv);
	
	if (typemap==1) // regusers map
	{ 
		var scaleControlDiv = document.createElement('DIV');
		var scaleControl = new ScaleControl(homeControlDiv, map, bounds);
		scaleControlDiv.index = 1;
		map.controls[google.maps.ControlPosition.TOP_RIGHT].push(scaleControlDiv);		
	}	
	return map;
}

var openedInfoWindow = null; // keep track of currently opened info window
//var markers = new Array();



function addMarker(map0, host) {

	var latLng = new google.maps.LatLng(host.latitude, host.longitude);
	//alert ("Coordinate:(" + host.latitude + "," + host.longitude+ ")");
	
	image = new google.maps.MarkerImage(chooseIcon(host.activity? host.activity: null,"availability","svg"),
			  //null,
		      // origin
		      //null,
		      // anchor
		      //new google.maps.Point(20, 20),
		      //null,
		      // scaledSize
		      //new google.maps.Size(20,20),
		      //null,
		      //new google.maps.Point(10, 10)
			  null,
		      // origin
		      null,
			  //new google.maps.Point(0,0),
		      // anchor
		      new google.maps.Point(10, 10),
		      // scaledSize
		      new google.maps.Size(20,20)
	);
	
	var marker = new google.maps.Marker({
	    position: latLng,
	    map: map0,
		//title: host.name==null? host.ip: host.name,
	    title: host.name==null? (host.ip+": "+ (host.activity? host.activity: "-")+" (availability)"): (host.name + ": " + (host.activity? host.activity: "-") + " (availability)"),
	    //icon: chooseIcon(host.activity? host.activity: null,"availability","png")
		icon: image		
	});
	//markers.push(marker);
	
	google.maps.event.addListener(marker, 'click', function() {
		if(openedInfoWindow != null)
			openedInfoWindow.close();
		openedInfoWindow = new google.maps.InfoWindow();
		openedInfoWindow.setContent(markerOnClickListener(host));
		//openedInfoWindow.open(map, marker);
		openedInfoWindow.setPosition(latLng);
		openedInfoWindow.open(map0);
	});
	return marker;
}


function addMarkerNodeType(map0, host) {
	var latLng = new google.maps.LatLng(host.latitude, host.longitude);
	//alert ("Coordinate:(" + host.latitude + "," + host.longitude+ ")");
	
	image = new google.maps.MarkerImage(chooseIconNodeType(host.nodeType? host.nodeType: null,"nodetype/stack","svg"),
			  //null,
		      // origin
		      //null,
		      // anchor
		      //new google.maps.Point(20, 20),
		      //null,
		      // scaledSize
		      //new google.maps.Size(20,20),
		      //null,
		      //new google.maps.Point(10, 10)
			  null,
		      // origin
		      null,
			  //new google.maps.Point(0,0),
		      // anchor
		      new google.maps.Point(20, 20),
		      // scaledSize
		      new google.maps.Size(30,40)
	);
	
	var marker = new google.maps.Marker({
	    position: latLng,
	    map: map0,
		//title: host.name==null? host.ip: host.name,
	    title: host.name==null? (host.ip+" "+ nodeType_str(host.nodeType)): (host.name + " " + nodeType_str(host.nodeType)),
	    //icon: chooseIcon(host.activity? host.activity: null,"availability","png")
		icon: image		
	});
	//markers.push(marker);
	
	google.maps.event.addListener(marker, 'click', function() {
		if(openedInfoWindow != null)
			openedInfoWindow.close();
		openedInfoWindow = new google.maps.InfoWindow();
		openedInfoWindow.setContent(markerOnClickListener(host));
		//openedInfoWindow.open(map, marker);
		openedInfoWindow.setPosition(latLng);
		openedInfoWindow.open(map0);
	});
}


function log10(val) {
	  return Math.log(val) / Math.log(10);
	}

function addMarkerRegUser(map0, host,scaletype,index_host) {
	//alert("Registered Users " + host.regUsers+ " zIndex" + index_host);
	var latLng = new google.maps.LatLng(host.latitude, host.longitude);
	var latoimg=20; //base value
	var gradient_level=0; // 0<=value<=100 
	
	if (scaletype==0){
		gradient_level = host.regUsers/100;
		latoimg = Math.round(10+(host.regUsers*40)/10000);
	}
	else {
		gradient_level = log10(host.regUsers)*25;
		latoimg = Math.round(10+log10(host.regUsers)*10);
	}
	
	//gradient_level=95;
	//var markerShape = {coord: [latoimg/2,latoimg/2, latoimg/2],  type: "circle"}; 
	//alert("Users="+ host.regUsers+ " Lato img ="+ latoimg);
	image = new google.maps.MarkerImage(chooseIcon(gradient_level,"regusers/pentagon/bluegradient","svg"),
			  null,
		      // origin
		      null,
			  //new google.maps.Point(0,0),
		      // anchor
		      new google.maps.Point(latoimg/2, latoimg/2),
		      // scaledSize
		      new google.maps.Size(latoimg,latoimg)
	);
	
	var marker = new google.maps.Marker({
	    position: latLng,
	    map: map0,
		title: host.name==null? (host.ip+": "+ host.regUsers+" registered users"): (host.name + ": " + host.regUsers + " registered users"),
	    zIndex: index_host,
		icon: image
	    //shape: markerShape
	});
	//markers.push(marker);
	//alert(host.name);
	
	google.maps.event.addListener(marker, 'click', function() {
		if(openedInfoWindow != null)
			openedInfoWindow.close();
		openedInfoWindow = new google.maps.InfoWindow();
		openedInfoWindow.setContent(markerOnClickListener(host));
		//openedInfoWindow.open(map, marker);
		openedInfoWindow.setPosition(latLng);
		openedInfoWindow.open(map0);
	});
}

/*function addMarker(host, users, nodeType) {
	var latLng = new google.maps.LatLng(host.latitude, host.longitude);
	var image;
	if (users!=0) {
		// a call to a calculateScaledSizeFunction and then
		image = new google.maps.MarkerImage(chooseIcon(host.activity? host.activity: null),
				  null,
			      // origin
			      null,
			      // anchor
			      new google.maps.Point(users/2, users/2),
			      // scaledSize
			      new google.maps.Size(users,users)
		);
	}
	else {
		image = new google.maps.MarkerImage(chooseIcon(host.activity? host.activity: null),
				  null,
			      null,
			      new google.maps.Point(24,24)
		);
	}

	var marker = new google.maps.Marker({
	    position: latLng,
	    map: map,
		title: host.name==null? host.ip: host.name,
	    icon: image
	});
	markers.push(marker);

	google.maps.event.addListener(marker, 'click', function() {
		if(openedInfoWindow != null)
			openedInfoWindow.close();
		openedInfoWindow = new google.maps.InfoWindow();
		openedInfoWindow.setContent(markerOnClickListener(host));
		openedInfoWindow.open(map, marker);
	});

	if (nodeType) {
		google.maps.event.addListener(marker, 'mouseover', function() {
			alert('data node type!');
		});
	}
}*/



/*function onSuccess(response, opts) {
	var hosts = eval( response.responseText );
	loadGrid(hosts);
	//var vertices = new Array();
	var latMax = -90, lngMax = -180, latMin = 90, lngMin = 180;
	
	for(var i = 0; i < hosts.length; i ++) {
		//vertices[i] = new google.maps.LatLng(hosts[i].latitude, hosts[i].longitude);
		if(hosts[i].latitude < latMin)
			latMin = hosts[i].latitude;
		if(hosts[i].longitude < lngMin)
			lngMin = hosts[i].longitude;
		if(hosts[i].latitude > latMax)
			latMax = hosts[i].latitude;
		if(hosts[i].longitude > lngMax)
			lngMax = hosts[i].longitude;
	}
	var latlngSW = new google.maps.LatLng(latMax, lngMin);
	var latlngNE = new google.maps.LatLng(latMin, lngMax);
	
	var bounds = new google.maps.LatLngBounds(latlngSW, latlngNE);
	createMap(bounds);
	for(var i = 0; i < hosts.length; i ++) 
		addMarker(hosts[i]);
}




var map;
function createMap(bounds) {
    var myOptions = {
		mapTypeId: google.maps.MapTypeId.ROADMAP,
		scrollwheel: false
	};
	map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
	map.fitBounds(bounds);
	
	var homeControlDiv = document.createElement('DIV');
	var homeControl = new HomeControl(homeControlDiv, map, bounds);
	homeControlDiv.index = 1;
	map.controls[google.maps.ControlPosition.TOP_RIGHT].push(homeControlDiv);
}

var openedInfoWindow = null; // keep track of currently opened info window
var markers = new Array();

*/
