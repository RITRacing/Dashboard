
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
    rpm.setVisual(new IncrementalRPMMeter(9000,11500, 500, 10500, 10500));
}
