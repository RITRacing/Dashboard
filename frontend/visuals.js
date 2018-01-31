/**
    Contains a value from the ECU, maintains a visual.
**/
function DashValue(name, unit, min, max){
    this.name = name;
    this.unit = unit;
    this.min = min;
    this.max = max;
    this.value = this.min;
    this.visual = null;

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

    this.rectangle = panel.createRectangle();
    this.rectangle.setWidth(35);
    this.rectangle.setHorizontalAnchor(jsgl.HorizontalAnchor.CENTER);
    this.rectangle.setLocationXY(x, height - barPadding - maxBarHeight);
    this.rectangle.getFill().setColor(coolColor);

    this.value = getDashLabel(this.unit, x, height - 50, 20, textColor);

    panel.addElement(this.label);
    panel.addElement(this.rectangle);
    panel.addElement(this.value);

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

function RPMMeter(min,max,low,mid,high){
    this.min = min;
    this.max = max;
    this.low = low;
    this.mid = mid;
    this.high = high;
    this.rectangle = panel.createRectangle();
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

function incrementalRPMMeter(min,max,incr,redval,blinkval){
    this.min = min;
    this.max = max;
    this.incr = incr;
    this.redval = redval;
    this.blinkval = blinkval;
    var bars = [];
    var barPadding = 20;
    var cumulativeWidth = width - 2 * barPadding;
    var incrWidth = cumulativeWidth/((max-min)/this.incr);

    var barWidth = incrWidth - 2 * barPadding;
    for(var i = 0; i <= (max-min)/this.incr; ++i){
        var r = panel.createRectangle();
        r.setHeight(50);
        r.setWidth(barWidth);
        r.setLocationXY(i * incrWidth + 2 * barPadding, 35);
        bars[bars.length] = r;
        r.getFill().setColor("gray");
        panel.addElement(r);
    }

    this.animator = new jsgl.util.Animator();
    this.animator.setStartValue(0);
    this.animator.setEndValue(1);
    this.animator.setRepeating(true);
    var on = true;
    var numbars = 0;
    this.animator.addStepListener(function(val){
        if(val == 1){
            for(var i = 0; i <= numbars; ++i){
                if(on){
                    bars[i].getFill().setColor('gray');
                }else{
                    bars[i].getFill().setColor('red');
                }
            }
            on = !on;
        }
    });
    this.animator.setDuration(500);

    this.setValue = function(val){
        var range = this.max - this.min;
        var testval = val - this.min;
        numbars = testval / this.incr;
        for(var i = 0; i < bars.length; ++i){

            if(i <= numbars){
                if(val < redval){
                    this.animator.pause();
                    bars[i].getFill().setColor("yellow");
                }else if(val <= blinkval){
                    this.animator.pause();
                    bars[i].getFill().setColor("red");
                }else{
                    bars[i].getFill().setColor("red");
                }
            }else {
                bars[i].getFill().setColor("gray");
            }
        }
        if(val >= blinkval){
            if(!this.animator.isPlaying()){
                this.animator.play();
            }
        }
    }

    this.destroy = function(){
        for(var i = 0; i < bars.length; ++i){
            panel.removeElement(bars[i]);
        }
    }
}

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

function Indicator(name,x,y,radius,min,max){
    this.label = getDashLabel(name, x, y + radius + 50, 20, textColor);
    panel.addElement(this.label);
    this.circle = panel.createCircle();
    this.circle.setRadius(radius);
    this.circle.getStroke().setColor("gray");
    this.circle.getFill().setColor("blue");
    this.circle.setCenterX(x);
    this.circle.setCenterY(y);
    panel.addElement(this.circle);

    this.min = min;
    this.max = max;
    this.setValue = function(val){
        var range = this.max - this.min;
        var testval = val - this.min;
        if(testval/range <= 1/2){
            this.circle.getFill().setColor(coolColor);
        }else if(testval/range <= 3/4){
            this.circle.getFill().setColor(nominalColor);
        }else{
            this.circle.getFill().setColor(maximalColor);
        }
    }

    this.destroy = function(){
        panel.removeElement(this.label);
        panel.removeElement(this.circle);
    }
}
