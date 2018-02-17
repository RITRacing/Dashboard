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
var carType = settings.car_type;

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
var oilt = new DashValue("oilT", "℃", 20, 160);
var oilp = new DashValue("oilP", "bar", 0, 1.6);
var watert = new DashValue("waterT", "℃", 20, 140);
var volt = new DashValue("volt", "V", 10.5, 14.5);
var gear = new DashValue("GEAR", "", 0, 12);
gear.update("N");
var rpm = new DashValue("RPM", "", 0, 11500);
var soc = new DashValue("SOC", "%", 0, 100);
var lambdactl = new DashValue("LAMDA CTL", "", 0, 1);
var flc = new DashValue("FLC", "", .5, 1.5);
var lfaulttext = new DashValue("", "", 0, 1); // the text for the fault
lfaulttext.update("");
var lfault = new DashValue("", "", 0, 1); // the boolean fault indicator
var current = new DashValue("Current", "A", -32000, 32000);

// updates visuals based on data received
// TODO - write code to receive lambdactl and flc messages
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

        //since the e car doesnt send gear, only the c car will switch here
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
    if("SOC" in  data){
        soc.update(data["SOC"]);
    }
    if("LFAULT" in data){
        lfaulttext.update(data["LFAULT"]);
        if(data["LFAULT"] === ""){
            lfault.update(0);
        }else{
            lfault.update(1);
        }
    }
    if("CURRENT" in data){
        current.update(data["CURRENT"]);
        if(current.value <= 10 && currentDisplay == driveDisplay){
            driveDisplay.hide();
            currentDisplay = parkDisplay;
            parkDisplay.show();
        }else if(current.value > 10 && currentDisplay == parkDisplay){
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
var parkDisplay;
var driveDisplay;
var currentDisplay;
if(carType == 'c'){
    parkDisplay = new MinParkDisplay();
    driveDisplay = new DriveDisplay();
    currentDisplay = parkDisplay;
}else{
    parkDisplay = new EParkDisplay();
    driveDisplay = new EDriveDisplay();
    currentDisplay = parkDisplay;
}

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
