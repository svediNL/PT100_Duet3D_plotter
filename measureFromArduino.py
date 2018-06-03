import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
#from matplotlib import style
import pandas as pd

SAMPLE_FREQ = 50
SAMPLE_TIME = 0.02
SAMPLE_TIME_MS = 20

ERROR_TEMP_MIN = 0
ERROR_TEMP_MAX = 60


ser= serial.Serial("/dev/tty.usbserial-A904OSQG", 9600)
ser.close()
ser.open()

xs = []
ys = []
buff = []

#style.use('fivethirtyeight')
fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)
i=0

def getSample():
	datain =  ser.readline() 
	if datain.count("\r") <= 1 and datain.count("\n") <= 1:
		datain = datain.replace("\r", "")
		datain = datain.replace("\n", "")
		mappedSensorValue =  (float(int(datain))/32)-256

		print " > raw input: ", datain
		print " > mapped input: ", mappedSensorValue, " [CELSIUS]"

	else: 
		print " > DOUBLE/INCOMPLETE MESSAGE"
		print "  > r: ", datain.count("\r")
		print "  > n: ", datain.count("\n")
		return 0

	if mappedSensorValue > ERROR_TEMP_MIN and mappedSensorValue < ERROR_TEMP_MAX:
		return mappedSensorValue
	else:
		print " > FAULTY VALUE"
		print "  > datain: ", datain
		print "  > mappedSensorValue: ", mappedSensorValue
		print "  > r: ", datain.count("\r")
		print "  > n: ", datain.count("\n")
		return 0

def animate():
	global i, buff
	val = getSample()
	xval = float(i*SAMPLE_TIME)
	xs.append(xval)
	ys.append(val)
	buff.append(val)

	ax1.clear()
	ax1.plot(xs, ys)
	ax1.grid(True)
	plt.pause(0.001)

	i+= 1

mBool = True

# ani = animation.FuncAnimation(fig, animate, frames = 1000)
# if raw_input(" Make sure the right COM port is selected in line 16, \n \r")
nSamples = int(raw_input("> How many samples to take? (@50Hz) : "))

print "> reset the arduino to start measurement, this might take multiple tries... "

while mBool:

	msg = ser.readline()
	if msg == "GO\r\n":
		print "> start measurement"
		for n in range(nSamples):
			print '> sample #', n+1, ': '
			print " > time: ", (n)*SAMPLE_TIME, " [SECONDS]"
			animate()
		mBool = False


print "!_:_!_:_!_:_! END OF MEASUREMENT !_:_!_:_!_:_!"

pandaData = pd.DataFrame(buff)
pandaData.to_csv("testarray.csv")
plt.show()