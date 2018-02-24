

/**
    Contains a value from the ECU, maintains a visual.
**/
function DashValue(name, unit, min, max){
    this.name = name; // displayed by certain visuals
    this.unit = unit; // unit displayed by certain visuals
    this.min = min; // minimum value
    this.max = max; // maximum value
    this.value = this.min; // default to minimum
    this.visual = null; // this is where we attach a visual

    //tells this value to update the visual when the value is changed
    this.setVisual = function(visual){
        this.visual = visual;
        this.visual.setValue(this.value);
    }

    //updates the value and if visual is not null, updates visual
    this.update = function(newVal){
       this.value = newVal;
       if(this.visual != null){
           this.visual.setValue(newVal);
       }
    }
}

//visual that shows a changing bar
function Meter(x, name, unit, min, max){
    this.unit = unit;
    this.max = max;
    this.min = min;
    this.label = getDashLabel(name, x, height - 25, 20, textColor);

    // initialize the rectangle that represents the value
    this.rectangle = panel.createRectangle();
    this.rectangle.setWidth(35);
    this.rectangle.setHorizontalAnchor(jsgl.HorizontalAnchor.CENTER);
    this.rectangle.setLocationXY(x, height - barPadding - maxBarHeight);
    this.rectangle.getFill().setColor(coolColor);

    // set up a label to show the value
    this.value = getDashLabel(this.unit, x, height - 50, 20, textColor);

    panel.addElement(this.label);
    panel.addElement(this.rectangle);
    panel.addElement(this.value);

    // change the rectangle to represent the value
    this.setValue = function(val){
        var range = this.max - this.min;
        var testval = val - this.min;

        this.value.setText(val + " " + this.unit);
        var trueval = maxBarHeight * (testval/range);
        this.rectangle.setHeight(trueval);
        this.rectangle.setY(height - barPadding - trueval);

        if(val > this.max){
            val = this.max;
        }else if(val < this.min){
            val = this.min;
        }

        //now set the color
        if (testval <= range/2){
            this.rectangle.getFill().setColor(coolColor);
        }else if (testval <= range * 3/4){
            this.rectangle.getFill().setColor(nominalColor);
        }else{
            this.rectangle.getFill().setColor(maximalColor);
        }
    }

    this.destroy = function(){
        panel.removeElement(this.label);
        panel.removeElement(this.rectangle);
        panel.removeElement(this.value);
    }
}

/*
Creates a meter at the top of the screen that represents RPM as a horizontal bar
*/
function RPMMeter(min,max,low,mid,high){
    this.min = min;
    this.max = max;
    this.low = low; // color for minimal values
    this.mid = mid; // color for nominal values
    this.high = high; // color for maximal values
    this.rectangle = panel.createRectangle(); // represents the RPM
    this.rectangle.setHeight(25);
    this.rectangle.setWidth(0);

    this.rectangle.setLocationXY(rpmPadding,35);
    this.rectangle.getFill().setColor(coolColor);
    panel.addElement(this.rectangle);

    //need this to keep track of lines for destruction
    this.lines = [];

    //set up the values
    for(i = 0; i < (this.max - this.min)/1000; ++i){
        var line = panel.createRectangle();
        var x = rpmPadding +(width - 2 * rpmPadding) * (i / ((this.max - this.min)/1000));
        line.setWidth(5);
        line.setHeight(25);
        line.setHorizontalAnchor(jsgl.HorizontalAnchor.CENTER);
        line.setLocationXY(x, 35);
        line.getFill().setColor(textColor);

        var val = getDashLabel("" +((this.min | 0)/1000+ i), x, 80, 20, textColor);
        panel.addElement(val);
        panel.addElement(line);
        this.lines.push(line);
        this.lines.push(val);
    }

    // change the size and color of rectangle based on the RPM
    this.setValue = function(val){
        var range = max - min;
        var trueval = val - min;
        var maxLen = width - rpmPadding * 2;
        var testval = trueval / range;
        if (val > this.max){
            val = this.max;
        }else if(val < this.min){
            val = this.min;
        }

        this.rectangle.setWidth(maxLen * testval);
        if (testval <= .50){
            this.rectangle.getFill().setColor(this.low);
        }else if (testval <= .75){
            this.rectangle.getFill().setColor(this.mid);
        }else{
            this.rectangle.getFill().setColor(this.high);
        }
    }

    this.destroy = function(){
        panel.removeElement(this.rectangle);
        for (var i = this.lines.length - 1; i >= 0; --i){
            panel.removeElement(this.lines[i]);
        }
    }
}

/*
Like RPMMeter except broken up into increments.
Only shows yellow and red. Blinks at specified value.
*/
function IncrementalRPMMeter(min,max,incr,redval,blinkval,backgroundBlink){
    this.min = min;
    this.max = max;
    this.incr = incr; // range of RPM an increment represents
    this.redval = redval; // value color switches from yellow to red at
    this.blinkval = blinkval; // value meter starts blinking at
    var bars = []; // holds the bars that represent the increments
    var barPadding = 20; // 1/2 the space in between 2 bars
    var blinkTime = 200; // time between on and off states of bars
    var cumulativeWidth = width - 2 * barPadding; // width of whole meter
    var incrWidth = cumulativeWidth/((max-min)/this.incr); // width of increment

    var barWidth = incrWidth - 2 * barPadding; // width of bar
    // set up the bars
    for(var i = 0; i <= (max-min)/this.incr; ++i){
        var r = panel.createRectangle();
        r.setHeight(50);
        r.setWidth(barWidth);
        r.setLocationXY(i * incrWidth + 2 * barPadding, 35);
        bars[bars.length] = r;
        r.getFill().setColor("gray");
        panel.addElement(r);
    }

    // this code sets up the animation required to make the bars blink
    this.animator = new jsgl.util.Animator();
    this.animator.setStartValue(0);
    this.animator.setEndValue(1);
    this.animator.setRepeating(true);
    var on = true;
    var backon = false; // whether or not the background should blink
    var numbars = 0;
    this.animator.addStepListener(function(val){
        if(val == 1){
            for(var i = 0; i <= numbars; ++i){
                if(on){
                    bars[i].getFill().setColor('gray');
                    if(backon) background.getFill().setColor(backgroundColor);
                }else{
                    if(backon)background.getFill().setColor('orange');
                    bars[i].getFill().setColor('red');
                }
            }
            on = !on;
        }
    });
    this.animator.setDuration(blinkTime);

    // light up correct amount of bars, start or stop blinking
    this.setValue = function(val){
        var range = this.max - this.min;
        var testval = val - this.min;
        numbars = testval / this.incr;

        // set all the bars accordingly
        for(var i = 0; i < bars.length; ++i){

            if(i <= numbars){
                if(val < redval){
                    bars[i].getFill().setColor("yellow");
                }else if(val <= blinkval){
                    bars[i].getFill().setColor("red");
                }else{
                    bars[i].getFill().setColor("red");
                }
            }else {
                bars[i].getFill().setColor("gray");
            }
        }
        // put background color back to normal OR turn on background blinking
        if(val >= 11000 && backgroundBlink){
            backon = true;
        }else{
            backon = false;
            background.getFill().setColor(backgroundColor);
        }

        // play animation if necessary
        if(val >= blinkval){
            if(!this.animator.isPlaying()){
                this.animator.play();
            }
        }else{
            this.animator.pause();
        }
    }

    this.destroy = function(){
        for(var i = 0; i < bars.length; ++i){
            panel.removeElement(bars[i]);
        }
    }
}

/*
Changing label that represents gear position.
*/
function GearLabel(x,y){
    this.label = getDashLabel("N",x,y,175,textColor);
    panel.addElement(this.label);
    this.setValue = function(val){
        this.label.setText(val);
    }
    this.destroy = function(){
        panel.removeElement(this.label);
    }
}

/*
Simple boolean Indicator (is it on or is it off?)
*/
function BooleanIndicator(x,y,size,name,disappears){
    this.title = getDashLabel(name,x,y,size/2,textColor);
    this.value = getDashLabel("TRUE",x,y+size * 4/5,size,textColor);
    if(disappears) this.value.setFontColor(backgroundColor);
    //this.value.setHorizontalAnchor(jsgl.HorizontalAnchor.RIGHT);
    panel.addElement(this.title);
    panel.addElement(this.value);
    this.disappears = disappears
    this.setValue = function(val){
        if(this.disappears){
            if(val)
                this.title.setFontColor(textColor);
            else
                this.title.setFontColor(backgroundColor);
        }else{
            if(val)
                this.value.setText("TRUE");
            else
                this.value.setText("FALSE");
        }
    }

    this.destroy = function(){
        panel.removeElement(this.title);
        panel.removeElement(this.value);
    }
}

/*
Changing label that represents any value.
*/
function StatLabel(x,y,size,name,unit){
    this.title = getDashLabel(name,x,y,size/2,textColor);
    this.value = getDashLabel("wait",x,y+size * 4/5,size,textColor);
    this.value.setHorizontalAnchor(jsgl.HorizontalAnchor.RIGHT);
    this.suffix = getDashLabel(unit,x+size * 2/5,y+size * 4/5,size/2,textColor);

    panel.addElement(this.title);
    panel.addElement(this.value);
    panel.addElement(this.suffix);

    this.setValue = function(val){
        this.value.setText(val);
    }

    this.destroy = function(){
        panel.removeElement(this.title);
        panel.removeElement(this.value);
        panel.removeElement(this.suffix);
    }
}

/*
Circle that represents a value using a color.
*/
function Indicator(name,x,y,radius,min,max){
    this.label = getDashLabel(name, x, y + radius + 50, 20, textColor);
    panel.addElement(this.label);
    var circle = panel.createCircle();
    circle.setRadius(radius);
    circle.getStroke().setColor("gray");
    circle.getFill().setColor("blue");
    circle.setCenterX(x);
    circle.setCenterY(y);
    panel.addElement(circle);

    this.min = min;
    this.max = max;

    var blinkTime = 200;

    this.animator = new jsgl.util.Animator();
    this.animator.setStartValue(0);
    this.animator.setEndValue(1);
    this.animator.setRepeating(true);

    var on = true;
    this.animator.addStepListener(function(val){
        if(val == 1){
            if(on){
                circle.getFill().setColor("gray");
            }else{
                circle.getFill().setColor("red");
            }
            on = !on;
        }
    });
    this.animator.setDuration(blinkTime);

    // set the circle's color representative of the new value
    this.setValue = function(val){
        var range = this.max - this.min;
        var testval = val - this.min;
        if(testval/range <= 1/2){
            if(this.animator.isPlaying())this.animator.pause();
            circle.getFill().setColor(coolColor);
        }else if(testval/range <= 3/4){
            if(this.animator.isPlaying())this.animator.pause();
            circle.getFill().setColor(nominalColor);
        }else{

            if(!this.animator.isPlaying())this.animator.play();
            circle.getFill().setColor(maximalColor);
        }
    }

    this.destroy = function(){
        panel.removeElement(this.label);
        panel.removeElement(circle);
    }
}
