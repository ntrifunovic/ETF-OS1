// File: PCBlist.cpp

#include <string.h>

#include "PCB.h"
#include "lock.h"
#include "PCBlist.h"

class CmpName : public List<PCB>::Cmp {
const TName &name;
public:
  CmpName(const TName &_name) : name(_name) { }
  bool cmp(const PCB* test) {
    // Zbog strcmp
    intLockBlock

    return strcmp(test->name, name) == 0;
  }  
};

class CmpID : public List<PCB>::Cmp {
const ID &id;
public:
  CmpID(const ID &_id) : id(_id) { }
  bool cmp(const PCB* test) {
     return test->id == id;
  }  
};

PCB* findByName(const PCBList& list, const TName& name) {
  CmpName cmpName(name);
  return list.find(&cmpName);
}

PCB* findByID(const PCBList& list, const ID& id) {
  CmpID cmpID(id);
  return list.find(&cmpID);
}
