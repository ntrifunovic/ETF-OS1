// File: PCB.h

#ifndef _PCB_H_
#define _PCB_H_

#include "thread.h"
#include "list.h"
#include "PCBlist.h"
#include "utility.h"
#include "kernel.h"

#define SP_DISP 20
#define SS_DISP 22
#define STACK_DISP 12
#define STACK_SIZE_DISP 4
#define OWNER_THREAD_SIZE_DISP 16

const StackSize MAX_STACK_SIZE = 65536;  // 64KB

class PCB {
  static ID MAX_ID;
  
public:
  const ID id;  // 0, 2
  const Time timeSlice;  // 2, 2
  const StackSize stackSize;  // 4, 4
  TName name;  // 8, 4
  
  void* stack;  // 12, 4
  Thread* const ownerThread;  // 16, 4
  
  unsigned sp, ss;  // 20, 2; 22, 2
  
  TimeListNode timeListPosition;
  PCBListNode waitingListPosition;
  
  // Da li je probudjena nit pre sa wakeUp (pre nego sto je trebala)
  volatile bool wokenUp;
  // Da li je zavrsena run metoda
  volatile bool completed;
  
  PCBList waitingForMeToComplete;
  PCBListNode threadsListPosition;
  
  PCB(TName _name,
      StackSize _stackSize,
      Time _timeSlice,
      Thread* _ownerThread
     );
     
  ~PCB();
  
  inline bool sleeping() const {
    return timeListPosition != NULL;
  }
  
  inline bool waiting() const {
    return waitingListPosition != NULL;
  }
  
  bool waitToComplete();  // Thread safe
  bool wakeUp();  // Thread safe
  void start();  // Thread safe
};
#endif // _PCB_H_ 

