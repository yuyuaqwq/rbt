#ifndef RBT_MAP_HPP_
#define RBT_MAP_HPP_

/*
* 事实上，对于有序关联容器存储int类型的场景，在我所了解的所有数据结构中，art是最优选(综合时间与空间开销)，b树是其次
* 在10M节点的场景下，art性能是rbtree的8x-15x，而btree的性能是rbtree的3x-5x(btree在节点内部使用顺序而非二分，更好利用缓存)
* 而空间开销，btree无人能出其右。
    - btree > art > rb
* 
* 而btree相较于rb的缺点是删除时会使其它迭代器失效。
* 
* 我实现的rb，由于不使用父节点，通过栈来保存路径，如果保留父节点，会使每个节点的空间开销变为20bytes，虽然依旧优于常规标准库实现，但总归是有些遗憾。
* 鉴于迭代器失效的影响并不会太大，故依旧不保留父节点。
*/

#include <utility>

namespace rbt {

template <class Key, 
    class Value, 
    class Comparer = std::less<Key>, 
    class _Alloc = std::allocator<std::pair<const Key, Value>>>
class map {
public:

};

} // namespace rbt

#endif // RBT_MAP_HPP_