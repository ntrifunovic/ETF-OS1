// File: kernel.h

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "schedule.h"

#include "sList.h"
#include "thread.h"
#include "PCB.h"

// Kod van OS-a koji se pokrece
extern int userMain(int argc, char* argv[]);
extern void tick();

typedef SortedList<Time, PCB*> TimeList;
typedef TimeList::Node* TimeListNode;

// Uspavane niti
extern TimeList timeBlocked;

// Sve niti koje su "zivi" objekti
extern PCBList threads;

// Glavna nit (nit koja izvrsava main)
// Koristi se za sistemski mod
extern PCB* mainThread;

// Trenutni broj niti u Scheduler-u
extern unsigned schedulerCount;

// Wrapper oko Scheduler::put()
inline void schedulerPut(PCB* &pcb) {
  // Scheduler::put - nije thread safe
  intLockBlock
  
  schedulerCount++;
  
  Scheduler::put(pcb);
}

// Wrapper oko Scheduler::get()
inline PCB* schedulerGet() {
  
  // Ako se nema budnih niti cekamo da se neka probudi (busy wait)
  while(schedulerCount == 0);
  
  // Scheduler::put - nije thread safe
  intLockBlock
  
  schedulerCount--;
  
  PCB* ret = Scheduler::get();
  
  return ret;
}

#endif  // _KERNEL_H_