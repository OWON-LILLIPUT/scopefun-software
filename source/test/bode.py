import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import scopefunapi
import math
import time
from statistics import mean
from scipy.signal import correlate
from scipy.signal import medfilt

ch1 = []
ch2 = []
VdivCh1 = 1
VdivCh2 = 1
freq_arr = []
V1_arr =[]
V2_arr =[]
V1_dB_arr = []
V2_dB_arr = []
Bode_A_arr = []
Bode_A_n_arr = []
Bode_P_arr = []
x = []

# scopefun setup
szCapture = 16*1024*1024
szFrame   = 16*1024*1024

print "sfApiInit"
ret = scopefunapi.sfApiInit()
print "sfCreateSFContext"
ctx = scopefunapi.sfCreateSFContext()
print "sfApiCreateContext"
ret = scopefunapi.sfApiCreateContext(ctx,szCapture)
print "sfSetActive"
ret = scopefunapi.sfSetActive(ctx,1)
print "sfSetThreadSafe"
ret = scopefunapi.sfSetThreadSafe(ctx,1)
print "sfSetTimeOut"
ret = scopefunapi.sfSetTimeOut(ctx,1000000)
print "sfSetNetwork"
ret = scopefunapi.sfSetNetwork(ctx)
print "sfSetframe*"
ret = scopefunapi.sfSetFrameVersion(ctx,2)
ret = scopefunapi.sfSetFrameHeader(ctx,1024)
ret = scopefunapi.sfSetFrameData(ctx,40000)
ret = scopefunapi.sfSetFramePacket(ctx,(1024*1024))

#raw_input("Press Enter to connect ...")
print "sfClientConnect"
ret = scopefunapi.sfClientConnect(ctx,'127.0.0.1',42250)
frame = scopefunapi.sfCreateSFrameData(ctx,szFrame)

def complement(val):
	if (val & (1 << 9)) != 0: # sign bit
		val = val - (1 << 10) # negative value
	return val 

def sethw(freq,xrange,VdivCh1,VdivCh2):
	#setup horizontal scale
	if   VdivCh1 == 1 :
		gvCh1=1430
		k1=0
	elif VdivCh1 == 2 :
		gvCh1=1788
		k1=0
	elif VdivCh1 == 3 :
		gvCh1=2138
		k1=0
	elif VdivCh1 == 4 :
		gvCh1=2604
		k1=0
	elif VdivCh1 == 5 :
		gvCh1=1426
		k1=1
	elif VdivCh1 == 6 :
		gvCh1=1784
		k1=1
	elif VdivCh1 == 7 :
		gvCh1=2252
		k1=1
	else:
		gvCh1=2634
		k1=1
	if   VdivCh2 == 1 :
		gvCh2=1424
		k2=0
	elif VdivCh2 == 2 :
		gvCh2=1772
		k2=0
	elif VdivCh2 == 3 :
		gvCh2=2128
		k2=0
	elif VdivCh2 == 4 :
		gvCh2=2590
		k2=0
	elif VdivCh2 == 5 :
		gvCh2=1420
		k2=1
	elif VdivCh2 == 6 :
		gvCh2=1770
		k2=1
	elif VdivCh2 == 7 :
		gvCh2=2242
		k2=1
	else:
		gvCh2=2618
		k2=1

	analogsw = 0b00110000 #DC coupling on both channels
	k1 = k1 << 1
	att = k1 | k2         #set ch1 and ch2 attenuation bits
	analogsw = analogsw | att
	
	#setup generator frequency
	fs = 200000000.0
	delta = 65536.0 * freq/fs
	gDelta = delta * 131071.0
	gDeltamaskH = long(4294901760)
	gDeltamaskL = 65535
	genDeltaH = (long(gDelta) & long(gDeltamaskH)) >> 16
	genDeltaL = int(gDelta) & int(gDeltamaskL)
	#create SHardware2 object which contains configration data
	hardware2config = scopefunapi.sfCreateSHardware2()
	hardware2config.controlAddr           = 13
	hardware2config.controlData           = 0
	hardware2config.vgaina                = gvCh1
	hardware2config.vgainb                = gvCh2
	hardware2config.offseta               = 65501 
	hardware2config.offsetb               = 65501 
	hardware2config.analogswitch          = analogsw
	hardware2config.triggerMode           = 0
	hardware2config.triggerSource         = 0
	hardware2config.triggerSlope          = 0
	hardware2config.triggerLevel          = 0
	hardware2config.triggerHis            = 3
	hardware2config.triggerPercent        = 0
	hardware2config.xRange                = xrange # sampling speed (0=4n, 1=8n, 2=20n, ...)
	hardware2config.holdoffH              = 0
	hardware2config.holdoffL              = 0
	hardware2config.sampleSizeH           = 0
	hardware2config.sampleSizeL           = 10000
	hardware2config.generatorType0        = 257
	hardware2config.generatorVoltage0     = 1750 # AWG voltage (mV)
	hardware2config.generatorOffset0      = 2
	hardware2config.generatorDeltaH0      = genDeltaH
	hardware2config.generatorDeltaL0      = genDeltaL
	hardware2config.generatorSqueareDuty0 = 0
	hardware2config.generatorType1        = 129
	hardware2config.generatorVoltage1     = 1024
	hardware2config.generatorOffset1      = 2
	hardware2config.generatorDeltaH1      = 0
	hardware2config.generatorDeltaL1      = 42949
	hardware2config.generatorSqueareDuty1 = 0
	hardware2config.average               = 768 #enable averaging on both channels
	ret = scopefunapi.sfHardwareConfig2(ctx,hardware2config) ###
	return 0 
	

#setup hardware for first frame
sethw(1,14,VdivCh1,VdivCh2)
time.sleep(1)

m=10 #number of ticks per decade
k=4  #number of sub-ticks
for n in range(k,         m*k,         1)       + \
		range(m*k,        m*k*10,      10)    + \
		range(m*k*10,     m*k*100,     100)   + \
		range(m*k*100,    m*k*1000,    1000)  + \
		range(m*k*1000,   m*k*10000,   10000) + \
		range(m*k*10000,  m*k*100000,  100000) + \
		range(m*k*100000, (m+1)*k*1000000, 1000000) :
	
	#set desired freq. sweep range
	fmin = 1
	fmax = 10000000
	
	freq=1.0*n/k
	if (freq < fmin) or (freq > fmax):
		continue
	
	#create a list indexes for pyplot x-axis
	x.append(freq)
	
	#set oscilloscope horizontal scale
	#and freq. resolution
	if n/k <= 2:
		xrange = 14
		timebase = 0.0002
	elif (n/k > 2) and (n/k <= 5):
		xrange = 13
		timebase = 0.00008
	elif (n/k > 5) and (n/k <= 10):
		xrange = 12
		timebase = 0.00004
	elif (n/k > 10) and (n/k <= 100):
		xrange = 11
		timebase = 0.00002
	elif (n/k > 100) and (n/k <= 1000):
		xrange = 10
		timebase = 0.000008
	elif (n/k > 1000) and (n/k <= 10000):
		xrange = 6
		timebase = 0.0000004
	elif (n/k > 10000) and (n/k <= 100000):
		xrange = 2
		timebase = 0.00000002
	elif (n/k > 100000) and (n/k <= 1000000):
		xrange = 1
		timebase = 0.000000008
	else:
		xrange = 0
		timebase = 0.000000004
	
	autoscale = 1
	while (autoscale == 1):   #autoscale Y-axis
	
		#reset variables
		del ch1[:]
		del ch2[:]
		
		#setup Y-scale
		if VdivCh1 == 1:
			Vr1=2.0
		elif VdivCh1 == 2:
			Vr1=1.0
		elif VdivCh1 == 3:
			Vr1=0.5
		elif VdivCh1 == 4:
			Vr1=0.2
		elif VdivCh1 == 5:
			Vr1=0.1
		elif VdivCh1 == 6:
			Vr1=0.05
		elif VdivCh1 == 7:
			Vr1=0.02
		else:
			Vr1=0.01
		
		if VdivCh2 == 1:
			Vr2=2.0
		elif VdivCh2 == 2:
			Vr2=1.0
		elif VdivCh2 == 3:
			Vr2=0.5
		elif VdivCh2 == 4:
			Vr2=0.2
		elif VdivCh2 == 5:
			Vr2=0.1
		elif VdivCh2 == 6:
			Vr2=0.05
		elif VdivCh2 == 7:
			Vr2=0.02
		else:
			Vr2=0.01
			
		#set AWG frequency, oscilloscope timebase and vertical scale
		sethw(freq,xrange,VdivCh1,VdivCh2)
		
		print("***")
		if n/k <= 80:
			time.sleep(0.4)
			#print "sfHardwareCaptureFrame - header(1024)"
			ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,1024,1)
			#print "sfHardwareCaptureFrame - data(multiple of 1024)"
			ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,40960,2)
		else:
			for n in range(4):
				time.sleep(0.18)
				#print "sfHardwareCaptureFrame - header(1024)"
				ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,1024,1)
				#print "sfHardwareCaptureFrame - data(multiple of 1024)"
				ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,40960,2)
		
		if transfered == 40960:
			# input
			for i in range(1,10000):
				mask0 = 0b1111111100
				mask1 = 0b0000000011
				mask2 = 0b00111111
				mask3 = 0b1111110000
				mask4 = 0b0000001111
				data0 = frame.data[(i*4)]
				data1 = frame.data[(i*4)+1]
				data2 = frame.data[(i*4)+2]
				hi1  = data0 << 2
				hi1  = hi1 & mask0
				lo1  = data1 >> 6
				lo1  = lo1 & mask1
				hi2  = data1 & mask2
				hi2  = hi2 << 4
				hi2  = hi2 & mask3
				lo2  = data2 >> 4
				lo2  = lo2 & mask4
				value1 = complement(hi1 | lo1)
				ch1.append(value1)
				value2 = complement(hi2 | lo2)
				ch2.append(value2)
			
			print("Freq: {0} Hz".format(freq))
			
			#CH1
			avg1=mean(ch1)
			# normalize samples by subtracting mean
			ch1[:] = [sample - avg1 for sample in ch1]
			ch1_f = medfilt(ch1,19) #filter noise
			Vmin1=min(ch1_f)
			Vmax1=max(ch1_f)
			Vpp1=abs(Vmax1-Vmin1)
			print (Vpp1)
			#autorange Y-scale
			if Vpp1 < 180 and VdivCh1 != 8:
				#increase vertical scale
				print("Vpp1(in) : {:.6f}".format((Vpp1/2)*Vr1*10/1024))
				print("Autoscaling CH1 Up...")
				VdivCh1 = VdivCh1 + 1
				autoscale = 1
				continue
			elif Vpp1 > 800 and VdivCh1 != 1:
				#decrease vertical scale
				print("Vpp1(out) : {:.6f}".format((Vpp1/2)*Vr2*10/1024))
				print("Autoscaling CH1 Down...")
				VdivCh1 = VdivCh1 - 1
				autoscale = 1
				continue
			else:
				autoscale = 0
			V1=(Vpp1/2)*(Vr1*10/1024)
			print("V1(in) : {:.6f}".format(V1))
			#CH2
			avg2=mean(ch2)
			ch2[:] = [sample - avg2 for sample in ch2]
			ch2_f = medfilt(ch2,19) #filter noise
			Vmin2=min(ch2_f)
			Vmax2=max(ch2_f)
			Vpp2=abs(Vmax2-Vmin2)
			#autorange Y-scale
			if Vpp2 < 180 and VdivCh2 != 8:
				#increase vertical scale
				print("Vpp2(out) : {:.6f}".format((Vpp2/2)*Vr2*10/1024))
				print("Autoscaling CH2 Up...")
				VdivCh2 = VdivCh2 + 1
				autoscale = 1
				continue
			elif Vpp2 > 800 and VdivCh2 != 1:
				#decrease vertical scale
				print("Vpp2(out) : {:.6f}".format((Vpp2/2)*Vr2*10/1024))
				print("Autoscaling CH2 Down...")
				VdivCh2 = VdivCh2 - 1
				autoscale = 1
				continue
			else:
				autoscale = 0
			V2=(Vpp2/2)*(Vr2*10/1024)
			print("V2(out): {:.6f}".format(V2))
			
			#Bode Amplitude
			Bode_A_arr.append(20*math.log10(V2/V1))
			print("V2/V1(dB): {:.3f}".format(20*math.log10(V2/V1)))
			
			V1_arr.append(V1)
			V2_arr.append(V2)
			
			#Bode Phase
			xcorr = correlate(ch1_f, ch2_f)
			dt = np.arange(-9999, 10000)
			#timeshift (in seconds)
			ts = (dt[xcorr.argmax()])*timebase
			print("Timeshift: {:.9f} s".format(ts))
			ps = 360*ts*freq
			#normalize to +/- 180 degrees
			if ps < -180.0:
				ps = ps + 360
			elif ps > 180.0:
				ps = ps - 360
			Bode_P_arr.append(ps)
			print("Phase: {:.3f} deg".format(ps))
			
			#if freq==50:
			#	plt.plot(ch1_f)
			#	plt.plot(ch2_f)
			#	plt.show()
		else:
			print("Transferred != 40960")

#find frequency at 0 phase
idx = int((np.abs(Bode_P_arr)).argmin())
if Bode_P_arr[idx] < 0.5:
	print("\n\rFound Bode plot zero phase: {:.2f} deg at {:.2f} Hz".format(Bode_P_arr[idx],x[idx]))
#Bode Amplitude
#normalize to 0 dB
Bode_A_max = max(Bode_A_arr)
for a in Bode_A_arr:
	Bode_A_n_arr.append(a-Bode_A_max)

#setup pyplot
plt.subplot(2, 1, 1)
plt.title('Bode plot')
plt.xscale('log')
plt.grid(True)
Bode_A_min = min(Bode_A_n_arr)
if Bode_A_min < -60:
	plt.yticks((0, -20, -40, -60, -80),(0, -20, -40, -60, -80))
	ybottom = -80
elif Bode_A_min < -40:
	plt.yticks((0, -20, -40, -60),(0, -20, -40, -60))
	ybottom = -60
elif Bode_A_min < -20:
	plt.yticks((0, -20, -40),(0, -20, -40))
	ybottom = -40
elif Bode_A_min < 0:
	plt.yticks((0, -20),(0, -20))
	ybottom = -20
else:
	ybottom = 0
plt.ylim(bottom=ybottom)  # adjust the bottom
plt.plot(x,Bode_A_n_arr)
plt.ylabel('Amplitude [dB]')
plt.xlabel('Frequency [Hz]')

plt.subplot(2, 1, 2)
plt.xscale('log')
plt.grid(True)
plt.plot(x,Bode_P_arr)
plt.ylabel('Phase [deg]')
plt.xlabel('Frequency [Hz]')
plt.show()

# cleanup
#raw_input("\n\rPress Enter to disconnect ...")
#print "cleanup"
plt.close('all')
scopefunapi.sfClientDisconnect(ctx)
scopefunapi.sfApiExit()