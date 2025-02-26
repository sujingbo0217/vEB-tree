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
  size_t u;  // universe size
  size_t min_v;
  size_t max_v;
  std::vector<bool> occupy;  // summary
  std::vector<std::shared_ptr<node_t<T>>> cluster;

  explicit node_t(size_t n) : u(n) {
    if (n == 2) return;
    size_t m = static_cast<size_t>(std::sqrt(n));
    occupy.resize(m);
    cluster.resize(m);
    for (auto &it : cluster) {
      it = std::make_shared<node_t<T>>(m);
    }
  }
};

template<typename T>
class vEBTree {
 public:
  // vEBTree() = default;
  // explicit vEBTree(size_t u);
  vEBTree();
  ~vEBTree() = default;

 public:
  bool insert(T x);
  T find(T x) const;
  T succ(T x) const;
  T pred(T x) const;

 private:
  void _build(const std::shared_ptr<node_t<T>> &root);
  bool _insert(const std::shared_ptr<node_t<T>> &node, T x);
  T _find(const std::shared_ptr<node_t<T>> &node, T x) const;
  T _successor(const std::shared_ptr<node_t<T>> &node, T x) const;
  T _predecessor(const std::shared_ptr<node_t<T>> &node, T x) const;

  std::pair<T, T> _split(T v, size_t u) const {
    size_t offset = 1ULL << (static_cast<size_t>(std::log2(u) / 2));
    T high = static_cast<T>(v / offset);
    T low = v % static_cast<T>(offset);
    // T high = static_cast<T>(v / std::sqrt(u));
    // T low = v % static_cast<T>(std::sqrt(u));
    return std::make_pair(high, low);
  }

  T _combine(T high, T low, size_t u) const {
    size_t offset = 1ULL << (static_cast<size_t>(std::log2(u) / 2));
    return high * offset + low;
  }

 private:
  size_t _n;
  std::shared_ptr<node_t<T>> _root;
  size_t NIL;
};

}  // namespace vebtree

#endif  // __VEBTREE_HPP__