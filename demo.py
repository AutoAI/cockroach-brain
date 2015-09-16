import time
import serial

s = serial.Serial("/dev/ttyACM0", 9600)

print("started")
time.sleep(5)

s.write("ff".decode('hex')+'\r\n')
print("lock brake");
time.sleep(2)
s.write("b0".decode('hex')+'\r\n')
time.sleep(2)
s.write("aa".decode('hex')+'\r\n')
print("gas on");
time.sleep(1)
s.write("c0".decode('hex')+'\r\n')
print("unlock brake")
time.sleep(2)
s.write("40".decode('hex')+'\r\n')
print("wiggle")
time.sleep(2)
s.write("7f".decode('hex')+'\r\n')
print("wiggle")
time.sleep(2)
s.write("bf".decode('hex')+'\r\n')
print("gas off")
time.sleep(1)
s.write("ff".decode('hex')+'\r\n')
print("stop")
time.sleep(2)