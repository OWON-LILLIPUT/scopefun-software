import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import scopefunapi

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

raw_input("Press Enter to connect ...")
print "sfClientConnect"
ret = scopefunapi.sfClientConnect(ctx,'127.0.0.1',42250)
frame   = scopefunapi.sfCreateSFrameData(ctx,szFrame)
display = scopefunapi.sfCreateSDisplay()

# histogram ploter setup
x = np.arange(10000)
fig, ax = plt.subplots()
y1 = 500 * np.sin(2 * np.pi * 500 * x / 10000)
n, bins, patches = plt.hist(y1, 50, facecolor='green', alpha=0.75)

def complement(val):
    if (val & (1 << 9)) != 0: # sign bit
        val = val - (1 << 10) # negative value
    return val 
	
# callback
def animate(frameno):
    	
    # capture
    print "sfHardwareCaptureFrame - header(1024)"
    ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,1024,1)
    print "sfHardwareCaptureFrame - data(multiple of 1024)"
    ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,40960,2)
    if transfered == 40960:
        # input
        for i in range(10000):
            mask0 = 1020
            mask1 = 3
            mask2 = 511
            data0 = frame.data[(i*4)]
            data1 = frame.data[(i*4)+1]
            hi  = data0 << 2
            hi  = hi & mask0
            lo  = data1 >> 6
            lo  = lo & mask1
            value = hi | lo
            y1[i] = complement(value)
        
        # draw histogram
        n, _ = np.histogram(y1, bins)
        for rect, h in zip(patches, n):
            rect.set_height(h)
                
    return patches

# animate
ani = animation.FuncAnimation(fig, animate, blit=False, interval=60,
                              repeat=False)
plt.show()