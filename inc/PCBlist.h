// File: PCBlist.h

#ifndef _PCB_LIST_H_
#define _PCB_LIST_H_

#include "list.h"
#include "thread.h"

typedef List<PCB> PCBList;
typedef List<PCB>::Node* PCBListNode;

//Nije Thread safe
PCB* findByName(const PCBList&, const TName&);

//Nije Thread safe
PCB* findByID(const PCBList&, const ID&);

#endif // _PCB_LIST_H_
