var ge;

google.load("earth", "1");
var latHost = -1;
var lngHost = -1;
var nameHost = '';
var activityHost = -1;
var glob_error=1;

function init() {
    google.earth.createInstance('server_map_canvas', initCB, failureCB);
    return glob_error;
}



function initCB(instance) {
    ge = instance;
    ge.getWindow().setVisibility(true);
    // add a navigation control

    ge.getNavigationControl().setVisibility(ge.VISIBILITY_AUTO);
    addFeatures();
    // set lookAt

    ge.getOptions().setFlyToSpeed(0.2);
    var lookAt = ge.createLookAt('');
    lookAt.setLatitude(latHost);
    lookAt.setLongitude(lngHost);
    lookAt.setRange(350000.0); //default is 0.0
    ge.getView().setAbstractView(lookAt);
    glob_error=1;

    //alert('initCB: ' + glob_error);

}

function failureCB(errorCode) {

	//alert('errorCode: ' + errorCode);
	glob_error=0;
	//alert('failureCB: ' + glob_error);

}

function initGE(lat, lng, name, activity) {

	latHost = lat;
	lngHost = lng;
	nameHost = name;
	activityHost = activity;
	var ret_init=init();
	//alert('initGE: ' + ret_init);
	return(ret_init);
}

function addFeatures() {
	var styleRed = createStyle('http://maps.google.com/mapfiles/kml/paddle/red-circle.png');
    var styleYellow = createStyle('http://maps.google.com/mapfiles/kml/paddle/ylw-circle.png');
    var styleGreen = createStyle('http://maps.google.com/mapfiles/kml/paddle/grn-circle.png');

    var style;

    if (activityHost>=0 && activityHost<34)
    	style = styleRed;
    else if (activityHost>=34 && activityHost<67)
    	style = styleYellow;
    else
    	style = styleGreen;
	var placemark = createPlacemark(nameHost, latHost, lngHost, style);
	ge.getFeatures().appendChild(placemark);
}

function createPlacemark(name, lat, lon, style) {
	var point = ge.createPoint(''); 
    point.setLatitude(lat);
    point.setLongitude(lon);
    point.setAltitude(0);
    placemark = ge.createPlacemark('');
    placemark.setName(name);
    placemark.setGeometry(point);
    placemark.setStyleSelector(style);
	return placemark;

}

function createStyle(iconHref) {
	var icon = ge.createIcon('');
	var style = ge.createStyle('');
	icon.setHref(iconHref);
    style.getIconStyle().setIcon(icon);
    style.getIconStyle().setScale(1.0);
	return style;
}

/* var ge;
google.load("earth", "1");
var latHost = -1;
var lngHost = -1;
var nameHost = '';
var activityHost = -1;

function init() {
	google.earth.createInstance('server_map_canvas', initCB, failureCB);
   
}

function initCB(instance) {	
   ge = instance;
   ge.getWindow().setVisibility(true);

   // add a navigation control
   ge.getNavigationControl().setVisibility(ge.VISIBILITY_AUTO);
   addFeatures();

   // set lookAt
   ge.getOptions().setFlyToSpeed(0.2);
   var lookAt = ge.createLookAt('');
   lookAt.setLatitude(latHost);
   lookAt.setLongitude(lngHost);
   lookAt.setRange(350000.0); //default is 0.0
   ge.getView().setAbstractView(lookAt);
}

function failureCB(errorCode) {
}

// Main function

function initGE(lat, lng, name, activity) {
	latHost = lat;
	lngHost = lng;
	nameHost = name;
	activityHost = activity;
	init();
}


function addFeatures() {
	var styleRed =
createStyle('http://maps.google.com/mapfiles/kml/paddle/red-circle.png');
   var styleYellow =
createStyle('http://maps.google.com/mapfiles/kml/paddle/ylw-circle.png');
   var styleGreen =
createStyle('http://maps.google.com/mapfiles/kml/paddle/grn-circle.png');

   var style;

   if (activityHost>=0 && activityHost<34)
   	style = styleRed;
   else if (activityHost>=34 && activityHost<67)
   	style = styleYellow;
   else
   	style = styleGreen;

	var placemark = createPlacemark(nameHost, latHost, lngHost, style);
	ge.getFeatures().appendChild(placemark);
}

function createPlacemark(name, lat, lon, style) {
	var point = ge.createPoint('');
   point.setLatitude(lat);
   point.setLongitude(lon);
   point.setAltitude(0);
   placemark = ge.createPlacemark('');
   placemark.setName(name);
   placemark.setGeometry(point);
   placemark.setStyleSelector(style);
	return placemark;
}

function createStyle(iconHref) {
	var icon = ge.createIcon('');
	var style = ge.createStyle('');
	icon.setHref(iconHref);
   style.getIconStyle().setIcon(icon);
   style.getIconStyle().setScale(1.0);
	return style;
} */


