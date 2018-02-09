//9000-11000
//flash and red at 10500
//flash whole screen black and white at 11000

//lambda control boolean
//flc number 1.500-.500

//waterT yellow=120 red=130 flash=135
//oilP <.2 yellow <.1 red 0flash
//low voltage indicator flash battery < 12
//hold when waterT < 50 and lambda control is false

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
            if(oilt.visual != null){
                oilt.visual.destroy();
                oilt.visual = null;
            }
            if(oilp.visual != null){
                oilp.visual.destroy();
                oilp.visual = null;
            }
            if(watert.visual != null){
                watert.visual.destroy();
                watert.visual = null;
            }
            if(volt.visual != null){
                volt.visual.destroy();
                volt.visual = null;
            }
            if(gear.visual != null){
                gear.visual.destroy();
                gear.visual = null;
            }
            if(rpm.visual != null){
                rpm.visual.destroy();
                rpm.visual = null;
            }
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
    oilt.setVisual(new StatLabel(width/8,height/4,50,oilt.name, oilt.unit));
    oilp.setVisual(new StatLabel(width/8,height/2,50,oilp.name,oilp.unit));
    watert.setVisual(new StatLabel(7*width/8,height/4,50,watert.name, watert.unit));
    volt.setVisual(new StatLabel(7*width/8,height/2,50,volt.name,volt.unit));
    rpm.setVisual(new RPMMeter(rpm.min,rpm.max,nominalColor,
    intermediateColor,maximalColor));
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
    oilt.setVisual(new Indicator("CEL",width - width/4,height/2,height/8,
    oilt.min, oilt.max));
    gear.setVisual(new GearLabel(width/4, height/2));
    rpm.setVisual(new IncrementalRPMMeter(9000,11500, 500, 10500, 10500, true));
}

/**
Shows all the time on E car, stuff like SOC, IRD, etc...
**/
var EDisplay = function(){
    Display.apply(this);
}

EDisplay.prototype = Object.create(Display.prototype);
EDisplay.prototype.constructor = EDisplay;

EDisplay.prototype.show = function(){
    soc.setVisual(new StatLabel(width/2,height/6,200,soc.name,soc.unit));
}
