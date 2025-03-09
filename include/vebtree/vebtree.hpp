/**
 *  Author: Bob
 *  Van Emde Boas Tree
 **/

#ifndef __VEBTREE_HPP__
#define __VEBTREE_HPP__

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <vector>

namespace vebtree {

template<typename T>
struct node_t {
  size_t u;  // universe size 8 -> 0
  std::optional<T> min_v; // 8 -> 4
  std::optional<T> max_v; // 8 -> 4
  std::unique_ptr<node_t<T>> summary; // 8 -> (0)
  std::vector<std::unique_ptr<node_t<T>>> cluster;  // TODO: optimize using hash table 24
  // std::unique_ptr<std::unique_ptr<node_t<T>>[]> cluster(n); 8
  
  // unique_ptr<char> p;
  // char* ip;
  // ip = new char[10];
  // auto p = std::make_unique_for_overwirte<node_t<T>[]>(n);

  explicit node_t(size_t n) : u(n), min_v(std::nullopt), max_v(std::nullopt) {
    if (n <= 2) return;
    size_t m = static_cast<size_t>(std::sqrt(n)); // 1 << (u / 2)
    summary = nullptr;
    cluster.resize(m);
    // for (auto &it : cluster) {
    //   it = std::make_unique<node_t<T>>(m);
    // }
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
  void insert(T x);
  void remove(T x);
  bool find(T x) const;
  std::optional<T> successor(T x, bool lower_bound = false) const;
  std::optional<T> predecessor(T x) const;

 private:
  inline std::pair<T, T> _split(T v, size_t u) const {
    T high = static_cast<T>(v / std::sqrt(u));
    T low = v % static_cast<T>(std::sqrt(u));
    return std::make_pair(high, low);
  }

  inline T _combine(T high, T low, size_t u) const {
    return high * static_cast<T>(std::sqrt(u)) + low;
  }

 private:
  size_t _n;
  std::unique_ptr<node_t<T>> _root;
};

}  // namespace vebtree

#endif  // __VEBTREE_HPP__