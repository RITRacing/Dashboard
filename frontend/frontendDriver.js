var fs = require("fs");
var net = require("net");
var contents = fs.readFileSync("/home/dash/f26dash/frontend/settings.json");
var settings = JSON.parse(contents);

//set some variables using the information in settings.json
var width = settings.display_width;
var height = settings.display_height;
var backgroundColor = settings.background;
var coolColor = settings.cool;
var nominalColor = settings.nominal;
var intermediateColor = settings.intermediate;
var maximalColor = settings.maximal;
var carType = settings.car_type;

var logoURL = "/home/dash/f26dash/frontend/resources/" + carType + "car.png";

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
logo.setWidth(244);
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
var dashValues = [];
var oilt = new DashValue("oilT", "℃", 20, 160);
dashValues[dashValues.length] = oilt;
var oilp = new DashValue("oilP", "bar", 0, 1.6);
dashValues[dashValues.length] = oilp;
var watert = new DashValue("waterT", "℃", 20, 140);
dashValues[dashValues.length] = watert;
var volt = new DashValue("volt", "V", 10.5, 14.5);
dashValues[dashValues.length] = volt;
var gear = new DashValue("GEAR", "", 0, 12);
gear.update("N");
dashValues[dashValues.length] = gear;
var rpm = new DashValue("RPM", "", 0, 11500);
dashValues[dashValues.length] = rpm;
var soc = new DashValue("SOC", "%", 0, 100);
dashValues[dashValues.length] = soc;
var lambdactl = new DashValue("LAMBDA CTL", "", 0, 1);
dashValues[dashValues.length] = lambdactl;
var flc = new DashValue("FLC", "", .5, 1.5);
dashValues[dashValues.length] = flc;
var lfaulttext = new DashValue("", "", 0, 1); // the text for the fault
lfaulttext.update("");
dashValues[dashValues.length] = lfaulttext;
var lfault = new DashValue("", "", 0, 1); // the boolean fault indicator
dashValues[dashValues.length] = lfault;
var current = new DashValue("Current", "A", -32000, 32000);
dashValues[dashValues.length] = current;
var autoup = new DashValue("Auto-Up", "", 0, 1);
dashValues[dashValues.length] = autoup;
var hold = new DashValue("Hold", "", 0, 1);
dashValues[dashValues.length] = hold;
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
    if("AUTOUP" in data){
        if(data["AUTOUP"])
            autoup.update(1);
        else
            autoup.update(0);
    }

    if("lctl" in data){
        console.log("LAMBDACTL in data!");
        if(data["lctl"])
            lambdactl.update(1);
        else {
            lambdactl.update(0);
        }
    }
    if("FLC" in data){
        console.log("FLC in data!");
        flc.update(data["FLC"]);
    }

    if(watert.value < 50 || lambdactl.value == 0){
        hold.update(1);
    }else{
        hold.update(0);
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
var client = new net.Socket();
/*
var datasocket = new WebSocket("ws:127.0.0.1:8787");
datasocket.onopen = function(event){
    datasocket.send("Dashboard Frontend");
};
var on = true;
datasocket.onmessage = function(event){
    var data = JSON.parse(event.data);
    updateData(data);
};
*/
client.connect(8787, "127.0.0.1", function(){
	client.write("hello from server");
});

client.on('data', function(evt){
	var jdata = JSON.parse(evt);
	updateData(jdata);
});
