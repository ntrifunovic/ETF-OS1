// File: utility.cpp

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include "lock.h"
#include "log.h"
#include "utility.h"

void* operator new(unsigned size) {
  //log3("\tLOG: Allocating %d bytes\n", size);
  
  intLockBlock
  
  void* ret = malloc(size);
  
  if(ret == NULL && size > 0) {
    #ifdef ERROR
    printf("\tERROR: !!!!!! OUT OF MEMORY !!!!!!");
    #endif
    
    log("\tERROR: !!!!!! OUT OF MEMORY !!!!!!");
    
    closeLog();
    
    exit(1); 
  }
    
  return ret;
}

void operator delete(void* loc) {
  //log3("\tLOG: Deallocating %p memory\n", loc);

  intLockBlock
   
  if(loc != NULL)
    free(loc);
}