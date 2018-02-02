var fs = require("fs");
var contents = fs.readFileSync("/home/dash/f26dash/frontend/settings.json");
var settings = JSON.parse(contents);

//set some variables using the information in settings.json
var width = settings.display_width;
var height = settings.display_height;
var logoURL = settings.logoURL;
var backgroundColor = settings.background;
var coolColor = settings.cool;
var nominalColor = settings.nominal;
var intermediateColor = settings.intermediate;
var maximalColor = settings.maximal;

//create the panel (acts as a canvas; consolidates all elements)
panel = new jsgl.Panel(document.getElementById("panel"));

//determine some constraints
var maxBarHeight = height - 270
var barPadding = 65; //how high off the bottom of the display the bottom of the bars are
var rpmPadding = width/8 - 35/2;

//add the logo
var logo = panel.createImage();
logo.setUrl(logoURL);
logo.setVerticalAnchor(jsgl.VerticalAnchor.MIDDLE); //the anchors set where (0,0) is on the shape/image/whatever
logo.setHorizontalAnchor(jsgl.HorizontalAnchor.CENTER);
logo.setLocationXY(width/2,height - 90);
logo.setWidth(100);
logo.setHeight(80);
logo.setZIndex(20);
panel.addElement(logo);

//create the background and set the text color to the inverse
var background = panel.createRectangle();
background.setLocationXY(0,0);
background.setHeight(height);
background.setWidth(width);
background.getFill().setColor(backgroundColor);
var textColor = "white";
if(backgroundColor == "white"){
    textColor = "black";
}
panel.addElement(background);

//instantiate DashValues, but don't point them to visuals,
//that is the job of the sub-classes
oilt = new DashValue("oilT", "℃", 20, 160);
oilp = new DashValue("oilP", "bar", 0, 1.6);
watert = new DashValue("waterT", "℃", 20, 140);
volt = new DashValue("volt", "V", 10.5, 14.5);
gear = new DashValue("GEAR", "", 0, 12);
gear.update("N");
rpm = new DashValue("RPM", "", 0, 10500);

function updateData(data){
    if("OILT" in data){
        oilt.update(data["OILT"]);
    }
    if("OILP" in data){
        oilp.update(data["OILP"]);
    }
    if("WATERT" in data){
        watert.update(data["WATERT"]);
    }
    if("BATT" in data){
        volt.update(data["BATT"]);
    }
    if("RPM" in data){
        rpm.update(data["RPM"]);
    }
    if("GEAR" in data){
        gear.update(data["GEAR"]);
        if(gear.value == "N" && currentDisplay == driveDisplay){
            driveDisplay.hide();
            currentDisplay = parkDisplay;
            parkDisplay.show();
        }else if(gear.value != "N" && currentDisplay == parkDisplay){
            parkDisplay.hide();
            currentDisplay = driveDisplay;
            driveDisplay.show();
        }
    }
}

function getDashLabel(name, x, y, size, color){
                var label = panel.createLabel();
                label.setText(name);
                label.setVerticalAnchor(jsgl.VerticalAnchor.MIDDLE);
                label.setHorizontalAnchor(jsgl.HorizontalAnchor.CENTER);
                label.setLocationXY(x,y);
                label.setFontSize(size);
                label.setFontColor(color);
                return label;
}

//var parkDisplay = new ParkDisplay();
var parkDisplay = new MinParkDisplay();
var driveDisplay = new DriveDisplay();
var currentDisplay = parkDisplay;

currentDisplay.show();

var datasocket = new WebSocket("ws:127.0.0.1:8765");
datasocket.onopen = function(event){
    datasocket.send("Dashboard Frontend");
};
var on = true;
datasocket.onmessage = function(event){
    var data = JSON.parse(event.data);
    updateData(data);
};
