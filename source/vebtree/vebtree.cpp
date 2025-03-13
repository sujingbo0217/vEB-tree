/**
 *  Author: Bob
 *  van Emde Boas Tree
 **/

#include "../../include/vebtree/vebtree.hpp"

using namespace vebtree;

template<typename T>
vEBTree<T>::vEBTree() : _root(MAX_BITS) {
  auto _build = [&](auto &self, node_t<T> &root, size_t s) {
    if (s <= 1) return;
    size_t n = (1ULL << (s / 2));
    for (size_t i = 0; i <= n; ++i) {
      root.A[i] = node_t<T>(s / 2);
      self(self, root.A[i], s / 2);
    }
  };
  _build(_build, _root, MAX_BITS);
}

template<typename T>
void vEBTree<T>::_ins(node_t<T> &node, T x, size_t s) {
  // lazy (every item except V.min will be recursively inserted)
  if (node.min_v == NIL) {
    node.min_v = node.max_v = x;
    return;
  }
  if (x < node.min_v) std::swap(node.min_v, x);
  if (x > node.max_v) node.max_v = x;
  if (s <= 1) return;
  auto [high, low] = _split(x, s / 2);
  if (node.A[high].min_v == NIL) {
    _ins(node.A[1ULL << (s / 2)], high, s / 2);
  }
  _ins(node.A[high], low, s / 2);
}

template<typename T>
void vEBTree<T>::_del(node_t<T> &node, T x, size_t s) {
  if (node.max_v == node.min_v) {
    // only the last node
    node.min_v = node.max_v = NIL;
    return;
  }
  // base case (w 0 and 1 only)
  if (s <= 1) {
    node.max_v = node.min_v = (x == 0 ? 1 : 0);
    return;
  }
  // if x is V.min (check if it's the last node in the cluster)
  if (x == node.min_v) {
    // get the first cluster id
    T next = node.A[1ULL << (s / 2)].min_v;
    if (next == NIL) {
      node.min_v = node.max_v;
      return;
    }
    // not last node
    // delete old & update V.min
    // remove new V.min from recursive storage
    x = node.min_v = _concat(next, node.A[next].min_v, s / 2);
  }
  // recursively delete
  auto [high, low] = _split(x, s / 2);
  _del(node.A[high], low, s / 2);
  // delete summary if it's the last one
  if (node.A[high].min_v == NIL) {
    _del(node.A[1ULL << (s / 2)], high, s / 2);
  }
  // if x is V.max (update V.max)
  if (x == node.max_v) {
    // get the last cluster id
    T last = node.A[1ULL << (s / 2)].max_v;
    if (last == NIL) {
      // delete the second last item (only one item left)
      node.max_v = node.min_v;
    } else {
      // find the last item in the cluster
      node.max_v = _concat(last, node.A[last].max_v, s / 2);
    }
  }
}

template<typename T>
bool vEBTree<T>::_find(const node_t<T> &node, T x, size_t s) const {
  if (x == node.min_v || x == node.max_v) return true;
  if (s <= 1) return false;
  auto [high, low] = _split(x, s / 2);
  if (node.A[high].min_v == NIL) return false;
  return _find(node.A[high], low, s / 2);
}

template<typename T>
T vEBTree<T>::_succ(const node_t<T> &node, T x, size_t s) const {
  // lazy (V.min is not stored recursively)
  if (x < node.min_v) return node.min_v;
  if (s <= 1) {
    if (x == 0 && node.max_v == 1) {
      return 1;
    }
    return NIL;
  }
  auto [high, low] = _split(x, s / 2);
  if (low < node.A[high].max_v) {
    low = _succ(node.A[high], low, s / 2);
    if (low != NIL) return _concat(high, low, s / 2);
  }
  high = _succ(node.A[1ULL << (s / 2)], high, s / 2);
  if (high == NIL) return NIL;  // larger than the last
  low = node.A[high].min_v;
  return _concat(high, low, s / 2);
}

template<typename T>
T vEBTree<T>::_pred(const node_t<T> &node, T x, size_t s) const {
  if (node.max_v != NIL && x > node.max_v) return node.max_v;
  if (s <= 1) {
    if (x == 1 && node.min_v == 0) {
      return 0;
    }
    return NIL;
  }
  auto [high, low] = _split(x, s / 2);
  if (node.A[high].min_v != NIL && low > node.A[high].min_v) {
    low = _pred(node.A[high], low, s / 2);
    if (low != NIL) return _concat(high, low, s / 2);
  }
  high = _pred(node.A[1ULL << (s / 2)], high, s / 2);
  if (high == NIL) return NIL;
  low = node.A[high].max_v;
  return _concat(high, low, s / 2);
}

template class vebtree::vEBTree<uint32_t>;  // Eval
template class vebtree::vEBTree<uint16_t>;  // Test
