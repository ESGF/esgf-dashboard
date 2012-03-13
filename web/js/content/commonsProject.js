/* Author: University of Salento and CMCC */

/* Author: University of Salento and CMCC */

function onSuccess(response, opts) {
	var hosts = eval( response.responseText );
	loadGrid(hosts);
	//var vertices = new Array();
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
	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax, lngMin);
	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin, lngMax);

	var bounds = new google.maps.LatLngBounds(latlngSW, latlngNE);
	createMap(bounds, 'map_canvas');
	for(var i = 0; i < hosts.length; i ++)
		addMarker(hosts[i], 0, 0);
}

function onSuccessUsers(response, opts) {
	var hosts = eval( response.responseText );
	//var vertices = new Array();
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
	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax, lngMin);
	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin, lngMax);

	var bounds = new google.maps.LatLngBounds(latlngSW, latlngNE);
	createMap(bounds, 'map_canvas_users');
	for(var i = 0; i < hosts.length; i ++)
		addMarker(hosts[i], 50*i, 0);
}

function onSuccessNodeType(response, opts) {
	var hosts = eval( response.responseText );
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
	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax, lngMin);
	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin, lngMax);

	var bounds = new google.maps.LatLngBounds(latlngSW, latlngNE);
	createMap(bounds, 'map_canvas_nodetype');
	for(var i = 0; i < hosts.length; i ++)
		addMarker(hosts[i], 0, 1);
}

var map;
function createMap(bounds, idDiv) {
   var myOptions = {
		mapTypeId: google.maps.MapTypeId.ROADMAP,
		scrollwheel: false
	};
	map = new google.maps.Map(document.getElementById(idDiv), myOptions);
	map.fitBounds(bounds);

	var homeControlDiv = document.createElement('DIV');
	var homeControl = new HomeControl(homeControlDiv, map, bounds);
	homeControlDiv.index = 1;
	map.controls[google.maps.ControlPosition.TOP_RIGHT].push(homeControlDiv);
}

var openedInfoWindow = null; // keep track of currently opened info window
var markers = new Array();

function addMarker(host, users, nodeType) {
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
}

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

function addMarker(host) {
	var latLng = new google.maps.LatLng(host.latitude, host.longitude);
	
	var marker = new google.maps.Marker({
	    position: latLng,
	    map: map,
		title: host.name==null? host.ip: host.name,
	    icon: chooseIcon(host.activity? host.activity: null)
	});
	markers.push(marker);
	
	google.maps.event.addListener(marker, 'click', function() {
		if(openedInfoWindow != null)
			openedInfoWindow.close();
		openedInfoWindow = new google.maps.InfoWindow();
		openedInfoWindow.setContent(markerOnClickListener(host));
		openedInfoWindow.open(map, marker);
	});
}*/
