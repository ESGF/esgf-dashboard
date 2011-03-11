/* Author: University of Salento and CMCC */

function onSuccess(response, opts) {
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
	var latlngSW = new google.maps.LatLng(/*latMin, lngMin);*/latMax, lngMin);
	var latlngNE = new google.maps.LatLng(/*latMax, lngMax);*/latMin, lngMax);
	
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
}
