/* Author: University of Salento and CMCC */

function generateWelcomeMsg(name, surname) {
	document.getElementById("loggedInUserSpan").appendChild(document.createTextNode(name + " " + surname));
}


function chooseIcon(avgActivity,imgtype,file_ext) {
	if(avgActivity < 10)
		return pathURL + "img/map_icons/"+ imgtype + "/0." + file_ext;
	else if(avgActivity < 20)
		return pathURL + "img/map_icons/"+ imgtype + "/1." + file_ext;
	else if(avgActivity < 30)
		return pathURL + "img/map_icons/"+ imgtype + "/2." + file_ext;
	else if(avgActivity < 40)
		return pathURL + "img/map_icons/"+ imgtype + "/3." + file_ext;
	else if(avgActivity < 50)
		return pathURL + "img/map_icons/"+ imgtype + "/4." + file_ext;
	else if(avgActivity < 60)
		return pathURL + "img/map_icons/"+ imgtype + "/5." + file_ext;
	else if(avgActivity < 70)
		return pathURL + "img/map_icons/"+ imgtype + "/6." + file_ext;
	else if(avgActivity < 80)
		return pathURL + "img/map_icons/"+ imgtype + "/7." + file_ext;
	else if(avgActivity < 90)
		return pathURL + "img/map_icons/"+ imgtype + "/8." + file_ext;
	else
		return pathURL + "img/map_icons/"+ imgtype + "/9." + file_ext;
}



/*function chooseIcon(avgActivity) {
	if(avgActivity < 40)
		return pathURL + "img/redMarker.png";
	else if(avgActivity < 80)
		return pathURL + "img/yellowMarker.png";
	else
		return pathURL + "img/greenMarker.png";
}*/

function urlencode (str) {
    str = (str+'').toString();
    // Tilde should be allowed unescaped in future versions of PHP (as reflected below), but if you want to reflect current
    // PHP behavior, you would need to add ".replace(/~/g, '%7E');" to the following.
    return encodeURIComponent(str).replace(/!/g, '%21').replace(/'/g, '%27').replace(/\(/g, '%28').
                                                                    replace(/\)/g, '%29').replace(/\*/g, '%2A').replace(/%20/g, '+');
}

function onFailure(response, opts) {
	// alert("Failure");
}

function changePercentage(val, metaData, record, rowIndex, colIndex, store) {
	if(val == null || val.length == 0) {
		metaData.style+='text-align: center;';
		return gridUnknownPercentageValue;
	}
	var value = parseInt(5.1*val);
    var colour;
	if(val > 50)
		colour = (510-value) + ", 255";
	else 
		colour = "255, " + value;
	metaData.style+='background-color: rgb(' + colour + ', 0); text-align: center;';
	return '<span>' + val + '%</span>';	
}

function changeComplementaryPercentage(val, metaData, record, rowIndex, colIndex, store) {
	if(val == null || val.length == 0) {
		metaData.style+='text-align: center;';
		return gridUnknownPercentageValue;
	}
	var cVal = 100. - val;
	var value = parseInt(5.1*cVal);
    var colour;
	if(cVal > 50)
		colour = (510-value) + ", 255";
	else 
		colour = "255, " + value;
	metaData.style+='background-color: rgb(' + colour + ', 0); text-align: center;';
	return '<span>' + val + '%</span>';	
}

function sortDate(value) {
	var format = Ext.form.DateField.prototype.format.replace('y', 'Y') + " H:i";
	return Date.parseDate(value, format).format("U");
}

var helpWindow = null;
function openHelpWindow(element, text) {
	if(helpWindow != null) helpWindow.close();
	helpWindow = new Ext.Window({
		title: 'Help',
        width: 400,
        height: 250,
        closeAction: 'close',
        layout: 'fit',
        items: [
            new Ext.Panel({
            	frame: true,
            	autoScroll: true,
            	layout: 'fit',
            	html: String.format('<p>{0}</p>', text)
            })
        ],
        buttons: [{
            text: 'Close',
            handler: function(){
        		helpWindow.close();
            }
        }]
	}).show(element);
}
