// File: PCB.cpp

#include <string.h>

#include "schedule.h"

#include "utility.h"
#include "lock.h"
#include "sysmode.h"
#include "log.h"
#include "tControl.h"
#include "PCB.h"

ID PCB::MAX_ID = -2;

static TName generateDefaultName(ID id) {
  int size = 0;
  List<char> idList;
  
  while(id > 0) {
    idList.pushBack(new char(id%10 + '0'));
    
    id /= 10; 
    size++;
  }
  
  TName name;
  
  name = new char[size + 7];
  
  name[size + 6] = 0;
  
  {
    intLockBlock
    strcpy(name, "thread");
  }
  
  while(size--) {
    char* last = idList.getFirst();
    name[6 + size] = *last;
    
    delete last;
  }
      
  return name;
}

PCB::~PCB() {
 delete threadsListPosition;
  
  delete[] name;
  delete[] stack;
}

PCB::PCB(TName _name,
      StackSize _stackSize,
      Time _timeSlice,
      Thread* _ownerThread
     ) : stackSize(MIN(_stackSize, MAX_STACK_SIZE)),
         timeSlice(_timeSlice),
         ownerThread(_ownerThread),
         // Da se ne bi desilo da dva PCB-a imaju isti ID prefiksni ++
         id(++MAX_ID),
         timeListPosition(NULL),
         waitingListPosition(NULL),
         completed(true) {  // posto indeksi idu od 0
  if (id == -1) {
    runningThread = mainThread = this;
  } else {
    stack = new char[stackSize];
    
    intLockBlock  // zbog threads
    threadsListPosition = threads.add(this);
  }
   
  sp = FP_OFF(stack);  // pokazuje na pocetak niza
  ss = FP_SEG(stack);
  
  if (_name == NULL) {
      name = generateDefaultName(id);
  } else {
    intLockBlock  // strcpy i strlen
    
    name = new char[strlen(_name)];
    
    strcpy(name, _name);
  }
} 

bool PCB::waitToComplete() {
  { // Mora se uzeti lock,
    // problem ako se nit zavrsi posle provere if(completed)
    intLockBlock
    
    if(completed)
      return true;

    // waitingForMeToComplete - nije thread safe
    runningThread->waitingListPosition =
      waitingForMeToComplete.add(runningThread);
  }
  
  log("\tLOG: Thread [%s] waiting for [%s]\n",
           runningThread->name, name);
    
  
  dispatch(false);
  
  return completed;
}

bool PCB::wakeUp() {
  log("\tLOG: Waiking up thread [%s]\n", name);

  intLockBlock  // Moze nastati problem ako se stanje niti
                // promeni posle njegove provere
                // problem dva wakeUp()-a
  
  if (sleeping()) {   
    delete timeListPosition;
    
    timeListPosition = NULL; 
    wokenUp = true;
    
    schedulerPut(this);
    
    return true;
  }
  
  if (waiting()) {
    delete waitingListPosition;
    
    waitingListPosition = NULL;
    wokenUp = true;
    
    schedulerPut(this);
    
    return true;
  }
  
  return false;
}

// Ova metoda se poziva autmatski kada se zavrsi run metoda neke niti
static void far killThread() {
  log("\tLOG: Thread [%s] finished job\n",
      runningThread->name);
      
  check(!inSystemMode(), "killThread - System Mode error");
  
  runningThread->completed = true;
  
  { // waitingForMeToComplete nije Thread safe
    intLockBlock
    
    while(!runningThread->waitingForMeToComplete.empty()) {
      PCB* pcb = runningThread->waitingForMeToComplete.get();
      
      log("\tLOG: Unblocking [%s]\n", pcb->name);
      
      { // Scheduler::put - nije thread safe
        schedulerPut(pcb);
      }
    }
  }
  
  dispatch(false);
}

void PCB::start() {
  log("\tLOG: Starting thread [%s]\n", name);
  
  asm {
    les	bx, dword ptr this
    les	bx, dword ptr es:[bx+OWNER_THREAD_SIZE_DISP]
    les	bx, dword ptr es:[bx]
    
    // pamtimo adresu metode run()
    mov	ax, word ptr es:[bx+4] // SEG
    mov	dx, word ptr es:[bx+6] // OFF
    
    les	bx, dword ptr this
    
    // Kreiramo stack content za ovaj proces
    mov si, word ptr es:[bx+STACK_SIZE_DISP]
    sub si, 12
    
    les	bx, dword ptr es:[bx+STACK_DISP]
    
    mov	word ptr es:[bx][si], ax
    mov	word ptr es:2+[bx][si], dx
  }
 
  // Postavljanje konteksta steka
  //  NAPOMENA: sp je na pocetku stack[0] - kod u PCB konstruktoru
  {
    // old sp, tj. bp ovo se resetuje pri izlasku iz dispatch-a
    *(unsigned*)stack = FP_OFF(stack) + stackSize - 14;

    char* stackEnd = (char*)stack +  stackSize;

    // Posle metode run izvrsava se metoda killThread
    // (podesavamo return adress-u na stacku)
    *(long*)(stackEnd - 8) = (long)killThread;
    *(long*)(stackEnd - 4) = (long)ownerThread; // Parametar metode run
  }
  
  completed = false;
  
  { // Scheduler::put - nije thread safe
    intLockBlock
    schedulerPut(this);
  }
}