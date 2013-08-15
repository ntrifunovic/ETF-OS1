// File: tControl.cpp

#include <stdio.h>

#include "schedule.h"

#include "lock.h"
#include "log.h"
#include "sysmode.h"
#include "PCB.h"
#include "kernel.h"
#include "tControl.h"

PCB* runningThread = NULL;

void dispatch(bool schedule) {
  static bool inside = false;  // da li smo vec u dispatch-u - mozemo opet
                               // uci u dispach ako se desi prekid
  log2("\tLOG: dispatch schedule:%s %d\n", schedule?"true":"false", systemMode);   
  {
    intLockBlock  // Locks this block
                  // problem ako se interrupt desi
                  // neposredno pre instukcije:  inside = true
                  // ovo moze dovesti do nekoegzistentnog stanja
    if (inside || inSystemMode()) {
      log("\tLOG: dispach canceled inside:%d, inSystemMode:%d\n", inside, inSystemMode());
      
      return;
    }
      
    inside = true;
  }
  
  log2("\tLOG: dispatch schedule:%s\n", schedule ? "true" : "false");   
 
  SAVE_REGISTERS

  asm {
    les bx, dword ptr runningThread;
    
    mov es:[bx+SP_DISP], sp; // runningThread->sp = sp
    mov es:[bx+SS_DISP], ss; // runningThread->ss = ss
  }
  
  {
    if (schedule) { // Ako se proces uspavljuje ne treba ga zakazivati
      // Scheduler::put - nije thread safe
      intLockBlock
      
      schedulerPut(runningThread);
    }
    
    runningThread = schedulerGet();
    
    log("\tLOG: yielded:%s\n", runningThread->name);
  }
  
  asm {
    les bx, dword ptr runningThread;
    
    mov sp, es:[bx+SP_DISP]; // sp = runningThread->sp
    mov ss, es:[bx+SS_DISP]; // ss = runningThread->ss
  }
  
  RESTORE_REGISTERS
  
  inside = false;
  
  {
    log("\tLOG: dispach finished inside:%d, inSystemMode:%d\n", inside, inSystemMode());
  }
  
  // check(runningThread->id!=-1, "dispach");
}