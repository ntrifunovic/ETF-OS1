// File: event.cpp

#include <stdlib.h>

#include "event.h"

#define OLDIVTPSTART 0x100

static List<IVTEntry> IVTEntries;

class CmpIVTNo : public List<IVTEntry>::Cmp {
const IVTNo &ivtNo;
public:
  CmpIVTNo(const IVTNo &_ivtNo) : ivtNo(_ivtNo) { }
  bool cmp(const IVTEntry* test) {
     return test->ivtNo == ivtNo;
  }  
};

IVTEntry* findIVTEntry(const List<IVTEntry> &list, const IVTNo& ivtNo) {
  CmpIVTNo cmpIVTNo(ivtNo);
  IVTEntry* ret;
  
  ret = list.find(&cmpIVTNo);
  
  return ret;
}

IVTEntry::IVTEntry(IVTNo _ivtNo, PrekidnaRutina _newInterrupt) :
    ivtNo(_ivtNo), newInterrupt(_newInterrupt) {

  IVTEntries.add(this);
  
  PrekidnaRutina *ptr;
  
  FP_SEG(ptr) = 0;
  FP_OFF(ptr) = ivtNo*IVTPENTRYSIZE;
    
  FP_SEG(oldInterrupt) = FP_SEG(*ptr);
  FP_OFF(oldInterrupt)  = FP_OFF(*ptr);
  
  {
    intLockBlock
    
    FP_SEG(*ptr) = FP_SEG(newInterrupt);
    FP_OFF(*ptr) = FP_OFF(newInterrupt);
  }
  
  FP_OFF(ptr) = (OLDIVTPSTART + ivtNo)*IVTPENTRYSIZE;
  
  FP_SEG(*ptr) = FP_SEG(oldInterrupt);
  FP_OFF(*ptr) = FP_OFF(oldInterrupt);
}

IVTEntry::~IVTEntry() {
  PrekidnaRutina *ptr;
  
  FP_SEG(ptr) = 0;
  FP_OFF(ptr) = ivtNo*IVTPENTRYSIZE;
  
  {
    intLockBlock
    
    FP_SEG(*ptr) = FP_SEG(oldInterrupt);
    FP_OFF(*ptr) = FP_OFF(oldInterrupt);
  }
}

KernelEv::KernelEv(IVTNo _ivtNo) : 
                                   ivtNo(_ivtNo),
                                   semaphore(0),
                                   eventListPosition(NULL) {  
  intLockBlock
  
  eventListPosition =
    findIVTEntry(IVTEntries, ivtNo)->events.add(this);

}

KernelEv::~KernelEv() {
  intLockBlock
  
  delete eventListPosition;
}

//------------------------------------------------------------------
//-------------------------------INTERFEJS--------------------------
//------------------------------------------------------------------

Event::Event(IVTNo ivtNo) : myImpl(new KernelEv(ivtNo)) {}

Event::~Event() {
  delete myImpl;
}

int Event::wait() {
  return myImpl->wait();
}

void Event::signal() {
  myImpl->signal();
}
