/**
 Effectively Abstract: provides a collection of dash visuals and a way to
 attach the information to them
 **/
var Display = function(){
    if(this.constructor === Display){
        throw new Error("Cannot Instantiate abstract class!");
    }else{

        //destroys all active visuals on hide

        this.hide = function(){
        dashValues.forEach(function(element){
            if(element.visual != null){
                element.visual.destroy();
                element.visual = null;
            }
        });
    }
    }
}

//constructs visuals and connects the DashValues to them
//MUST add visuals to visual array
//Effectively abstract: must be defined in sub class
Display.prototype.show = function(){
    throw new Error("Must Override this function!");
}

/**
 Display shown when the car is parked.
 **/
var ParkDisplay = function(){
    Display.apply(this);
}

//need this junk to do inheritance in this strange language
ParkDisplay.prototype = Object.create(Display.prototype);
ParkDisplay.prototype.constructor = ParkDisplay;

ParkDisplay.prototype.show = function(){
    oilt.setVisual(new Meter(width/8, oilt.name, oilt.unit, oilt.min,
      oilt.max));
    oilp.setVisual(new Meter(width/4, oilp.name, oilp.unit, oilp.min,
      oilp.max));
    watert.setVisual(new Meter(3 * width / 4, watert.name, watert.unit,
      watert.min, watert.max));
    volt.setVisual(new Meter(7 * width/8, volt.name, volt.unit, volt.min,
      volt.max));
    rpm.setVisual(new RPMMeter(rpm.min,rpm.max, nominalColor,
      intermediateColor, maximalColor));
    gear.setVisual(new GearLabel(width/2,height/2));
}

/**
Minimalized display shown when the car is parked, can hold more information
*/
var MinParkDisplay = function(){
    Display.apply(this);
}

MinParkDisplay.prototype = Object.create(Display.prototype);
MinParkDisplay.prototype.constructor = MinParkDisplay;

MinParkDisplay.prototype.show = function(){
    oilt.setVisual(new StatLabel(width/7,height/4,50,oilt.name, oilt.unit,textColor));
    oilp.setVisual(new StatLabel(width/7,height/2,50,oilp.name,oilp.unit,textColor));
    watert.setVisual(new StatLabel(6*width/7,height/4,50,watert.name, watert.unit,textColor));
    volt.setVisual(new StatLabel(6*width/7,height/2,50,volt.name,volt.unit,textColor));
    rpm.setVisual(new RPMMeter(rpm.min,rpm.max,nominalColor,
    intermediateColor,maximalColor));
    lambdactl.setVisual(new BooleanIndicator(width/2, height/4,50, lambdactl.name, false, textColor));
    flc.setVisual(new StatLabel(width/2, height/2,50, flc.name, flc.unit,textColor));
    autoup.setVisual(new BooleanIndicator(width/7+20, 3*height/4 + 50, 115, autoup.name, true, "red"));
    hold.setVisual(new BooleanIndicator(6*width/7-20,3*height/4 + 50,150,hold.name,true,"yellow"));
}

/**
 Display shown when the car is in motion
 **/
var DriveDisplay = function(){
    Display.apply(this);
}

DriveDisplay.prototype = Object.create(Display.prototype);
DriveDisplay.prototype.constructor = DriveDisplay;

DriveDisplay.prototype.show = function(){
    cel.setVisual(new Indicator(cel.name,width - width/4,height/2,height/8,
    cel.min, cel.max));
    gear.setVisual(new GearLabel(width/4, height/2));
    rpm.setVisual(new IncrementalRPMMeter(9000,11500, 500, 10500, 10500, true));
    autoup.setVisual(new BooleanIndicator(width/2, height/2, 100, autoup.name, true, "red"));
    volt.setVisual(new BatteryIndicator(width/2, height/2 - 100, 12, "yellow"));
}

/**
Shows all the time on E car, stuff like SOC, IRD (current), etc...
**/
var EParkDisplay = function(){
    Display.apply(this);
}

EParkDisplay.prototype = Object.create(Display.prototype);
EParkDisplay.prototype.constructor = EParkDisplay;

EParkDisplay.prototype.show = function(){
    soc.setVisual(new StatLabel(3*width/16 + 25,height/4,50,soc.name,soc.unit,textColor));
    current.setVisual(new StatLabel(width/2, height/4,50,current.name, current.unit,textColor));
    lfaulttext.setVisual(new StatLabel(width - 10, 10, 35,
        lfaulttext.name, lfaulttext.unit,textColor));
    lfault.setVisual(new Indicator("", 3 * width/4, height/4, height/8,
    lfault.min, lfault.max));
    mcstate.setVisual(new StatLabel(width-20, 3*height/4-70, 30, mcstate.name, mcstate.unit, textColor));
    maxtnum.setVisual(new StatLabel(width/4 + 50, height/2, 50, "#", maxtnum.unit, textColor));
    maxt.setVisual(new StatLabel(width/8, height/2, 50, maxt.name, maxt.unit, textColor));
    minvnum.setVisual(new StatLabel(width/4 + 50, 3*height/4, 50, "#", minvnum.unit, textColor));
    minv.setVisual(new StatLabel(width/8, 3*height/4, 50, minv.name, minv.unit, textColor));
}

/**
Omits the lfault text indicator for when driving
**/
var EDriveDisplay = function(){
    Display.apply(this);
}

EDriveDisplay.prototype = Object.create(Display.prototype);
EDriveDisplay.prototype.constructor = EDriveDisplay;

EDriveDisplay.prototype.show = function(){
    soc.setVisual(new StatLabel(width/4,height/4,70,soc.name,soc.unit,textColor));
    current.setVisual(new StatLabel(width/4, 3 * height/4 - 70,70,current.name, current.unit,textColor));
    lfault.setVisual(new Indicator("", 3 * width/4, height/4, height/8,
    lfault.min, lfault.max));
    mcstate.setVisual(new StatLabel(width-20, 3*height/4-70, 30, mcstate.name, mcstate.unit, textColor));
}

/**
* Display the frontend switches to when tapped
* Contains gear pressure and gear voltage
**/
var SecParkDisplay = function(){
    Display.apply(this);
}

SecParkDisplay.prototype = Object.create(Display.prototype);
SecParkDisplay.prototype.constructor = SecParkDisplay;

SecParkDisplay.prototype.show = function(){
    gearp.setVisual(new StatLabel(width/2,height/4,50,gearp.name, gearp.unit,textColor));
    gearv.setVisual(new StatLabel(width/2,height/2,50,gearv.name,gearv.unit,textColor));
}
