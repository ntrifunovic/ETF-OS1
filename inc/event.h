// File: event.h

#ifndef _EVENT_H_
#define _EVENT_H_

#include "semaphor.h"

#include "utility.h"
#include "lock.h"
#include "log.h"
#include "list.h"
#include "tControl.h"
#include "kernel.h"
#include "sysmode.h"

typedef unsigned char IVTNo;

class KernelEv;

class Event {
public:
  Event (IVTNo ivtNo);
  ~Event ();

  int wait  ();
  void signal();

 private:
   KernelEv* myImpl;
};

class KernelEv {
  Semaphore semaphore;
  IVTNo ivtNo;
  
  List<KernelEv>::Node* eventListPosition;
public:
  KernelEv(IVTNo);
  ~KernelEv();
  
  int wait() {
    log("\tLOG: Thread [%s] waiting on Event %d\n",
       runningThread->name,
       ivtNo);
        
    return semaphore.wait();
  }
  void signal() {
    log("\tLOG: Event %d happened\n", ivtNo);
    
    intLockBlock
    
    while(semaphore.val() < 0) {
      semaphore.signal();
    }
  }
};

class IVTEntry {
public:
  PrekidnaRutina oldInterrupt;
  PrekidnaRutina newInterrupt;
  
  IVTNo ivtNo;
  // Event-ovi kreirani na ovom ulazu
  List<KernelEv> events;

  IVTEntry(IVTNo, PrekidnaRutina);
  ~IVTEntry();
};

extern void interrupted(IVTEntry&);

#define PREPAREENTRY(_ivtNo, _callOld)\
extern IVTEntry ivtEntry##_ivtNo;\
\
void interrupt interrupt##_ivtNo(){\
  ENTERSYSTEMMODE\
  \
  log2("EVENT\n");\
  \
  if (_callOld) {\
    ivtEntry##_ivtNo.oldInterrupt();\
  }\
  \
  for(List<KernelEv>::Node* node = ivtEntry##_ivtNo.events.start->next;\
      node != ivtEntry##_ivtNo.events.end;\
      node = node->next) {\
    node->value->signal();\
  }\
  \
  EXITSYSTEMMODE\
}\
IVTEntry ivtEntry##_ivtNo(_ivtNo, interrupt##_ivtNo)

#endif // _EVENT_H_