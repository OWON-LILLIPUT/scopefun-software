////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 - 2019 David Košenina
//
//    This file is part of ScopeFun Oscilloscope.
//
//    ScopeFun Oscilloscope is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ScopeFun Oscilloscope is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
#include<osciloscope/osciloscope.h>


////////////////////////////////////////////////////////////////////////////////
// defines
////////////////////////////////////////////////////////////////////////////////
// #define MOUSE_DEBUG
#define MAXIMUM_HISTORY_SIZE  (4*GIGABYTE - 1)
#define MINIMUM_HISTORY_COUNT 8

#define CYPRESS_VID 1204
#define CYPRESS_PID 243

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER(Osciloscope);

int SDLCALL RenderThreadFunction(void* data);

////////////////////////////////////////////////////////////////////////////////
// variables
////////////////////////////////////////////////////////////////////////////////

OscFileThread::OscFileThread()
{
    SDL_AtomicSet(&atomic, 0);
    thread = 0;
}

int OscFileThread::isRunning()
{
    return SDL_AtomicGet(&atomic);
}

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeGrid
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeGrid::set(float x, float y)
{
    xCount = x;
    xMax   =  xCount / 2.f;
    xMin   = -xCount / 2.f;
    yCount = y;
    yMax   =  yCount / 2.f;
    yMin   = -yCount / 2.f;
}

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeCamera
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeCamera::OsciloscopeCamera()
{
    zoom  = 1.f;
    theta = 135.f;
    phi   = 45.f;
}

////////////////////////////////////////////////////////////////////////////////
//
// OscContext
//
////////////////////////////////////////////////////////////////////////////////
OscContext::OscContext()
{
    lock = 0;
}

void OscContext::setSimulate(SSimulate* sim)
{
    SDL_AtomicLock(&lock);
    simulate = *sim;
    SDL_AtomicUnlock(&lock);
}
void OscContext::getSimulate(SSimulate* sim)
{
    SDL_AtomicLock(&lock);
    *sim = simulate;
    SDL_AtomicUnlock(&lock);
}
void OscContext::setDisplay(SDisplay* dis)
{
    SDL_AtomicLock(&lock);
    display = *dis;
    SDL_AtomicUnlock(&lock);
}
void OscContext::getDisplay(SDisplay* dis)
{
    SDL_AtomicLock(&lock);
    *dis = display;
    SDL_AtomicUnlock(&lock);
}

////////////////////////////////////////////////////////////////////////////////
//
// ThreadApi
//
////////////////////////////////////////////////////////////////////////////////
ThreadApi::ThreadApi()
{
   lock = 0;
   SDL_AtomicSet(&sync, 0);
   resultClearAll();

   SDL_AtomicSet(&open,0);
   SDL_AtomicSet(&connected,0);
   SDL_AtomicSet(&simulate, 0);
   SDL_AtomicSet(&vid, 0);
   SDL_AtomicSet(&pid, 0);
   SDL_AtomicSet(&sid, 0);
   SDL_AtomicSet(&version, HARDWARE_VERSION_2);
   SDL_AtomicSet(&header, 0);
   SDL_AtomicSet(&data, 0);
   SDL_AtomicSet(&packet, 0);

   SDL_memset(&usbData, 0, sizeof(SUsb));
   usbSize = 0;
   SDL_memset(&fx2Data, 0, sizeof(SFx2));
   fx2Size = 0;
   SDL_memset(&fpgaData, 0, sizeof(SFpga));
   fpgaSize = 0;
   SDL_memset(&eepromData, 0, sizeof(SEeprom));
   eepromSize   = 0;
   eepromOffset = 0;

   simulateTimeValue = 0.0;
   SDL_memset(&simulateData, 0, sizeof(SSimulate));
   SDL_AtomicSet(&simulateOnOff,0);
   SDL_memset(&displayData, 0, sizeof(SDisplay));
   SDL_memset(&config1, 0, sizeof(SHardware1));
   SDL_memset(&config2, 0, sizeof(SHardware2));
}

////////////////////////////////////////////////////////////////////////////////
//
// thread
//
////////////////////////////////////////////////////////////////////////////////
void ThreadApi::function(EThreadApiFunction f)
{
   SDL_AtomicLock(&lock);
      SDL_AtomicAdd(&sync, 1);
      SDL_AtomicSet(&ret[func],0);
      func.pushBack(f);
   SDL_AtomicUnlock(&lock);
 }

void ThreadApi::wait()
{
   while (SDL_AtomicCAS(&sync, 0, 0) == SDL_FALSE)
   {
      SDL_Delay(1);
   }
}

int ThreadApi::result(EThreadApiFunction func)
{
   return SDL_AtomicGet(&ret[func]);
}

void ThreadApi::resultClear(EThreadApiFunction func)
{
   SDL_AtomicSet(&ret[func], 0);
}

void ThreadApi::resultClearAll()
{
   for (int i = 0; i < afLast; i++)
      resultClear((EThreadApiFunction)i);
}

void ThreadApi::update()
{
   // default
   int iconnected = 0;
   int iopened = 0;
   int isimulate = 0;
   int iversion = SDL_AtomicGet(&version);
   isimulate  = sfIsSimulate(getCtx());
   iconnected = sfIsConnected(getCtx());
   sfHardwareIsOpened(getCtx(), &iopened);
   SDL_AtomicSet(&connected, iconnected);
   SDL_AtomicSet(&simulate, isimulate);
   SDL_AtomicSet(&open, iopened);

   // functions
   int decr = 0;
   Array<EThreadApiFunction, 22>  execute;
   SDL_AtomicLock(&lock);
   decr = -1 * func.getCount();
   for (int i = 0; i < func.getCount(); i++)
      execute.pushBack(func[i]);
   func.clear();
   SDL_AtomicUnlock(&lock);
   while (true)
   {
      uint count = execute.getCount();
      if (count == 0) break;

      EThreadApiFunction f = execute.first();
      execute.popFront();

      int iret = 0;
      switch (f) {
      case afInit:
         SDL_AtomicLock(&lock);
         sfApiCreateContext(getCtx(), memory);
         sfApiInit();
         sfSetThreadSafe(getCtx(), threadSafe);
         sfSetActive(getCtx(), active);
         sfSetTimeOut(getCtx(), timeout);
         SDL_AtomicUnlock(&lock);
         break;
      case afIsOpened:
         iret = sfHardwareIsOpened(getCtx(), &iopened);
         SDL_AtomicSet(&open, iopened);
         break;
      case afOpenUsb:
         SDL_AtomicLock(&lock);
         iret += sfHardwareOpen(getCtx(), &usbData, iversion);
         iret += sfHardwareIsOpened(getCtx(), &iopened);
         SDL_AtomicSet(&open, iopened);
         SDL_AtomicUnlock(&lock);
         break;
      case afUploadFpga:
         SDL_AtomicLock(&lock);
         iret += sfHardwareUploadFpga(getCtx(), &fpgaData);
         SDL_AtomicUnlock(&lock);
         break;
      case afUploadFxx:
         SDL_AtomicLock(&lock);
         iret += sfHardwareUploadFx2(getCtx(), &fx2Data);
         SDL_AtomicUnlock(&lock);
         break;
      case afResetUsb:
         iret += sfHardwareReset(getCtx());
         break;
      case afCloseUsb:
         iret += sfHardwareClose(getCtx());
         break;
      case afEEPROMRead:
         SDL_AtomicLock(&lock);
         iret += sfHardwareEepromRead(getCtx(), &eepromData, eepromSize, eepromOffset);
         SDL_AtomicUnlock(&lock);
         break;
      case afEEPROMReadFirmwareID:
         SDL_AtomicLock(&lock);
         iret += sfHardwareEepromReadFirmwareID(getCtx(), &eepromData, eepromSize, eepromOffset);
         SDL_AtomicUnlock(&lock);
         break;
      case afEEPROMWrite:
         SDL_AtomicLock(&lock);
         iret += sfHardwareEepromWrite(getCtx(), &eepromData, eepromSize, eepromOffset);
         SDL_AtomicUnlock(&lock);
         break;
      case afEEPROMErase:
         iret += sfHardwareEepromErase(getCtx());
         break;
      case afSetFrame:
      {
         int iversion = SDL_AtomicGet(&version);
         int iheader = SDL_AtomicGet(&header);
         int idata = SDL_AtomicGet(&data);
         int ipacket = SDL_AtomicGet(&packet);
         iret += sfSetFrameVersion(getCtx(), iversion);
         iret += sfSetFrameHeader(getCtx(), iheader);
         iret += sfSetFrameData(getCtx(), idata);
         iret += sfSetFramePacket(getCtx(), ipacket);
      }
      break;
      case afGetFrame:
      {
         int iversion = 0;
         int iheader = 0;
         int idata = 0;
         int ipacket = 0;
         iret += sfGetFrameVersion(getCtx(), &iversion);
         iret += sfGetFrameHeader(getCtx(), &iheader);
         iret += sfGetFrameData(getCtx(), &idata);
         iret += sfGetFramePacket(getCtx(), &ipacket);
         SDL_AtomicSet(&version, iversion);
         SDL_AtomicSet(&header, iheader);
         SDL_AtomicSet(&data, idata);
         SDL_AtomicSet(&packet, ipacket);
      }
      break;
      case afHardwareConfig1:
      {
         SHardware1 hw1 = { 0 };
         getConfig1(&hw1);
         iret += sfHardwareConfig1(getCtx(), &hw1);
      }
      break;
      case afHardwareConfig2:
      {
         SHardware2 hw2 = { 0 };
         getConfig2(&hw2);
         iret += sfHardwareConfig2(getCtx(), &hw2);
      }
      break;
      case afSimulate:
      {
         SDL_AtomicLock(&lock);
         double time = simulateTimeValue;
         SDL_AtomicUnlock(&lock);
         iret += sfSimulate(getCtx(), time);
      }
      break;
      case afSetSimulateData:
         SDL_AtomicLock(&lock);
         iret += sfSetSimulateData(getCtx(), &simulateData);
         SDL_AtomicUnlock(&lock);
         break;
      case afSetSimulateOnOff:
         iret += sfSetSimulateOnOff(getCtx(), SDL_AtomicGet(&simulateOnOff));
         break;
      case afServerUpload:
         iret += sfServerUpload(getCtx());
         break;
      case afServerDownload:
         iret += sfServerDownload(getCtx());
         break;
      case afGetClientDisplay:
         SDL_AtomicLock(&lock);
         iret += sfGetClientDisplay(getCtx(), &displayData);
         SDL_AtomicUnlock(&lock);
         break;
      case afUploadGenerator:
         SDL_AtomicLock(&lock);
         iret += sfHardwareUploadGenerator(getCtx(), &generatorData);
         SDL_AtomicUnlock(&lock);
         break;
      case afSetUsb:
         iret += sfSetUsb(getCtx());
         break;
      case afSetNetwork:
         iret += sfSetNetwork(getCtx());
         break;
      case afClientConnect:
         iret += sfClientConnect(getCtx(), ip.asChar(), port);
         break;
      case afClientDisconnect:
         iret += sfClientDisconnect(getCtx());

         break;
      };
      SDL_AtomicSet(&ret[f], iret);
   }
   SDL_AtomicAdd(&sync, decr);
}

////////////////////////////////////////////////////////////////////////////////
//
// api
//
////////////////////////////////////////////////////////////////////////////////
int ThreadApi::getVersion()
{
   return SDL_AtomicGet(&version);
}
int ThreadApi::isOpen()
{
   return SDL_AtomicGet(&open);
}
int ThreadApi::isConnected()
{
   return SDL_AtomicGet(&connected);
}
int ThreadApi::isSimulate()
{
   return SDL_AtomicGet(&simulate);
}
void ThreadApi::setInit(int mem, int ithread, int iactive, int tt)
{
   SDL_AtomicLock(&lock);
      memory = mem;
      threadSafe = ithread;
      active = iactive;
      timeout = tt;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::setFrame(int  v, int  h, int  d, int  p)
{
   SDL_AtomicSet(&version, v);
   SDL_AtomicSet(&header, h);
   SDL_AtomicSet(&data, d);
   SDL_AtomicSet(&packet, p);
}
void ThreadApi::getFrame(int* v, int* h, int* d, int* p)
{
   *v = SDL_AtomicGet(&version);
   *h = SDL_AtomicGet(&header);
   *d = SDL_AtomicGet(&data);
   *p = SDL_AtomicGet(&packet);
}
void ThreadApi::setUSB(SUsb* usb)
{
   SDL_AtomicLock(&lock);
      usbData = *usb;
      usbSize = sizeof(SUsb);
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::getUSB(SUsb* usb)
{
   SDL_AtomicLock(&lock);
      *usb = usbData;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::setConfig1(SHardware1* c1)
{
   SDL_AtomicLock(&lock);
      config1 = *c1;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::getConfig1(SHardware1* c1)
{
   SDL_AtomicLock(&lock);
      *c1 = config1;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::setConfig2(SHardware2* c2)
{
   SDL_AtomicLock(&lock);
      config2 = *c2;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::getConfig2(SHardware2* c2)
{
   SDL_AtomicLock(&lock);
      *c2 = config2;
   SDL_AtomicUnlock(&lock);
}

void ThreadApi::setEEPROM(SEeprom* data, int  size, int  offset)
{
   SDL_AtomicLock(&lock);
      eepromData   = *data;
      eepromSize   = size;
      eepromOffset = offset;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::getEEPROM(SEeprom* data, int* size, int* offset)
{
   SDL_AtomicLock(&lock);
      *data   = eepromData;
      *size   = eepromSize;
      *offset = eepromOffset;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::setSimulateData(SSimulate* sim)
{
   SDL_AtomicLock(&lock);
      simulateData = *sim;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::getSimulateData(SSimulate* sim)
{
   SDL_AtomicLock(&lock);
      *sim = simulateData;
   SDL_AtomicUnlock(&lock);
}

void ThreadApi::setGeneratorData(SGenerator* gen)
{
   SDL_AtomicLock(&lock);
      generatorData = *gen;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::getGeneratorData(SGenerator* gen)
{
   SDL_AtomicLock(&lock);
      *gen = generatorData;
   SDL_AtomicUnlock(&lock);
}

void ThreadApi::setSimulateOnOff(int onoff)
{
   SDL_AtomicSet(&simulateOnOff,onoff);
}
void ThreadApi::getSimulateOnOff(int* onoff)
{
   *onoff = SDL_AtomicGet(&simulateOnOff);
}
void ThreadApi::setDisplay(SDisplay* dis)
{
   SDL_AtomicLock(&lock);
      displayData = *dis;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::getDisplay(SDisplay* dis)
{
   SDL_AtomicLock(&lock);
      if( displayData.ch0 || displayData.ch1 || displayData.fun || displayData.dig ) // speed optimization
         *dis = displayData;
   SDL_AtomicUnlock(&lock);
}
void ThreadApi::setIpPort(const char* sip, uint iport)
{
   SDL_AtomicLock(&lock);
      ip   = sip;
      port = iport;
   SDL_AtomicUnlock(&lock);
}

////////////////////////////////////////////////////////////////////////////////
//
// procedure
//
////////////////////////////////////////////////////////////////////////////////
int ThreadApi::writeFpgaToArtix7(SHardware1* ctrl1, SHardware2* ctrl2, OscHardware* hw)
{
   SDL_AtomicLock(&lock);

   // usb
   usbData = hw->getUSB();
   usbSize = sizeof(SUsb);

   // fx2
   {
      char usbPath[1024] = { 0 };
      int         ret = pFormat->formatPath(usbPath, 1024, hw->usbFirmware.asChar());
      fx2Size = (uint)SDL_strlen(usbPath);
      fx2Data.size = fx2Size;
      SDL_memcpy(fx2Data.data.bytes, usbPath, fx2Size);
      fx2Data.data.bytes[fx2Size] = 0;
   }

   // fpga
   {
      char*  buffer = 0;
      ilarge bufferSize = 0;
      char fpgaPath[1024] = { 0 };
      int         ret = pFormat->formatPath(fpgaPath, 1024, hw->fpgaFirmware.asChar());
      int fret = fileLoad(fpgaPath, &buffer, &bufferSize);
      fpgaData.size = fpgaSize = bufferSize;
      SDL_memcpy(fpgaData.data.bytes, buffer, fpgaSize);
      pMemory->free(buffer);
   }

   SDL_AtomicUnlock(&lock);

   // old hardware ?
   int ver = SDL_AtomicGet(&version);
   if (ver == HARDWARE_VERSION_1)
   {
      // open
      function(afOpenUsb);

      // upload
      function(afUploadFxx);

      // close
      function(afCloseUsb);
   }

   // open
   openUSB(hw);

   // callibration
   useEepromCallibration(hw);

   // fpga
   function(afUploadFpga);
   wait();

   // delay
   SDL_Delay(4000);

   // control
   hardwareControlFunction(ctrl1,ctrl2);
   wait();

   // ret
   return result(afUploadFpga);
}

int ThreadApi::openUSB(OscHardware* hw)
{
   if (SDL_AtomicGet(&open) == 0)
   {
      SUsb usb = hw->getUSB();
      setUSB(&usb);
      function(afOpenUsb);
      wait();
      if (SDL_AtomicGet(&open) == 0)
      {
         usb.idVendor  = CYPRESS_VID;
         usb.idProduct = CYPRESS_PID;
         setUSB(&usb);
         function(afOpenUsb);
         wait();
      }
   }
   return 0;
}
int ThreadApi::useEepromCallibration(OscHardware* hw)
{
   // callibration
   if (SDL_AtomicGet(&open) == 1)
   {
      SDL_AtomicLock(&lock);
      eepromSize = sizeof(OscHardware);
      eepromOffset = 256000;
      SDL_AtomicUnlock(&lock);
      function(afEEPROMRead);
      wait();

      // use callibration from eeprom
      if (result(afEEPROMRead) == 0 || eepromData.data.bytes[0] == 0)
      {
         cJSON* json = hw->json;
         SDL_memcpy(hw, &eepromData, eepromSize);
         hw->json = json;
      }
   }
   return 0;
}
int ThreadApi::writeUsbToEEPROM(OscHardware* hw)
{
   openUSB(hw);

   if (SDL_AtomicGet(&open) > 0)
   {
      SDL_AtomicLock(&lock);

      char fpgaPath[1024] = { 0 };
      int         ret = pFormat->formatPath(fpgaPath, 1024, hw->usbFirmware.asChar());
      fx2Size = (uint)SDL_strlen(fpgaPath);
      fx2Data.size = fx2Size;
      SDL_memcpy(fx2Data.data.bytes, fpgaPath, fx2Size);
      fx2Data.data.bytes[fx2Size] = 0;

      int iversion = SDL_AtomicGet(&version);
      if (iversion == HARDWARE_VERSION_1)
      {
         memset(&eepromData, 0, sizeof(SEeprom));
         eepromData.data.bytes[0] = 0xC0;
         eepromData.data.bytes[1] = (usbData.idVendor & 0xff);
         eepromData.data.bytes[2] = (usbData.idVendor >> 8) & 0xff;
         eepromData.data.bytes[3] = (usbData.idProduct & 0xff);
         eepromData.data.bytes[4] = (usbData.idProduct >> 8) & 0xff;
         eepromData.data.bytes[5] = (usbData.idSerial & 0xff);
         eepromData.data.bytes[6] = (usbData.idSerial & 0xff);
         eepromSize = 7;
         eepromOffset = 0;
      }
      if (iversion == HARDWARE_VERSION_2)
      {
         char path[1024] = { 0 };
         int         ret = pFormat->formatPath(path, 1024, hw->usbFirmware.asChar());
         eepromSize = sizeof(SEeprom);
         eepromOffset = 0;
         fileLoadPtr(path, (char*)&eepromData, &eepromSize);
      }
      SDL_AtomicUnlock(&lock);

      SUsb usb = hw->getUSB();
      setUSB(&usb);

      function(afUploadFxx);
      wait();
      function(afCloseUsb);
      wait();
      function(afOpenUsb);
      wait();
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Writting FX3 will start.", "Please wait and do not disconnect USB.",0 );
      function(afEEPROMWrite);
      wait();
   }
   int iret = result(afOpenUsb) + result(afUploadFxx) + result(afEEPROMWrite);
   if (iret == SCOPEFUN_SUCCESS)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Write USB data to EEPROM was successfull.", 0);
   }
   else
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Write USB data to EEPROM failed. Install WinUSB Device driver first.", 0);
   }
   return iret;
}

int ThreadApi::readFirmwareIDFromEEPROM(OscHardware* hw)
{
   int iversion = SDL_AtomicGet(&version);
   if (iversion == HARDWARE_VERSION_1)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "This function is supported only with hardware version 2 and above.", 0);
      return SCOPEFUN_FAILURE;
   }

   openUSB(hw);

   if (SDL_AtomicGet(&open) > 0)
   {
      SDL_AtomicLock(&lock);
      int iversion = SDL_AtomicGet(&version);
      if (iversion == HARDWARE_VERSION_1)
      {
         eepromSize = 7;
         eepromOffset = 0;
      }
      if (iversion == HARDWARE_VERSION_2)
      {
         eepromSize   = SCOPEFUN_EEPROM_FIRMWARE_NAME_BYTES;
         eepromOffset = 0;
      }
      SDL_AtomicUnlock(&lock);
      function(afEEPROMReadFirmwareID);
      wait();
   }
   int iret = result(afEEPROMReadFirmwareID);
   if (iret == SCOPEFUN_SUCCESS)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Read Frimware ID data from EEPROM was successfull.", 0);
   }
   else
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Read Firmware ID data from EEPROM failed.", 0);
   }
   return iret;
}

int ThreadApi::readUsbFromEEPROM(OscHardware* hw,int readsize)
{
   int iversion = SDL_AtomicGet(&version);
   if (iversion == HARDWARE_VERSION_1)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "This function is supported only with hardware version 2 and above.", 0);
      return SCOPEFUN_FAILURE;
   }

   openUSB(hw);

   if (SDL_AtomicGet(&open) > 0)
   {
      SDL_AtomicLock(&lock);
         int iversion = SDL_AtomicGet(&version);
         if (iversion == HARDWARE_VERSION_1)
         {
            eepromSize   = 7;
            eepromOffset = 0;
         }
         if (iversion == HARDWARE_VERSION_2)
         {
            ilarge size = readsize;
            if (size == 0)
            {
               char path[1024] = { 0 };
               int         ret = pFormat->formatPath(path, 1024, hw->usbFirmware.asChar());
               fileSize(path, &size);
            }
            eepromSize   = size;
            eepromOffset = 0;
         }
      SDL_AtomicUnlock(&lock);
      function(afEEPROMRead);
      wait();
   }
   int iret = result(afEEPROMRead);
   if (iret == SCOPEFUN_SUCCESS)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Read usb data from EEPROM was successfull.", 0);
   }
   else
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Read usb data from EEPROM failed.", 0);
   }
   return iret;
}


int ThreadApi::writeCallibrateSettingsToEEPROM(OscHardware* hw)
{
   int iversion = SDL_AtomicGet(&version);
   if (iversion == HARDWARE_VERSION_1)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "This function is supported only with hardware version 2 and above.", 0);
      return SCOPEFUN_FAILURE;
   }

   openUSB(hw);

   if (SDL_AtomicGet(&open) > 0)
   {
      // clear
      SDL_AtomicLock(&lock);
         eepromSize   = sizeof(OscHardware);
         eepromOffset = 256000;
         SDL_memset((void*)&eepromData, 0, (size_t)eepromSize);
      SDL_AtomicUnlock(&lock);
      function(afEEPROMWrite);
      wait();

      // write
      SDL_AtomicLock(&lock);
         eepromSize   = sizeof(OscHardware);
         eepromOffset = 256000;
         SDL_memcpy( (void*)&eepromData, (void*)hw, (size_t)eepromSize);
      SDL_AtomicUnlock(&lock);
      function(afEEPROMWrite);
      wait();
   }
   int iret = result(afEEPROMWrite);
   if (iret == SCOPEFUN_SUCCESS)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Write callibration data to EEPROM was successfull.", 0);
   }
   else
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Write callibration data to EEPROM failed.", 0);
   }
   return iret;
}
int ThreadApi::readCallibrateSettingsFromEEPROM(OscHardware* hw)
{
   int iversion = SDL_AtomicGet(&version);
   if (iversion == HARDWARE_VERSION_1)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "This function is supported only with hardware version 2 and above.", 0);
      return SCOPEFUN_FAILURE;
   }

   openUSB(hw);

   if ( SDL_AtomicGet(&open) > 0 )
   {
      SDL_AtomicLock(&lock);
         eepromSize = sizeof(OscHardware);
         eepromOffset = 256000;
      SDL_AtomicUnlock(&lock);
      function(afEEPROMRead);
      wait();
   }
   int iret = result(afEEPROMRead);
   if (iret == SCOPEFUN_SUCCESS)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Read callibration data from EEPROM was successfull.", 0);
   }
   else
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Read callibration data from EEPROM failed.", 0);
   }
   return iret;
}


int ThreadApi::eraseEEPROM(OscHardware* hw)
{
   openUSB(hw);

   // upload
   function(afUploadFxx);

   // old hardware ?
   int ver = SDL_AtomicGet(&version);
   if (ver == HARDWARE_VERSION_1)
   {
      // close
      function(afCloseUsb);

      // open
      function(afOpenUsb);
   }

   function(afEEPROMErase);
   wait();

   int iret = result(afEEPROMErase);
   if (iret == SCOPEFUN_SUCCESS)
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", "Erase EEPROM was successfull.", 0);
   }
   else
   {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Erase EEPROM failed.", 0);
   }
   return iret;
}

int ThreadApi::simulateTime(double time)
{
   SDL_AtomicLock(&lock);
      simulateTimeValue = time;
   SDL_AtomicUnlock(&lock);
   function(afSimulate);
   return 0;
}

int ThreadApi::captureFrameData(SFrameData* buffer, int toReceive, int* transfered,int type)
{
   return sfHardwareCapture(getCtx(), buffer, toReceive, transfered, type );
}

int ThreadApi::hardwareControlFunction(SHardware1* hw1, SHardware2* hw2)
{
   setConfig1(hw1);
   setConfig2(hw2);
   int ver = SDL_AtomicGet(&version);
   if (ver == HARDWARE_VERSION_1)
   {
      function(afHardwareConfig1);
   }
   if (ver == HARDWARE_VERSION_2)
   {
      function(afHardwareConfig2);
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Osciloscope
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeManager::OsciloscopeManager()
{
    dtUpdate = 0.0;
    dtRender = 0.0;
    ctx = new OscContext();
    captureBuffer = 0;
    control.setVersion(1);
    control.Default();
    control.setVersion(2);
    control.Default();
    serverThread = 0;
    serverActive = false;
    scrollThread = false;
    frameTime = 0.f;
    clearRenderTarget = 1;
    clearThermal = 0;
    analogWindowSize = 1.f;
    sliderMode  = 0;
    openglFocus = 1;
    SDL_AtomicSet(&etsClear, 1);
    signalMode = SIGNAL_MODE_PAUSE;
    windowSlot = 0;
}

void* malloc_fn2(size_t sz)
{
    return pMemory->allocate(sz);
}

void free_fn2(void* ptr)
{
    return pMemory->free(ptr);
}

cJSON_Hooks hooks;

int usbHotPlugCallback(int flags)
{
    return 0;
}

int SDLCALL CaptureDataThreadFunction(void* data);
int SDLCALL GenerateFrameThreadFunction(void* data);
int SDLCALL ControlHardwareThreadFunction(void* data);

int OsciloscopeManager::start()
{
    ////////////////////////////////////////////////
    // load settings
    ////////////////////////////////////////////////
    hooks.malloc_fn = malloc_fn2;
    hooks.free_fn = free_fn2;
    cJSON_InitHooks(&hooks);
    settings.load();
    wheel = 0;
    signalPosition = 0.f;
    signalZoom     = 1.f;
    sliderPosition = 0.5f;
    ////////////////////////////////////////////////
    // api
    ////////////////////////////////////////////////
    pOsciloscope->thread.setInit(settings.getSettings()->memoryFrame * MEGABYTE,1,1,0);
    pOsciloscope->thread.function(afInit);
    sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
    pOsciloscope->thread.setFrame(HARDWARE_VERSION_2, SCOPEFUN_FRAME_2_HEADER, 40000, SCOPEFUN_FRAME_2_PACKET);
    pOsciloscope->thread.function(afSetFrame);

    ////////////////////////////////////////////////
    // apply settings
    ////////////////////////////////////////////////
    control.setYRangeScaleA(vc2Volt, 1.f);
    control.setYRangeScaleB(vc2Volt, 1.f);
    ////////////////////////////////////////////////
    // apply canvas allocation settings
    ////////////////////////////////////////////////
    pCanvas3d->setVertexBufferSize(settings.getSettings()->renderVertexBufferSizeMegaByte * 1024 * 1024);
    pCanvas3d->setThreadBuffer(settings.getSettings()->renderThreadCount);
    pCanvas2d->setThreadBuffer(settings.getSettings()->renderThreadCount);
    ////////////////////////////////////////////////
    // generator fs
    ////////////////////////////////////////////////
    control.setGeneratorFrequency0(50.f, settings.getHardware()->generatorFs);
    control.setGeneratorFrequency1(50.f, settings.getHardware()->generatorFs);
    ////////////////////////////////////////////////
    // custom signal
    ////////////////////////////////////////////////
    FORMAT_BUFFER();
    FORMAT_PATH("data/signal/custom.signal");
    window.hardwareGenerator.loadCustomData(0, formatBuffer);
    window.hardwareGenerator.loadCustomData(1, formatBuffer);
    //////////////////////////////////////////////////////////
    // thread count
    //////////////////////////////////////////////////////////
    uint cpuCount = SDL_GetCPUCount();
    pRender->setThreadCount(min(settings.getSettings()->renderThreadCount, MAX_THREAD));
    //////////////////////////////////////////////////////////
    // allocate memory for signal
    //////////////////////////////////////////////////////////
    allocate();
    //////////////////////////////////////////////////////////
    // setup thread data
    //////////////////////////////////////////////////////////
    renderThreadActive = true;
    captureDataThreadActive = true;
    generateFrameThreadActive = true;
    controlHardwareThreadActive = true;
    updateThreadActive = true;
    threadLoop.capture.setCount(min(settings.getSettings()->renderThreadCount, MAX_THREAD));
    threadLoop.update.setCount(min(settings.getSettings()->renderThreadCount, MAX_THREAD));
    //////////////////////////////////////////////////////////
    // camera setup
    //////////////////////////////////////////////////////////
    cameraOsc.ortho.View.Pos() = Vector4(0.f, 0.f, -1.0f, 1.f);
    cameraOsc.zoom = 1.f;
    cameraFFT.ortho.View.Pos() = Vector4(0.f, 0.f, -1.0f, 1.f);
    cameraFFT.zoom = 1.f;
    grid.set(10.f, 10.f);
    ////////////////////////////////////////////////
    // SDL
    ////////////////////////////////////////////////
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    pRender->height = pOsciloscope->settings.getSettings()->windowDisplayWidth;
    pRender->width  = pOsciloscope->settings.getSettings()->windowDisplayHeight;
    int renderWidth = pOsciloscope->settings.getSettings()->windowDisplayWidth;
    int renderHeight = pOsciloscope->settings.getSettings()->windowDisplayHeight;
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    int cx = 0;
    int cy = mode.h / 16;
    int cw = 35 * mode.w / 80;
    int ch = 7 * mode.h / 8;
    pOsciloscope->sdlX = cx - (pOsciloscope->settings.getSettings()->windowControlWidth + pOsciloscope->settings.getSettings()->windowDisplayWidth) / 2;
    pOsciloscope->sdlY = cy - pOsciloscope->settings.getSettings()->windowDisplayHeight / 2;
    pOsciloscope->sdlW = pOsciloscope->settings.getSettings()->windowDisplayWidth;
    pOsciloscope->sdlH = pOsciloscope->settings.getSettings()->windowDisplayHeight;
    pOsciloscope->sdlX = cx;
    pOsciloscope->sdlY = cy;
    pOsciloscope->sdlW = cw;
    pOsciloscope->sdlH = ch;
    pRender->height = cw;
    pRender->width  = ch;
    renderWidth  = cw;
    renderHeight = ch;
    pOsciloscope->sdlContext = 0;
    pOsciloscope->sdlWindow = 0;
    if(pOsciloscope->settings.getSettings()->renderShaders21 == 0)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        pOsciloscope->sdlWindow = SDL_CreateWindow("ScopeFun", pOsciloscope->sdlX, pOsciloscope->sdlY, renderWidth, renderHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
        pOsciloscope->sdlContext = SDL_GL_CreateContext(pOsciloscope->sdlWindow);
    }
    if(!pOsciloscope->sdlContext)
    {
        if(pOsciloscope->sdlWindow)
           SDL_DestroyWindow(pOsciloscope->sdlWindow);
        SDL_GL_ResetAttributes();
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        pOsciloscope->sdlWindow = SDL_CreateWindow("ScopeFun", pOsciloscope->sdlX, pOsciloscope->sdlY, renderWidth, renderHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS);
        pOsciloscope->sdlContext = SDL_GL_CreateContext(pOsciloscope->sdlWindow);
        if(!pOsciloscope->sdlContext)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "error", "At least OpenGL 2.1(shaders) is requeired in order to run this program.", 0);
            return 1;
        }
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "warning", "Thermal rendering will be disabled.\nYou need OpenGL 3.2 graphics in order to use this feature.", 0);
        grSetMode(OPENGL_MODE_21);
    }
    else
    {
        grSetMode(OPENGL_MODE_32);
    }
    #if !defined(LINUX) && !defined(MAC)
    SDL_GetWindowSize(pOsciloscope->sdlWindow, &pOsciloscope->sdlW, &pOsciloscope->sdlH);
    #endif
    //////////////////////////////////////////////////
    //// glew
    //////////////////////////////////////////////////
    grCreateDevice();
    ////////////////////////////////////////////////
    // icon
    ////////////////////////////////////////////////
    #ifndef MAC
    FORMAT_PATH("data/icon/icon64.bmp")
    SDL_Surface* icon = SDL_LoadBMP(formatBuffer);
    SDL_SetWindowIcon(pOsciloscope->sdlWindow, icon);
    SDL_FreeSurface(icon);
    #endif
    ////////////////////////////////////////////////////////////
    //// shaders
    ////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        pOsciloscope->shadowLine3dShader = grCreateShader();
        pOsciloscope->shadowColorShader = grCreateShader();
        pOsciloscope->shadowCoolingShader = grCreateShader();
        pOsciloscope->shadowLine3dShader->compile("shadow/line3d.fx", pCanvas3d->declPos3D, GrShaderDefine(0));
        pOsciloscope->shadowColorShader->compile("shadow/color.fx", pCanvas2d->declPosTex2D, GrShaderDefine(0));
        pOsciloscope->shadowCoolingShader->compile("shadow/cooling.fx", pCanvas2d->declPosTex2D, GrShaderDefine(0));
    }
    ////////////////////////////////////////////////////////////
    //// Canvas / Fonts
    ////////////////////////////////////////////////////////////
    pRender->start();
    pCanvas2d->start();
    pCanvas3d->start();
    pFont->start();
    //////////////////////////////////////////////////
    //// resize
    //////////////////////////////////////////////////
    pRender->resize(pOsciloscope->sdlW, pOsciloscope->sdlH);
    //////////////////////////////////////////////////
    //// onApplicationInit
    //////////////////////////////////////////////////
    onApplicationInit();
    //////////////////////////////////////////////////////////
    // launch capture and update thread
    //////////////////////////////////////////////////////////
    startThreads();
    ////////////////////////////////////////////////
    // callbacks
    ////////////////////////////////////////////////
    pRender->registerCallback(this, 10);
    pCamera->registerCamera(&cameraOsc.ortho);
    pCamera->registerCamera(&cameraFFT.ortho);
    return 0;
}

void OsciloscopeManager::startThreads()
{
    FORMAT_BUFFER();
    ///////////////////////////////////////////////
    // launch render thread
    ///////////////////////////////////////////////
    usbTransfer = 0;
    renderLock = 0;
    uploadFirmwareLock = 0;
    displayLock = 0;
    captureLock = 0;
    SDL_AtomicSet(&contextCreated, 0);
    //////////////////////////////////////////////////////////
    // setup canvas
    //////////////////////////////////////////////////////////
    for(uint i = 0; i < pRender->getThreadCount(); i++)
    {
        pCanvas3d->threadBegin(i);
        pCanvas2d->threadBegin(i);
    }
    ///////////////////////////////////////////////
    // launche capture thread
    ///////////////////////////////////////////////
    SDL_zero(renderWindow);
    SDL_zero(renderData);
    pControlHardware = SDL_CreateThread(ControlHardwareThreadFunction,  "ControlHardware", this);
    thread.wait();

    pCaptureData = SDL_CreateThread(CaptureDataThreadFunction, "CaptureData", this);
    pGenerateFrame = SDL_CreateThread(GenerateFrameThreadFunction, "GenerateFrame", this);
}

void OsciloscopeManager::exitThreads()
{
    int status = 0;
    ///////////////////////////////////////////////
    // thread cleanup
    ///////////////////////////////////////////////
    captureDataThreadActive = false;
    SDL_WaitThread(pCaptureData, &status);
    pCaptureData = 0;

    generateFrameThreadActive = false;
    SDL_WaitThread(pGenerateFrame, &status);
    pGenerateFrame = 0;

    controlHardwareThreadActive = false;
    SDL_WaitThread(pControlHardware, &status);
    pControlHardware = 0;

    renderThreadActive = false;
    pRenderThread  = 0;
}


void OsciloscopeManager::oscCameraSetup(int enable)
{
    if(enable)
    {
        Vector4 pos = Vector4(1.f, 1.f, -1.f, 1.f);
        Vector4 at = Vector4(0.f, 0.f, 0.5f, 1.f);
        Vector4 up = Vector4(0.f, 1.f, 0.f, 1.f);
        cameraOsc.zoom = 1.f;
        cameraOsc.ortho.lookAt(pos, at, up);
    }
    else
    {
        Vector4 pos = Vector4(0.f, 0.f, -1.f, 1);
        Vector4 at = Vector4(0.f, 0.f, 0.f, 1.f);
        Vector4 up = Vector4(0.f, 1.f, 0.f, 1.f);
        cameraOsc.zoom = 1.f;
        cameraOsc.ortho.lookAt(pos, at, up);
    }
    cameraOsc.theta = 0;
    cameraOsc.phi = 0;
}

uint OsciloscopeManager::max3dTesselation(uint value, WndMain& wnd)
{
    int onOff = 0;
    if(wnd.channel01.OscOnOff)
    {
        onOff++;
    }
    if(wnd.channel02.OscOnOff)
    {
        onOff++;
    }
    if(wnd.function.OscOnOff)
    {
        onOff++;
    }
    ularge triangleBytes = 32 * 3; // pos, normal
    ularge safetyBytes   = 2 * MEGABYTE; // pos, normal
    while(1)
    {
        ularge neededBytes = (ularge(onOff * settings.getSettings()->historyFrameDisplay) * ularge(NUM_SAMPLES * 2) * triangleBytes) / value;
        ularge actualBytes = (ularge(settings.getSettings()->renderVertexBufferSizeMegaByte) * MEGABYTE - safetyBytes);
        if(neededBytes < actualBytes || value > 1024)
        {
            return value;
        }
        value++;
    }
    return value;
}

void OsciloscopeManager::fftCameraSetup(int enable)
{
    if(enable)
    {
        Vector4 pos = Vector4(1.f, 1.f, -1.f,  1.f);
        Vector4  at = Vector4(0.f, 0.f,  0.5f, 1.f);
        Vector4  up = Vector4(0.f, 1.f,  0.f,  1.f);
        cameraFFT.zoom = 1.f;
        cameraFFT.ortho.lookAt(pos, at, up);
    }
    else
    {
        Vector4 pos = Vector4(0.f, 0.f, -1.f, 1);
        Vector4 at = Vector4(0.f, 0.f, 0.f, 1.f);
        Vector4 up = Vector4(0.f, 1.f, 0.f, 1.f);
        cameraFFT.zoom = 1.f;
        cameraFFT.ortho.lookAt(pos, at, up);
    }
    cameraFFT.theta = 0;
    cameraFFT.phi = 0;
}

int OsciloscopeManager::update(float dt)
{
    if(fileThread.isRunning())
    {
        return 0;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // setup
    ////////////////////////////////////////////////////////////////////////////////
    // clearRenderTarget = !window.horizontal.ETS;

    ////////////////////////////////////////////////////////////////////////////////
    // mouse
    ////////////////////////////////////////////////////////////////////////////////
    int mX = pInput->getMouseX();
    int mY = pInput->getMouseY();
    int mRelX = pInput->getMouseRelX();
    int mRelY = pInput->getMouseRelY();
    int mWheel = pInput->getMouseWheel();
    ////////////////////////////////////////////////////////////////////////////////
    // window size
    ////////////////////////////////////////////////////////////////////////////////
    int wWidth = 0;
    int wHeight = 0;
    SDL_GetWindowSize(sdlWindow, &wWidth, &wHeight);
    ////////////////////////////////////////////////////////////////////////////////
    // relative mouse position
    ////////////////////////////////////////////////////////////////////////////////
    float fRelX = float(mRelX) / float(wWidth);
    float fRelY = float(mRelY) / float(wHeight);
    ////////////////////////////////////////////////////////////////////////////////
    // dimensions
    ////////////////////////////////////////////////////////////////////////////////
    float width = (float)pRender->width;
    float height = (float)pRender->height;
    float aspect = (float)pRender->aspect;
    ////////////////////////////////////////////////////////////////////////////////
    // speed
    ////////////////////////////////////////////////////////////////////////////////
    float moveSpeed    = 22.f;
    ////////////////////////////////////////////////////////////////////////////////
    // move speed
    ////////////////////////////////////////////////////////////////////////////////
    Vector4 CamMoveSpeed = Vector4(moveSpeed * dt, moveSpeed * dt, 0.f, 0.f);
    ////////////////////////////////////////////////////////////////////////////////
    // View
    ////////////////////////////////////////////////////////////////////////////////
    if(!window.fftDigital.is(VIEW_SELECT_OSC_3D) && !window.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        if((mX > 0 && mX < wWidth && mY > 0.f && mY < wHeight))
        {
            ////////////////////////////////////////////////////////////////////////////////
            // zoom
            ////////////////////////////////////////////////////////////////////////////////
            if(mWheel)
            {
                clearThermal      = 1;
                clearRenderTarget = 1;
                clearEts(1);
                // capture
                double captureTime = window.horizontal.Capture;
                // precision
                ularge precision = 1000;
                // frame
                ularge  frameCount = ularge(window.horizontal.FrameSize) * precision;
                double  frameTime  = frameCount * captureTime;
                // grid
                ularge  gridCount  = frameCount / 10;
                double  gridTime   = double(gridCount) * captureTime;
                // time
                ularge zoomCount   = signalZoom /*cameraOsc.zoom**/  * gridCount;
                // factor
                double zeros  = floor(log(double(zoomCount)) / log(10.0));
                double factor = pow(10, zeros);
                double gridDividers[3] = { 0, 0, 0 };
                int index = 0;
                for(int i = 1; i < 10; i++)
                {
                    if(window.horizontal.FrameSize % i == 0)
                    {
                        gridDividers[index] = i;
                        index++;
                        if(index > 2)
                        {
                            break;
                        }
                    }
                }
                double gridNumber[9] = { 0 };
                gridNumber[0] = gridDividers[0] / 10.0;
                gridNumber[1] = gridDividers[1] / 10.0;
                gridNumber[2] = gridDividers[2] / 10.0;
                gridNumber[3] = gridDividers[0];
                gridNumber[4] = gridDividers[1];
                gridNumber[5] = gridDividers[2];
                gridNumber[6] = gridDividers[0] * 10.0;
                gridNumber[7] = gridDividers[1] * 10.0;
                gridNumber[8] = gridDividers[2] * 10.0;
                double time = 0.0;
                if(mWheel < 0)
                {
                    for(int i = 0; i < 9; i++)
                    {
                        time = double(gridNumber[i] * factor);
                        if(time > (zoomCount + 10))
                        {
                            break;
                        }
                    }
                }
                else
                {
                    for(int i = 8; i >= 0 ; i--)
                    {
                        time = double(gridNumber[i] * factor);
                        if(time < (zoomCount - 10))
                        {
                            break;
                        }
                    }
                }
                // zoom
                double zoom = clamp<double>(double(time) / double(gridCount), 0.0, 1.0);
                // zoom
                double zoomMax = 1.0;
                double zoomMin = 1.0 / double(window.horizontal.FrameSize);
                signalZoom = clamp<float>(zoom, zoomMin, zoomMax);
                cameraFFT.zoom = zoom;
                cameraOsc.zoom = zoom;
            }
            ////////////////////////////////////////////////////////////////////////////////
            // measure
            ////////////////////////////////////////////////////////////////////////////////
            float mouseNormalized = float(mX) / float(pRender->width);
            if(aspect > 1)
            {
                mouseNormalized = float(mX) / float(pRender->height);
            }
            mouseNormalized *= 1.2f;
            mouseNormalized -= (0.5f * 1.2f);
            float mouseNormalizedY = float(mY) / (float(pRender->width) * analogWindowSize);
            mouseNormalizedY  = mouseNormalizedY * 1.2f;
            mouseNormalizedY -= (0.5f * 1.2f);
            float mouseOSC  =  mouseNormalized * cameraOsc.zoom + (cameraOsc.ortho.View.Pos().x + 0.5);
            float mouseFFT  =  mouseNormalized * cameraFFT.zoom + (cameraFFT.ortho.View.Pos().x);
            float mouseY = -mouseNormalizedY;
            ////////////////////////////////////////////////////////////////////////////////
            // move
            ////////////////////////////////////////////////////////////////////////////////
            if(pInput->isClick(SDL_BUTTON_LEFT))
            {
                window.measure.data.pickX0.onClick();
                window.measure.data.pickX1.onClick();
                window.measure.data.pickY0.onClick();
                window.measure.data.pickY1.onClick();
                window.measure.data.pickFFT0.onClick();
                window.measure.data.pickFFT1.onClick();
            }
            window.measure.data.pickX0.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickX1.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickY0.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickY1.onUpdate(mouseOSC, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickFFT0.onUpdate(mouseFFT, mouseY, 0.f, window.horizontal.Frame);
            window.measure.data.pickFFT1.onUpdate(mouseFFT, mouseY, 0.f, window.horizontal.Frame);
            if(pInput->isMouse(SDL_BUTTON_LEFT))
            {
                clearRenderTarget = 1;
                clearEts(1);
                float zoomOffset = (1.f / signalZoom) / 2.f;
                OsciloscopeSlider slider;
                slider.Rectangle(sliderRectW, sliderRectH, sliderRectX, sliderRectY, pRender->width, pRender->height, analogWindowSize);
                slider.MinMax(sliderMin, sliderMax, pRender->width, pRender->height, sliderPosition - 0.5f, analogWindowSize, signalZoom /*cameraFFT.zoom */);
                insideSliderBox  = (mX > sliderMin && mX < sliderMax && mY >= sliderRectY && mY <= sliderRectY + sliderRectH);
                insideSliderArea = (mY >= sliderRectY && mY <= sliderRectY + sliderRectH);
                // slider
                if((sliderMode == 1 || insideSliderBox))
                {
                    clearThermal = 1;
                    sliderMode = 1;
                    sliderPosition += (float(mRelX) / float(pRender->width));
                    sliderPosition = clamp<float>(sliderPosition, 0.f, 1.f);
                    signalPosition = clamp<float>(-sliderPosition / signalZoom + 0.5 / signalZoom, signalPosition - 0.6f / signalZoom, signalPosition + 0.6f / signalZoom);
                }
                // up / down
                else if(insideSliderArea)
                {
                    sliderMode = 2;
                    analogWindowSize += (float(mRelY)) / (float(pRender->width));
                }
                else if(sliderMode == 2)
                {
                    analogWindowSize += (float(mRelY)) / (float(pRender->width));
                }
                else
                {
                    if(fRelX != 0.0f)
                    {
                        clearThermal = 1;
                        float moveFactor = (cameraFFT.ortho.width / cameraOsc.ortho.width);
                        Vector4 Move = Vector4(-fRelX, 0, 0, 1);
                        Vector4 MoveFFT = Move * CamMoveSpeed * Vector4(cameraFFT.zoom * moveFactor);
                        Vector4 MoveOsc = Move * CamMoveSpeed * Vector4(cameraOsc.zoom);
                        signalPosition += fRelX;
                    }
                }
            }
            else
            {
                sliderMode = 0;
            }
            ////////////////////////////////////////////////////////////////////////////////
            // reset
            ////////////////////////////////////////////////////////////////////////////////
            if(pInput->isMouse(SDL_BUTTON_RIGHT))
            {
                clearThermal = 1;
                clearRenderTarget = 1;
                clearEts(1);
                cameraFFT.ortho.View.Pos() = Vector4(0, 0, -1.f, 1.f);
                cameraFFT.zoom = 1.f;
                cameraOsc.ortho.View.Pos() = Vector4(0, 0, -1.f, 1.f);
                cameraOsc.zoom = 1.f;
                signalPosition = 0.f;
                signalZoom     = 1.f;
                sliderPosition = 0.5f;
            }
        }
    }
    analogWindowSize = clamp(analogWindowSize, 0.f, 1.f / aspect);
    float oscW = width;
    float oscH = width * analogWindowSize;
    float oscA = oscW / oscH;
    float fftW = width;
    float fftH = height - width * analogWindowSize;
    float fftA = fftW / fftH;
    fftAspectX = width / (height - width * analogWindowSize);
    fftAspectY = (width - height * analogWindowSize) / height;
    oscAspectX = width / (width * analogWindowSize);
    oscAspectY = (height * analogWindowSize) / height;
    float scale = (0.5f / aspect);
    ////////////////////////////////////////////////////////////////////////////////
    // FFT Camera
    ////////////////////////////////////////////////////////////////////////////////
    float fftWidth  = 1.2f;
    float fftHeight = 1.2f;
    fftScaleX = 1.f * scale;
    fftScaleY = fftA * scale;
    cameraFFT.ortho.setOrthographic(fftWidth, fftHeight, 0.0001f, 1.2f);
    ////////////////////////////////////////////////////////////////////////////////
    // orthographics
    ////////////////////////////////////////////////////////////////////////////////
    float oscWidth  = 1.2f;
    float oscHeight = 1.2f;
    oscScaleX = 1.f * scale;
    oscScaleY = oscA * scale;
    cameraOsc.ortho.setOrthographic(oscWidth, oscHeight, 0.0001f, 1.2f);
    ////////////////////////////////////////////////////////////////////////////////
    // 3d fft
    ////////////////////////////////////////////////////////////////////////////////
    if(window.fftDigital.is(VIEW_SELECT_FFT_3D))
    {
        // persepctive
        int sizeX       = 0;
        int sizeY       = 0;
        int sizeYOsc    = 0;
        int mouseInView = 0;
        {
            sizeX       = width;
            sizeY       = height - width * analogWindowSize;
            sizeYOsc    = width * analogWindowSize;
            mouseInView = mX > 0 && mX < sizeX && mY > width * analogWindowSize && mY < height;
        }
        float aspect = float(sizeX) / float(sizeY);
        cameraFFT.ortho.setPerspective(45.f, aspect, 0.1f, 7.f);
        // mouse
        if(mouseInView)
        {
            if(pInput->isClick(SDL_BUTTON_LEFT))
            {
                window.measure.data.pickFFT0.onClick();
                window.measure.data.pickFFT1.onClick();
            }
            Matrix4x4 proj = matMultiply(matInverse(cameraFFT.ortho.Perspective), cameraFFT.ortho.View);
            Vector4     l0 = matUnprojectScreenSpace(mX, sizeY - (mY - sizeYOsc), sizeX, sizeY, proj, 0.25f);
            Vector4     l1 = matUnprojectScreenSpace(mX, sizeY - (mY - sizeYOsc), sizeX, sizeY, proj, 0.75f);
            Vector4      l = vecNormalize3d(l1 - l0);
            // pick X
            {
                Vector4      n = Vector4(0, 0, 1, 0);
                float dotUpper = -1.f * vecDot3d(l0, n);
                float dotlower = vecDot3d(l, n);
                if(dotlower != 0.f)
                {
                    float d = dotUpper / dotlower;
                    Vector4 pos = Vector4(d) * l + l0;
                    window.measure.data.pickFFT0.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                    window.measure.data.pickFFT1.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                }
            }
            if(pInput->isMouse(SDL_BUTTON_LEFT))
            {
                cameraFFT.theta =  TO_RAD * float(mRelX) / float(sizeX) * dt * 10000.f;
                cameraFFT.phi   = -TO_RAD * float(mRelY) / float(sizeY) * dt * 10000.f;
                clearThermal = 1;
                clearEts(1);
            }
            else
            {
                cameraFFT.theta = 0;
                cameraFFT.phi = 0;
            }
        }
        // rotate
        cameraFFT.ortho.View = matMultiply(cameraFFT.ortho.View, matRotationY(cameraFFT.theta));
        cameraFFT.ortho.View = matMultiply(cameraFFT.ortho.View, matRotationAxisAngle(cameraFFT.ortho.View.RowX(), cameraFFT.phi));
        // scale
        fftScaleX = 1.f;
        fftScaleY = 1.f;
        // wheel
        if(mWheel && mouseInView)
        {
            clearThermal = 1;
            clearRenderTarget = 1;
            clearEts(1);
            cameraFFT.zoom -= 0.01f * dt * mWheel;
            cameraFFT.zoom = clamp<float>(cameraFFT.zoom, 0.05f, 2.f);
        }
        float  cameraR = cameraFFT.zoom * 2.0f;
        cameraFFT.ortho.View.Pos() = Vector4(0, 0, 0.5f, 1) + cameraFFT.ortho.View.RowZ() * Vector4(-cameraR, -cameraR, -cameraR, 1);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // 3d osc
    ////////////////////////////////////////////////////////////////////////////////
    if(window.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        // persepctive
        int sizeX       = 0;
        int sizeY       = 0;
        int mouseInView = 0;
        {
            sizeX = width;
            sizeY = width * analogWindowSize;
            mouseInView = mX > 0 && mX < sizeX && mY > 0 && mY < sizeY;
        }
        float aspect = float(sizeX) / float(sizeY);
        cameraOsc.ortho.setPerspective(45.f, aspect, 0.1f, 7.f);
        // mouse
        if(mouseInView)
        {
            ////////////////////////////////////////////////////////////////////////////////
            // move
            ////////////////////////////////////////////////////////////////////////////////
            if(pInput->isClick(SDL_BUTTON_LEFT))
            {
                window.measure.data.pickX0.onClick();
                window.measure.data.pickX1.onClick();
                window.measure.data.pickY0.onClick();
                window.measure.data.pickY1.onClick();
            }
            Matrix4x4 proj = matMultiply(matInverse(cameraOsc.ortho.Perspective), cameraOsc.ortho.View);
            Vector4     l0 = matUnprojectScreenSpace(mX, sizeY - mY, sizeX, sizeY, proj, 0.25f);
            Vector4     l1 = matUnprojectScreenSpace(mX, sizeY - mY, sizeX, sizeY, proj, 0.75f);
            Vector4      l = vecNormalize3d(l1 - l0);
            // pick X
            {
                Vector4      n = Vector4(0, 0, 1, 0);
                float dotUpper = -1.f * vecDot3d(l0, n);
                float dotlower = vecDot3d(l, n);
                if(dotlower != 0.f)
                {
                    float d = dotUpper / dotlower;
                    Vector4 pos = Vector4(d) * l +  l0;
                    window.measure.data.pickX0.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                    window.measure.data.pickX1.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                }
            }
            // pick Y
            {
                Vector4     n = Vector4(0, 0, 1, 0);
                float dotUpper = -1.f * vecDot3d(l0, n);
                float dotlower = vecDot3d(l, n);
                if(dotlower != 0.f)
                {
                    float d = dotUpper / dotlower;
                    Vector4 pos = Vector4(d) * l + l0;
                    window.measure.data.pickY0.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                    window.measure.data.pickY1.onUpdate(pos.x, pos.y, pos.z, window.horizontal.Frame);
                }
            }
            if(pInput->isMouse(SDL_BUTTON_LEFT))
            {
                cameraOsc.theta =  TO_RAD * float(mRelX) / float(sizeX) * dt * 10000.f;
                cameraOsc.phi   = -TO_RAD * float(mRelY) / float(sizeY) * dt * 10000.f;
                clearThermal = 1;
                clearEts(1);
            }
            else
            {
                cameraOsc.theta = 0;
                cameraOsc.phi = 0;
            }
        }
        // rotate
        cameraOsc.ortho.View = matMultiply(cameraOsc.ortho.View, matRotationY(cameraOsc.theta));
        cameraOsc.ortho.View = matMultiply(cameraOsc.ortho.View, matRotationAxisAngle(cameraOsc.ortho.View.RowX(), cameraOsc.phi));
        // scale
        oscScaleX = 1.f;
        oscScaleY = 1.f;
        // wheel
        float zoomZoom = 0.f;
        if(mWheel && mouseInView)
        {
            zoomZoom = 0.1f * dt * float(mWheel);
            clearThermal = 1;
            clearEts(1);
            cameraOsc.zoom -= 0.01f * dt * mWheel;
            cameraOsc.zoom = clamp<float>(cameraOsc.zoom, 0.05f, 2.f);
        }
        // position
        float  cameraR = cameraOsc.zoom * 2.0f;
        cameraOsc.ortho.View.Pos() = Vector4(0, 0, 0.5f, 1) + cameraOsc.ortho.View.RowZ() * Vector4(-cameraR, -cameraR, -cameraR, 1);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // mouuse debug
    ////////////////////////////////////////////////////////////////////////////////
    #ifdef MOUSE_DEBUG
    int lheight = 25;
    pFont->setSize(0.5f);
    FORMAT_BUFFER();
    FORMAT("Mouse  : %d,%d", mX, mY);
    pFont->writeText(100, 100,  formatBuffer);
    FORMAT("Mouse Rel : %d,%d", mRelX, mRelY);
    pFont->writeText(100, 100 + lheight, formatBuffer);
    FORMAT("Left   : %d", pInput->isMouse(SDL_BUTTON_LEFT));
    pFont->writeText(100, 100 + 2 * lheight,  formatBuffer);
    /*
    FORMAT("Middle : %d",pInput->isMouse(SDL_BUTTON_MIDDLE));
    pFont->writeText(100,100+lheight*3,formatBuffer );

    FORMAT("Right  : %d",pInput->isMouse(SDL_BUTTON_RIGHT));
    pFont->writeText(100,100+lheight*4,formatBuffer  );

    FORMAT("Wheel  : %d",mWheel);
    pFont->writeText(100,100+lheight*5, formatBuffer );*/
    #endif
    return 0;
}

void setAnalogViewport(int width, int height, float size)
{
    size = max(size, 0.000000001f);
    grViewport(0, float(height - width * size), (float)width, (float)width * size, 0.f, 1.f);
}

void setFFTViewport(int width, int height, float size)
{
    size = max(size, 0.000000001f);
    grViewport(0, 0, (float)width, (float)(height - width * size), 0.f, 1.f);
}



void OsciloscopeManager::renderThread(uint threadId, OsciloscopeThreadData& threadData, OsciloscopeThreadRenderer& renderer, OsciloscopeFFT& fft)
{
    WndMain&                 wndMain = threadData.window;
    OsciloscopeRenderData&    render = threadData.render;
    OsciloscopeFrame&          frame = threadData.frame;
    MeasureData&             measure = measureData[threadId];
    ////////////////////////////////////////////////////////////////////////////////
    // begin
    ////////////////////////////////////////////////////////////////////////////////
    pRender->GetFlag(threadId).set(render.flags.get());
    pCanvas3d->threadBegin(threadId);
    pCanvas2d->threadBegin(threadId);
    pFont->threadStart(threadId);
    ////////////////////////////////////////////////////////////////////////////////
    // measure signal
    ////////////////////////////////////////////////////////////////////////////////
    renderer.measureSignal(threadId, threadData, measure, fft);
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_THERMAL);
    pCanvas2d->setMode(threadId, RENDER_MODE_THERMAL);
    ////////////////////////////////////////////////////////////////////////////////
    // preOscRender
    ////////////////////////////////////////////////////////////////////////////////
    renderer.preOscRender(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // width/height
    ////////////////////////////////////////////////////////////////////////////////
    float width  = (float)pRender->width;
    float height = (float)pRender->height;
    ////////////////////////////////////////////////////////////////////////////////
    // heating
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        if(wndMain.horizontal.Mode != SIGNAL_MODE_PAUSE && wndMain.thermal.enabled)
        {
            ////////////////////////////////////////////////////////////////////////////////
            // Shadow Channel01
            ////////////////////////////////////////////////////////////////////////////////
            if(wndMain.channel01.OscOnOff)
            {
                renderer.renderAnalog(threadId, threadData, 0.f, 0, 1, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, render.colorChannel0, wndMain.channel01.Invert);
            }
            ////////////////////////////////////////////////////////////////////////////////
            // Shadow Channel02
            ////////////////////////////////////////////////////////////////////////////////
            if(wndMain.channel02.OscOnOff)
            {
                renderer.renderAnalog(threadId, threadData, 0.f, 1, 1, frame, wndMain.horizontal.Capture, wndMain.channel02.Capture, render.colorChannel1, wndMain.channel02.Invert);
            }
        }
    }
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_COOLING);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_COOLING);
    ////////////////////////////////////////////////////////////////////////////////
    // cooling
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        if(wndMain.horizontal.Mode != SIGNAL_MODE_PAUSE && wndMain.thermal.enabled)
        {
            pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_RECTANGLEUV, 1);
            pCanvas2d->bRectangleUV(threadId, Vector4(0.f, 0.f, 0.f, 1.f), Vector4((float)render.width, (float)render.height, 1.f, 0.f));
            Vector4 vector;
            vector.x = window.thermal.cooling;
            vector.y = 0.f;
            vector.z = 0.f;
            vector.w = 0.f;
            pCanvas2d->addConstant(threadId, vector);
            pCanvas2d->endBatch(threadId, -1, render.shadowTexture, BLEND_MODE_COPY, SAMPLE_STATE_CANVAS2D, render.shadowCoolingShader);
        }
    }
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_BLEND_IN_HEATING);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_BLEND_IN_HEATING);
    ////////////////////////////////////////////////////////////////////////////////
    // blend
    ////////////////////////////////////////////////////////////////////////////////
    if(render.shadowTexture != 0)
    {
        int debug = 1;
    }
    pCanvas2d->beginBatch(threadId, CANVAS2D_BATCH_RECTANGLEUV, 1);
    pCanvas2d->bRectangleUV(threadId, Vector4(0.f, 0.f, 0.f, 1.f), Vector4((float)render.width, (float)render.height, 1.f, 0.f));
    pCanvas2d->endBatch(threadId, -1, render.shadowTexture, BLEND_MODE_ALPHA, SAMPLE_STATE_CANVAS2D, render.shadowColorShader);
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_3D);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_3D);
    ////////////////////////////////////////////////////////////////////////////////
    // fps
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderFps(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // Signal 3d
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.fftDigital.is(VIEW_SELECT_OSC_3D))
    {
        if(!threadData.history.isEmpty())
        {
            if(wndMain.channel01.OscOnOff || wndMain.channel02.OscOnOff || wndMain.function.OscOnOff)
            {
                Ring<OsciloscopeFrame> currentFrames = threadData.history;
                if(currentFrames.getCount())
                {
                    float    zDelta = 1.f / (currentFrames.getCount() - 1);
                    float         z = 1.f;
                    OsciloscopeFrame newFrame;
                    uint count = currentFrames.getCount();
                    for(uint i = 0; i < count; i++)
                    {
                        currentFrames.read(newFrame);
                        if(wndMain.channel01.OscOnOff)
                        {
                            uint r = COLOR_R(render.colorChannel0);
                            uint g = COLOR_G(render.colorChannel0);
                            uint b = COLOR_B(render.colorChannel0);
                            renderer.renderAnalog3d(threadId, threadData, i, z, 0, newFrame, wndMain.horizontal.Capture, wndMain.channel01.Capture, COLOR_ARGB(wndMain.display.alpha3dCh0, r, g, b), wndMain.channel01.Invert);
                        }
                        if(wndMain.channel02.OscOnOff)
                        {
                            uint r = COLOR_R(render.colorChannel1);
                            uint g = COLOR_G(render.colorChannel1);
                            uint b = COLOR_B(render.colorChannel1);
                            renderer.renderAnalog3d(threadId, threadData, i, z, 1, newFrame, wndMain.horizontal.Capture, wndMain.channel01.Capture, COLOR_ARGB(wndMain.display.alpha3dCh1, r, g, b), wndMain.channel02.Invert);
                        }
                        if(wndMain.function.OscOnOff)
                        {
                            uint r = COLOR_R(render.colorFunction);
                            uint g = COLOR_G(render.colorFunction);
                            uint b = COLOR_B(render.colorFunction);
                            renderer.renderAnalogFunction3d(threadId, threadData, newFrame, i, z, COLOR_ARGB(wndMain.display.alpha3dFun, r, g, b));
                        }
                        z -= zDelta;
                    }
                    if(wndMain.channel01.OscOnOff)
                    {
                        uint r = COLOR_R(render.colorChannel0);
                        uint g = COLOR_G(render.colorChannel0);
                        uint b = COLOR_B(render.colorChannel0);
                        renderer.renderSurface3d(threadId, threadData, 0, COLOR_ARGB(wndMain.display.alpha3dCh0, r, g, b));
                    }
                    if(wndMain.channel02.OscOnOff)
                    {
                        uint r = COLOR_R(render.colorChannel1);
                        uint g = COLOR_G(render.colorChannel1);
                        uint b = COLOR_B(render.colorChannel1);
                        renderer.renderSurface3d(threadId, threadData, 1, COLOR_ARGB(wndMain.display.alpha3dCh1, r, g, b));
                    }
                    if(wndMain.function.OscOnOff)
                    {
                        uint r = COLOR_R(render.colorFunction);
                        uint g = COLOR_G(render.colorFunction);
                        uint b = COLOR_B(render.colorFunction);
                        renderer.renderSurface3d(threadId, threadData, -1, COLOR_ARGB(wndMain.display.alpha3dFun, r, g, b));
                    }
                }
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_2D);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_ANALOG_2D);
    ////////////////////////////////////////////////////////////////////////////////
    // Grid
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderAnalogGrid(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // Axis
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderAnalogAxis(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // Units
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderAnalogUnits(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // ETS clear
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.horizontal.ETS)
    {
        if(wndMain.channel01.OscOnOff)
        {
            if(frame.etsAttr & ETS_CLEAR)
            {
                renderer.renderAnalog(threadId, threadData, 0.f, 0, 0, threadData.etsClear, wndMain.horizontal.Capture, wndMain.channel01.Capture, 0xff303030, wndMain.channel01.Invert);
            }
        }
        if(wndMain.channel02.OscOnOff)
        {
            if(frame.etsAttr & ETS_CLEAR)
            {
                renderer.renderAnalog(threadId, threadData, 0.f, 1, 0, threadData.etsClear, wndMain.horizontal.Capture, wndMain.channel02.Capture, 0xff303030, wndMain.channel02.Invert);
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Signal Channel01
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.channel01.OscOnOff)
    {
        renderer.renderAnalog(threadId, threadData, 0.f, 0, 0, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, render.colorChannel0, wndMain.channel01.Invert);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Signal Channel02
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.channel02.OscOnOff)
    {
        renderer.renderAnalog(threadId, threadData, 0.f, 1, 0, frame, wndMain.horizontal.Capture, wndMain.channel02.Capture, render.colorChannel1, wndMain.channel02.Invert);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Function
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.function.OscOnOff)
    {
        renderer.renderAnalogFunction(threadId, threadData, 0.f, wndMain.function.Type, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, wndMain.channel02.Capture, render.colorFunction, wndMain.channel01.Invert, wndMain.channel02.Invert);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // FunctionXY
    ////////////////////////////////////////////////////////////////////////////////
    if(wndMain.function.xyGraph)
    {
        renderer.renderAnalogFunctionXY(threadId, threadData, frame, wndMain.horizontal.Capture, wndMain.channel01.Capture, wndMain.channel02.Capture, render.colorXyGraph);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Mode
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_MEASURE);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_MEASURE);
    ////////////////////////////////////////////////////////////////////////////////
    // measurre
    ////////////////////////////////////////////////////////////////////////////////
    renderer.renderMeasure(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // preFftRender
    ////////////////////////////////////////////////////////////////////////////////
    renderer.preFftRender(threadId, threadData);
    if(wndMain.fftDigital.is(VIEW_SELECT_DIGITAL))
    {
        ////////////////////////////////////////////////////////////////////////////////
        // Mode
        ////////////////////////////////////////////////////////////////////////////////
        pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_DIGITAL);
        pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_DIGITAL);
        ////////////////////////////////////////////////////////////////////////////////
        // digital
        ////////////////////////////////////////////////////////////////////////////////
        int bits = (int)pow(float(2), float(wndMain.display.digitalBits + 3));
        renderer.renderDigitalGrid(threadId, threadData, bits, 16);
        renderer.renderDigitalAxis(threadId, threadData, bits, 16);
        renderer.renderDigitalUnit(threadId, threadData, bits, 16);
        renderer.renderDigital(threadId, threadData, measure, bits, 16);
    }
    else
    {
        ////////////////////////////////////////////////////////////////////////////////
        // Mode
        ////////////////////////////////////////////////////////////////////////////////
        pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_FFT3D);
        pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_FFT3D);
        renderer.renderMeasureFFT(threadId, threadData);
        ////////////////////////////////////////////////////////////////////////////////
        // FFT 3d
        ////////////////////////////////////////////////////////////////////////////////
        if(wndMain.fftDigital.is(VIEW_SELECT_FFT_3D))
        {
            if(!threadData.history.isEmpty())
            {
                Ring<OsciloscopeFrame> currentFrames = threadData.history;
                int framesCount = currentFrames.getCount();
                float    zDelta = 1.f / framesCount;
                float         z = 1.f;
                OsciloscopeFrame out;
                for(uint i = 0; i < (uint)framesCount; i++)
                {
                    currentFrames.read(out);
                    if(wndMain.channel01.FFTOnOff)
                    {
                        uint r = COLOR_R(render.colorChannel0);
                        uint g = COLOR_G(render.colorChannel0);
                        uint b = COLOR_B(render.colorChannel0);
                        renderer.renderFFT(threadId, threadData, measure, fft, out, false, z, 0, COLOR_ARGB(25, r, g, b));
                    }
                    if(wndMain.channel02.FFTOnOff)
                    {
                        uint r = COLOR_R(render.colorChannel1);
                        uint g = COLOR_G(render.colorChannel1);
                        uint b = COLOR_B(render.colorChannel1);
                        renderer.renderFFT(threadId, threadData, measure, fft, out, false, z, 1, COLOR_ARGB(25, r, g, b));
                    }
                    if(wndMain.function.FFTOnOff)
                    {
                        uint r = COLOR_R(render.colorFunction);
                        uint g = COLOR_G(render.colorFunction);
                        uint b = COLOR_B(render.colorFunction);
                        renderer.renderFFT(threadId, threadData, measure, fft, out, true, z, 0, COLOR_ARGB(25, r, g, b));
                    }
                    z -= zDelta;
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////////////
        // Mode
        ////////////////////////////////////////////////////////////////////////////////
        pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_FFT2D);
        pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_FFT2D);
        ////////////////////////////////////////////////////////////////////////////////
        // FFT Grid
        ////////////////////////////////////////////////////////////////////////////////
        renderer.renderFFTGrid(threadId, threadData);
        ////////////////////////////////////////////////////////////////////////////////
        // FFT Units
        ////////////////////////////////////////////////////////////////////////////////
        renderer.renderFFTUnits(threadId, threadData);
        ////////////////////////////////////////////////////////////////////////////////
        // FFT Axis
        ////////////////////////////////////////////////////////////////////////////////
        renderer.renderFFTAxis(threadId, threadData);
        ////////////////////////////////////////////////////////////////////////////////
        // FFT
        ////////////////////////////////////////////////////////////////////////////////
        if(wndMain.fftDigital.is(VIEW_SELECT_FFT_2D))
        {
            if(wndMain.channel01.FFTOnOff)
            {
                renderer.renderFFT(threadId, threadData, measure, fft, threadData.frame, false, 0.f, 0, render.colorChannel0);
            }
            if(wndMain.channel02.FFTOnOff)
            {
                renderer.renderFFT(threadId, threadData, measure, fft, threadData.frame, false, 0.f, 1, render.colorChannel1);
            }
            if(wndMain.function.FFTOnOff)
            {
                renderer.renderFFT(threadId, threadData, measure, fft, threadData.frame, true, 0.f, 1, render.colorFunction);
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // slider
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->setMode(threadId, RENDER_MODE_COLOR_SLIDER);
    pCanvas2d->setMode(threadId, RENDER_MODE_COLOR_SLIDER);
    renderer.renderSlider(threadId, threadData);
    ////////////////////////////////////////////////////////////////////////////////
    // end
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->threadEnd(threadId);
    pCanvas2d->threadEnd(threadId);
}

void OsciloscopeManager::renderMain(uint threadId)
{
    ////////////////////////////////////////////////////////////////////////////////
    // setup
    ////////////////////////////////////////////////////////////////////////////////
    grSetRenderTarget(depth, color);
    grClearDepthStencil(depth, 0, 1.f, 0);
    ////////////////////////////////////////////////////////////////////////////////
    // clear
    ////////////////////////////////////////////////////////////////////////////////
    Flag64 flags = pRender->GetFlag(threadId);
    if(flags.is(rfClearRenderTarget))
    {
        grClearRenderTarget(0, renderData.colorBackground);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // begin
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->mainBegin(threadId);
    pCanvas2d->mainBegin(threadId);
    ////////////////////////////////////////////////////////////////////////////////
    // depth stenci
    ////////////////////////////////////////////////////////////////////////////////
    grStateDepthStencil(DEPTH_STENCIL_OFF);
    grStateRasterizer(RASTERIZER_CULLNONE);
    ////////////////////////////////////////////////////////////////////////////////
    // frame [0-1]
    ////////////////////////////////////////////////////////////////////////////////
    int shadowFrame0 =  pTimer->getFrame(0) % 2;
    int shadowFrame1 = (pTimer->getFrame(0) + 1) % 2;
    if(window.horizontal.Mode == SIGNAL_MODE_PAUSE)
    {
        shadowFrame0 = 0;
        shadowFrame1 = 1;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // clear
    ////////////////////////////////////////////////////////////////////////////////
    if((!window.thermal.enabled || clearThermal) && grGetMode() == OPENGL_MODE_32)
    {
        clearThermal = 0;
        grSetRenderTarget(depth, aShadow[0]);
        grClearRenderTarget(aShadow[0], 0x00000000);
        grSetRenderTarget(depth, aShadow[1]);
        grClearRenderTarget(aShadow[1], 0x00000000);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // width, height
    ////////////////////////////////////////////////////////////////////////////////
    float width = (float)pRender->width;
    float height = (float)pRender->height;
    ////////////////////////////////////////////////////////////////////////////////
    // setup - heating
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        grSetRenderTarget(0, aShadow[shadowFrame0]);
        setAnalogViewport(width, height, analogWindowSize);
        ////////////////////////////////////////////////////////////////////////////////
        // render - heating
        ////////////////////////////////////////////////////////////////////////////////
        if(window.horizontal.Mode != SIGNAL_MODE_PAUSE && window.thermal.enabled)
        {
            pCanvas3d->render(threadId, RENDER_MODE_THERMAL);
            pCanvas2d->render(threadId, RENDER_MODE_THERMAL);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // copy
    ////////////////////////////////////////////////////////////////////////////////
    if(window.horizontal.Mode != SIGNAL_MODE_PAUSE)
    {
        grViewport(0, 0, width, height, 0.f, 1.f);
        grCopyRenderTarget(aShadow[shadowFrame1], aShadow[shadowFrame0]);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // cooling
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        grSetRenderTarget(depth, aShadow[shadowFrame1]);
        if(window.horizontal.Mode != SIGNAL_MODE_PAUSE && window.thermal.enabled)
        {
            pCanvas2d->render(threadId, RENDER_MODE_COLOR_COOLING);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // setup - color
    ////////////////////////////////////////////////////////////////////////////////
    grSetRenderTarget(depth, color);
    grViewport(0, 0, width, height, 0.f, 1.f);
    ////////////////////////////////////////////////////////////////////////////////
    // blend in heating
    ////////////////////////////////////////////////////////////////////////////////
    if(grGetMode() == OPENGL_MODE_32)
    {
        pCanvas2d->render(threadId, RENDER_MODE_COLOR_BLEND_IN_HEATING);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // depth test on / off
    ////////////////////////////////////////////////////////////////////////////////
    if(window.display.depthTest3d)
    {
        grStateDepthStencil(DEPTH_ON);
    }
    ////////////////////////////////////////////////////////////////////////////////
    // setup - analog 3d
    ////////////////////////////////////////////////////////////////////////////////
    setAnalogViewport(width, height, analogWindowSize);
    pCanvas3d->dirLight = Vector4(-cameraOsc.ortho.View.RowZ().x, -cameraOsc.ortho.View.RowZ().y, -cameraOsc.ortho.View.RowZ().z, 0);
    ////////////////////////////////////////////////////////////////////////////////
    // render - analog 3d
    ////////////////////////////////////////////////////////////////////////////////
    grStateRasterizer(RASTERIZER_CULLNONE);
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_ANALOG_3D);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_ANALOG_3D);
    ////////////////////////////////////////////////////////////////////////////////
    // cull off
    ////////////////////////////////////////////////////////////////////////////////
    grStateRasterizer(RASTERIZER_CULLNONE);
    pCanvas3d->dirLight = Vector4(0, 0.f, 1.f, 0.f);
    ////////////////////////////////////////////////////////////////////////////////
    // render - analog 3d
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_ANALOG_2D);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_ANALOG_2D);
    ////////////////////////////////////////////////////////////////////////////////
    // setup - measure
    ////////////////////////////////////////////////////////////////////////////////
    grStateDepthStencil(DEPTH_ON);
    ////////////////////////////////////////////////////////////////////////////////
    // render - measure
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_MEASURE);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_MEASURE);
    ////////////////////////////////////////////////////////////////////////////////
    // setup - digital
    ////////////////////////////////////////////////////////////////////////////////
    grStateDepthStencil(DEPTH_STENCIL_OFF);
    if(window.display.depthTest3d)
    {
        grStateDepthStencil(DEPTH_ON);
    }
    setFFTViewport(width, height, analogWindowSize);
    pCanvas3d->dirLight = Vector4(0, -1.f, 0.f, 0.f);
    ////////////////////////////////////////////////////////////////////////////////
    // render - digital
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_DIGITAL);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_DIGITAL);
    ////////////////////////////////////////////////////////////////////////////////
    // render - FFT3D
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_FFT3D);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_FFT3D);
    ////////////////////////////////////////////////////////////////////////////////
    // setup - FFT2D
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->dirLight = Vector4(0, 0.f, 1.f, 0.f);
    ////////////////////////////////////////////////////////////////////////////////
    // render - FFT2D
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_FFT2D);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_FFT2D);
    ////////////////////////////////////////////////////////////////////////////////
    // setup - slider
    ////////////////////////////////////////////////////////////////////////////////
    grViewport(0, 0, (float)width, (float)height, -1.f, 1.f);
    ////////////////////////////////////////////////////////////////////////////////
    // render - slider
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->render(threadId, RENDER_MODE_COLOR_SLIDER);
    pCanvas2d->render(threadId, RENDER_MODE_COLOR_SLIDER);
    ////////////////////////////////////////////////////////////////////////////////
    // end
    ////////////////////////////////////////////////////////////////////////////////
    pCanvas3d->mainEnd(threadId);
    pCanvas2d->mainEnd(threadId);
    ////////////////////////////////////////////////////////////////////////////////
    // output to screen
    ////////////////////////////////////////////////////////////////////////////////
    grCopyToBackBuffer(color, 0, 0, pRender->width, pRender->height);
    int ret = SDL_GL_SetSwapInterval(0);
    SDL_GL_SwapWindow(sdlWindow);
}

int OsciloscopeManager::stop()
{
    //////////////////////////////////////////////////
    // cleanup
    //////////////////////////////////////////////////
    pRender->stop();
    pCanvas2d->stop();
    pCanvas3d->stop();
    pFont->stop();
    SDL_DestroyWindow(pOsciloscope->sdlWindow);
    deallocate();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// uploadFirmware
//
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//
// callbacks
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeManager::onApplicationInit()
{
    pOsciloscope->setThreadPriority(THREAD_ID_MAIN);
    int renderWidth  = pRender->width;
    int renderHeight = pRender->height;
    int ret = SDL_GL_SetSwapInterval(0);
    color = grCreateRenderTarget(RENDER_TARGET_TYPE_RGBA,  renderWidth, renderHeight);
    if(settings.getSettings()->renderDepthBuffer)
    {
        depth = grCreateDepthStencil(DEPTH_STENCIL_TYPE_Z32, renderWidth, renderHeight);
    }
    else
    {
        depth = 0;
    }
    for(int i = 0; i < 2; i++)
    {
        aShadow[i] = grCreateRenderTarget(RENDER_TARGET_TYPE_FLOAT32, renderWidth, renderHeight);
    }
}

int SDLCALL EventFilter(void* userdata, SDL_Event* event)
{
    pInput->onEvent(*event);
    if(event->type == SDL_SYSWMEVENT)
    {
        return 1;
    }
    return 0;
}

int SDLCALL RenderThreadFunction(void* data)
{
    return 0;
    pOsciloscope->setThreadPriority(THREAD_ID_RENDER);
    //////////////////////////////////////////////////
    // timer
    //////////////////////////////////////////////////
    pTimer->init(TIMER_MAIN);
    pTimer->init(TIMER_RENDER);
    pTimer->init(TIMER_RENDER_THREAD);
    while(SDL_AtomicCAS(&pOsciloscope->contextCreated, 0, 1) == SDL_FALSE) {};
    //////////////////////////////////////////////////
    // main loop
    //////////////////////////////////////////////////
    while(pOsciloscope->renderThreadActive)
    {
        pOsciloscope->onApplicationIdle();
    }
    return 0;
}

bool OsciloscopeManager::onApplicationIdle()
{
    // render
    pTimer->deltaTime(TIMER_RENDER_THREAD);
    dtRender += pTimer->getDelta(TIMER_RENDER_THREAD);
    // fps
    uint maxFps = 0;
    switch(window.speed)
    {
        case USB_SPEED_AUTOMATIC:
            {
                double maxFrameTime = window.horizontal.Capture * double(NUM_SAMPLES);
                double maxFrameFps  = 1.0 / maxFrameTime;
                maxFps = settings.getSettings()->speedHigh;
                if(maxFrameFps <= double(settings.getSettings()->speedMedium))
                {
                    maxFps = settings.getSettings()->speedMedium;
                }
                if(maxFrameFps <= double(settings.getSettings()->speedLow))
                {
                    maxFps = settings.getSettings()->speedLow;
                }
            }
            break;
        case USB_SPEED_LOW:
            maxFps = settings.getSettings()->speedLow;
            break;
        case USB_SPEED_MEDIUM:
            maxFps = settings.getSettings()->speedMedium;
            break;
        case USB_SPEED_HIGH:
            maxFps = settings.getSettings()->speedHigh;
            break;
    };
    double        maxDelta = 1.0 / double(maxFps);
    if(dtRender >= maxDelta)
    {
        dtRender = 0.0;
        SDL_AtomicLock(&renderLock);
        // sync capture - > user interface
        window.horizontal.uiActive = renderWindow.horizontal.uiActive;
        window.horizontal.uiRange  = renderWindow.horizontal.uiRange;
        window.horizontal.uiValue  = renderWindow.horizontal.uiValue;
        if(signalMode != SIGNAL_MODE_PAUSE)
        {
            window.horizontal.Frame = renderWindow.horizontal.Frame;
        }
        // sync user interface -> capture
        renderWindow = window;
        renderData.cameraFFT = cameraFFT.ortho;
        renderData.cameraOsc = cameraOsc.ortho;
        renderData.width = pRender->width;
        renderData.height = pRender->height;
        renderData.sliderMode = sliderMode;
        renderData.sliderSize = analogWindowSize;
        renderData.fftScaleX = fftScaleX;
        renderData.fftScaleY = fftScaleY;
        renderData.oscScaleX = oscScaleX;
        renderData.oscScaleY = oscScaleY;
        renderData.zoomFFT = cameraFFT.zoom;
        renderData.zoomOsc = cameraOsc.zoom;
        renderData.signalPosition = signalPosition;
        renderData.signalZoom     = signalZoom;
        renderData.sliderPosition = sliderPosition;
        renderData.maxEts         = settings.getHardware()->fpgaEtsCount;
        renderData.shadowTexture  = aShadow[pTimer->getFrame(0) % 2];
        renderData.shadowLine3dShader = shadowLine3dShader;
        renderData.shadowColorShader = shadowColorShader;
        renderData.shadowCoolingShader = shadowCoolingShader;
        renderData.analogChannelYVoltageStep0 = settings.getHardware()->getAnalogStep(window.horizontal.Capture, 0, window.channel01.Capture);
        renderData.analogChannelYVoltageStep1 = settings.getHardware()->getAnalogStep(window.horizontal.Capture, 1, window.channel02.Capture);
        renderData.analogChannelOffsets0 = settings.getHardware()->getAnalogOffsetDouble(window.horizontal.Capture, 0, window.channel01.Capture);
        renderData.analogChannelOffsets1 = settings.getHardware()->getAnalogOffsetDouble(window.horizontal.Capture, 1, window.channel02.Capture);
        renderData.analogChannelPositin0 = control.getYPositionA();
        renderData.analogChannelPositin1 = control.getYPositionB();
        renderData.colorBackground = settings.getColors()->renderBackground;
        renderData.colorTime       = settings.getColors()->renderTime;
        renderData.colorChannel0   = settings.getColors()->renderChannel0;
        renderData.colorChannel1   = settings.getColors()->renderChannel1;
        renderData.colorFunction   = settings.getColors()->renderFunction;
        renderData.colorXyGraph    = settings.getColors()->renderXyGraph;
        renderData.colorGrid         = settings.getColors()->renderGrid;
        renderData.colorBorder       = settings.getColors()->renderBorder;
        renderData.colorTrigger      = settings.getColors()->renderTrigger;
        renderData.colorDigital      = settings.getColors()->renderDigital;
        renderData.etsAttr           = 0;
        renderData.flags.bit(rfClearRenderTarget,clearRenderTarget);
        SDL_AtomicUnlock(&renderLock);
        // lock
        uint renderId = 0;
        bool ret = false;
        while(!ret)
        {
            ret = pOsciloscope->threadLoop.update.consumerLock(renderId, false);
            if(ret)
            {
                pTimer->deltaTime(TIMER_RENDER);
                // render
                renderMain(renderId);
                if( SDL_AtomicGet(&pOsciloscope->captureBuffer->drawState) == DRAWSTATE_FILL)
                  SDL_AtomicSet(&pOsciloscope->captureBuffer->drawState, DRAWSTATE_DRAW);

                // measure
                window.measure.data.pick = measureData[renderId].pick;
                window.measure.data.history[MEASURE_HISTORY_CURRENT] = measureData[renderId].history[MEASURE_HISTORY_CURRENT];
                window.measure.data.history[MEASURE_HISTORY_MINIMUM].Minimum(measureData[renderId].history[MEASURE_HISTORY_CURRENT]);
                window.measure.data.history[MEASURE_HISTORY_MAXIMUM].Maximum(measureData[renderId].history[MEASURE_HISTORY_CURRENT]);
                window.measure.data.history[MEASURE_HISTORY_AVERAGE].Average(measureData[renderId].history[MEASURE_HISTORY_CURRENT]);
                // unlock
                pOsciloscope->threadLoop.update.consumerUnlock(renderId);
                return true;
            }
            else
            {
                if(settings.getSettings()->delayRender > 0)
                {
                    SDL_Delay(settings.getSettings()->delayRender);
                }
            }
        }
    }
    else
    {
        double diffMiliSeconds = (maxDelta - dtRender) * 1000.0;
        SDL_Delay(uint(diffMiliSeconds));
    }
    return false;
}

void OsciloscopeManager::onResize(int width, int height, int oldWidth, int oldHeight)
{
    grResizeRenderTarget(color,  width, height);
    if(settings.getSettings()->renderDepthBuffer)
    {
        grResizeDepthStencil(depth, width, height);
    }
    for(int i = 0; i < 2; i++)
    {
        grResizeRenderTarget(aShadow[i], width, height);
    }
    float oldAspect = float(oldWidth) / float(oldHeight);
    float newAspect = float(width)    / float(height);
    analogWindowSize = analogWindowSize * (oldAspect / newAspect);
}

void maximizeOglWindow(int& w, int& h);

int OsciloscopeManager::onEvent(SDL_Event& event)
{
    if(event.window.type == SDL_WINDOWEVENT)
    {
        if(event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
        {
        }
        if(event.window.event == SDL_WINDOWEVENT_RESTORED)
        {
        }
        if(event.window.event == SDL_WINDOWEVENT_ENTER)
        {
            openglFocus = 1;
            sliderMode  = 0;
        }
        if(event.window.event == SDL_WINDOWEVENT_LEAVE)
        {
            openglFocus = 0;
            sliderMode  = 0;
        }
    }
    return 0;
}

void OsciloscopeManager::onCallibrateFrameCaptured(OsciloscopeFrame& frame, int version)
{
    if(callibrate.active)
    {
        callibrate.frame++;
        int framesPerCapture = 0;
        switch(callibrate.mode)
        {
            case acOffsetCapture:
            case acGainCapture:
            case acGeneratorCapture:
            case acStepCapture:
                framesPerCapture = settings.getHardware()->referenceFramesPerCapture;
                break;
            default:
                callibrate.frame = 0;
        };
        if(callibrate.frame >= framesPerCapture)
        {
            switch(callibrate.mode)
            {
                case acStartMessageBox:
                    {
                        if(callibrate.messageBox == acmbOk)
                        {
                            callibrate.mode    = acOffsetStart;
                            callibrate.type    = ctNormal;
                            callibrate.channel = 0;
                            callibrate.debug << "acStartMessageBox: ok \n";
                        }
                        if(callibrate.messageBox == acmbCancel)
                        {
                            callibrate.active  = 0;
                            callibrate.resetUI = true;
                            callibrate.debug << "acStartMessageBox: cancel \n";
                        }
                    }
                    break;
                case acOffsetStart:
                    {
                        callibrate.voltage = vc2Volt;
                        callibrate.offsetMax = settings.getHardware()->referenceOffsetMaxValue;
                        callibrate.offsetMin = settings.getHardware()->referenceOffsetMinValue;
                        callibrate.iteration = 0;
                        callibrate.mode   = acOffsetSetup;
                        callibrate.debug << "acOffsetStart";
                        callibrate.debug << "\n";
                    }
                    break;
                case acOffsetSetup:
                    {
                        callibrate.offset = callibrate.offsetMin + (callibrate.offsetMax - callibrate.offsetMin) / 2.0;
                        control.setYRangeScaleA(settings.getHardware()->referenceGainValue[callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                        control.setYRangeScaleB(settings.getHardware()->referenceGainValue[callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                        control.setYPositionA(callibrate.offset);
                        control.setYPositionB(callibrate.offset);
                        control.setAnalogSwitchBit(CHANNEL_A_GROUND, 1);
                        control.setAnalogSwitchBit(CHANNEL_B_GROUND, 1);
                        control.transferData();
                        callibrate.mode = acOffsetCapture;
                        callibrate.debug << "acOffsetSetup:";
                        callibrate.debug << "  setYPositionAB: ";
                        callibrate.debug << callibrate.offset;
                        callibrate.debug << "\n";
                        callibrate.debug << "  offsetMin: ";
                        callibrate.debug << callibrate.offsetMin;
                        callibrate.debug << "\n";
                        callibrate.debug << "  offsetMax: ";
                        callibrate.debug << callibrate.offsetMax;
                        callibrate.debug << "\n";
                    }
                    break;
                case acOffsetCapture:
                    {
                        int ch = callibrate.channel;
                        callibrate.offsetMeasured = 0;
                        for(int sample = 0; sample < frame.analog[ch].getCount(); sample++)
                        {
                            callibrate.offsetMeasured += frame.getAnalogDouble(ch, sample);
                        }
                        callibrate.offsetMeasured /= double(frame.analog[ch].getCount());
                        callibrate.mode = acOffsetCalculate;
                        callibrate.debug << "acOffsetCapture:";
                        callibrate.debug << "  offsetMeasured: ";
                        callibrate.debug << callibrate.offsetMeasured;
                        callibrate.debug << "\n";
                    }
                    break;
                case acOffsetCalculate:
                    {
                        double range = fabsf(callibrate.offsetMax - callibrate.offsetMin) / 5.0;
                        callibrate.debug << "acOffsetCalculate:";
                        if( ( fabs(callibrate.offsetMeasured) < 0.00025) || (callibrate.iteration >= settings.getHardware()->referenceMaxIterations) || (range < 0.001) )
                        {
                            callibrate.debug << "  callibratedOffset: ";
                            callibrate.debug << double(callibrate.offset);
                            callibrate.debug << "\n";
                            callibrate.debug << "  callibratedIteration: ";
                            callibrate.debug << callibrate.iteration;
                            callibrate.debug << "\n";
                            settings.getHardware()->callibratedOffsets[callibrate.type][callibrate.channel][callibrate.voltage] = round(callibrate.offset);
                            callibrate.mode = acOffsetVoltageChange;
                        }
                        else
                        {
                            callibrate.debug << "  OffsetStep: ";
                            callibrate.debug << range;
                            callibrate.debug << "\n";
                            if(callibrate.offsetMeasured > 0.f)
                            {
                                callibrate.offsetMin += range;
                            }
                            else
                            {
                                callibrate.offsetMax -= range;
                            }
                            callibrate.iteration++;
                            callibrate.mode = acOffsetSetup;
                        }
                    }
                    break;
                case acOffsetVoltageChange:
                    {
                        callibrate.offsetMax = settings.getHardware()->referenceOffsetMaxValue;
                        callibrate.offsetMin = settings.getHardware()->referenceOffsetMinValue;
                        if(callibrate.voltage == vc10Mili)
                        {
                            if(callibrate.channel == 0)
                            {
                                callibrate.channel = 1;
                                callibrate.mode = acOffsetStart;
                            }
                            else
                            {
                                if(callibrate.type == ctNormal)
                                {
                                    callibrate.channel = 0;
                                    control.setAnalogSwitchBit(CHANNEL_INTERLEAVE, 1);
                                    callibrate.type = ct500Mhz;
                                    callibrate.mode = acOffsetStart;
                                }
                                else // 500 Mhz
                                {
                                    callibrate.channel = 0;
                                    callibrate.mode    = acOffsetEnd;
                                }
                            }
                        }
                        else
                        {
                            callibrate.iteration = 0;
                            callibrate.voltage   = (VoltageCapture)((int)callibrate.voltage + 1);
                            callibrate.offsetMax = settings.getHardware()->referenceOffsetMaxValue;
                            callibrate.offsetMin = settings.getHardware()->referenceOffsetMinValue;
                            callibrate.mode = acOffsetSetup;
                        }
                        callibrate.debug << "acOffsetVoltageChange:";
                        callibrate.debug << "  voltage: ";
                        callibrate.debug << callibrate.voltage;
                        callibrate.debug << "\n";
                    }
                    break;
                case acOffsetEnd:
                    {
                        control.setAnalogSwitchBit(CHANNEL_INTERLEAVE, 0);
                        callibrate.type = ctNormal;
                        callibrate.messageBox = acmbChannel00;
                        callibrate.mode = acGeneratorMessageBox;
                        callibrate.debug << "acOffsetEnd: \n";
                    }
                    break;
                case acGeneratorMessageBox:
                    {
                        if(callibrate.messageBox == acmbOk)
                        {
                            callibrate.mode = acGeneratorStart;
                            callibrate.debug << "acGeneratorMessageBox: ok \n";
                        }
                        if(callibrate.messageBox == acmbCancel)
                        {
                            callibrate.active  = 0;
                            callibrate.resetUI = true;
                            callibrate.debug << "acGeneratorMessageBox: cancel \n";
                        }
                    }
                    break;
                case acGeneratorStart:
                    {
                        callibrate.gainMode = 0;
                        callibrate.voltage = vc20Mili;
                        window.hardwareGenerator.type0 = GENERATOR_DC;
                        window.hardwareGenerator.type1 = GENERATOR_DC;
                        window.hardwareGenerator.onOff0 = 1;
                        window.hardwareGenerator.onOff1 = 1;
                        control.setGeneratorType0(GENERATOR_DC);
                        control.setGeneratorType1(GENERATOR_DC);
                        control.setGeneratorOn0(1);
                        control.setGeneratorOn1(1);
                        control.setGeneratorOffset0(0);
                        control.setGeneratorOffset1(0);
                        window.channel01.Ground = 0;
                        window.channel02.Ground = 0;
                        control.setAnalogSwitchBit(CHANNEL_A_GROUND, 0);
                        control.setAnalogSwitchBit(CHANNEL_B_GROUND, 0);
                        window.channel01.AcDc = 1;
                        window.channel02.AcDc = 1;
                        control.setAnalogSwitchBit(CHANNEL_A_ACDC, 1);
                        control.setAnalogSwitchBit(CHANNEL_B_ACDC, 1);
                        control.setYPositionA(window.channel01.YPosition + settings.getHardware()->callibratedOffsets[callibrate.type][0][callibrate.voltage]);
                        control.setYPositionB(window.channel02.YPosition + settings.getHardware()->callibratedOffsets[callibrate.type][1][callibrate.voltage]);
                        control.setYRangeScaleA(settings.getHardware()->referenceGainValue[callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                        control.setYRangeScaleB(settings.getHardware()->referenceGainValue[callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                        control.transferData();
                        callibrate.iteration    = 0;
                        callibrate.generatorMax = settings.getHardware()->referenceGeneratorMaxValue;
                        callibrate.generatorMin = settings.getHardware()->referenceGeneratorMinValue;
                        callibrate.mode = acGeneratorSetup;
                        callibrate.debug << "acGeneratorStart: \n";
                        callibrate.debug << "  generatorMin: ";
                        callibrate.debug << callibrate.generatorMin;
                        callibrate.debug << "\n";
                        callibrate.debug << "  generatorMax: ";
                        callibrate.debug << callibrate.generatorMax;
                        callibrate.debug << "\n";
                    }
                    break;
                case acGeneratorSetup:
                    {
                        callibrate.generatorOffset = callibrate.generatorMin + (callibrate.generatorMax - callibrate.generatorMin) / 2.0;
                        control.setYPositionA(settings.getHardware()->callibratedOffsets[callibrate.type][0][callibrate.voltage]);
                        control.setYPositionB(settings.getHardware()->callibratedOffsets[callibrate.type][1][callibrate.voltage]);
                        control.setGeneratorOffset0(callibrate.generatorOffset);
                        control.setGeneratorOffset1(callibrate.generatorOffset);
                        control.transferData();
                        callibrate.mode = acGeneratorCapture;
                        callibrate.debug << "acGeneratorSetup: \n";
                        callibrate.debug << "  generatorOffset: ";
                        callibrate.debug << callibrate.generatorOffset;
                        callibrate.debug << "\n";
                    }
                    break;
                case acGeneratorCapture:
                    {
                        int ch = 0;
                        callibrate.generatorOffset = 0;
                        for(int sample = 0; sample < frame.analog[ch].getCount(); sample++)
                        {
                            callibrate.generatorOffset += frame.getAnalogShort(ch, sample);
                        }
                        callibrate.generatorOffset /= double(frame.analog[ch].getCount());
                        callibrate.mode = acGeneratorCalculate;
                        callibrate.debug << "acGeneratorCapture: \n";
                        callibrate.debug << "  generatorOffset: ";
                        callibrate.debug << callibrate.generatorOffset;
                        callibrate.debug << "\n";
                    }
                    break;
                case acGeneratorCalculate:
                    {
                        double reference  = (settings.getHardware()->referenceGeneratorMaxValue - settings.getHardware()->referenceGeneratorMinValue) / 2.0;
                        double range      = (callibrate.generatorMax - callibrate.generatorMin) / 2.0;
                        double percentage = 100.0 * (range / reference);
                        bool         exit = (callibrate.offsetMin > callibrate.offsetMax);
                        if(exit || abs(percentage) < settings.getHardware()->referenceExitPercentage || callibrate.iteration >= settings.getHardware()->referenceMaxIterations)
                        {
                            settings.getHardware()->callibratedOffsetsGenerator[ctNormal][callibrate.channel] = (callibrate.generatorMin + callibrate.generatorMax) / 2.0;
                            settings.getHardware()->callibratedOffsetsGenerator[ct500Mhz][callibrate.channel] = (callibrate.generatorMin + callibrate.generatorMax) / 2.0;
                            callibrate.mode = acGeneratorEnd;
                        }
                        else
                        {
                            if(callibrate.generatorOffset < 0.f)
                            {
                                callibrate.generatorMin += range;
                            }
                            else
                            {
                                callibrate.generatorMax -= range;
                            }
                            callibrate.iteration++;
                            callibrate.mode = acGeneratorSetup;
                        }
                        callibrate.debug << "acGeneratorCalculate: \n";
                        callibrate.debug << "  percentage: ";
                        callibrate.debug << percentage;
                        callibrate.debug << "\n";
                    }
                    break;
                case acGeneratorEnd:
                    {
                        if(callibrate.channel == 0)
                        {
                            callibrate.iteration  = 0;
                            callibrate.channel    = 1;
                            callibrate.voltage    = vc2Volt;
                            callibrate.mode       = acGeneratorMessageBox;
                            callibrate.messageBox = acmbChannel10;
                            callibrate.debug << "acGeneratorEnd: channel 1 \n";
                        }
                        else
                        {
                            callibrate.iteration  = 0;
                            callibrate.voltage    = vc2Volt;
                            callibrate.mode       = acGainMsgBoxCh0;
                            callibrate.messageBox = acmbChannel00;
                            callibrate.debug << "acGeneratorEnd: gain \n";
                        }
                    }
                    break;
                case acGainMsgBoxCh0:
                    {
                        if(callibrate.messageBox == acmbOk)
                        {
                            callibrate.channel = 0;
                            callibrate.mode    = acGainStart;
                            callibrate.debug << "acGainMsgBoxCh0: ok \n";
                        }
                        if(callibrate.messageBox == acmbCancel)
                        {
                            callibrate.active  = 0;
                            callibrate.resetUI = true;
                            callibrate.debug << "acGainMsgBoxCh0: cancel \n";
                        }
                    }
                    break;
                case acGainStart:
                    {
                        callibrate.voltage   = vc2Volt;
                        callibrate.iteration = 0;
                        callibrate.mode = acGainSetup;
                        callibrate.gainMax   = settings.getHardware()->referenceGainMax[callibrate.voltage];
                        callibrate.gainMin   = settings.getHardware()->referenceGainMin[callibrate.voltage];
                        callibrate.gainSet        = callibrate.gainMin + (callibrate.gainMax - callibrate.gainMin) / 2;
                        callibrate.gainSetPrevius = callibrate.gainMax;
                        callibrate.debug << "acGainStart: \n";
                        callibrate.debug << "  gainMin: ";
                        callibrate.debug << callibrate.gainMin;
                        callibrate.debug << "\n";
                        callibrate.debug << "  gainMax: ";
                        callibrate.debug << callibrate.gainMax;
                        callibrate.debug << "\n";
                    }
                    break;
                case acGainSetup:
                    {
                        double gainGenOffset = settings.getHardware()->referenceGainVoltage[callibrate.voltage] / settings.getHardware()->referenceGeneratorVoltagePerStep;
                        if(callibrate.type == ct500Mhz)
                        {
                            control.setXRange(0);
                            control.setAnalogSwitchBit(CHANNEL_INTERLEAVE, 1);
                        }
                        else
                        {
                            control.setXRange(5);
                            control.setAnalogSwitchBit(CHANNEL_INTERLEAVE, 0);
                        }
                        control.setYPositionA(settings.getHardware()->callibratedOffsets[callibrate.type][0][callibrate.voltage]);
                        control.setYPositionB(settings.getHardware()->callibratedOffsets[callibrate.type][1][callibrate.voltage]);
                        control.setGeneratorOffset0(gainGenOffset + settings.getHardware()->callibratedOffsetsGenerator[callibrate.type][0]);
                        control.setGeneratorOffset1(gainGenOffset + settings.getHardware()->callibratedOffsetsGenerator[callibrate.type][1]);
                        control.setYRangeScaleA(callibrate.gainSet, (uint)control.getAttr(callibrate.voltage));
                        control.setYRangeScaleB(callibrate.gainSet, (uint)control.getAttr(callibrate.voltage));
                        control.transferData();
                        callibrate.mode = acGainCapture;
                        callibrate.debug << "acGainSetup: ";
                        callibrate.debug << (int)callibrate.type;
                        callibrate.debug << "\n";
                        callibrate.debug << "  gainSet: ";
                        callibrate.debug << callibrate.gainSet;
                        callibrate.debug << "\n";
                    }
                    break;
                case acGainCapture:
                    {
                        int          ch = callibrate.channel;
                        double yGridMax = double(grid.yCount) / 2.0;
                        callibrate.gainVoltageUp = 0;
                        for(int sample = 0; sample < frame.analog[ch].getCount(); sample++)
                        {
                            callibrate.gainVoltageUp += frame.getAnalog(ch, sample);
                        }
                        callibrate.gainVoltageUp /= double(frame.analog[ch].getCount());
                        callibrate.gainVoltageUp *= yGridMax * double(captureVoltFromEnum(callibrate.voltage));
                        callibrate.mode = acGainCalculate;
                        callibrate.debug << "acGainCapture: ";
                        callibrate.debug <<    callibrate.gainVoltageUp;
                        callibrate.debug << "\n";
                    }
                    break;
                case acGainCalculate:
                    {
                        double               up = callibrate.gainVoltageUp;
                        double ref            = settings.getHardware()->referenceGainVoltage[callibrate.voltage];
                        callibrate.percentage = 100.0 * (up / ref);
                        if(fabsf(callibrate.percentage - 100.0) < settings.getHardware()->referenceExitPercentage || callibrate.iteration >= settings.getHardware()->referenceMaxIterations)
                        {
                            settings.getHardware()->callibratedGainValue[callibrate.type][callibrate.channel][callibrate.voltage] = callibrate.gainSet;
                            callibrate.mode = acGainVoltageChange;
                        }
                        else
                        {
                            callibrate.iteration++;
                            int gain = callibrate.gainSetPrevius;
                            callibrate.gainSetPrevius = callibrate.gainSet;
                            int delta = abs(gain - callibrate.gainSet) / 2;
                                delta = max(1, (int)abs(callibrate.percentage - 100));
                            if(callibrate.percentage > 100.f)
                            {
                                callibrate.gainSet -= delta;
                            }
                            else
                            {
                                callibrate.gainSet += delta;
                            }
                            callibrate.mode = acGainSetup;
                        }
                        callibrate.debug << "acGainCalculate: ";
                        callibrate.debug << "  percentage: ";
                        callibrate.debug <<     callibrate.percentage;
                        callibrate.debug << "\n";
                        callibrate.debug << "  iteration: ";
                        callibrate.debug <<     callibrate.iteration;
                        callibrate.debug << "\n";
                    }
                    break;
                case acGainVoltageChange:
                    {
                        callibrate.iteration = 0;
                        callibrate.gainMax        = settings.getHardware()->referenceGainMax[callibrate.voltage];
                        callibrate.gainMin        = settings.getHardware()->referenceGainMin[callibrate.voltage];
                        callibrate.gainSet        = callibrate.gainMin + (callibrate.gainMax - callibrate.gainMin) / 2;
                        callibrate.gainSetPrevius = callibrate.gainMax;
                        if(callibrate.voltage == vc10Mili)
                        {
                            if(callibrate.gainMode == 0)
                            {
                                callibrate.voltage = vc2Volt;
                                callibrate.type    = ct500Mhz;
                                callibrate.mode    = acGainSetup;
                            }
                            if(callibrate.gainMode == 1)
                            {
                                callibrate.type       = ctNormal;
                                callibrate.voltage    = vc2Volt;
                                callibrate.messageBox = acmbChannel01;
                                callibrate.mode       = acGainMsgBoxCh1;
                                callibrate.channel    = 1;
                            }
                            if(callibrate.gainMode == 2)
                            {
                                callibrate.type       = ct500Mhz;
                                callibrate.voltage    = vc2Volt;
                                callibrate.messageBox = acmbChannel00;
                                callibrate.mode       = acGainMsgBoxCh1;
                                callibrate.channel    = 1;
                            }
                            if(callibrate.gainMode == 3)
                            {
                                callibrate.messageBox = acmbChannel00;
                                callibrate.channel = 0;
                                callibrate.mode = acStepStart;
                                callibrate.type = ctNormal;
                            }
                            callibrate.gainMode++;
                        }
                        else
                        {
                            callibrate.voltage = (VoltageCapture)((int)callibrate.voltage + 1);
                            callibrate.mode = acGainSetup;
                        }
                        callibrate.debug << "acGainVoltageChange: ";
                        callibrate.debug << "  acGainVoltage: ";
                        callibrate.debug << callibrate.voltage;
                        callibrate.debug << "  gainMin: ";
                        callibrate.debug << callibrate.gainMin;
                        callibrate.debug << "\n";
                        callibrate.debug << "  gainMax: ";
                        callibrate.debug << callibrate.gainMax;
                        callibrate.debug << "\n";
                        callibrate.debug << "  gainSet: ";
                        callibrate.debug << callibrate.gainSet;
                        callibrate.debug << "\n";
                    }
                    break;
                case acGainMsgBoxCh1:
                    {
                        if(callibrate.messageBox == acmbOk)
                        {
                            callibrate.debug << "acGainMsgBoxCh1: ok \n";
                            callibrate.mode    = acGainStart;
                        }
                        if(callibrate.messageBox == acmbCancel)
                        {
                            callibrate.debug << "acGainMsgBoxCh1: cancel \n";
                            callibrate.active  = 0;
                            callibrate.resetUI = true;
                        }
                    }
                    break;
                case acStepStart:
                    {
                        if(callibrate.messageBox == acmbOk)
                        {
                            callibrate.channel = 0;
                            callibrate.voltage = vc2Volt;
                            //callibrate.stepMax = settings.getHardware()->referenceStepMax;
                            //callibrate.stepMin = settings.getHardware()->referenceStepMin;
                            callibrate.stepMax = 1000;
                            callibrate.stepMin = 0;
                            callibrate.mode = acStepSetup;
                            callibrate.debug << "acStepStart: ok \n";
                        }
                        if(callibrate.messageBox == acmbCancel)
                        {
                            callibrate.active = 0;
                            callibrate.resetUI = true;
                            callibrate.debug << "acStepStart: cancel \n";
                        }
                    }
                    break;
                case acStepSetup:
                    {
                        callibrate.stepReference = int(callibrate.stepMin + (callibrate.stepMax - callibrate.stepMin) / 2.0);
                        window.channel01.Ground = 1;
                        window.channel02.Ground = 1;
                        control.setAnalogSwitchBit(CHANNEL_A_GROUND, 1);
                        control.setAnalogSwitchBit(CHANNEL_B_GROUND, 1);
                        if(callibrate.type == ct500Mhz)
                        {
                            control.setXRange(0);
                            control.setAnalogSwitchBit(CHANNEL_INTERLEAVE, 1);
                        }
                        else
                        {
                            control.setXRange(5);
                            control.setAnalogSwitchBit(CHANNEL_INTERLEAVE, 0);
                        }
                        if(callibrate.channel == 0)
                        {
                            control.setYPositionA(callibrate.stepReference + settings.getHardware()->callibratedOffsets[callibrate.type][0][callibrate.voltage]);
                            control.setYRangeScaleA(settings.getHardware()->callibratedGainValue[callibrate.type][0][callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                            control.setYPositionB(settings.getHardware()->callibratedOffsets[callibrate.type][1][callibrate.voltage]);
                            control.setYRangeScaleB(settings.getHardware()->callibratedGainValue[callibrate.type][1][callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                        }
                        else
                        {
                            control.setYPositionA(settings.getHardware()->callibratedOffsets[callibrate.type][0][callibrate.voltage]);
                            control.setYRangeScaleA(settings.getHardware()->callibratedGainValue[callibrate.type][0][callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                            control.setYPositionB(callibrate.stepReference + settings.getHardware()->callibratedOffsets[callibrate.type][1][callibrate.voltage]);
                            control.setYRangeScaleB(settings.getHardware()->callibratedGainValue[callibrate.type][1][callibrate.voltage], (uint)control.getAttr(callibrate.voltage));
                        }
                        control.transferData();
                        callibrate.mode = acStepCapture;
                        callibrate.debug << "acStepSetup: ";
                        callibrate.debug << "  CH: ";
                        callibrate.debug << callibrate.channel;
                        callibrate.debug << "  stepReference: ";
                        callibrate.debug << callibrate.stepReference;
                        callibrate.debug << "\n";
                        SDL_Delay(50);
                    }
                    break;
                case acStepCapture:
                    {
                        int          ch = callibrate.channel;
                        callibrate.stepMeasuredOffsetVoltage = 0;
                        callibrate.stepMeasuredOffsetValue   = 0;
                        for(int sample = 0; sample < frame.analog[ch].getCount(); sample++)
                        {
                            callibrate.stepMeasuredOffsetValue   += frame.getAnalogShort(ch, sample);
                            callibrate.stepMeasuredOffsetVoltage += frame.getAnalog(ch, sample);
                        }
                        callibrate.stepMeasuredOffsetValue   /= double(frame.analog[ch].getCount());
                        callibrate.stepMeasuredOffsetVoltage /= double(frame.analog[ch].getCount());
                        callibrate.mode = acStepCalculate;
                        callibrate.debug << "  stepMeasuredOffsetValue: ";
                        callibrate.debug << callibrate.stepMeasuredOffsetValue;
                        callibrate.debug << "\n";
                        callibrate.debug << "  stepMeasuredOffsetVoltage: ";
                        callibrate.debug << callibrate.stepMeasuredOffsetVoltage;
                        callibrate.debug << "\n";
                    }
                    break;
                case acStepCalculate:
                    {
                        if(callibrate.iteration > settings.getHardware()->referenceMaxIterations || abs(callibrate.stepMeasuredOffsetValue) >= settings.getHardware()->referenceStepMin && abs(callibrate.stepMeasuredOffsetValue) <= settings.getHardware()->referenceStepMax)
                        {
                            double yGridMax      = grid.yCount / 2.0;
                            double dCapture      = double(captureVoltFromEnum(callibrate.voltage));
                            double  iStepVoltage = yGridMax * callibrate.stepMeasuredOffsetVoltage * dCapture;
                            settings.getHardware()->callibratedVoltageStep[callibrate.type][callibrate.channel][callibrate.voltage] = double(iStepVoltage) / callibrate.stepReference;
                            callibrate.mode = acStepVoltageChange;
                            callibrate.debug << "  stepiStepVoltage: ";
                            callibrate.debug << double(iStepVoltage);
                            callibrate.debug << "\n";
                            callibrate.debug << "  callibratedVoltageStep: ";
                            callibrate.debug << (double(iStepVoltage) / callibrate.stepReference) * 100.0;
                            callibrate.debug << "\n";
                        }
                        else
                        {
                            double delta = (callibrate.stepMax - callibrate.stepMin) / 4.0;
                            if(abs(callibrate.stepMeasuredOffsetValue) >= settings.getHardware()->referenceStepMax)
                            {
                                callibrate.stepMax -= delta;
                            }
                            else if(abs(callibrate.stepMeasuredOffsetValue) <= settings.getHardware()->referenceStepMin)
                            {
                                callibrate.stepMin += delta;
                            }
                            callibrate.mode = acStepSetup;
                            callibrate.iteration++;
                            callibrate.debug << "acStepCalculate: ";
                            callibrate.debug << "\n";
                            callibrate.debug << "  stepMin: ";
                            callibrate.debug << callibrate.stepMin;
                            callibrate.debug << "\n";
                            callibrate.debug << "  stepMax: ";
                            callibrate.debug << callibrate.stepMax;
                            callibrate.debug << "\n";
                        }
                    }
                    break;
                case acStepVoltageChange:
                    {
                        callibrate.iteration = 0;
                        callibrate.stepMax = 1000;
                        callibrate.stepMin = 0;
                        if(callibrate.voltage == vc10Mili)
                        {
                            if(callibrate.type == ctNormal)
                            {
                                callibrate.type    = ct500Mhz;
                                callibrate.voltage = vc2Volt;
                                callibrate.mode    = acStepSetup;
                            }
                            else
                            {
                                if(callibrate.channel == 0)
                                {
                                    callibrate.type       = ctNormal;
                                    callibrate.channel    = 1;
                                    callibrate.voltage    = vc2Volt;
                                    callibrate.mode       = acStepMsgBoxCh1;
                                    callibrate.messageBox = acmbChannel01;
                                }
                                else
                                {
                                    callibrate.messageBox = acmbEnd;
                                    callibrate.mode       = acEndMessageBox;
                                }
                            }
                        }
                        else
                        {
                            callibrate.voltage   = (VoltageCapture)((int)callibrate.voltage + 1);
                            callibrate.mode      = acStepSetup;
                        }
                        callibrate.debug << "acStepVoltageChange: ";
                        callibrate.debug << "  voltage: ";
                        callibrate.debug << callibrate.voltage;
                        callibrate.debug << "\n";
                        callibrate.debug << "  stepMin: ";
                        callibrate.debug << callibrate.stepMin;
                        callibrate.debug << "\n";
                        callibrate.debug << "  stepMax: ";
                        callibrate.debug << callibrate.stepMax;
                        callibrate.debug << "\n";
                    }
                    break;
                case acStepMsgBoxCh1:
                    {
                        if(callibrate.messageBox == acmbOk)
                        {
                            callibrate.debug << "acStepMsgBoxCh1: ok \n";
                            callibrate.mode    = acStepSetup;
                        }
                        if(callibrate.messageBox == acmbCancel)
                        {
                            callibrate.active = 0;
                            callibrate.resetUI = true;
                            callibrate.debug << "acStepMsgBoxCh1: cancel \n";
                        }
                    }
                    break;
                case acEndMessageBox:
                    {
                        if(callibrate.messageBox == acmbOk)
                        {
                            callibrate.mode = acSaveSettings;
                            callibrate.debug << "acEndMessageBox: ok \n";
                        }
                        if(callibrate.messageBox == acmbCancel)
                        {
                            callibrate.resetUI = true;
                            callibrate.active  = 0;
                            callibrate.debug << "acEndMessageBox: cancel \n";
                        }
                    }
                    break;
                case acSaveSettings:
                    {
                        pOsciloscope->settings.getHardware()->save();
                        callibrate.mode = acStartMessageBox;
                        callibrate.active = 0;
                        callibrate.debug << "acSaveSettings: \n";
                        FORMAT_BUFFER();
                        FORMAT_PATH("data/callibrate/debug.log");
                        fileSaveString((const char*)formatBuffer, callibrate.debug.get());
                        callibrate.resetUI = true;
                    }
                    break;
            }
        }
        // progress
        window.progress.uiValue = (callibrate.mode + 1);
        window.progress.uiRange = (acEnd - 1);
        window.progress.uiActive = 1;
    }
}

void OsciloscopeManager::AutoCallibrate()
{
    SHardware1 hw1 = pOsciloscope->control.control1.client1Get();
    SHardware2 hw2 = pOsciloscope->control.control2.client2Get();
    thread.hardwareControlFunction(&hw1, &hw2);
    thread.wait();
    callibrate.clear();
    callibrate.active      = 1;
    callibrate.mode        = acStartMessageBox;
    callibrate.voltage     = vc2Volt;
    callibrate.frame       = 0;
    callibrate.generator   = 0;
    callibrate.messageBox  = acmbStart;
    signalMode = SIGNAL_MODE_CAPTURE;
    SSimulate sim = GetServerSim();
    pOsciloscope->transmitSim(sim);
}

double OsciloscopeManager::getTriggerVoltagePerStep()
{
    float  captureVoltage = 0.f;
    if(window.trigger.Source == 0)
    {
        captureVoltage = window.channel01.Capture;
    }
    else
    {
        captureVoltage = window.channel02.Capture;
    }
    return captureVoltage * grid.yCount / 1024.0;
}

double OsciloscopeManager::getTriggerVoltage()
{
    float  captureVoltage = 0.f;
    if(window.trigger.Source == 0)
    {
        captureVoltage = window.channel01.Capture;
    }
    else
    {
        captureVoltage = window.channel02.Capture;
    }
    return captureVoltage;
}

void OsciloscopeManager::setFrame(int frame)
{
}

void OsciloscopeManager::allocate()
{
    // hardware capture buffer
    pOsciloscope->sizeHardwareCapture = settings.getSettings()->memoryFrame * MEGABYTE;
    pOsciloscope->ptrHardwareCapture  = (byte*)pMemory->allocate(pOsciloscope->sizeHardwareCapture);
    SDL_memset(pOsciloscope->ptrHardwareCapture, 0, pOsciloscope->sizeHardwareCapture);
    // allocate capture buffer
    byte* display = (byte*)pMemory->allocate(SCOPEFUN_FRAME_MEMORY);
    byte* rld = 0;
    if(settings.getSettings()->memoryRld)
    {
        rld = (byte*)pMemory->allocate(settings.getSettings()->memoryRld * MEGABYTE);
    }
    captureBuffer = new CaptureBuffer(display, SCOPEFUN_FRAME_MEMORY, rld, settings.getSettings()->memoryRld * MEGABYTE);
    // display
    uint toAllocateDisplay = settings.getSettings()->historyFrameDisplay * sizeof(OsciloscopeFrame);
    pTmpData = (OsciloscopeFrame*)pMemory->allocate(toAllocateDisplay);
    tmpHistory.init((OsciloscopeFrame*)pTmpData, settings.getSettings()->historyFrameDisplay);
    // threads
    for(uint i = 0; i < MAX_THREAD; i++)
    {
        uint toAllocate = settings.getSettings()->historyFrameDisplay * sizeof(OsciloscopeFrame);
        OsciloscopeFrame* pThreadHistory = (OsciloscopeFrame*)pMemory->allocate(toAllocate);
        captureData[i].history.init(pThreadHistory, settings.getSettings()->historyFrameDisplay);
    }
    // how much memory is still available
    ularge        min = MINIMUM_HISTORY_COUNT * sizeof(CapturePacket);
    ularge       free = pMemory->maximum(min, MAXIMUM_HISTORY_SIZE);
    ularge        max = settings.getSettings()->memoryHistory * MEGABYTE;
    if(max == 0)
    {
        max = free;
    }
    ularge toAllocate = clamp(free, min, max);
    // history
    byte* pPacketData = (byte*)pMemory->allocate(toAllocate);
    captureBuffer->historyMemory.init(pPacketData, toAllocate, window.storage.getPacketSize());
    // memory
    captureBuffer->setMemory();
    // settings
    settings.getSettings()->historyFrameCount = captureBuffer->historyMemory.ringFrame.getSize();
}


void OsciloscopeManager::deallocate()
{
    captureBuffer->historySSD.freeInterfaceMemory();
    captureBuffer->historyMemory.freeInterfaceMemory();
    captureBuffer->historyMemory.freePacketMemory();
    pMemory->free(pTmpData);
    for(uint i = 0; i < MAX_THREAD; i++)
    {
        pMemory->free(captureData[i].history.getData());
    }
}

void OsciloscopeManager::setThreadPriority(ThreadID id)
{
    switch(id)
    {
        case THREAD_ID_CAPTURE:
            SDL_SetThreadPriority((SDL_ThreadPriority)settings.getSettings()->priorityCapture);
            break;
        case THREAD_ID_UPDATE:
            SDL_SetThreadPriority((SDL_ThreadPriority)settings.getSettings()->priorityUpdate);
            break;
        case THREAD_ID_MAIN:
            SDL_SetThreadPriority((SDL_ThreadPriority)settings.getSettings()->priorityMain);
            break;
        case THREAD_ID_RENDER:
            SDL_SetThreadPriority((SDL_ThreadPriority)settings.getSettings()->priorityRender);
            break;
    };
}

void OsciloscopeManager::clientUploadGenerator(SGenerator& generator)
{
    SDL_memcpy(&window.hardwareGenerator.custom.analog0.bytes[0],  &generator.analog0.bytes[0], sizeof(window.hardwareGenerator.custom.analog0));
    SDL_memcpy(&window.hardwareGenerator.custom.analog1.bytes[0],  &generator.analog1.bytes[0], sizeof(window.hardwareGenerator.custom.analog1));
    SDL_memcpy(&window.hardwareGenerator.custom.digital.bytes[0],  &generator.digital.bytes[0], sizeof(window.hardwareGenerator.custom.digital));
    //   int ret = usbTransferDataOut( 4, (byte*)&window.hardwareGenerator.custom, sizeof(window.hardwareGenerator.custom), 0, pOsciloscope->settings.usbEp4TimeOut );
}

void OsciloscopeManager::clientUploadDisplay(SDisplay& display)
{
}

SSimulate OsciloscopeManager::GetServerSim()
{
    // ch0
    sim.active0     = window.softwareGenerator.channel[0].onOff;
    sim.peakToPeak0 = window.softwareGenerator.channel[0].peakToPeak;
    sim.period0     = window.softwareGenerator.channel[0].period;
    sim.speed0      = window.softwareGenerator.channel[0].speed;
    sim.avery0      = window.softwareGenerator.channel[0].every;
    sim.type0       = (ESimulateType)window.softwareGenerator.channel[0].type;
    // ch1
    sim.active1     = window.softwareGenerator.channel[1].onOff;
    sim.peakToPeak1 = window.softwareGenerator.channel[1].peakToPeak;
    sim.period1     = window.softwareGenerator.channel[1].period;
    sim.speed1      = window.softwareGenerator.channel[1].speed;
    sim.avery1      = window.softwareGenerator.channel[1].every;
    sim.type1       = (ESimulateType)window.softwareGenerator.channel[1].type;
    // other stuff
    sim.time       = window.horizontal.Capture;
    sim.voltage0   = window.channel01.Capture;
    sim.voltage1   = window.channel02.Capture;
    sim.etsActive  = window.horizontal.ETS;
    sim.etsIndex   = settings.getHardware()->fpgaEtsIndex;
    sim.etsMax     = settings.getHardware()->fpgaEtsCount;
    return sim;
}
void OsciloscopeManager::transmitSim(SSimulate& sim)
{
   pOsciloscope->thread.setSimulateData(&sim);
   pOsciloscope->thread.function(afSetSimulateData);
   pOsciloscope->thread.function(afServerUpload);
}

void OsciloscopeManager::simOnOff(int value)
{
   pOsciloscope->thread.setSimulateOnOff(value);
   pOsciloscope->thread.function(afSetSimulateOnOff);
   pOsciloscope->thread.function(afServerUpload);
}

void OsciloscopeManager::clearEts(int value)
{
   SDL_AtomicSet(&etsClear,value);
   if (window.horizontal.ETS)
      clearRenderTarget = 0;
}

void OsciloscopeManager::setupControl(WndMain window)
{
    int version = control.getVersion();
    for(int i = 1; i < 3; i++)
    {
        control.setVersion(i);
        settings.setVersion(i);
        control.setAnalogSwitchBit(CHANNEL_A_GROUND, window.channel01.Ground);
        control.setAnalogSwitchBit(CHANNEL_B_GROUND, window.channel02.Ground);
        control.setAnalogSwitchBit(CHANNEL_A_ACDC, window.channel01.AcDc);
        control.setAnalogSwitchBit(CHANNEL_B_ACDC, window.channel02.AcDc);
        control.setAnalogSwitchBit(CHANNEL_ETS, window.horizontal.ETS);
        control.setAnalogSwitchBit(CHANNEL_ATTR_A, pOsciloscope->control.getAttr(captureVoltFromValue(window.channel01.Capture)));
        control.setAnalogSwitchBit(CHANNEL_ATTR_B, pOsciloscope->control.getAttr(captureVoltFromValue(window.channel02.Capture)));
        control.setEts(window.horizontal.ETS);
        control.setControl(window.horizontal.Control);
        control.setYRangeScaleA(captureVoltFromValue(window.channel01.Capture), window.channel01.Scale);
        control.setYPositionA(window.channel01.YPosition + pOsciloscope->settings.getHardware()->getAnalogOffset(window.horizontal.Capture, 0, window.channel01.Capture));
        control.setYRangeScaleB(captureVoltFromValue(window.channel02.Capture), window.channel02.Scale);
        control.setYPositionB(window.channel02.YPosition + pOsciloscope->settings.getHardware()->getAnalogOffset(window.horizontal.Capture, 1, window.channel02.Capture));
        control.setTriggerSource(window.trigger.Source);
        control.setTriggerMode(window.trigger.Mode);
        control.setTriggerSlope(window.trigger.Slope);
        control.setTriggerPre(window.trigger.Percent);
        control.setTriggerHis(window.trigger.His);
        control.setTriggerLevel(window.trigger.Level);
        control.setXRange(captureTimeFromValue(window.horizontal.Capture));
        control.setSampleSize(window.horizontal.FrameSize);
        control.setHoldoff(window.trigger.Holdoff);
        control.setDigitalStart(window.trigger.stageStart);
        control.setDigitalMode(window.trigger.stageMode);
        control.setDigitalChannel(window.trigger.stageChannel);
        for(int stage = 0; stage < 4; stage++)
        {
            control.setDigitalDelay((DigitalStage)stage, window.trigger.delay[stage]);
        }
        for(int stage = 0; stage < 4; stage++)
        {
            for(int bit = 0; bit < 16; bit++)
            {
                control.setDigitalMask((DigitalStage)stage, (DigitalBit)bit, window.trigger.mask[stage][bit]);
            }
        }
        for(int stage = 0; stage < 4; stage++)
        {
            for(int bit = 0; bit < 16; bit++)
            {
                control.setDigitalPattern((DigitalStage)stage, (DigitalBit)bit, (DigitalPattern)window.trigger.pattern[stage][bit]);
            }
        }
        control.setGeneratorType0((GeneratorType)window.hardwareGenerator.type0);
        control.setGeneratorOn0(window.hardwareGenerator.onOff0);
        control.setGeneratorSlope0(window.hardwareGenerator.sawSlopePositive0);
        control.setGeneratorVoltage0(window.hardwareGenerator.voltage0);
        control.setGeneratorOffset0(window.hardwareGenerator.offset0 + pOsciloscope->settings.getHardware()->getGeneratorOffset(window.horizontal.Capture,0));
        control.setGeneratorFrequency0(window.hardwareGenerator.frequency0, pOsciloscope->settings.getHardware()->generatorFs);
        control.setGeneratorSquareDuty0(window.hardwareGenerator.squareDuty0);
        control.setGeneratorType1((GeneratorType)window.hardwareGenerator.type1);
        control.setGeneratorOn1(window.hardwareGenerator.onOff1);
        control.setGeneratorSlope1(window.hardwareGenerator.sawSlopePositive1);
        control.setGeneratorVoltage1(window.hardwareGenerator.voltage1);
        control.setGeneratorOffset1(window.hardwareGenerator.offset1 + pOsciloscope->settings.getHardware()->getGeneratorOffset(window.horizontal.Capture,1));
        control.setGeneratorFrequency1(window.hardwareGenerator.frequency1, pOsciloscope->settings.getHardware()->generatorFs);
        control.setGeneratorSquareDuty1(window.hardwareGenerator.squareDuty1);
        control.setDigitalVoltage(window.digitalSetup.voltage);
        control.setDigitalInputOutput(window.digitalSetup.inputOutput15, window.digitalSetup.inputOutput7);
        for(int i = 0; i < 16; i++)
        {
            control.setDigitalOutputBit(i, window.digital.output[i]);
        }
        control.setDigitalClockDivide(window.digitalSetup.divider);
    }
    control.setVersion(version);
    settings.setVersion(version);
}

////////////////////////////////////////////////////////////////////////////////
//
// ETS
//
////////////////////////////////////////////////////////////////////////////////
void OsciloscopeETS::clear()
{
    etsHistory.clear();
    etsIndex = 0;
}

void OsciloscopeETS::redraw(OsciloscopeRenderData& render,SDL_atomic_t* redrawEts)
{
   if (SDL_AtomicGet(redrawEts) == 12)
   {
      render.flags.bit(rfClearRenderTarget, 0);
      SDL_AtomicSet(redrawEts, 0);
   }
   if (SDL_AtomicGet(redrawEts) > 1 && SDL_AtomicGet(redrawEts) < 12 )
   {
      render.flags.bit(rfClearRenderTarget, 1);
      SDL_AtomicSet(redrawEts, SDL_AtomicGet(redrawEts) + 1);
   }
   if (SDL_AtomicGet(redrawEts) == 1)
   {
      etsIndex = etsHistory.getCount() - 1;
      etsIndex = max(0U, etsIndex);
      SDL_AtomicSet(redrawEts, 2);
      render.flags.bit(rfClearRenderTarget, 1);
   }
}

void OsciloscopeETS::onFrameChange(int framechange, Ring<CapturePacket> threadHistory, OsciloscopeRenderData& render)
{
    // return;
    render.flags.raise(rfClearRenderTarget);
    // ets, display
    clear();
    // frame
    CaptureFrame frame;
    for(int i = max<int>(0, framechange - 32); i < framechange; i++)
    {
        pOsciloscope->captureBuffer->captureFrame(frame, i);
        pOsciloscope->captureBuffer->historyRead(frame, frame.version, frame.header, frame.data, frame.packet);
        pOsciloscope->captureBuffer->display(oscFrame, frame.version, frame.header, frame.data, frame.packet);
        onCapture(oscFrame, render);
    }
    redraw(render,&pOsciloscope->etsClear);
}

void OsciloscopeETS::onCapture(OsciloscopeFrame& frame, OsciloscopeRenderData& render)
{
    // attributes, index
    etsAttr  = 0;
    etsIndex = 0;
    // frame to clear
    for(int i = 0; i < etsHistory.getCount(); i++)
    {
        if(frame.ets == etsHistory[i].ets)
        {
            etsClear = etsHistory[i];
            etsHistory.remove(i);
            etsAttr = ETS_CLEAR;
            break;
        }
    }
    // add frames
    if(etsHistory.getCount() < etsHistory.getSize() &&
       etsHistory.getCount() < (int)pOsciloscope->settings.getSettings()->historyFrameCount)
    {
        etsHistory.pushBack(frame);
    }
    render.etsAttr = etsAttr;
}

void OsciloscopeETS::onPause(OsciloscopeFrame& frame, WndMain& window)
{
    if(window.horizontal.ETS)
    {
        etsAttr = 0;
        if(etsHistory.getCount())
        {
            if(etsIndex >= 0 && etsIndex < (uint)etsHistory.getCount())
            {
                frame = etsHistory[etsIndex];
                etsIndex--;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// CaptureThread
//
////////////////////////////////////////////////////////////////////////////////
void SendToRenderer(OsciloscopeFrame& captureFrame,
                    WndMain& window,
                    OsciloscopeRenderData& render,
                    OsciloscopeETS& ets,
                    OsciloscopeThreadRenderer& renderer,
                    OsciloscopeFFT& fft,
                    OsciloscopeThreadData& captureData, uint delay)
{
    pTimer->deltaTime(TIMER_CAPTURE);
    SDL_AtomicLock(&pOsciloscope->displayLock);
    pOsciloscope->display = captureFrame;
    SDL_AtomicUnlock(&pOsciloscope->displayLock);
    // history
    pOsciloscope->tmpHistory.write(captureFrame);
    // ets
    ets.onCapture(captureFrame, render);
    // send to renderer
    uint captureId = 0;
    bool ret = false;
    while(!ret && pOsciloscope->captureDataThreadActive)
    {
        ret = pOsciloscope->threadLoop.update.producerLock(captureId, false);
        if(ret)
        {
            Ring<OsciloscopeFrame> history = pOsciloscope->tmpHistory;
            OsciloscopeFrame       frame;
            uint count = history.getCount();
            captureData.history.clear();
            for(uint i = 0; i < count; i++)
            {
                history.read(frame);
                captureData.history.write(frame);
            }
            captureData.etsClear = ets.etsClear;
            captureData.frame    = frame;
            captureData.render   = render;
            captureData.window   = window;
            // render
            renderer.clearFast();
            fft.clear();
            pOsciloscope->renderThread(captureId, captureData, renderer, fft);
            pOsciloscope->threadLoop.update.producerUnlock(captureId);
        }
        else
        {
            if(delay > 0)
            {
                SDL_Delay(delay);
            }
        }
    }
}

int SDLCALL CaptureDataThreadFunction(void* data)
{
    byte*    buffer = (byte*)pOsciloscope->ptrHardwareCapture;
    SignalMode mode = pOsciloscope->signalMode;
    pTimer->init(TIMER_HARDWARE);
    float time         = 0;
    uint received      = 0;
    uint frameSize     = 0;
    uint frameDataSize = 0;
    float mbTimer   = 0;
    uint  bandWidth = 0;
    SDL_MemoryBarrierAcquire();
    while(pOsciloscope->captureDataThreadActive)
    {
        // timer
        pTimer->deltaTime(TIMER_HARDWARE);
        time += pTimer->getDelta(TIMER_HARDWARE);
        mbTimer += pTimer->getDelta(TIMER_HARDWARE);

        int capture = SDL_AtomicGet(&pOsciloscope->threadCapture);

        // connected
        int isConnected = pOsciloscope->thread.isConnected();

        // simulation
        int isSimulation = pOsciloscope->thread.isSimulate();

        // open
        int isOpen = pOsciloscope->thread.isOpen();

        // frame
        int version = 0;
        int headerSize = 0;
        int maxData = 0;
        int packetSize = 0;
        pOsciloscope->thread.getFrame(&version, &headerSize, &maxData, &packetSize);

        // capture and uncompress
        mode = pOsciloscope->signalMode;
        if(mode == SIGNAL_MODE_CAPTURE || mode == SIGNAL_MODE_SIMULATE)
        {
            if(received == frameSize)
            {
                // local simulation ?
                if(isSimulation && !isConnected)
                {
                    pOsciloscope->thread.simulateTime(time);
                }
                // client display ?
                if(isConnected)
                {
                    pOsciloscope->thread.function(afServerDownload);
                    pOsciloscope->thread.function(afGetClientDisplay);
                }
                bandWidth += received;
                if(mbTimer > 1.0)
                {
                    uint mb = double(bandWidth) / mbTimer;
                    mb = mb / (1024 * 1024);
                    SDL_AtomicSet(&pOsciloscope->bandWidth, mb);
                    mbTimer   = 0.f;
                    bandWidth = 0;
                }
                received = 0;
                SDL_memset(buffer, 0, headerSize);
            }
            if(received > frameSize && frameSize > 0)
            {
                received = 0;
                frameSize = 0;
            }
            else if(received < (uint)headerSize)
            {
                // capture
                int transfered = 0;
                int  ret = pOsciloscope->thread.captureFrameData( (SFrameData*)(buffer), headerSize, &transfered, SCOPEFUN_CAPTURE_TYPE_HEADER);
                if(transfered > 0)
                {
                    // just for testing saving control bits in the header till this is done properly in firmware
                    if(pOsciloscope->settings.getSettings()->windowDebug == 2 && int(received) + transfered >= headerSize)
                    {
                        SFrameHeader1* header1 = (SFrameHeader1*)buffer;
                        SFrameHeader2* header2 = (SFrameHeader2*)buffer;
                        if(pOsciloscope->control.getVersion() == 1)
                        {
                            SHardware1 hw = pOsciloscope->control.control1.client1Get();
                            SDL_memcpy((void*)(char*) & (header1->hardware.bytes[0]), (void*)(char*) & (hw), sizeof(SHardware1));
                        }
                        if(pOsciloscope->control.getVersion() == 2)
                        {
                            SHardware2 hw = pOsciloscope->control.control2.client2Get();
                            SDL_memcpy((void*)(char*) & (header2->hardware.bytes[0]), (void*)(char*) & (hw), sizeof(SHardware2));
                        }
                        uint crc = 0;
                        for(int i = 0; i < headerSize - 1; i++)
                        {
                            crc += buffer[i];
                        }
                        byte byteCRC = (crc % headerSize) & 0xFF;
                        buffer[headerSize - 1] = byteCRC;
                    }
                    // new frame
                    if(received == 0)
                    {
                        pOsciloscope->captureBuffer->uncompressNew();
                    }
                    // samples uncompress
                    pOsciloscope->captureBuffer->uncompress(buffer, received, transfered, version, headerSize, maxData, packetSize, received == 0);
                    pOsciloscope->captureBuffer->historyWrite(0, version, headerSize, maxData, packetSize, received == 0);
                    received += transfered;
                    // frame size
                    frameDataSize = pOsciloscope->captureBuffer->getFrameDataSize(buffer, version, headerSize, maxData, packetSize);
                    frameSize = frameDataSize;
                    if(frameSize == 0)
                    {
                        received = 0;
                        SDL_Delay(10);
                        continue;
                    }
                    frameSize += headerSize;
                }
                else
                {
                    SDL_Delay(10);
                }
            }
            else
            {
                // toReceive
                uint toReceive  = min<uint>(pOsciloscope->sizeHardwareCapture, frameSize - received);
                uint maxReceive = captureTimeMaxReceive(capture, version);
                      toReceive = min<uint>(maxReceive, toReceive);

                // capture
                int transfered = 0;
                int  ret = pOsciloscope->thread.captureFrameData( (SFrameData*)(buffer), toReceive, &transfered, SCOPEFUN_CAPTURE_TYPE_DATA);
                if(transfered > 0)
                {
                    // samples uncompress
                    pOsciloscope->captureBuffer->uncompress(buffer, received, transfered, version, frameDataSize, frameSize, packetSize, false);
                    pOsciloscope->captureBuffer->historyWrite(frameSize, version, headerSize, frameDataSize, packetSize, false);
                    received += transfered;
                }
                else
                {
                    SDL_Delay(10);
                }
            }
        }
        else
        {
            SDL_Delay(100);
        }
    }
    SDL_MemoryBarrierRelease();
    pMemory->free(buffer);
    return 0;
}

int SDLCALL GenerateFrameThreadFunction(void* data)
{
    pOsciloscope->setThreadPriority(THREAD_ID_CAPTURE);
    pTimer->init(TIMER_CAPTURE);
    pTimer->init(TIMER_GENERATE);
    OsciloscopeFrame       captureFrame;
    WndMain                captureWindow;
    OsciloscopeRenderData  captureRender;
    ularge captureFreq            = SDL_GetPerformanceFrequency();
    ularge captureStart           = 0;
    ularge captureTime            = 0;
    uint       frame = 0;
    SignalMode mode  = SIGNAL_MODE_PAUSE;
    SDL_zero(captureWindow);
    SDL_zero(captureRender);
    SDL_zero(captureFrame);
    OsciloscopeThreadRenderer renderer;
    OsciloscopeFFT            fft;
    OsciloscopeThreadData* pCaptureData = (OsciloscopeThreadData*)new OsciloscopeThreadData();
    uint              toAllocate = pOsciloscope->settings.getSettings()->historyFrameDisplay * sizeof(OsciloscopeFrame);
    OsciloscopeFrame* memory     = (OsciloscopeFrame*)pMemory->allocate(toAllocate);
    pCaptureData->history.init(memory, pOsciloscope->settings.getSettings()->historyFrameDisplay);
    fft.init();
    renderer.init(pOsciloscope->settings.getSettings()->historyFrameDisplay);
    renderer.clear();
    OsciloscopeETS&     ets = pOsciloscope->ets;
    ularge              playFrameIdx = 0;
    uint delayCapture = pOsciloscope->settings.getSettings()->delayCapture;
    int received = 0;
    while(pOsciloscope->generateFrameThreadActive)
    {
        // sync multiple capture threads
        SDL_AtomicLock(&pOsciloscope->captureLock);

        // sync user interface and render data with capture thread
        SDL_AtomicLock(&pOsciloscope->renderLock);

        ////////////////////////////////////////////////////////////////////
        // sync capture thread with user interface
        ////////////////////////////////////////////////////////////////////
        switch(mode)
        {
            case SIGNAL_MODE_PAUSE:
                pOsciloscope->renderWindow.horizontal.uiActive = 0;
                break;
            case SIGNAL_MODE_PLAY:
                pOsciloscope->renderWindow.horizontal.uiActive = 1;
                pOsciloscope->renderWindow.horizontal.uiValue  = playFrameIdx;
                pOsciloscope->renderWindow.horizontal.Frame    = playFrameIdx;
                pOsciloscope->renderWindow.horizontal.uiRange  = pOsciloscope->captureBuffer->captureFrameCount();
                break;
            case SIGNAL_MODE_SIMULATE:
            case SIGNAL_MODE_CAPTURE:
                pOsciloscope->renderWindow.horizontal.uiActive = 1;
                pOsciloscope->renderWindow.horizontal.uiValue  = max<int>(0, pOsciloscope->captureBuffer->captureFrameCount() - 1);
                pOsciloscope->renderWindow.horizontal.Frame    = max<int>(0, pOsciloscope->captureBuffer->captureFrameCount() - 1);
                pOsciloscope->renderWindow.horizontal.uiRange  = pOsciloscope->captureBuffer->captureFrameCount();
                break;
            case SIGNAL_MODE_CLEAR:
                pOsciloscope->renderWindow.horizontal.uiActive = 1;
                pOsciloscope->renderWindow.horizontal.uiValue  = 0;
                pOsciloscope->renderWindow.horizontal.Frame    = 0;
                pOsciloscope->renderWindow.horizontal.uiRange  = 0;
                break;
        };
        ////////////////////////////////////////////////////////////////////
        // sync user interface data with capture thread
        ////////////////////////////////////////////////////////////////////
        captureWindow = pOsciloscope->renderWindow;
        captureRender = pOsciloscope->renderData;

        ////////////////////////////////////////////////////////////////////
        // sync custom display data
        ////////////////////////////////////////////////////////////////////
        pOsciloscope->thread.getDisplay(&pCaptureData->customData);

        SDL_AtomicUnlock(&pOsciloscope->renderLock);
        ////////////////////////////////////////////////////////////////////
        // change
        //////////////////////////////////////////////////////////////////////
        if(pOsciloscope->signalMode != mode)
        {
            ////////////////////////////////////////////////////////////////////
            // play / stop
            ////////////////////////////////////////////////////////////////////
            switch(pOsciloscope->signalMode)
            {
                case SIGNAL_MODE_PLAY:
                    playFrameIdx = clamp<ularge>(captureWindow.horizontal.Frame, 0, captureWindow.horizontal.uiRange - 1);
                    ets.clear();
                    break;
                case SIGNAL_MODE_PAUSE:
                    ets.redraw(captureRender,&pOsciloscope->etsClear);
                    break;
                case SIGNAL_MODE_CLEAR:
                    pOsciloscope->captureBuffer->clear();
                    captureFrame.clear();
                    break;
                case SIGNAL_MODE_CAPTURE:
                    ets.clear();
                    captureFreq  = SDL_GetPerformanceFrequency();
                    captureStart = SDL_GetPerformanceCounter();
                    pTimer->init(TIMER_CAPTURE);
                    pTimer->init(TIMER_GENERATE);
                    break;
                case SIGNAL_MODE_SIMULATE:
                    ets.clear();
                    captureFreq  = SDL_GetPerformanceFrequency();
                    captureStart = SDL_GetPerformanceCounter();
                    pTimer->init(TIMER_CAPTURE);
                    pTimer->init(TIMER_GENERATE);
                    break;
            };
            mode = pOsciloscope->signalMode;
        }
        // redraw ets ?
        ets.redraw(captureRender, &pOsciloscope->etsClear);

        // mode ?
        mode = pOsciloscope->signalMode;
        ////////////////////////////////////////////////////////////////////
        // mode
        ////////////////////////////////////////////////////////////////////
        SDL_MemoryBarrierAcquire();
        switch(mode)
        {
            case SIGNAL_MODE_PLAY:
                {
                    CaptureFrame cf;
                    pOsciloscope->captureBuffer->captureFrame(cf, playFrameIdx);
                    pOsciloscope->captureBuffer->historyRead(cf, cf.version, cf.header, cf.data, cf.packet);
                    playFrameIdx++;
                    if(playFrameIdx >= pOsciloscope->captureBuffer->captureFrameCount())
                    {
                        playFrameIdx = 0;
                    }
                    SDL_AtomicSet(&pOsciloscope->syncUI, 1);
                    pOsciloscope->captureBuffer->display(captureFrame, cf.version, cf.header, cf.data, cf.packet);
                    SendToRenderer(captureFrame, captureWindow, captureRender, ets, renderer, fft, *pCaptureData, delayCapture);
                    break;
                }
            case SIGNAL_MODE_SIMULATE:
            case SIGNAL_MODE_CAPTURE:
                {
                    // frame
                    CaptureFrame cf;
                    int frameLast = pOsciloscope->captureBuffer->captureFrameLast();
                    if(pOsciloscope->window.horizontal.Full)
                    {
                        pOsciloscope->captureBuffer->captureFrame(cf, max(0, frameLast - 1));
                    }
                    else
                    {
                        pOsciloscope->captureBuffer->captureFrame(cf, frameLast);
                    }
                    // render
                    if(cf.frameSize > 0 && cf.packetCount > 1)
                    {
                        pOsciloscope->captureBuffer->historyRead(cf, cf.version, cf.header, cf.data, cf.packet);
                        if(pOsciloscope->captureBuffer->display(captureFrame, cf.version, cf.header, cf.data, cf.packet))
                        {
                            pOsciloscope->onCallibrateFrameCaptured(captureFrame, cf.version);
                        }
                    }
                    SendToRenderer(captureFrame, captureWindow, captureRender, ets, renderer, fft, *pCaptureData, delayCapture);
                    break;
                };
            case SIGNAL_MODE_CLEAR:
            case SIGNAL_MODE_PAUSE:
                {
                    captureWindow.horizontal.Frame = min<uint>(captureWindow.horizontal.Frame, pOsciloscope->captureBuffer->captureFrameCount() - 1);
                    CaptureFrame cf;
                    pOsciloscope->captureBuffer->captureFrame(cf, captureWindow.horizontal.Frame);
                    pOsciloscope->captureBuffer->historyRead(cf, cf.version, cf.header, cf.data, cf.packet);
                    pOsciloscope->captureBuffer->display(captureFrame, cf.version, cf.header, cf.data, cf.packet);
                    if(frame != captureWindow.horizontal.Frame)
                    {
                        ets.onFrameChange(captureWindow.horizontal.Frame, pOsciloscope->threadHistory, captureRender);
                        frame = captureWindow.horizontal.Frame;
                    }
                    ets.onPause(captureFrame, captureWindow);
                    SendToRenderer(captureFrame, captureWindow, captureRender, ets, renderer, fft, *pCaptureData, delayCapture);
                    break;
                };
            default:
                break;
        };
        SDL_MemoryBarrierRelease();
        SDL_AtomicUnlock(&pOsciloscope->captureLock);
        if(delayCapture > 0)
        {
            SDL_Delay(delayCapture);
        }
    }
    return 0;
}

int SDLCALL ControlHardwareThreadFunction(void* data)
{
   SDL_MemoryBarrierAcquire();
   pOsciloscope->setThreadPriority(THREAD_ID_CAPTURE);
   while (pOsciloscope->controlHardwareThreadActive)
   {
      pOsciloscope->thread.update();
      SDL_Delay(1);
   }
   SDL_MemoryBarrierRelease();
   return 0;
}

SFContext* getCtx()
{
    return &pOsciloscope->ctx->sf;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
