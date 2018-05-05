import serial
import io
import numpy as np
import matplotlib.pyplot as plt

ser= serial.Serial("/dev/tty.usbmodemFA131", 9600)
sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser), newline = '\r')

ser.close()
ser.open()

ser.write("M105")
print "message send"
datain =  ser.readline() 

print datain
print "END"