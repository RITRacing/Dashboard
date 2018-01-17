all: dash_bin 

package.nw : index.html package.json 
	zip package.nw $^

dash_bin : package.nw
	cat dep/nwjs/nw package.nw > dash_bin;
	chmod 755 dash_bin

clean:
	rm package.nw
	
