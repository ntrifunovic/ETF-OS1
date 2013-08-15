#include "lock.h"

#include <stdio.h>

InteruptLock::InteruptLock(bool _lock) : locked(false) {
  if(_lock)
    lock();
}
InteruptLock::~InteruptLock() {
  unlock();
}

void InteruptLock::lock() {
  if (locked)
    return;

 locked = true;

 int _flags;
    
  asm { 
        pushf
        pop _flags
        cli
  }
      
  flags = _flags;
}

void InteruptLock::unlock() {
  if(!locked) 
    return;

  int _flags = flags;
  
  asm {
        push _flags
        popf;
  }
  
  locked = false;
}