import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
#from matplotlib import style
import pandas as pd

SAMPLE_FREQ = 50
SAMPLE_TIME = 0.02
SAMPLE_TIME_MS = 20

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
	datain = datain.replace("\r", "")
	datain = datain.replace("\n", "")
	
	mappedSensorValue =  (float(int(datain))/32)-256
	return mappedSensorValue
	#print datain
	#print mappedSensorValue

def animate():
	global i, buff

	val = getSample()
	xval = float(i*SAMPLE_TIME)

	print (val)

	xs.append(xval)
	ys.append(val)
	buff.append(val)

	#if mappedSensorValue < 150 and mappedSensorValue > -30:
	ax1.clear()
	ax1.plot(xs, ys)
	plt.pause(0.001)
	
	# else : 
	# 	mappedSensorValue = float('nan')
	# 	print "incorrect measurement", datain

	i+= 1

mBool = True

# ani = animation.FuncAnimation(fig, animate, frames = 1000)

nSamples = int(raw_input("> How many samples to take? (@50Hz) : "))

print "> reset the arduino to start measurement,"
print "  this might take multiplt tries"

while mBool:

	msg = ser.readline()
	if msg == "GO\r\n":
		print "start measurement"
		for n in range(nSamples):
			animate()
		mBool = False


print "!_:_!_:_!_:_! END OF MEASUREMENT !_:_!_:_!_:_!"

pandaData = pd.DataFrame(buff)
pandaData.to_csv("testarray.csv")
plt.show()