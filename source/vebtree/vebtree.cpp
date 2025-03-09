/**
 *  Author: Bob
 *  Van Emde Boas Tree
 **/

#include "../../include/vebtree/vebtree.hpp"

using namespace vebtree;

template<typename T>
vEBTree<T>::vEBTree() : _n(static_cast<size_t>(std::numeric_limits<T>::max()) + 1) {
  _root = std::make_unique<node_t<T>>(_n);
  auto _build = [&](auto &self, const std::unique_ptr<node_t<T>> &root) {
    if (!root || root->u <= 2) return;
    size_t n = static_cast<size_t>(std::sqrt(root->u));

    root->summary = std::make_unique<node_t<T>>(n);
    self(self, root->summary);

    for (auto &node : root->cluster) {
      node = std::make_unique<node_t<T>>(n);
      self(self, node);
    }
  };
  _build(_build, _root);
}

template<typename T>
void vEBTree<T>::insert(T x) {
  auto _insert = [&](auto &&self, const std::unique_ptr<node_t<T>> &node, T x) {
    // lazy (every item except V.min will be recursively inserted)
    if (!node->min_v.has_value()) {
      node->min_v = node->max_v = x;
      return;
    }
    if (x < node->min_v.value()) {
      std::swap(*(node->min_v), x);
      // T tmp = node->min_v.value();
      // node->min_v.emplace(x);
      // x = tmp;
    }
    if (x > node->max_v) node->max_v.emplace(x);
    if (node->u <= 2) return;

    auto [high, low] = _split(x, node->u);
    if (!node->cluster[high]->min_v.has_value()) {
      self(self, node->summary, high);
    }
    self(self, node->cluster[high], low);
  };
  _insert(_insert, _root, x);
}

template<typename T>
void vEBTree<T>::remove(T x) {
  auto _remove = [&](auto &&self, const std::unique_ptr<node_t<T>> &node, T x) {
    if (!node->min_v.has_value()) return;
    // if x is V.min (check if it's the last node in the cluster)
    if (x == node->min_v.value()) {
      // get the first next cluster
      auto nxt = node->summary->min_v;
      if (!nxt.has_value()) {
        // no nodes in cluster
        node->min_v = node->max_v = std::nullopt;
        return;
      }
      // not last node
      // delete old & update V.min
      // remove V.min from recursive storage
      T new_min = _combine(*nxt, node->cluster[nxt.value()]->min_v.value(), node->u);
      x = new_min;
      node->min_v.emplace(new_min);
    }
    // recursively delete
    auto [high, low] = _split(x, node->u);
    self(self, node->cluster[high], low);
    // delete summary if it's the last one
    if (!node->cluster[high]->min_v.has_value()) {
      self(self, node->summary, high);
    }
    // if x is V.max (update V.max)
    if (x == node->max_v.value()) {
      // get the last cluster
      auto nxt = node->summary->max_v;
      if (!nxt.has_value()) {
        // delete the second last item (only one item left)
        node->max_v = node->min_v;
      } else {
        // find the last item in the cluster
        T new_max = _combine(nxt.value(), node->cluster[nxt.value()]->max_v.value(), node->u);
        node->max_v.emplace(new_max);
      }
    }
  };
  _remove(_remove, _root, x);
}

template<typename T>
bool vEBTree<T>::find(T x) const {
  const auto _find = [&](auto &&self, const std::unique_ptr<node_t<T>> &node, T x) -> bool {
    if (x == node->min_v.value() || x == node->max_v.value()) return true;
    if (node->u <= 2) return false;
    auto [high, low] = _split(x, node->u);
    if (!node->cluster[high]->min_v.has_value()) {
      return self(self, node->summary, high);
    }
    return self(self, node->cluster[high], low);
  };
  return _find(_find, _root, x);
}

// pass u
// replace optional
// use upper_bound
template<typename T>
std::optional<T> vEBTree<T>::successor(T x, bool is_lower_bound) const {
  const auto lower_bound = [&](auto &&self, const std::unique_ptr<node_t<T>> &node, T x,
                               bool is_summary = false) -> std::optional<T> {
    if (is_summary) {
      // lazy (V.min is not stored recursively)
      if (node->min_v.has_value() && x < node->min_v.value()) {
        return node->min_v.value();
      }

      if (node->u <= 2) {
        if (x == 0 && node->max_v.value_or(0) == 1) {
          return 1;
        }
        return std::nullopt;
      }

      auto [high, low] = _split(x, node->u);
      if (node->cluster[high]->max_v.has_value() && low < node->cluster[high]->max_v.value()) {
        auto res = self(self, node->cluster[high], low, true);
        if (res.has_value()) return _combine(high, res.value(), node->u);
      }

      auto succ = self(self, node->summary, high, true);
      if (!succ.has_value()) return std::nullopt;  // larger than the last
      high = succ.value();
      low = node->cluster[high]->min_v.value();
      return _combine(high, low, node->u);
    }

    // lazy (V.min is not stored recursively)
    if (node->min_v.has_value() && x <= node->min_v.value()) {
      return node->min_v.value();
    }

    if (node->u <= 2) {
      if (node->max_v.has_value() && node->min_v.value() < node->max_v.value()
          && x <= node->max_v.value()) {
        return node->max_v.value();
      }
      return std::nullopt;
    }

    auto [high, low] = _split(x, node->u);
    if (node->cluster[high]->max_v.has_value() && low <= node->cluster[high]->max_v.value()) {
      auto res = self(self, node->cluster[high], low, false);
      if (res.has_value()) return _combine(high, res.value(), node->u);
    }

    auto succ = self(self, node->summary, high, true);
    if (!succ.has_value()) return std::nullopt;  // larger than the last
    high = succ.value();
    low = node->cluster[high]->min_v.value();
    return _combine(high, low, node->u);
  };

  const auto upper_bound
      = [&](auto &&self, const std::unique_ptr<node_t<T>> &node, T x) -> std::optional<T> {
    // lazy (V.min is not stored recursively)
    if (node->min_v.has_value() && x < node->min_v.value()) {
      return node->min_v.value();
    }

    if (node->u <= 2) {
      if (x == 0 && node->max_v.value_or(0) == 1) {
        return 1;
      }
      return std::nullopt;
    }

    auto [high, low] = _split(x, node->u);
    if (node->cluster[high]->max_v.has_value() && low < node->cluster[high]->max_v.value()) {
      auto res = self(self, node->cluster[high], low);
      if (res.has_value()) return _combine(high, res.value(), node->u);
    }

    auto succ = self(self, node->summary, high);
    if (!succ.has_value()) return std::nullopt;  // larger than the last
    high = succ.value();
    low = node->cluster[high]->min_v.value();
    return _combine(high, low, node->u);
  };

  return (is_lower_bound ? lower_bound(lower_bound, _root, x) : upper_bound(upper_bound, _root, x));
}

template<typename T>
std::optional<T> vEBTree<T>::predecessor(T x) const {
  return std::nullopt;
}

template class vebtree::vEBTree<uint32_t>;  // Eval
template class vebtree::vEBTree<uint16_t>;  // Test
