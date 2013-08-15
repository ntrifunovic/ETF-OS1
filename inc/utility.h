// File: utility.h

#ifndef _UTILITY_H_
#define _UTILITY_H_

#define IVTPENTRYSIZE 0x4

typedef void interrupt (*PrekidnaRutina)();

#define FP_SEG( fp )( (unsigned )( void _seg * )( void far * )( fp ))
#define FP_OFF( fp )( (unsigned )( fp ))

#define NULL 0L

#define true 1
#define false 0

typedef int bool;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

#include <stdlib.h>
#include "log.h"

#define CHECK

inline void check(bool cond, char* name) {
  #ifdef CHECK
  if(!cond) {
    log("CHECK <%s> FAILED", name);
    exit(1);
  }
  #endif
}

#define SAVE_REGISTERS\
asm {\
  push ax;\
  push bx;\
  push cx;\
  push dx;\
  push es;\
  push si;\
  push di;\
  push bp;\
}

#define RESTORE_REGISTERS\
asm {\
  pop bp;\
  pop di;\
  pop si;\
  pop es;\
  pop dx;\
  pop cx;\
  pop bx;\
  pop ax;\
}

void* far operator new(unsigned size);
void far operator delete(void *);

#endif // _UTILITY_H_