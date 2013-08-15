// File: kernel.h

#include "schedule.h"
#include "thread.h"

#include "lock.h"
#include "log.h"
#include "sysmode.h"
#include "tControl.h"
#include "PCB.h"
#include "sList.h"
#include "kernel.h"

// externals iz kernel.h
TimeList timeBlocked;

PCB* mainThread;

InteruptLock systemModeLock(false);

unsigned schedulerCount = 0;

class UserMainThread : public Thread {
  int argc;
  char** argv;

public:
  int ret;

  UserMainThread(int _argc, char** _argv) : Thread("userMain"),
                                           argc(_argc),
                                           argv(_argv) {}
  void run() { 
    ret = userMain(argc, argv);
  }
  
};

class MainThread : public Thread {
public:
  // Ne treba nam stack, koristimo stack od glavnog programa
  MainThread() : Thread("Main", 0, 0) {}
};

PrekidnaRutina oldTimer; // stara prekidna rutina

unsigned systemMode = 0;

// Ovu metodu treba pozvati svaki put kada se desi prekid na tajmeru
void timeBlockedThreadsMenagment() {
  timeBlocked.decrase();
  
  log("\tLOG: peeKey() = %d\n",
      timeBlocked.empty() ? -1 : timeBlocked.peekKey());
      
  // Dok ima niti koje treba da se odblokiraju, odblokiraj ih
  while(!timeBlocked.empty() && timeBlocked.peekKey() == 0) {
    log("\tLOG: Unblocking from sleep\n");
    
    PCB* pcb =  timeBlocked.get();
    
    check(pcb != NULL,
          "pcb == NULL u timeBlockedThreadsMenagment()");

    pcb->timeListPosition = NULL;
    schedulerPut(pcb);

    log("\tLOG: Sleep ended [%s]\n", pcb->name);
  }
}

// prekidna rutina za tajmer
void interrupt timer() {
  static unsigned timerCount = 0;
  
  ENTERSYSTEMMODE
  
  // poziv stare prekidne rutine koja se 
  // nalazila na 08h, a sad je na 60h
  asm int 60h;
  
  timerCount++;
  
  tick();
  
  log("\tLOG: timer\n");
  
  timeBlockedThreadsMenagment();
  
  EXITSYSTEMMODE
    
  if (runningThread->timeSlice != 0 &&
      timerCount >= runningThread->timeSlice) { 
    timerCount = 0;
    asm sti; // Dozvoljavamo prekide
    dispatch(true);
  }
  
}

void inicTimer() { // postavlja novu prekidnu rutinu
  PrekidnaRutina *ptr;
  
  FP_SEG(ptr) = 0;
  FP_OFF(ptr) = 0x8*IVTPENTRYSIZE;
    
  FP_SEG(oldTimer) = FP_SEG(*ptr);
  FP_OFF(oldTimer)  = FP_OFF(*ptr);
  
  {
    intLockBlock
    
    FP_SEG(*ptr) = FP_SEG(timer);
    FP_OFF(*ptr) = FP_OFF(timer);
  }
  
  FP_OFF(ptr) = 0x60*IVTPENTRYSIZE;
  
  FP_SEG(*ptr) = FP_SEG(oldTimer);
  FP_OFF(*ptr) = FP_OFF(oldTimer);
}

void restoreTimer() { // restore-uje prekidne rutine u prvobitno stanje
  PrekidnaRutina *ptr;
  
  FP_SEG(ptr) = 0;
  FP_OFF(ptr) = 0x8*IVTPENTRYSIZE;
  
  {
    intLockBlock
    
    FP_SEG(*ptr) = FP_SEG(oldTimer);
    FP_OFF(*ptr) = FP_OFF(oldTimer);
  }
}


int main(int argc, char* argv[]) {
  MainThread mainThread;
  
  initLog();
  
  int ret;
  
  inicTimer();
 
  {
    UserMainThread userThread(argc, argv);
    userThread.start();
    userThread.waitToComplete();
    ret = userThread.ret;
  }
  
  // Cekamo zaostale niti da zavrse
  {
    InteruptLock lock(true);
  
    while(!threads.empty()) {
      PCB* pcb = threads.peek();
      
      log("\tLOG: Waiting for [%s] to finish\n", pcb->name);
      
      lock.unlock();
      pcb->waitToComplete();
      lock.lock();
    }
  }

  restoreTimer();
  
  log("\tLOG: -----------------HAPPY ENDING-----------------\n");
  
  closeLog();
  
  return ret;
}