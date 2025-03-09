/**
 *  Author: Bob
 *  Van Emde Boas Tree
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

// TODO: Modify
// template<typename T>
// void vEBTree<T>::_del(node_t<T> &node, T x, size_t s) {
//   if (!node->min_v.has_value()) return;
//   // if x is V.min (check if it's the last node in the cluster)
//   if (x == node->min_v.value()) {
//     // get the first next cluster
//     auto nxt = node->summary->min_v;
//     if (!nxt.has_value()) {
//       // no nodes in cluster
//       node->min_v = node->max_v = std::nullopt;
//       return;
//     }
//     // not last node
//     // delete old & update V.min
//     // remove V.min from recursive storage
//     T new_min = _combine(*nxt, node->cluster[nxt.value()]->min_v.value(), node->u);
//     x = new_min;
//     node->min_v.emplace(new_min);
//   }
//   // recursively delete
//   auto [high, low] = _split(x, node->u);
//   self(self, node->cluster[high], low);
//   // delete summary if it's the last one
//   if (!node->cluster[high]->min_v.has_value()) {
//     self(self, node->summary, high);
//   }
//   // if x is V.max (update V.max)
//   if (x == node->max_v.value()) {
//     // get the last cluster
//     auto nxt = node->summary->max_v;
//     if (!nxt.has_value()) {
//       // delete the second last item (only one item left)
//       node->max_v = node->min_v;
//     } else {
//       // find the last item in the cluster
//       T new_max = _combine(nxt.value(), node->cluster[nxt.value()]->max_v.value(), node->u);
//       node->max_v.emplace(new_max);
//     }
//   }
// }

template<typename T>
bool vEBTree<T>::_find(const node_t<T> &node, T x, size_t s) const {
  if (x == node.min_v || x == node.max_v) return true;
  if (s <= 1) return false;
  auto [high, low] = _split(x, s / 2);
  if (node.A[high].min_v == NIL) {
    // return _find(node.A[1ULL << (s / 2)], high, s / 2);
    return false;
  }
  return _find(node.A[high], low, s / 2);
}

// pass u
// replace optional
// use upper_bound
template<typename T>
T vEBTree<T>::_succ(const node_t<T> &node, T x, size_t s) const {
    // lazy (V.min is not stored recursively)
    if (x < node.min_v) {
      return node.min_v;
    }

    if (s <= 1) {
      if (x == 0 && node.max_v == 1) {
        return 1;
      }
      return NIL;
    }

    auto [high, low] = _split(x, s / 2);
    if (low < node.A[high].max_v) {
      low = _succ(node.A[high], low, s / 2);
      if (low != NIL) return _combine(high, low, s / 2);
    }

    high = _succ(node.A[1ULL << (s / 2)], high, s / 2);
    if (high == NIL) return NIL;  // larger than the last
    low = node.A[high].min_v;
    return _combine(high, low, s / 2);
}

// template<typename T>
// std::optional<T> vEBTree<T>::predecessor(T x) const {
//   return std::nullopt;
// }

template class vebtree::vEBTree<uint32_t>;  // Eval
template class vebtree::vEBTree<uint16_t>;  // Test
