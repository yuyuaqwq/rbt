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
        /* ʹ�䱣֤ջ�ı�� */
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
    * ��֤���������
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
                /* ��ɫ�ڵ㲻��ʾ�߶� */
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
    * �滻�º��ӽڵ�
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
    * ����
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
    * ����
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
    * ջָ��node�ĸ���
    * new_node����node�ҽӵ��丸�ڵ���
    * new_node�������ӽڵ㲻��
    * node������ժ��
    * node��parent��left��right����
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
    * ȡ�ֵܽڵ�
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
    * �����в���ڵ���ƽ�����
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
        /* ��ʼ����ά�� */
        while (cur_id != kInvalidAddress) {
            cur = allocator_.reference(cur_id);
            if (cur->GetColor() == kBlack) {
                /* ��ǰ�ڵ�(����ڵ�ĸ��ڵ�)�Ǻ�ɫ��ɶ��������(��2�ڵ� / 3�ڵ�Ĳ��룬ֱ�Ӻϲ�) */
                break;
            }
            if (stack.empty()) {
                /* û�и��ڵ㣬���ݵ����ڵ��ˣ�ֱ��Ⱦ�� */
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
                /* �ֵܽڵ��Ǻ�ɫ��˵����4�ڵ�Ĳ��룬����(����������־��Ǳ�ɫ)�����ڵ����ϲ��룬�������� */
                cur->SetColor(kBlack);
                sibling->SetColor(kBlack);
                ins_node_id = parent_id;         /* ����Ϊ�ýڵ����ϲ��� */
                ins_node = allocator_.reference(ins_node_id);
                if (stack.empty()) {
                    ins_node->SetColor(kBlack);     /* ���ڵ㣬Ⱦ�ڲ����� */
                    break;
                }
                ins_node->SetColor(kRed);
                parent_id = stack.front(); stack.pop_back();
                /* ��parent��������ڵ� */
            }
            else {
                /* û���ֵܽڵ���ֵܽڵ��Ǻ�ɫ��˵����3�ڵ�Ĳ��룬���Բ��룬����Ҫ������ת�����Ϊ4�ڵ�
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

                if (root_ == old_sub_root_id) {        /* ԭ�����������ڵ�������������ĸ��ڵ㣬���Ҫ������ */
                    root_ = new_sub_root_id;
                }
                break;        /* ֻ�ǲ��룬δ���ѣ�����û�иı���ɫ��������Ҫ���� */
            }
            cur_id = parent_id;
            allocator_.dereference(cur);
        }
        allocator_.dereference(cur);
    }
    /*
    * ������ɾ���ڵ���ƽ�����
    */
    void DeleteFixup(IteratorStack& stack, NodeAddress del_node_id, bool is_parent_left) {
        Node* del_node = allocator_.reference(del_node_id);
        NodeAddress parent_id = kInvalidAddress;

        Color del_color = del_node->GetColor();
        if (del_color == kRed) { /* �Ǻ�ɫ�ģ���3/4�ڵ㣬��Ϊ��ʱһ����Ҷ�ӽڵ�(��ڵ㲻����ֻ��һ���ӽڵ�)��ֱ���Ƴ� */ }
        /* �Ǻ�ɫ�ģ�������һ���ӽڵ㣬˵����3�ڵ㣬��Ϊ2�ڵ㼴�� */
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
        /* ����ά��ɾ����ɫ�ڵ㣬��û���ӽڵ�(2�ڵ�)����� */
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
            /* ��ɫ�ڵ�һ�����ֵܽڵ� */
            if (sibling->GetColor() == kRed) {
                /* �ֵܽڵ�Ϊ�죬˵���ֵܽڵ��븸�ڵ��γ�3�ڵ㣬�������ֵܽڵ�Ӧ���Ǻ��ֵܽڵ���ӽڵ�
                    ��ת����ʱֻ��ʹ���ֵܽڵ�͸��ڵ��γɵ�3�ڵ��ɫ����λ�õ�������ǰ�ڵ���ֵܽڵ��Ϊԭ�ֵܽڵ���ӽڵ� */
                NodeAddress old_sub_root_id = parent_id;
                Node* old_sub_root = allocator_.reference(old_sub_root_id);
                old_sub_root->SetColor(kRed);
                sibling->SetColor(kBlack);
                if (old_sub_root->GetLeft() == sibling_id) {
                    new_sub_root_id = RotateRight(grandpa, old_sub_root_id, old_sub_root);
                    sibling_id = old_sub_root->GetLeft();     /* �½���ҽӹ����Ľڵ� */
                    allocator_.dereference(sibling);
                    sibling = allocator_.reference(sibling_id);
                }
                else {
                    new_sub_root_id = RotateLeft(grandpa, old_sub_root_id, old_sub_root);
                    sibling_id = old_sub_root->GetRight();     /* �½���ҽӹ����Ľڵ� */
                    allocator_.dereference(sibling);
                    sibling = allocator_.reference(sibling_id);
                }
                if (root_ == old_sub_root_id) {
                    root_ = new_sub_root_id;
                }
                allocator_.dereference(old_sub_root);
                /* grandpa��Ϊ�¸��ڵ� */
                allocator_.dereference(grandpa);
                grandpa_id = new_sub_root_id;
                grandpa = allocator_.reference(grandpa_id);
            }

            /* �����ֵܽڵ�һ��Ϊ�� */

            /* ֶ�ӽڵ�Ϊ�죬���ֵܽڵ���3 / 4�ڵ����������ֵܽ�ڵ�(�����ֵܽڵ㣬�½����׽ڵ�) */
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
                /* �ýڵ�����ԭ�ȵ��Ӹ��ڵ㣬ҲҪ������ɫ */
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
                /* ���ڵ�Ϊ�죬�����ڵ���3/4�ڵ㣬�����½����ֵܽڵ�ϲ�
                    |5|8|                 |5|
                        /    |        ->        /
                    3     6    -9-            3     |6|8| */
                sibling->SetColor(kRed);
                parent->SetColor(kBlack);
                break;
            }
            else {
                /* ���ڵ�Ϊ�ڣ������ڵ���2�ڵ㣬�ֵܽڵ�Ҳ��2�ڵ㣬�½����ڵ����ֵܽڵ�ϲ����൱������ɾ�����ڵ㣬��������
                    Ϊʲô����3/4�ڵ㣿��Ϊ�ڸ��ڵ������3���ֵܽڵ��Ǻ죬4�Ļ�����ʱ���ڵ��Ǻ� */
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
            /* ���ڵ�Ⱦ�� */
            root->SetColor(kBlack);
        }
        allocator_.dereference(root);
    }

    /*
    * �����в���ڵ�
    * ���������Ѵ��ڵĽڵ㷵��false
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
                // �ҵ���ȵĽڵ�ʱ��ջ��ָ���ҵ��Ľڵ�
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
    * ����������ڵ�
    * �����ظ�key�����ر����ǵ�node_id�����򷵻�InvalidId�����node_id�Ѿ���������ˣ�Ҳ�ᱻ����
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
                /* ��ȵ���� */
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
    * ����������ڵ�
    * �������ظ�key�����ر����ǵ�ԭnode������InvalidId�����put_node_id�Ѿ���������ˣ�Ҳ�ᱻ����(����ֵ == put_node_id)
    */
    NodeAddress Put(NodeAddress put_node_id) {
        IteratorStack stack;
        NodeAddress old_id = Put(stack, put_node_id);
        if (old_id == kInvalidAddress) InsertFixup(stack, put_node_id);
        return old_id;
    }

    /*
    * ������ɾ���ڵ�
    * ���ر�ɾ���Ľڵ�(���滻����ǰλ�õ���������С�ڵ�)���������л�������
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
            /* �����Ҹ����ӽڵ㣬�ҵ�ǰ�ڵ������������С�Ľڵ㣬����С�ڵ��滻����ǰ�ڵ����ڵ�λ�ã�ժ����ǰ�ڵ㣬�൱���Ƴ�����С�ڵ� */
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
            /* ���滻����ǰλ�õ���С�ڵ㣬��֤����·������ȷ */
            new_node_id = min_node_id;
            Node* min_node_parent = allocator_.reference(min_node_parent_id);

            /* ��С�ڵ�̳д�ɾ���ڵ������������Ϊ��С�ڵ�϶�û����ڵ㣬����ֱ�Ӹ�ֵ */
            min_node->SetLeft(node->GetLeft());

            NodeAddress old_right_id = min_node->GetRight();
            /* ��С�ڵ�����Ǵ�ɾ���ڵ���ҽڵ� */
            if (node->GetRight() != min_node_id) {
                /* ��min_node��ԭ�ȵ�λ��ժ������������������ */
                min_node_parent->SetLeft(min_node->GetRight());

                /* ��С�ڵ�̳д�ɾ���ڵ�������� */
                min_node->SetRight(node->GetRight());
                if (is_parent_left) *is_parent_left = true;
            }
            else {
                if (is_parent_left) *is_parent_left = false;
            }
            allocator_.dereference(min_node_parent);

            /* �����йҽ� */
            Hitch(parent, node_id, min_node_id);

            /* Ҳ����ѡ��ֱ�ӽ��������ڵ�����ݣ����ǿ������� */

            node_id = min_node_id;

            /* ���ݿ�����Ҫ�ģ�node��Ϊԭ�ȵ�min_node��ֻ�ǲ��ҵ�����(node�ĸ��ڵ㲻ָ��node) */
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
                /* ֻ�����ӽڵ� */
                Hitch(parent, node_id, node->GetRight());
            }
            else if (node->GetLeft() != kInvalidAddress) {
                /* ֻ�����ӽڵ� */
                Hitch(parent, node_id, node->GetLeft());
            }
            else {
                /* û���ӽڵ㣬ֱ�ӴӸ��ڵ���ժ���˽ڵ� */
                Hitch(parent, node_id, kInvalidAddress);
            }
        }
        if (parent) {
            allocator_.dereference(parent);
        }
        return node_id;
    }
    /*
    * ɾ������ָ���ڵ�
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
            /* ��Ҫ������ɫ */;
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
    * ����ָ���ڵ�
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
    * ���·���Ƿ���Ϻ��������
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