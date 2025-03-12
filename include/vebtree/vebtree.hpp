/**
 *  Author: Bob
 *  van Emde Boas Tree
 **/

#ifndef __VEBTREE_HPP__
#define __VEBTREE_HPP__

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <memory>
// #include <optional>
// #include <vector>

namespace vebtree {

template<typename T>
struct node_t {
  // 16B
  T min_v;
  T max_v;
  std::unique_ptr<node_t<T>[]> A;  // [cluster[], summary]

  node_t() : min_v((T)-1), max_v((T)-1), A(nullptr) {}

  explicit node_t(size_t s) : min_v((T)-1), max_v((T)-1) {
    if (s <= 1) return;
    size_t n = (1 << (s / 2)) + 1;
    A = std::make_unique<node_t<T>[]>(n);
  }
};

template<typename T>
class vEBTree {
 public:
  vEBTree();
  ~vEBTree() = default;

 public:
  void insert(T x) {
    _ins(_root, x, MAX_BITS);
  }
  void remove(T x) { 
    _del(_root, x, MAX_BITS); 
  }
  bool find(T x) const {
    return _find(_root, x, MAX_BITS);
  }
  T successor(T x) const {
    return _succ(_root, x, MAX_BITS);
  }
  T predecessor(T x) const {
    return _pred(_root, x, MAX_BITS);
  }

 private:
  void _ins(node_t<T> &node, T x, size_t s);
  void _del(node_t<T> &node, T x, size_t s);
  bool _find(const node_t<T> &node, T x, size_t s) const;
  T _succ(const node_t<T> &node, T x, size_t s) const;
  T _pred(const node_t<T> &node, T x, size_t s) const;

  inline std::pair<T, T> _split(T v, size_t s) const {
    T high = static_cast<T>(v / (1ULL << s));
    T low = v % static_cast<T>(1ULL << s);
    return std::make_pair(high, low);
  }

  inline T _concat(T high, T low, size_t s) const {
    return high * static_cast<T>(1ULL << s) + low;
  }

 private:
  size_t MAX_BITS = std::numeric_limits<T>::digits;
  T NIL = (T)-1;
  node_t<T> _root;
};

}  // namespace vebtree

#endif  // __VEBTREE_HPP__