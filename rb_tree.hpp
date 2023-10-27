#ifndef RBT_RB_TREE_HPP_
#define RBT_RB_TREE_HPP_

#include <utility>
#include <cassert>
#include <array>

#include <fpoo/memory_pool.hpp>

namespace rbt {

template <class RbTreeT>
class RbTreeUncheckedConstIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;

    using NodeAddress = typename RbTreeT::NodeAddress;
    using IteratorStack = typename RbTreeT::IteratorStack;

    using value_type = typename RbTreeT::value_type;
    using difference_type = typename RbTreeT::difference_type;
    using pointer = typename RbTreeT::const_pointer;
    using reference = const value_type&;

    RbTreeUncheckedConstIterator(RbTreeT& rb_tree, NodeAddress node_address, IteratorStack&& stack) noexcept :
        rb_tree_ { rb_tree },
        node_address_ { node_address }, 
        stack_{ std::move(stack) } {
        
    }

    [[nodiscard]] reference operator*() const noexcept {
        return node_address_->element;
    }

    [[nodiscard]] pointer operator->() const noexcept {
        return std::pointer_traits<pointer>::pointer_to(**this);
    }

    RbTreeUncheckedConstIterator& operator++() noexcept {
        node_address_ = node_address_->next_;
        return *this;
    }

    RbTreeUncheckedConstIterator operator++(int) noexcept {
        RbTreeUncheckedConstIterator tmp = *this;
        node_address_ = node_address_->next_;
        return tmp;
    }

    RbTreeUncheckedConstIterator& operator--() noexcept {
        node_address_ = node_address_->prev_;
        return *this;
    }

    RbTreeUncheckedConstIterator operator--(int) noexcept {
        RbTreeUncheckedConstIterator tmp = *this;
        node_address_ = node_address_->prev_;
        return tmp;
    }

    [[nodiscard]] bool operator==(const RbTreeUncheckedConstIterator& right) const noexcept {
        return node_address_ == right.node_address_;
    }

    IteratorStack stack_;
    NodeAddress node_address_;

    RbTreeT& rb_tree_;
};

template <class RbTreeT>
class RbTreeConstIterator : public RbTreeUncheckedConstIterator<RbTreeT> {
public:
    using Base = RbTreeUncheckedConstIterator<RbTreeT>;
    using iterator_category = std::bidirectional_iterator_tag;

    using value_type = typename RbTreeT::value_type;
    using difference_type = typename RbTreeT::difference_type;
    using pointer = typename RbTreeT::const_pointer;
    using reference = const value_type&;

    using Base::Base;

    [[nodiscard]] reference operator*() const noexcept {
        //return node_address_->element;
    }

    [[nodiscard]] pointer operator->() const noexcept {
        return pointer_traits<pointer>::pointer_to(**this);
    }

    RbTreeConstIterator& operator++() noexcept {
        this->node_address_ = this->node_address_->next_;
        return *this;
    }

    RbTreeConstIterator operator++(int) noexcept {
        RbTreeConstIterator tmp = *this;
        ++*this;
        return tmp;
    }

    RbTreeConstIterator& operator--() noexcept {
        const auto new_ptr = this->node_address_->prev_;
        this->node_address_ = new_ptr;
        return *this;
    }

    RbTreeConstIterator operator--(int) noexcept {
        RbTreeConstIterator tmp = *this;
        --*this;
        return tmp;
    }

    [[nodiscard]] bool operator==(const RbTreeConstIterator& right) const noexcept {
        return this->node_address_ == right.node_address_;
    }
};

template <class RbTreeT>
class RbTreeIterator : public RbTreeConstIterator<RbTreeT> {
public:
    using Base = RbTreeIterator<RbTreeT>;
    using iterator_category = std::bidirectional_iterator_tag;

    using value_type = typename RbTreeT::value_type;
    using difference_type = typename RbTreeT::difference_type;
    using pointer = typename RbTreeT::pointer;
    using reference = value_type&;

    using Base::Base;

    _NODISCARD reference operator*() const noexcept {
        return const_cast<reference>(Base::operator*());
    }

    _NODISCARD pointer operator->() const noexcept {
        return std::pointer_traits<pointer>::pointer_to(**this);
    }

    RbTreeIterator& operator++() noexcept {
        Base::operator++();
        return *this;
    }

    RbTreeIterator operator++(int) noexcept {
        RbTreeIterator tmp = *this;
        Base::operator++();
        return tmp;
    }

    RbTreeIterator& operator--() noexcept {
        Base::operator--();
        return *this;
    }

    RbTreeIterator operator--(int) noexcept {
        RbTreeIterator tmp = *this;
        Base::operator--();
        return tmp;
    }
};

template <class Traits>
class RbTree {
protected:
    friend class RbTreeUncheckedConstIterator<RbTree<Traits>>;

    using Key = Traits::Key;
    using Value = Traits::Value;
    using Element = Traits::Element;

    using NodeAddress = uint32_t;
    using Color = uint32_t;

    static constexpr Color kBlack = 0x0;
    static constexpr Color kRed = 0x1;
    static constexpr NodeAddress kMaxAddress = 0x7ffffffe;
    static constexpr NodeAddress kInvalidAddress = 0x7fffffff;

    class IteratorStack {
    public:
        NodeAddress& front() noexcept {
            return stack_[cur_pos_ - 1];
        }

        constexpr void push_back(const NodeAddress& value) {
            stack_[cur_pos_++] = value;
        }

        void pop_back() {
            --cur_pos_;
        }

        void clear() {
            cur_pos_ = 0;
        }

        bool empty() {
            return cur_pos_ == 0;
        }

    private:
        std::array<NodeAddress, 62> stack_;
        uint32_t cur_pos_ = 0;
    };
    //using IteratorStack = std::vector<NodeAddress>;


    class Node {
    public:
        Node() : element_{} {
            color_ = kBlack;
            left_ = kInvalidAddress;
            right_ = kInvalidAddress;
        }
        //~Node() = default;

        NodeAddress GetLeft() {
            return left_;
        }

        NodeAddress GetRight() {
            return right_;
        }

        Color GetColor() {
            return color_;
        }

        void SetLeft(NodeAddress left_addr) {
            left_ = left_addr;
        }

        void SetRight(NodeAddress right_addr) {
            right_ = right_addr;
        }

        void SetColor(Color color) {
            color_ = color;
        }

        Element& GetElement() {
            return element_;
        }

    private:
        struct {
            Color color_ : 1;
            NodeAddress left_ : sizeof(NodeAddress) * 8 - 1;
        };
        NodeAddress right_;
        Element element_;

    };
    using AllocatorType = fpoo::CompactMemoryPool<Node>;

public:
    using key_type = Key;
    using value_type = Value;
    using size_type = uint32_t;
    using difference_type = int32_t;

    using key_compare = typename Traits::KeyCompare;
    using value_compare = typename Traits::ValueCompare;

    using allocator_type = AllocatorType;

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::allocator_traits<AllocatorType>::pointer;
    using const_pointer = std::allocator_traits<AllocatorType>::const_pointer;

    using iterator = RbTreeIterator<RbTree<Traits>>;
    using const_iterator = RbTreeConstIterator<RbTree<Traits>>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //using node_type = typename Traits::NodeType;

public:
    RbTree() {
    }

public:

    void clear() noexcept {

    }

    [[nodiscard]] size_type size() const noexcept {
        return size_;
    }

    [[nodiscard]] size_type max_size() const noexcept {
        return kMaxAddress;
    }

    [[nodiscard]] bool empty() const noexcept {
        return root_ == kInvalidAddress;
    }

    //[[nodiscard]] allocator_type get_allocator() const noexcept {
    //    return static_cast<allocator_type>();
    //}

    key_compare key_comp() const {
        return key_compare{};
    }

    value_compare value_comp() const {
        return value_compare{};
    }


    iterator find(const Key& key) {
        IteratorStack stack;
        auto [node_addr, ordering] = Find(stack, key);
        if (ordering != 0) {
            return end();
        }
        return iterator{ *this, node_addr, std::move(stack) };
    }

    const_iterator find(const Key& key) const {
        IteratorStack stack;
        auto [node_addr, ordering] = Find(stack, key);
        if (ordering != 0) {
            return end();
        }
        return const_iterator{ *this, node_addr, std::move(stack) };
    }

    template <class K, class Kc = key_compare, class = typename Kc::is_transparent>
    iterator find(const K& x) {
        IteratorStack stack;
        auto [node_addr, ordering] = Find(stack, x);
        if (ordering != 0) {
            return end();
        }
        return iterator{ *this, node_addr, std::move(stack) };
    }

    template<class K, class Kc = key_compare, class = typename Kc::is_transparent>
    const_iterator find(const K& x) const {
        IteratorStack stack;
        auto [node_addr, ordering] = Find(stack, x);
        if (ordering != 0) {
            return end();
        }
        return const_iterator{ *this, node_addr, std::move(stack) };
    }

    template<class K> bool contains(const K& x) const {
        return find(x) != end();
    }

    std::pair<iterator, bool> insert(const value_type& value) {
        auto node_addr = allocator_.allocate();
        if (node_addr > kMaxAddress) {
            throw std::bad_alloc();     // "The maximum node limit of the tree has been reached."
        }

        Node* node = allocator_.reference(node_addr);
        std::construct_at<Node>(node);
        node->GetElement() = value;

        IteratorStack stack;
        auto success = Insert(stack, node_addr);
        if (!success) {
            std::destroy_at<Node>(node);
            allocator_.dereference(node);
            allocator_.deallocate(node_addr);

            node_addr = stack.front();
            stack.pop_back();
            return std::pair{ iterator{ *this, node_addr, std::move(stack) }, false };
        }
        ++size_;
        allocator_.dereference(node);
        /* 使其保证栈的变更 */
        InsertFixup(stack, node_addr);
        return std::pair{ iterator{ *this, node_addr, std::move(stack) }, true };
    }

    //std::pair<iterator, bool> insert(value_type&& value) {

    //}

    size_type erase(const key_type& key) {
        IteratorStack stack;
        auto [del_node_id, ordering] = Find(stack, key);
        if (del_node_id == kInvalidAddress) return 0;
        auto node = Delete(stack, del_node_id);
        if (node != kInvalidAddress) {
            return 1;
        }
        return 0;
    }

    /*
    * iterator
    */
    iterator begin() noexcept {
        auto [node_addr, stack] = First();
        return iterator{ *this, node_addr, std::move(stack) };
    }

    const_iterator begin() const noexcept {
        auto [node_addr, stack] = First();
        return const_iterator{ *this, node_addr, std::move(stack) };
    }

    iterator end() noexcept {
        return iterator{ *this, kInvalidAddress, IteratorStack{} };
    }

    const_iterator end() const noexcept {
        return const_iterator{ *this, kInvalidAddress,  IteratorStack{} };
    }

    [[nodiscard]] reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    [[nodiscard]] const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    [[nodiscard]] const_iterator cbegin() const noexcept {
        return begin();
    }

    [[nodiscard]] const_iterator cend() const noexcept {
        return end();
    }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    [[nodiscard]] const_reverse_iterator crend() const noexcept {
        return rend();
    }

protected:
    NodeAddress Find(const Key& key) {
        IteratorStack stack;
        auto [node_addr, ordering] = Find(stack, key);
        return ordering == 0 ? node_addr : kInvalidAddress;
    }

    /*
    * 验证红黑树性质
    */
    bool VerifyTree() {
        Node* node = allocator_.reference(root_);
        if (!node) return true;
        bool correct = false;
        do {
            if (node->GetColor() != kBlack) break;
            typename AllocatorType::difference_type high = 1;
            while (node->GetLeft() != kInvalidAddress) {
                NodeAddress node_id = node->GetLeft();
                allocator_.dereference(node);
                node = allocator_.reference(node_id);
                /* 红色节点不表示高度 */
                if (node->GetColor() == kBlack) {
                    high++;
                }
            }
            correct = CheckPath(kInvalidAddress, root_, 0, high);
        } while (false);
        allocator_.dereference(node);
        return correct;
    }


    std::tuple<NodeAddress, IteratorStack> First() const noexcept {
        IteratorStack stack;
        NodeAddress cur_id = root_;
        if (cur_id == kInvalidAddress) {
            return kInvalidAddress;
        }
        Node* cur = allocator_.reference(cur_id);
        while (cur.GetLeft() != kInvalidAddress) {
            stack.push_back(cur_id);
            cur_id = cur.GetLeft();
            allocator_.dereference(cur);
            cur = allocator_.reference(cur_id);
        }
        allocator_.dereference(cur);
        return std::tuple{ cur_id, std::move(stack) };
    }

    std::tuple<NodeAddress, IteratorStack> Last() const noexcept {
        IteratorStack stack;
        NodeAddress cur_id = root_;
        if (cur_id == kInvalidAddress) {
            return kInvalidAddress;
        }
        Node* cur = allocator_.reference(cur_id);
        while (cur.GetRight() != kInvalidAddress) {
            stack.push_back(cur_id);
            cur_id = cur.GetRight();
            allocator_.dereference(cur);
            cur = allocator_.reference(cur_id);
        }
        allocator_.dereference(cur);
        return std::tuple{ cur_id, std::move(stack) };
    }

private:
    /*
    * 替换新孩子节点
    */
    void ReplaceChild(Node* node, NodeAddress node_id, NodeAddress new_node_id) {
        if (node->GetLeft() == node_id) {
            node->SetLeft(new_node_id);
        }
        else {
            assert(node->GetRight() == node_id);
            node->SetRight(new_node_id);
        }
    }
    /*
    * 右旋
    */
    NodeAddress RotateRight(Node* sub_root_parent, NodeAddress sub_root_id, Node* sub_root) {
        NodeAddress new_sub_root_id = sub_root->GetLeft();
        if (new_sub_root_id == kInvalidAddress) {
            return sub_root_id;
        }
        Node* new_sub_root = allocator_.reference(new_sub_root_id);

        if (sub_root_parent != nullptr) {
            ReplaceChild(sub_root_parent, sub_root_id, new_sub_root_id);
        }

        sub_root->SetLeft(new_sub_root->GetRight());
        new_sub_root->SetRight(sub_root_id);

        allocator_.dereference(new_sub_root);
        return new_sub_root_id;
    }
    /*
    * 左旋
    */
    NodeAddress RotateLeft(Node* sub_root_parent, NodeAddress sub_root_id, Node* sub_root) {
        NodeAddress new_sub_root_id = sub_root->GetRight();
        if (new_sub_root_id == kInvalidAddress) {
            return sub_root_id;
        }
        Node* new_sub_root = allocator_.reference(new_sub_root_id);

        if (sub_root_parent != nullptr) {
            ReplaceChild(sub_root_parent, sub_root_id, new_sub_root_id);
        }

        sub_root->SetRight(new_sub_root->GetLeft());
        new_sub_root->SetLeft(sub_root_id);

        allocator_.dereference(new_sub_root);
        return new_sub_root_id;
    }
    /*
    * 栈指向node的父亲
    * new_node代替node挂接到其父节点下
    * new_node的左右子节点不变
    * node从树中摘除
    * node的parent、left和right不变
    */
    void Hitch(Node* node_parent, NodeAddress node_id, NodeAddress new_node_id) {
        Node* node = allocator_.reference(node_id);
        Node* new_node = allocator_.reference(new_node_id);
        if (node_parent != nullptr) {
            ReplaceChild(node_parent, node_id, new_node_id);
        }
        if (root_ == node_id) {
            root_ = new_node_id;
        }
        allocator_.dereference(node);
        allocator_.dereference(new_node);
    }
    /*
    * 取兄弟节点
    */
    NodeAddress GetSiblingNode(Node* node_parent, NodeAddress node_id, Node* node) {
        NodeAddress ret;
        if (node_parent->GetLeft() == node_id) {
            ret = node_parent->GetRight();
        }
        else {
            assert(node_parent->GetRight() == node_id);
            ret = node_parent->GetLeft();
        }
        return ret;
    }

    /*
    * 向树中插入节点后的平衡操作
    */
    void InsertFixup(IteratorStack& stack, NodeAddress ins_node_id) {
        Node* ins_node = allocator_.reference(ins_node_id);
        ins_node->SetColor(kBlack);
        if (stack.empty()) {
            ins_node->SetColor(kBlack);
            allocator_.dereference(ins_node);
            return;
        }
        auto& cur_id = stack.front(); stack.pop_back();

        ins_node->SetColor(kRed);
        allocator_.dereference(ins_node);

        Node* cur = nullptr;
        /* 开始回溯维护 */
        while (cur_id != kInvalidAddress) {
            cur = allocator_.reference(cur_id);
            if (cur->GetColor() == kBlack) {
                /* 当前节点(插入节点的父节点)是黑色，啥都不用做(是2节点 / 3节点的插入，直接合并) */
                break;
            }
            if (stack.empty()) {
                /* 没有父节点，回溯到根节点了，直接染黑 */
                cur->SetColor(kBlack);
                break;
            }
            auto& parent_id = stack.front(); stack.pop_back();

            Node* parent = allocator_.reference(parent_id);
            NodeAddress sibling_id = GetSiblingNode(parent, cur_id, cur);
            allocator_.dereference(parent);
            
            Node* sibling = nullptr;
            if (sibling_id != kInvalidAddress) {
                sibling = allocator_.reference(sibling_id);
            }
            if (sibling && sibling->GetColor() == kRed) {
                /* 兄弟节点是红色，说明是4节点的插入，分裂(红黑树的体现就是变色)，父节点向上插入，继续回溯 */
                cur->SetColor(kBlack);
                sibling->SetColor(kBlack);
                ins_node_id = parent_id;         /* 更新为该节点向上插入 */
                ins_node = allocator_.reference(ins_node_id);
                if (stack.empty()) {
                    ins_node->SetColor(kBlack);     /* 根节点，染黑并结束 */
                    break;
                }
                ins_node->SetColor(kRed);
                parent_id = stack.front(); stack.pop_back();
                /* 将parent视作插入节点 */
            }
            else {
                /* 没有兄弟节点或兄弟节点是黑色，说明是3节点的插入，可以并入，但需要利用旋转将其变为4节点
                    *        10b                                5b
                    *     5r     20b    ->     !2r         10r
                    * !2r                                                 20b
                */
                  assert(sibling_id == kInvalidAddress || sibling->GetColor() == kBlack);
                NodeAddress new_sub_root_id;
                NodeAddress old_sub_root_id = parent_id;
                Node* old_sub_root = allocator_.reference(old_sub_root_id);
                Node* new_sub_root_parent = nullptr;
                if (!stack.empty()) {
                    auto& new_sub_root_parent_id = stack.front();
                    new_sub_root_parent = allocator_.reference(new_sub_root_parent_id);
                }
                if (old_sub_root->GetLeft() == cur_id) {
                    if (cur->GetRight() == ins_node_id) {
                        RotateLeft(old_sub_root, cur_id, cur);
                    }
                    new_sub_root_id = RotateRight(new_sub_root_parent, old_sub_root_id, old_sub_root);
                }
                else {
                    if (cur->GetLeft() == ins_node_id) {
                        RotateRight(old_sub_root, cur_id, cur);
                    }
                    new_sub_root_id = RotateLeft(new_sub_root_parent, old_sub_root_id, old_sub_root);
                }
                if (new_sub_root_parent) allocator_.dereference(new_sub_root_parent);
                Node* new_sub_root = allocator_.reference(new_sub_root_id);
                new_sub_root->SetColor(kBlack);
                old_sub_root->SetColor(kRed);
                allocator_.dereference(new_sub_root);

                if (root_ == old_sub_root_id) {        /* 原来的子树根节点可能是整棵树的根节点，因此要检查更新 */
                    root_ = new_sub_root_id;
                }
                break;        /* 只是并入，未分裂，向上没有改变颜色，不再需要回溯 */
            }
            cur_id = parent_id;
            allocator_.dereference(cur);
        }
        allocator_.dereference(cur);
    }
    /*
    * 向树中删除节点后的平衡操作
    */
    void DeleteFixup(IteratorStack& stack, NodeAddress del_node_id, bool is_parent_left) {
        Node* del_node = allocator_.reference(del_node_id);
        NodeAddress parent_id = kInvalidAddress;

        Color del_color = del_node->GetColor();
        if (del_color == kRed) { /* 是红色的，是3/4节点，因为此时一定是叶子节点(红节点不可能只有一个子节点)，直接移除 */ }
        /* 是黑色的，但是有一个子节点，说明是3节点，变为2节点即可 */
        else if (del_node->GetLeft() != kInvalidAddress) {
            Node* del_node_left = allocator_.reference(del_node->GetLeft());
            del_node_left->SetColor(kBlack);
            allocator_.dereference(del_node_left);
        }
        else if (del_node->GetRight() != kInvalidAddress) {
            Node* del_node_right = allocator_.reference(del_node->GetRight());
            del_node_right->SetColor(kBlack);
            allocator_.dereference(del_node_right);
        }
        else {
            if (!stack.empty()) {
                parent_id = stack.front(); stack.pop_back();
            }
        }

        NodeAddress new_sub_root_id;
        NodeAddress grandpa_id = kInvalidAddress;
        /* 回溯维护删除黑色节点，即没有子节点(2节点)的情况 */
        Node* parent = nullptr, * sibling = nullptr, * grandpa = nullptr;
        if (parent_id != kInvalidAddress) parent = allocator_.reference(parent_id);

        while (parent_id != kInvalidAddress) {
            NodeAddress sibling_id = is_parent_left ? parent->GetRight() : parent->GetLeft();
            sibling = allocator_.reference(sibling_id);
            if (!stack.empty()) {
                grandpa_id = stack.front(); stack.pop_back();
                grandpa = allocator_.reference(grandpa_id);
            }
            else grandpa = nullptr;
            /* 黑色节点一定有兄弟节点 */
            if (sibling->GetColor() == kRed) {
                /* 兄弟节点为红，说明兄弟节点与父节点形成3节点，真正的兄弟节点应该是红兄弟节点的子节点
                    旋转，此时只是使得兄弟节点和父节点形成的3节点红色链接位置调换，当前节点的兄弟节点变为原兄弟节点的子节点 */
                NodeAddress old_sub_root_id = parent_id;
                Node* old_sub_root = allocator_.reference(old_sub_root_id);
                old_sub_root->SetColor(kRed);
                sibling->SetColor(kBlack);
                if (old_sub_root->GetLeft() == sibling_id) {
                    new_sub_root_id = RotateRight(grandpa, old_sub_root_id, old_sub_root);
                    sibling_id = old_sub_root->GetLeft();     /* 下降后挂接过来的节点 */
                    allocator_.dereference(sibling);
                    sibling = allocator_.reference(sibling_id);
                }
                else {
                    new_sub_root_id = RotateLeft(grandpa, old_sub_root_id, old_sub_root);
                    sibling_id = old_sub_root->GetRight();     /* 下降后挂接过来的节点 */
                    allocator_.dereference(sibling);
                    sibling = allocator_.reference(sibling_id);
                }
                if (root_ == old_sub_root_id) {
                    root_ = new_sub_root_id;
                }
                allocator_.dereference(old_sub_root);
                /* grandpa变为新根节点 */
                allocator_.dereference(grandpa);
                grandpa_id = new_sub_root_id;
                grandpa = allocator_.reference(grandpa_id);
            }

            /* 至此兄弟节点一定为黑 */

            /* 侄子节点为红，即兄弟节点是3 / 4节点的情况，向兄弟借节点(上升兄弟节点，下降父亲节点) */
            Node* sibling_right = allocator_.reference(sibling->GetRight());
            Node* sibling_left = allocator_.reference(sibling->GetLeft());
            if (sibling->GetRight() != kInvalidAddress && sibling_right->GetColor() == kRed ||
                sibling->GetLeft() != kInvalidAddress && sibling_left->GetColor() == kRed) {
                Color parent_color = parent->GetColor();
                parent->SetColor(kBlack);
                NodeAddress old_sub_root_id = parent_id;
                Node* new_sub_root_parent = nullptr;
                if (parent->GetLeft() == sibling_id) {
                    if (sibling->GetLeft() == kInvalidAddress || sibling_left->GetColor() == kBlack) {
                        sibling_right->SetColor(kBlack);
                        sibling_id = RotateLeft(parent, sibling_id, sibling);
                    }
                    else {
                        sibling_left->SetColor(kBlack);
                    }
                    new_sub_root_id = RotateRight(grandpa, parent_id, parent);
                }
                else {
                    if (sibling->GetRight() == kInvalidAddress || sibling_right->GetColor() == kBlack) {
                        sibling_left->SetColor(kBlack);
                        sibling_id = RotateRight(parent, sibling_id, sibling);
                    }
                    else {
                        sibling_right->SetColor(kBlack);
                    }
                    new_sub_root_id = RotateLeft(grandpa, parent_id, parent);
                }
                allocator_.dereference(sibling);
                sibling = allocator_.reference(sibling_id);
                /* 该节点会接替原先的子根节点，也要接替颜色 */
                sibling->SetColor(parent_color);
                if (root_ == old_sub_root_id) {
                    root_ = new_sub_root_id;
                }
                allocator_.dereference(sibling_right);
                allocator_.dereference(sibling_left);
                break;
            }
            allocator_.dereference(sibling_right);
            allocator_.dereference(sibling_left);

            if (parent->GetColor() == kRed) {
                /* 父节点为红，即父节点是3/4节点，分裂下降与兄弟节点合并
                    |5|8|                 |5|
                        /    |        ->        /
                    3     6    -9-            3     |6|8| */
                sibling->SetColor(kRed);
                parent->SetColor(kBlack);
                break;
            }
            else {
                /* 父节点为黑，即父节点是2节点，兄弟节点也是2节点，下降父节点与兄弟节点合并，相当于向上删除父节点，继续回溯
                    为什么不是3/4节点？因为黑父节点如果是3，兄弟节点是红，4的话回溯时父节点是红 */
                sibling->SetColor(kRed);
            }
            NodeAddress child_id = parent_id;
            parent_id = grandpa_id;
            allocator_.dereference(parent);
            parent = grandpa;
            if (parent != nullptr) {
                is_parent_left = parent->GetLeft() == child_id;
            }
        }
        allocator_.dereference(sibling);
        allocator_.dereference(parent);

        Node* root = allocator_.reference(root_);
        if (root && root->GetColor() == kRed) {
            /* 根节点染黑 */
            root->SetColor(kBlack);
        }
        allocator_.dereference(root);
    }

    /*
    * 向树中插入节点
    * 插入树中已存在的节点返回false
    */
    bool Insert(IteratorStack& stack, NodeAddress node_id) {
        Node* node = allocator_.reference(node_id);
        stack.clear();

        if (root_ == kInvalidAddress) {
            root_ = node_id;
            return true;
        }
        NodeAddress cur_id = root_;
        Node* cur = nullptr;
        bool success = true;
        while (cur_id != kInvalidAddress) {
            stack.push_back(cur_id);

            Node* cur = allocator_.reference(cur_id);
            Key& cur_key = cur->GetElement().key;
            Key& find_key = node->GetElement().key;

            std::strong_ordering ordering = find_key <=> cur_key;;
            if (ordering < 0) {
            // if (key_compare{}(find_key, cur_key)) {
                if (cur->GetLeft() == kInvalidAddress) {
                    cur->SetLeft(node_id);
                    break;
                }
                cur_id = cur->GetLeft();
            }
            else if (ordering > 0) {
            // else if (key_compare{}(cur_key, find_key)) {
                if (cur->GetRight() == kInvalidAddress) {
                    cur->SetRight(node_id);
                    break;
                }
                cur_id = cur->GetRight();
            }
            else {
                // 找到相等的节点时，栈上指向找到的节点
                success = false;
                break;
            }
            allocator_.dereference(cur);
        }
        if (cur) allocator_.dereference(cur);
        allocator_.dereference(node);
        return success;
    }

    /*
    * 向树中推入节点
    * 覆盖重复key，返回被覆盖的node_id，否则返回InvalidId，如果node_id已经被插入过了，也会被返回
    */
    NodeAddress Put(IteratorStack& stack, NodeAddress node_id) {
        Node* node = allocator_.reference(node_id);
        stack.clear();
        if (root_ == kInvalidAddress) {
            root_ = node_id;
            return kInvalidAddress;
        }
        NodeAddress cur_id = root_;
        Node* cur = NULL;
        NodeAddress old_id = kInvalidAddress;
        NodeAddress parent_id = kInvalidAddress;
        while (cur_id != kInvalidAddress) {
            stack.push_back(cur_id);
            cur = allocator_.reference(cur_id);
            Key& cur_key = cur->GetElement().key;
            Key& node_key = node->GetElement().key;

            //auto ordering = cur_key <=> node_key;
            //if (ordering < 0) {
            if (key_compare{}(cur_key, node_key)) {
                if (cur->GetRight() == kInvalidAddress) {
                    cur->SetRight(node_id);
                    break;
                }
                parent_id = cur_id;
                cur_id = cur->GetRight();
            }
            //else if (ordering > 0) {
            else if (key_compare{}(node_key, cur_key)) {
                if (cur->GetLeft() == kInvalidAddress) {
                    cur->SetLeft(node_id);
                    break;
                }
                parent_id = cur_id;
                cur_id = cur->GetLeft();
            }
            else {
                /* 相等的情况 */
                stack.pop_back();
                old_id = cur_id;
                if (cur_id == node_id) break;
                if (parent_id != kInvalidAddress) {
                    Node* parent = allocator_.reference(parent_id);
                    ReplaceChild(parent, cur_id, node_id);
                    allocator_.dereference(parent);
                }
                else {
                    root_ = node_id;
                }
                *node = *cur;
                break;
            }
            allocator_.dereference(cur);
        }
        if (cur) allocator_.dereference(cur);
        allocator_.dereference(node);
        return old_id;
    }
    /*
    * 向树中推入节点
    * 允许覆盖重复key，返回被覆盖的原node，否则InvalidId，如果put_node_id已经被插入过了，也会被返回(返回值 == put_node_id)
    */
    NodeAddress Put(NodeAddress put_node_id) {
        IteratorStack stack;
        NodeAddress old_id = Put(stack, put_node_id);
        if (old_id == kInvalidAddress) InsertFixup(stack, put_node_id);
        return old_id;
    }

    /*
    * 从树中删除节点
    * 返回被删除的节点(或被替换到当前位置的右子树最小节点)，构造所有回溯条件
    */
    NodeAddress Delete(IteratorStack& stack, NodeAddress node_id, bool* is_parent_left) {
        assert(node_id != kInvalidAddress);
        Node* node = allocator_.reference(node_id);
        if (stack.empty()) {
            return kInvalidAddress;
        }
        NodeAddress& parent_id = stack.front(); stack.pop_back();
        Node* parent = NULL;
        if (parent_id != NULL) {
            assert(*parent_id != kInvalidAddress);
            parent = allocator_.reference(parent_id);
        }
        if (node->GetLeft() != kInvalidAddress && node->GetRight() != kInvalidAddress) {
            /* 有左右各有子节点，找当前节点的右子树中最小的节点，用最小节点替换到当前节点所在的位置，摘除当前节点，相当于移除了最小节点 */
            stack.push_back(node_id);
            NodeAddress new_node_id = stack.front();
            NodeAddress min_node_id = node->GetRight();
            NodeAddress min_node_parent_id = kInvalidAddress;
            Node* min_node = allocator_.reference(min_node_id);
            while (min_node->GetLeft() != kInvalidAddress) {
                stack.push_back(min_node_id);
                min_node_parent_id = min_node_id;
                min_node_id = min_node->GetLeft();
                allocator_.dereference(min_node);
                min_node = allocator_.reference(min_node_id);
            }
            /* 被替换到当前位置的最小节点，保证回溯路径的正确 */
            new_node_id = min_node_id;
            Node* min_node_parent = allocator_.reference(min_node_parent_id);

            /* 最小节点继承待删除节点的左子树，因为最小节点肯定没有左节点，所以直接赋值 */
            min_node->SetLeft(node->GetLeft());

            NodeAddress old_right_id = min_node->GetRight();
            /* 最小节点可能是待删除节点的右节点 */
            if (node->GetRight() != min_node_id) {
                /* 将min_node从原先的位置摘除，用其右子树代替 */
                min_node_parent->SetLeft(min_node->GetRight());

                /* 最小节点继承待删除节点的右子树 */
                min_node->SetRight(node->GetRight());
                if (is_parent_left) *is_parent_left = true;
            }
            else {
                if (is_parent_left) *is_parent_left = false;
            }
            allocator_.dereference(min_node_parent);

            /* 最后进行挂接 */
            Hitch(parent, node_id, min_node_id);

            /* 也可以选择直接交换两个节点的数据，但是开销不定 */

            node_id = min_node_id;

            /* 回溯可能需要的，node变为原先的min_node，只是不挂到树上(node的父节点不指向node) */
            node->SetLeft(kInvalidAddress);
            node->SetRight(old_right_id);
        }
        else {
            if (is_parent_left) {
                if (parent != NULL) {
                    *is_parent_left = parent->GetLeft() == node_id;
                    assert(*is_parent_left || *is_parent_left == false && parent->GetRight() == node_id);
                }
                else {
                    *is_parent_left = false;
                }
            }

            if (node->GetRight() != kInvalidAddress) {
                /* 只有右子节点 */
                Hitch(parent, node_id, node->GetRight());
            }
            else if (node->GetLeft() != kInvalidAddress) {
                /* 只有左子节点 */
                Hitch(parent, node_id, node->GetLeft());
            }
            else {
                /* 没有子节点，直接从父节点中摘除此节点 */
                Hitch(parent, node_id, kInvalidAddress);
            }
        }
        if (parent) {
            allocator_.dereference(parent);
        }
        return node_id;
    }
    /*
    * 删除树中指定节点
    */
    bool Delete(IteratorStack& stack, NodeAddress del_node_id) {
        assert(del_node_id != kInvalidAddress);
        bool is_parent_left;
        NodeAddress del_min_node_id = Delete(stack, del_node_id, &is_parent_left);
        if (del_min_node_id == kInvalidAddress) {
            return false;
        }
        if (del_min_node_id != del_node_id) {
            Node* del_node = allocator_.reference(del_node_id);
            Node* del_min_node = allocator_.reference(del_min_node_id);
            /* 需要交换颜色 */;
            Color old_color = del_min_node->GetColor();
            del_min_node->SetColor(del_node->GetColor());
            del_node->SetColor(old_color);
            allocator_.dereference(del_min_node);
            allocator_.dereference(del_node);
        }
        DeleteFixup(stack, del_node_id, is_parent_left);
        return true;
    }

    /*
    * 查找指定节点
    */
    std::tuple<NodeAddress, std::strong_ordering> Find(IteratorStack& stack, const Key& find_key) {
        NodeAddress cur_id = root_;
        stack.clear();
        NodeAddress perv_id = kInvalidAddress;
        std::strong_ordering ordering;
        while (cur_id != kInvalidAddress) {
            perv_id = cur_id;
            Node* cur = allocator_.reference(cur_id);
            Key& cur_key = cur->GetElement().key;
            ordering = find_key <=> cur_key;
            if (ordering < 0) {
            //if (key_compare{}(key, cur_key)) {
                //ordering = -1;
                cur_id = cur->GetLeft();
            }
            else if (ordering > 0) {
            //else if (key_compare{}(cur_key, key)) {
                //ordering = 1;
                cur_id = cur->GetRight();
            }
            else {
                //ordering = 0;
                allocator_.dereference(cur);
                return std::tuple{ cur_id, ordering };
            }
            if (cur_id != kInvalidAddress) {
                stack.push_back(perv_id);
            }
            allocator_.dereference(cur);
        }
        return { perv_id, ordering };
    }

    /*
    * 检查路径是否符合红黑树性质
    */
    bool CheckPath(NodeAddress parent_id, NodeAddress node_id, AllocatorType::difference_type cur_high, AllocatorType::difference_type max_high) {
        if (node_id == kInvalidAddress) {
            return cur_high == max_high;
        }
        bool correct = false;
        Node* node = nullptr;
        Node* parent = nullptr;
        do {
            node = allocator_.reference(node_id);
            if (parent_id != kInvalidAddress) {
                parent = allocator_.reference(parent_id);
                if (node->GetColor() == kRed && parent->GetColor() == kRed) {
                    break;
                }
            }
            if (node->GetColor() == kBlack) { cur_high++; }
            correct = CheckPath(node_id, node->GetLeft(), cur_high, max_high) && CheckPath(node_id, node->GetRight(), cur_high, max_high);
        } while (false);
        if (node) allocator_.dereference(node);
        if (parent) allocator_.dereference(parent);
        return correct;
    }


private:
    AllocatorType allocator_;
    NodeAddress root_ = kInvalidAddress;
    size_type size_ = 0;
};

} // namespace rbt

#endif // RBT_RB_TREE_HPP_