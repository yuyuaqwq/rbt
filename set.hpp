#ifndef RBT_SET_HPP_
#define RBT_SET_HPP_

#include <rbt/rb_tree.hpp>

namespace rbt {

template <class KeyT, class KeyCompareT>
class SetTraits {
public:
    using Key = KeyT;
    using Value = Key;
    struct Element {
        void operator=(const Key& k) {
            key = k;
        }
        void operator=(Key&& k) {
            key = std::move(k);
        }
        Key key;
    };

    using KeyCompare = KeyCompareT;
    using ValueCompare = KeyCompare;
};

template <class Key, class Compare = std::less<Key>>
class set : public RbTree<SetTraits<Key, Compare>> {
private:
    using Tree = RbTree<SetTraits<Key, Compare>>;
public:
    std::pair<Tree::iterator, bool> insert(Tree::value_type&& value) {
        Tree::Put(std::move(value));

    }


};

} // namespace rbt

#endif  // RBT_SET_HPP_