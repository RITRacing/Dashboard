import serial
import time
import socket
ser = serial.Serial("/dev/ttyS0", baudrate=9600)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(("127.0.0.1", 8787))
sock.listen(1)
cs, addr = sock.accept()
print("accepted")
while True:
	w = ''
	s = ''
	while s != '@':
		s = str(ser.read())[2]
		w += s;
        
	cs.send(str.encode(w))
	
