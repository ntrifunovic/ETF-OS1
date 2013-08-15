// File: sysmode.h

#ifndef _SYSMODE_H_
#define _SYSMODE_H_

#include "tControl.h"
#include "utility.h"
#include "log.h"
#include "lock.h"

extern unsigned systemMode;

extern InteruptLock systemModeLock;

inline bool inSystemMode() {
  return systemMode > 0;
}

#define _ENTERSYSTEMMODE\
if (runningThread != mainThread && systemMode == 0) {\
    asm {\
      mov ax, sp;\
      mov dx, ss;\
      \
      les bx, dword ptr mainThread;\
      \
      mov sp, es:[bx+SP_DISP];\
      mov ss, es:[bx+SS_DISP];\
      \
      push dx;\
      push ax;\
    }\
  }
  
#define _EXITSYSTEMMODE\
if (runningThread != mainThread && systemMode == 0) {\
    asm {\
      pop ax;\
      pop dx;\
      \
      mov ss, dx;\
      mov sp, ax;\
    }\
  }
  
#define ENTERSYSTEMMODE\
  log2("sys IN:%d\n", systemMode);\
  \
  systemModeLock.lock();\
  _ENTERSYSTEMMODE\
  systemMode++;\
  systemModeLock.unlock();\
  {
  
#define EXITSYSTEMMODE\
  }\
  systemModeLock.lock();\
  systemMode--;\
  _EXITSYSTEMMODE\
  systemModeLock.unlock();\
  log2("sys OUT:%d\n", systemMode);
  
#define ENTERSYSTEMMODE1(x)\
  log2("sys IN:%d\n", systemMode);\
  \
  systemModeLock.lock();\
  _##x = x;\
  _ENTERSYSTEMMODE\
  systemMode++;\
  systemModeLock.unlock();\
  {
    
#define EXITSYSTEMMODE1(x)\
  }\
  systemModeLock.lock();\
  systemMode--;\
  _EXITSYSTEMMODE\
  _##x = x;\
  systemModeLock.unlock();\
  log2("sys OUT:%d\n", systemMode);
    
#endif  // _SYSMODE_H_