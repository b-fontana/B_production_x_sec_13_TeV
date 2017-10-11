#ifndef B_VECTOR
#define B_VECTOR

#include <type_traits>
#include <utility>

template <class T>
class BVector {
 public:
  static_assert(std::is_pointer<T>::value,"Class BVector can only be used with pointers"); //T has to be a pointer
  BVector(): v_prvt(nullptr),size_prvt(0) {};
  BVector(std::size_t);
  ~BVector();
  decltype(*std::declval<T>()) at(std::size_t);

 private:
  T v_prvt;
  std::size_t size_prvt;
};

#endif