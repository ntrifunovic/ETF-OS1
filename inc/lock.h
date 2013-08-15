// File: lock.h

#ifndef _LOCK_H_
#define _LOCK_H_

#include "utility.h"

#define intLockBlock InteruptLock interuptLock;

class InteruptLock {
  int flags;
  bool locked;
  
public:
  InteruptLock(bool lock = true);
  ~InteruptLock();
  
  void lock();  // blokira prekide
  void unlock();  // vraca u stanje pre poziva lock,
                  // ako lock nije pozvan nema efekta
};

#endif // _LOCK_H_