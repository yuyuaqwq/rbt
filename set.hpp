#ifndef RBT_SET_HPP_
#define RBT_SET_HPP_

#include <rbt/rb_tree.hpp>

namespace rbt {
template <class Key>
class SetTraits {
public:
    using KeyType = Key;
    struct ElementType {
        void operator=(const KeyType& k) {
            key = k;
        }
        void operator=(KeyType&& k) {
            key = std::move(k);
        }
        KeyType key;
    };
};

template <class Key>
class set : public RbTree<SetTraits<Key>> {
private:
    using Tree = RbTree<SetTraits<Key>>;
public:

    template <typename KeyReference>
    void insert(KeyReference&& key) {
        Tree::Put(std::forward<KeyReference>(key));
    }

    void find() {

    }
};

} // namespace rbt

#endif  // RBT_SET_HPP_