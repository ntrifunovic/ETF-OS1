// File: log.cpp

#include "utility.h"
#include "lock.h"
#include "log.h"

#include "list.h"
#include "pair.h"

#ifdef LOG

FILE* logFile = stdout;

volatile bool logging = false;

// static List< pair<const char*, va_list> > buffer;

void vlog(const char* format, va_list &arglist) {
  {
    intLockBlock
    
    if (logging)
      return;
 
    // buffer.pushBack(makePair(format, arglist));

    logging = true;
  }
  
  { // lock za buffer b 
    // InteruptLock lock(true);
    
    // while(!buffer.empty()) {
      // pair<const char*, va_list>* curr =  buffer.getFirst();
      
      // check(curr != NULL, "curr == 0, log.cpp");
      
      // lock.unlock();
      
      { // Da bi se zakljucali prekidi po izlasku
        // iz vfprintf ako su bili zakljucani pre ulaska
        intLockBlock
        
        vfprintf(logFile, format, arglist);
      }
      
      // delete curr;
      // lock.lock();
    // }
  }
  
  logging = false; 
}

#endif // LOG