all: dash_bin 

package.nw : index.html frontendDriver.js displays.js visuals.js package.json dep/jsgl.min.js resources/ecar.png resources/ccar.png
	zip package.nw $^

dash_bin : package.nw
	cat dep/nwjs/nw package.nw > dash_bin;
	chmod 755 dash_bin

clean:
	rm package.nw
	
