// File: sList.h

#ifndef _S_LIST_H_
#define _S_LIST_H_

#include <stdio.h>

#include "list.h"
#include "pair.h"

template <class TKEY, class TVALUE>
class SortedList{
  typedef List< pair<TKEY, TVALUE> > ListOfPairs;
  ListOfPairs lista;
public:
  typedef ListOfPairs::Node Node;

  // Smanjuje vrednost svih kljuceva za 1
  void decrase();
  
  Node* add(TKEY key, TVALUE value) {
   for(Node* cvor = lista.start->next; cvor != lista.end && cvor->value->first < key; cvor =  cvor->next);
   return lista.pushBefore(makePair(key, value), cvor);
  }
  
  int empty() {
    return lista.empty();
  }
  
  TKEY peekKey() {
    return lista.peek()->first;
  }
  
  TVALUE peekValue() {
    return lista.peek()->second;
  }
  
  TVALUE get();
};

template <class TKEY, class TVALUE>
void SortedList<TKEY, TVALUE>::decrase() {
  for(Node* cvor = lista.start->next; cvor != lista.end; cvor =  cvor->next)
    cvor->value->first--;
}

template <class TKEY, class TVALUE>
TVALUE SortedList<TKEY, TVALUE>::get() {
   pair<TKEY, TVALUE>* ret = lista.get();
   
   if (ret == NULL)
    return NULL;
    
   TVALUE value = ret->second;
   delete ret;
   
   return value;
}

#endif // _S_LIST_H_
