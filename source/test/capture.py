import scopefunapi
import time

szCapture = 16*1024*1024
szFrame   = 16*1024*1024

# setup
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
               
# data structures
frame          = scopefunapi.sfCreateSFrameData(ctx,szFrame)
display        = scopefunapi.sfCreateSDisplay()

#temperature
temperature    = 0

# time
startTime      = time.time()
timeIndex     = 0
timeRange     = 600

#frame count
frameCount = 0

while 1:

    # capture
    print "sfHardwareCaptureFrame - header(1024)"
    ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,1024,1)
    print "sfHardwareCaptureFrame - data(multiple of 1024)"
    ret,transfered = scopefunapi.sfHardwareCapture(ctx,frame,40960,2)
    if transfered == 40960:
                                        
        # increment frameCount
        print "frameCount"
        frameCount = frameCount + 1
        if frameCount > 9999:
            frameCount = 0                      
                
        # fill channel1 with raising temperature based on time
        print "temerature"
        captureTime = time.time() - startTime
        temperature = 200*( (time.time() - startTime) % timeRange ) / timeRange
        timeStop = 10000*captureTime/timeRange
        for i in range(int(timeIndex),int(timeStop)):
            display.analog1[i] = ( temperature - 150 ) / 300.0
            timeIndex = timeStop
            if timeIndex > 9999:
                timeIndex = 0
                
        # draw to screen
        print "sfClientDisplay"
        display.ch0 = 0
        display.ch1 = 1
        scopefunapi.sfClientDisplay(ctx,0,display)
                
# cleanup
raw_input("Press Enter to disconnect ...")

print "cleanup"
scopefunapi.sfClientDisconnect()
scopefunapi.sfApiExit()
