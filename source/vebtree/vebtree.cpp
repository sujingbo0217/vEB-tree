/**
 *  Author: Bob
 *  Van Emde Boas Tree
 **/

#include "../../include/vebtree/vebtree.hpp"

#include <cassert>

using namespace vebtree;

template<typename T>
vEBTree<T>::vEBTree()
    : _n(static_cast<size_t>(std::numeric_limits<T>::max()) + 1), NIL((size_t)-1) {
  _root = std::make_shared<node_t<T>>(_n);
  _build(_root);
}

template<typename T>
void vEBTree<T>::_build(const std::shared_ptr<node_t<T>> &root) {
  if (!root || root->u <= 2) return;
  size_t n = static_cast<size_t>(std::sqrt(root->u));
  for (auto &node : root->cluster) {
    if (!node) node = std::make_shared<node_t<T>>(n);
    node->min_v = NIL;
    node->max_v = NIL;
    _build(node);
  }
}

template<typename T>
bool vEBTree<T>::insert(T x) {
  bool ok = _insert(_root, x);
  return ok;
}

template<typename T>
bool vEBTree<T>::_insert(const std::shared_ptr<node_t<T>> &node, T x) {
  if (!node) return false;
  if (node->min_v == NIL && node->max_v == NIL) {
    node->min_v = node->max_v = x;
    return true;
  }
  if (x == (T)node->min_v || x == (T)node->max_v) return true;
  if (x < (T)node->min_v) {
    T tmp = (T)node->min_v;
    node->min_v = x;
    x = tmp;
  }
  if (x > (T)node->max_v) node->max_v = x;
  if (node->u <= 2) return true;

  auto [high, low] = _split(x, node->u);
  node->occupy[high] = true;
  return _insert(node->cluster[high], low);
}

template<typename T>
T vEBTree<T>::find(T x) const {
  T res = _find(_root, x);
  return res;
}

template<typename T>
T vEBTree<T>::_find(const std::shared_ptr<node_t<T>> &node, T x) const {
  if (!node) return NIL;
  if (x == node->min_v || x == node->max_v) return x;
  auto [high, low] = _split(x, node->u);
  assert(node->occupy[high]);
  T res = _find(node->cluster[high], low);
  return _combine(high, res, node->u);
}

template<typename T>
T vEBTree<T>::succ(T x) const {
  T ans = _successor(_root, x);
  return ans;
}

template<typename T>
T vEBTree<T>::_successor(const std::shared_ptr<node_t<T>> &node, T x) const {
  if (!node || node->max_v < x) return NIL;
  if (node->min_v != NIL && x <= (T)node->min_v) return (T)node->min_v;
  if (node->u <= 2) {
    if (x == 0 && node->max_v == 0) return 0;
    if ((x == 0 || x == 1) && node->max_v == 1) return 1;
    return NIL;
  }

  auto [high, low] = _split(x, node->u);
  if (node->occupy[high]) {
    auto low_max_v = node->cluster[high]->max_v;
    if (low_max_v != NIL && low <= low_max_v) {
      // search in the current widget
      T res = _successor(node->cluster[high], low);
      if (res != NIL) return _combine(high, res, node->u);
    }
    ++high;
  }

  // find the first non-empty widget in cluster
  size_t m = node->cluster.size();
  while (high < static_cast<T>(m) && !node->occupy[high]) ++high;
  if (high == m || node->cluster[high]->min_v == NIL) return NIL;
  T res = (T)node->cluster[high]->min_v;
  return _combine(high, res, node->u);
}

template<typename T>
T vEBTree<T>::pred(T x) const {
  T ans = _predecessor(_root, x);
  return ans;
}

template<typename T>
T vEBTree<T>::_predecessor(const std::shared_ptr<node_t<T>> &node, T x) const {
  return 0;
}

template class vebtree::vEBTree<uint32_t>;  // Eval
template class vebtree::vEBTree<uint16_t>;  // Test
