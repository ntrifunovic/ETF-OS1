#ifndef _INTLOCK_H_
#define _INTLOCK_H_

#define intLock { asm pushf; asm cli; }
#define intUnlock asm popf;

#endif