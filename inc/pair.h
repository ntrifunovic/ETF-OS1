// File: pair.h

#ifndef _PAIR_H_
#define _PAIR_H_

template <class T, class V>
struct pair {
  T first;
  V second;
  
  pair(T _first, V _second) : first(_first),
                              second(_second) {}
};

template <class T, class V>
inline pair<T,V>* makePair(T first, V second) {
  return new pair<T,V>(first, second);
}

#endif // _PAIR_H_