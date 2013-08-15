// File: semaphor.cpp

#include "schedule.h"

#include "utility.h"
#include "lock.h"
#include "log.h"
#include "PCBList.h"
#include "tControl.h"
#include "semaphor.h"

// bool semPreempt = false;

class KernelSem {
  int value;
  PCBList blocked;
  
  #ifdef LOG
  static ID MAX_ID;
  const ID id;  
  #endif
  
public:  
  KernelSem(int val) : value(val)
  #ifdef LOG
  // Da se ne bi desilo da dvojca imaju isti ID prefiksni ++
  , id(++MAX_ID) 
  #endif
  {}
  
  int wait();
          
  void signal();
             
  int val() const {
    return value;
  }
};

#ifdef LOG
ID KernelSem::MAX_ID = 0;
#endif

int KernelSem::wait() {
  bool block = false;
  
  { // value - nije thread safe
    // blocked - nije thread safe
    intLockBlock
    
    value--;
    
    if (value < 0) {
      block = true;
    
      runningThread->waitingListPosition = blocked.add(runningThread);
      runningThread->wokenUp = false;
      
      #ifdef LOG
      log("\tLOG: Thread [%s] waiting on semaphore <%d>\n",
          runningThread->name, id);
      #endif
    }
  }
  
  if(semPreempt || block)
    dispatch(!block);
  
  
  return !block || !runningThread->wokenUp;
}

void KernelSem::signal() {
  
  { // Ne sme da se promeni lista blokiranih,
    // Scheduler::put - nije thread safe
    // value - nije thread safe
    // blocked - nije thread safe
    intLockBlock
    
    value++;
      
    if (!blocked.empty()) {
      PCB* pcb = blocked.get();
    
      pcb->waitingListPosition = NULL;
      schedulerPut(pcb);
      
      #ifdef LOG
      log("\tLOG: Unblocking [%s] from semaphore <%d>\n", pcb->name, id);
      #endif
    }
  }
  
  if (semPreempt)
    dispatch();
}

//------------------------------------------------------------------
//-------------------------------INTERFEJS--------------------------
//------------------------------------------------------------------

Semaphore::Semaphore(int init) : myImpl(new KernelSem(init)) {}

Semaphore::~Semaphore() {
  delete myImpl; 
}

int Semaphore::wait() {
  return myImpl->wait();
}

void Semaphore::signal() {
  myImpl->signal();
}

int Semaphore::val() const {
  return myImpl->val();
}