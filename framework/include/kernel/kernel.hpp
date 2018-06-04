#ifndef FRAMEWORK_KERNEL_HPP__
#define FRAMEWORK_KERNEL_HPP__

#include <map>

namespace framework {

template <class key, class F>
class Kernel {

private:
  std::map<key, F > callbacks_;

  public:

    void add(key type, F function) {
      callbacks_[type] = function;
    }

    template <class T >
    void process(key k, T data) {
      // do something clever with the arguments?
      callbacks_[k](data);
    }

};

}

#endif // FRAMEWORK_KERNELL_HPP__