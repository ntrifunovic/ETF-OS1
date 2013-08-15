// File: list.h

#ifndef _LIST_H_
#define _LIST_H_

#include "utility.h"
#include "lock.h"

#include "log.h"

template <class T> 
class List {
public:
  class Cmp {
    public:
    // Cmp je funkcija koja vraca vrednost razlicitu od 0 za trazeni element
    virtual bool cmp(const T*) = 0;  
  };
  
  class Node {
    public:
    T* value;
    Node *previous, *next;
    
    // Pri kreiranju cvora update-uju se i susedi
    Node(T* _value, Node* _previous = NULL, Node* _next = NULL)
    : value(_value), previous(_previous), next(_next) {
      if (previous != NULL)
        previous->next = this;
      if (next != NULL)
        next->previous = this;
    }
    
    // Pri brisanju prevezuje se ostatka liste
    ~Node() {
      if (next != NULL)
        next->previous = previous;
      if (previous != NULL)
        previous->next = next;
    }
  
  };
  
  Node *start, *end;
  
private:
  Node* findNode(Cmp* cmp) const {
    for(Node* curr = start->next; curr != end; curr = curr->next)
      if (cmp->cmp(curr->value))
        return curr;
        
    return NULL;
  }
  
public:
  List() : start(new Node(NULL)),
                 end(new Node(NULL, start)) {}
 
  T* find(Cmp* cmp) const {
    Node* ret = findNode(cmp);
    return ret == NULL ? NULL : ret->value;
  }
  
  ~List() {
    while(start->next != NULL) {
      Node *temp = start->next;  // !!! GLUPI BC31 poziva destruktor sa
                                 // skrivenim parametrom po referenci
      delete temp;
    }
    delete start;
  }
  
  int empty() const {
    return start->next == end;
  }
  
  // Dodaje element na pocetak
  Node* add(T& value) {
    return pushFront(&value);
  }
  
  // Dodaje element na pocetak
  Node* add(T* value) {
    return pushFront(value);
  }
  
  Node* pushFront(T& value) {
    return pushFront(&value);
  }
  
  Node* pushFront(T* value) {
    return pushAfter(value, start);
  }
  
  Node* pushBack(T& value) {
    return pushBack(&value);
  }
  
  Node* pushBack(T* value) {
    return pushBefore(value, end);
  } 
  
  Node* pushAfter(T* value, Node* node) {
    return new Node(value, node, node->next);
  }
  
  Node* pushBefore(T* value, Node* node) {
    return new Node(value, node->previous, node);
  }
  
  // Vraca element sa pocetka bez uklanjanja iz lise
  T* peek() const {
    return start->next->value;
  }
  
  // Vraca element sa pocetka
  T* get() {
    return getFirst();
  }
  
  T* getFirst() { 
    if (start->next == end)
      return NULL;
    T* ret = start->next->value;
    Node* pom = start->next;
    delete pom; // !!! GLUPI BC31 poziva destruktor sa
                // skrivenim parametrom po referenci 
    return ret;
  }
  
  T* getLast() {
    if (end->previous == start)
      return NULL;
    T* ret = end->previous->value;
    Node* pom = end->previous;  // !!! GLUPI BC31 poziva destruktor sa
                                // skrivenim parametrom po referenci
    delete pom;
    return ret;
  }
  
  /*
  // Brise samo prvi pronadjen Node ne brise se vrednost
  void remove(Cmp cmp) {
    Node* del = findNode(cmp);
    delete del;
  }
  */
  
  void clear() {
    while(!empty())
      get();
  }
};
#endif // _LIST_H_ 