import serial
ser = serial.Serial("/dev/ttyS0", baudrate=9600)

while True:
	
	s = str(ser.read())
	print(s)
	
