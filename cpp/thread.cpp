// File: thread.cpp

#include <stdio.h>

#include "schedule.h"

#include "utility.h"
#include "lock.h"
#include "log.h"
#include "sysmode.h"
#include "PCB.h"
#include "PCBlist.h"
#include "sList.h"
#include "kernel.h"
#include "tControl.h"
#include "thread.h"

PCBList threads;

void dispatch() {
  dispatch(true);
}

Thread::Thread(TName name,
               StackSize stackSize,
               Time timeSlice) : 
                  myPCB(new PCB(name, stackSize, timeSlice, this)) {}

Thread::~Thread() {
  while(!waitToComplete());

  delete myPCB;
}

ID Thread::getId() {
  return myPCB->id;
}

TName Thread::getName() {
  return myPCB->name;
}

Thread* Thread::getThreadById(ID id) {
  PCB* ret;
  
  { // zbog threads
    intLockBlock
    
    ret = findByID(threads, id);
  }
  
  return ret == NULL ? NULL : ret->ownerThread;
}

ID _getIdOf(TName name) {
  log3("\tLOG: getIdOf([%s])\n", name);

  PCB* pcb;
  
  { // zbog threads
    intLockBlock
    
    pcb = findByName(threads, name);
  }
  
  return pcb == NULL ? -1 : pcb->id;
}

// !!! Vraca se -1 ako ne postoji nit sa ovim nazivom
ID Thread::getIdOf(TName name) {
  static TName _name;
  static ID ret;
  
  ID _ret;
  
  ENTERSYSTEMMODE1(name)
  
  ret = _getIdOf(_name);
  
  EXITSYSTEMMODE1(ret) 
  
  return _ret;
}

bool Thread::waitToComplete() {
  return myPCB->waitToComplete();
}

bool Thread::wakeUp() {
  static const Thread* _this;
  static bool ret;
  
  bool _ret;
  
  ENTERSYSTEMMODE1(this)
  
  ret = _this->myPCB->wakeUp();
  
  EXITSYSTEMMODE1(ret) 
  
  return _ret;
}

void Thread::start() {
  static const Thread* _this;
  
  ENTERSYSTEMMODE1(this)
  
  _this->myPCB->start();
  
  EXITSYSTEMMODE
}

void prepareSleep(Time timeToSleep) {
  log("\tLOG: Going to sleep [%s]\n", runningThread->name);
  
  runningThread->wokenUp = false;
  
  { // timeBlocked nije thread safe
    intLockBlock 
    
    runningThread->timeListPosition = 
        timeBlocked.add(timeToSleep, runningThread);
  }
}

int Thread::sleep(Time timeToSleep) {
  static Time _timeToSleep;
  
  ENTERSYSTEMMODE1(timeToSleep)
  
  prepareSleep(_timeToSleep);
  
  EXITSYSTEMMODE
  
  dispatch(false);

  return !runningThread->wokenUp;
}
