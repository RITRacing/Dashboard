try{
var fs = require("fs");
var net = require("net");
var child = require("child_process");
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

function restart(){
    child.exec("sudo systemctl restart dash_backend");
    setTimeout(function(){
        child.exec("sudo systemctl restart dash_frontend");
    }, 3000);
}

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
soc.update(999);
dashValues[dashValues.length] = soc;
var lambdactl = new DashValue("LAMBDA CTL", "", 0, 1);
dashValues[dashValues.length] = lambdactl;
var flc = new DashValue("FLC", "", .5, 1.5);
dashValues[dashValues.length] = flc;
var lfaulttext = new DashValue("", "", 0, 1); // the text for the fault
lfaulttext.update("");
dashValues[dashValues.length] = lfaulttext;
var mcstate = new DashValue("VSM", "", 0, 1);
mcstate.update("no data");
dashValues[dashValues.length] = mcstate;
var lfault = new DashValue("", "", 0, 1); // the boolean fault indicator
dashValues[dashValues.length] = lfault;
var current = new DashValue("Current", "A", -32000, 32000);
dashValues[dashValues.length] = current;
var autoup = new DashValue("Auto-Up", "", 0, 1);
dashValues[dashValues.length] = autoup;
var hold = new DashValue("Hold", "", 0, 1);
dashValues[dashValues.length] = hold;
var cel = new DashValue("", "", 0, 1);
dashValues[dashValues.length] = cel;
var gearp = new DashValue("GearP", "bar", 0, 100);
dashValues[dashValues.length] = gearp;
var gearv = new DashValue("GearV", "v", 0, 5);
dashValues[dashValues.length] = gearv;
var speed = new DashValue("Speed", "kph", 0, 10000);
dashValues[dashValues.length] = speed;
var maxtnum = new DashValue("MaxTnum", "", 0, 254);
maxtnum.update(999);
dashValues[dashValues.length] = maxtnum;
var maxt = new DashValue("MaxT", "℃", 0, 1000);
dashValues[dashValues.length] = maxt;
var minvnum = new DashValue("MinVnum", "", 0, 254);
dashValues[dashValues.length] = minvnum;
var minv = new DashValue("MinV", "v", 0, 3.6);
dashValues[dashValues.length] = minv;

// updates visuals based on data received
function setCEL(wt, op){
    if(wt>=135 || op < .05)
        cel.update(1);
    else if(wt>=130 || op < .1)
        cel.update(3/4);
    else if(wt>=120 || op < .2)
        cel.update(1/2);
    else
        cel.update(0);

    var celtext = "";
    if(wt >= 120)
        celtext += "W ";
    if(op < .2)
        celtext += "O";

    // since every variable is public in this language...
    if(cel.visual != null)
    cel.visual.setText(celtext);

}

function updateData(data){
    if("OILT" in data){
        oilt.update(data["OILT"]);
    }
    if("OILP" in data){
        oilp.update(data["OILP"].substring(0,3));
        setCEL(watert.value, oilp.value);
    }
    if("WATERT" in data){
        watert.update(data["WATERT"]);
        setCEL(watert.value, oilp.value);
    }
    if("BATT" in data){
        volt.update(data["BATT"].substring(0,4));
    }
    if("RPM" in data){
        rpm.update(data["RPM"]);
    }
    if("GEAR" in data){

        //since the e car doesnt send gear, only the c car will switch here
        gear.update(data["GEAR"]);
        if(gear.value == 0)
            gear.value = "N";
        if(carType != 't'){
            if(gear.value == "N" && currentDisplay == driveDisplay){
                driveDisplay.hide();
                currentDisplay = parkDisplay;
                parkDisplay.show();
            }else if(gear.value != "N" && (currentDisplay == parkDisplay
                || currentDisplay == secondaryDisplay)){
                parkDisplay.hide();
                currentDisplay = driveDisplay;
                driveDisplay.show();
            }
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
        /*
        if(current.value <= 10 && currentDisplay == driveDisplay){
            driveDisplay.hide();
            currentDisplay = parkDisplay;
            parkDisplay.show();
        }else if(current.value > 10 && currentDisplay == parkDisplay){
            parkDisplay.hide();
            currentDisplay = driveDisplay;
            driveDisplay.show();
        }
        */
    }
    if("AUTOUP" in data){
        if(data["AUTOUP"])
            autoup.update(1);
        else
            autoup.update(0);
    }

    if("LCTL" in data){
        console.log("LAMBDACTL in data!");
        if(data["LCTL"] == "0")
            lambdactl.update(0);
        else {
            lambdactl.update(1);
        }
    }
    if("FLC" in data){
        console.log("FLC in data!");
        flc.update(data["FLC"].substring(0,4));
    }

    if("MCS" in  data){
        mcstate.update(data["MCS"]);
    }

    if ("GEARP" in data){
        gearp.update(data["GEARP"].substring(0,4));
    }

    if ("GEARV" in data){
        gearv.update(data["GEARV"].substring(0,5));
    }

    if("SPEED" in data){
        speed.update(data["SPEED"]);
    }

    if("MAXTNUM" in data){
        maxtnum.update(data["MAXTNUM"]);
    }

    if("MAXT" in data){
        maxt.update(data["MAXT"]);
    }

    if("MINVNUM" in data){
        minvnum.update(data["MINVNUM"]);
    }

    if("MINV" in data){
        minv.update(data["MINV"].substring(0,3));
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
var secondaryDisplay;
if(carType == 'c'){
    parkDisplay = new MinParkDisplay();
    secondaryDisplay = new SecParkDisplay();
    driveDisplay = new DriveDisplay();
    currentDisplay = parkDisplay;
}else if(carType == 'e'){
    parkDisplay = new EParkDisplay();
    driveDisplay = new EDriveDisplay();
    currentDisplay = parkDisplay;
}else if(carType == 't'){
    currentDisplay = new TelemetryDisplay();
}

currentDisplay.show();

function switchDisplay(eventArgs){
    if(carType == 'c'){
        if(currentDisplay == parkDisplay){
            currentDisplay.hide();
            currentDisplay = secondaryDisplay;
            currentDisplay.show();
        }else if(currentDisplay == secondaryDisplay){
            currentDisplay.hide();
            currentDisplay = parkDisplay;
            currentDisplay.show();
        }
    }
}

panel.addClickListener(switchDisplay);

var client = new net.Socket();

client.connect(8787, "127.0.0.1", function(){
	client.write("hello from server");
    console.log("setup socket");
});

client.on("error", function(err){
    restart();
});

var chunk = "";
client.on('data', function(data) {
    try{
        chunk += data.toString(); // Add string on the end of the variable 'chunk'
        var d_index = chunk.indexOf('@'); // Find the delimiter

        // While loop to keep going until no delimiter can be found
        while (d_index > -1) {
            var json = JSON.parse(chunk.substring(0,d_index)); // Parse the current string
            updateData(json); // Function that does something with the current chunk of valid json.

            chunk = chunk.substring(d_index+1); // Cuts off the processed chunk
            d_index = chunk.indexOf('@'); // Find the new delimiter
        }
    }catch(err){
        console.log(err);
        restart();
    }
});
}catch(err){
    console.log(err);
    restart();
}
