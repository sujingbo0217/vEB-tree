/**
 *  Author: Bob
 *  Van Emde Boas Tree
**/

#ifndef __VEBTREE_HPP__
#define __VEBTREE_HPP__

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

namespace vebtree {

template<typename T>
struct node_t {
  size_t _n;
  T min_v;
  T max_v;
  std::vector<bool> occupy;
  std::vector<std::shared_ptr<node_t<T>>> A;  // cluster

  explicit node_t(size_t n)
      : _n(n),
        min_v(std::numeric_limits<T>::max()),
        max_v(std::numeric_limits<T>::min()) {
    if (n <= 2) {
      // (high bit, low bit)
      return;
    }
    size_t sub_n = static_cast<size_t>(std::ceil(std::sqrt(n)));
    occupy.resize(sub_n);
    A.resize(sub_n);
    for (auto &a : A) {
      a = std::make_shared<node_t<T>>(sub_n);
    }
  }
};

template<typename T>
class vEBTree {
 public:
  vEBTree() = default;
  explicit vEBTree(size_t u);
  ~vEBTree() = default;

 public:
  bool insert(T x);
  bool find(T x) const;
  T succ(T x) const;
  T pred(T x) const;

 private:
  void _build(const std::shared_ptr<node_t<T>> &root, size_t u);
  bool _insert(const std::shared_ptr<node_t<T>> &node, T x);
  bool _find(const std::shared_ptr<node_t<T>> &node, T x) const;
  T _successor(const std::shared_ptr<node_t<T>> &node, T x) const;
  T _predecessor(const std::shared_ptr<node_t<T>> &node, T x) const;

  size_t _update_u(const size_t u) {
    size_t mx = std::numeric_limits<size_t>::max();
    size_t low = 1;
    size_t high = std::floor(std::log2(mx));
    while (high - low > 1) {
      size_t mid = (high - low) / 2 + low;
      if (std::floor(std::pow(2, std::pow(2, mid))) >= u) {
        high = mid;
      } else {
        low = mid;
      }
    }
    _u = std::pow(2, std::pow(2, high));
    return _u;
  }

  std::pair<T, T> _split(T v) {
    size_t offset = static_cast<size_t>(std::log2(v));
    T high = (v >> offset);
    T low = v - (high << offset);
    return std::make_pair(high, low);
  }

 private:
  size_t _u;
  std::shared_ptr<node_t<T>> _root;
};

}  // namespace vebtree

#endif  // __VEBTREE_HPP__