#include "UserCode/B_production_x_sec_13_TeV/interface/BVector.h"
#include <cassert>

template <class T>
BVector<T>::BVector(std::size_t s) {
  if (v_prvt != nullptr) {
    v_prvt = new T[s];
  }
}

template <class T>
BVector<T>::~BVector() {
  if (v_prvt!=nullptr) delete [] v_prvt;
}

template <class T>
decltype(*std::declval<T>()) BVector<T>::at(std::size_t index) {
  if (index < size_prvt) {
    return v_prvt[index];
  }
  assert(false);
}


