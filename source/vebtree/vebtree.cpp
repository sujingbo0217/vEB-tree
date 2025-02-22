/**
 *  Author: Bob
 *  Van Emde Boas Tree
 **/

#include "../../include/vebtree/vebtree.hpp"

using namespace vebtree;

template<typename T>
vEBTree<T>::vEBTree(size_t u) {
  _update_u(u);
  _root = std::make_shared<node_t<T>>(_u);
  _build(_root, _u);
}

template<typename T>
void vEBTree<T>::_build(const std::shared_ptr<node_t<T>> &root, size_t u) {
  auto [high, low] = _split(u);
  for (auto &node : root->A) {
    node = std::make_shared<node_t<T>>(high);
  }
}

template<typename T>
bool vEBTree<T>::insert(T x) {
  bool ok = _insert(_root, x);
  return ok;
}

template<typename T>
bool vEBTree<T>::_insert(const std::shared_ptr<node_t<T>> &node, T x) {
  if (node->occupy.size() < 4) return false;
  if (node->min_v == std::numeric_limits<T>::max() &&
      node->max_v == std::numeric_limits<T>::min()) {
    node->min_v = node->max_v = x;
    return true;
  }
  if (x < node->min_v) std::swap(x, node->min_v);
  if (x > node->max_v) std::swap(x, node->max_v);
  if (x == node->min_v || x == node->max_v) return true;
  auto [high, low] = _split(x);
  if (!node->occupy[high]) {
    node->occupy[high] = true;
    return _insert(node->A[high], low);
  }
  return false;
}

template<typename T>
bool vEBTree<T>::find(T x) const {
  bool found = _find(_root, x);
  return found;
}

template<typename T>
bool vEBTree<T>::_find(const std::shared_ptr<node_t<T>> &root, T x) const {
  if (x == root->min_v || x == root->max_v) return true;
  auto [high, low] = _split(x);
  if (!root->occupy[high]) return false;
  return _find(root->A[high], low);
}

template<typename T>
T vEBTree<T>::succ(T x) const {
  T ans = _successor(_root, x);
  return ans;
}

template<typename T>
T vEBTree<T>::_successor(const std::shared_ptr<node_t<T>> &root, T x) const {
  if (x > root->max_v) return -1;
  if (x <= root->min_v) return root->min_v;
  auto [high, low] = _split(x);
  T ans = _successor(root->A[high], low);
  if (ans != -1) return ans;
  while (high + 1 < static_cast<T>(root->occupy.size()) && !root->occupy[++high]);
  ans = _successor(root->A[high], 0);
  return ans;
}

template<typename T>
T vEBTree<T>::pred(T x) const {
  T ans = _predecessor(_root, x);
  return ans;
}

template<typename T>
T vEBTree<T>::_predecessor(const std::shared_ptr<node_t<T>> &root, T x) const {
  if (x < root->min_v) return -1;
  if (x >= root->max_v) return root->max_v;
  auto [high, low] = _split(x);
  T ans = _predecessor(root->A[high], low);
  if (ans != -1) return ans;
  while (high - 1 >= 0 && !root->occupy[--high]);
  ans = _predecessor(root->A[high], std::numeric_limits<T>::max());
  return ans;
}