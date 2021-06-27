import socket
from BaseHTTPServer import HTTPServer
from SimpleHTTPServer import SimpleHTTPRequestHandler
from os import curdir, sep
import scopefunapi
import time
from statistics import mean
import json
import random

#initialize variables
ch1 = []
ch2 = []
Vr1 = 1
Vr2 = 1
ch1_test = []
ch2_test = []
VdivCh1 = 2
VdivCh2 = 2
samples_dict = {}
# scopefun setup
szCapture = 16*1024*1024
szFrame   = 16*1024*1024

#initialize ScopeFun client
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
	if   VdivCh1 == 1 : # 2 V/div
		gvCh1=1430
		k1=0
	elif VdivCh1 == 2 : # 1 V/div
		gvCh1=1788
		k1=0
	elif VdivCh1 == 3 : # 0.5 V/div
		gvCh1=2138
		k1=0
	elif VdivCh1 == 4 : # 0.2 V/div
		gvCh1=2604
		k1=0
	elif VdivCh1 == 5 : # 0.1 V/div
		gvCh1=1426
		k1=1
	elif VdivCh1 == 6 : # 0.05 V/div
		gvCh1=1784
		k1=1
	elif VdivCh1 == 7 : # 0.02 V/div
		gvCh1=2252
		k1=1
	else:               # 0.01 V/div
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
	hardware2config.generatorType1        = 257
	hardware2config.generatorVoltage1     = 1500
	hardware2config.generatorOffset1      = 2
	hardware2config.generatorDeltaH1      = genDeltaH
	hardware2config.generatorDeltaL1      = genDeltaL
	hardware2config.generatorSqueareDuty1 = 0
	#hardware2config.average               = 768 #enable averaging on both channels
	ret = scopefunapi.sfHardwareConfig2(ctx,hardware2config) ###
	return 0

class MyHandler(SimpleHTTPRequestHandler):
	
	#Handler for the GET requests
	def do_GET(self):
	
		del ch1[:]
		del ch2[:]
		#setup ScopeFun harware and get frame data
		#set generator freq
		freq = 100000
		#set oscilloscope horizontal scale and vertical scale
		xrange = 1  # 8ns
		VdivCh1 = 2 # 1V/div
		VdivCh2 = 2
		
		#set AWG frequency, oscilloscope timebase and vertical scale
		sethw(freq,xrange,VdivCh1,VdivCh2)
			
		print("***")
		#print "sfHardwareCaptureFrame - header(1024)"
		ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,1024,1)
		#print "sfHardwareCaptureFrame - data(multiple of 1024)"
		ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,40960,2)
		
		if transfered == 40960:
			#save current time
			samples_dict['time'] = time.time() #seconds since epoch
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
			Vmin1=min(ch1)
			Vmax1=max(ch1)
			Vpp1=abs(Vmax1-Vmin1)
			V1=(Vpp1/2)*(Vr1*10/1024)
			print("V1: {:.6f}".format(V1))
			#CH2
			avg2=mean(ch2)
			Vmin2=min(ch2)
			Vmax2=max(ch2)
			Vpp2=abs(Vmax2-Vmin2)
			V2=(Vpp2/2)*(Vr2*10/1024)
			print("V2: {:.6f}".format(V2))
			
			samples_dict['ch1'] = ch1
			samples_dict['ch2'] = ch2
			#create a json object containing ch1, ch2 samples and timestamp
			samples_json = json.dumps(samples_dict,sort_keys=True,indent=4).encode('utf-8')
		
		else:
		
			rand_int_arr1 = [random.randint(-2,2) for _ in range(10000)]
			rand_int_arr2 = [random.randint(-2,2) for _ in range(10000)]
			samples_dict['time'] = time.time() #seconds since epoch
			samples_dict['ch1'] = rand_int_arr1
			samples_dict['ch2'] = rand_int_arr2
			samples_json = json.dumps((samples_dict), sort_keys=True)
		
		if self.path=="/":
			self.path="/index.html"

		try:
			#Check the file extension required and
			#set the right mime type
			sendReply = False
			if self.path.endswith(".html"):
				mimetype='text/html'
				sendReply = True
			if self.path.endswith(".jpg"):
				mimetype='image/jpg'
				sendReply = True
			if self.path.endswith(".gif"):
				mimetype='image/gif'
				sendReply = True
			if self.path.endswith(".js"):
				mimetype='application/javascript'
				sendReply = True
			if self.path.endswith(".css"):
				mimetype='text/css'
				sendReply = True
			if self.path.endswith(".txt"):
				mimetype='text/txt'
				sendReply = True
			#send samples in json format
			if self.path.endswith(".json"):
				print("JSON size:", len(samples_json))
				self.send_response(200)
				self.send_header('Access-Control-Allow-Origin', '*')
				self.send_header('Content-type','application/json')
				self.end_headers()
				self.wfile.write(samples_json)
				#self.wfile.write(test_json)
				sendReply = False
				
			#Open the static file (if requested) and send it
			if sendReply == True:
				f = open(curdir + sep + self.path) 
				self.send_response(200)
				self.send_header('Content-type',mimetype)
				self.end_headers()
				self.wfile.write(f.read())
				f.close()
			return

		except IOError:
			print("error")
			self.send_error(404,'Not Found (IOError): %s' % self.path)

#setup IPv6
class HTTPServerV6(HTTPServer):

	address_family = socket.AF_INET6

def main():

	#server = HTTPServerV6(('::', 80), MyHandler)
	server = HTTPServer(('', 80), MyHandler)
	server.serve_forever()
	
if __name__ == '__main__':

	main()