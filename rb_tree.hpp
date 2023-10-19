#ifndef RBT_RB_TREE_HPP_
#define RBT_RB_TREE_HPP_

#include <utility>
#include <cassert>
#include <vector>

#include <fpoo/memory_pool.hpp>

namespace rbt {

template <class Traits>
class RbTree {
public:
    using Key = Traits::KeyType;
    using Element = Traits::ElementType;

    using NodeAddress = uint32_t;
    using Color = uint32_t;
    using IteratorStack = std::vector<NodeAddress>;

    static constexpr Color kBlack = 0x0;
    static constexpr Color kRed = 0x1;
    static constexpr NodeAddress kMaxAddress = 0x7ffffffe;
    static constexpr NodeAddress kInvalidAddress = 0x7fffffff;

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
    template <typename ElementReference>
    void Put(ElementReference&& ele) {
        auto node_addr = allocator_.allocate();
        if (node_addr > kMaxAddress) {
            throw std::runtime_error("The maximum node limit of the tree has been reached.");
        }

        Node* node = allocator_.reference(node_addr);
        std::construct_at<Node>(node);
        if constexpr (std::is_lvalue_reference_v<decltype(ele)>) {
            node->GetElement() = ele;
        }
        else {
            node->GetElement() = std::move(ele);
        }
        allocator_.dereference(node);
        Put(node_addr);

    }

    NodeAddress Find(const Key& key) {
        IteratorStack stack;
        auto [node_addr, ordering] = Find(stack, key);
        return ordering == 0 ? node_addr : kInvalidAddress;
    }

    bool Delete(const Key&& key) {
        IteratorStack stack;
        auto [del_node_id, ordering] = Find(stack, key);
        if (del_node_id == kInvalidAddress) return false;
        return Delete(stack, del_node_id);
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
            AllocatorType::difference_type high = 1;
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

//    /*
//    * 迭代器相关
//    */
//    NodeAddress IteratorLocate(* tree, Iterator* iterator, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Key* key, LIBYUC_CONTAINER_RB_TREE_COMPARER_Type_Diff* cmp_diff) {
//        RbBsTreeStackVectorInit(&iterator->stack);
//        iterator->cur_id = RbIteratorLocate((RbBsTree*)tree, &iterator->stack, key, cmp_diff);
//        return iterator->cur_id;
//    }
//    NodeAddress IteratorFirst(* tree, Iterator* iterator) {
//        RbBsTreeStackVectorInit(&iterator->stack);
//        iterator->cur_id = RbBsTreeIteratorFirst((RbBsTree*)tree, &iterator->stack);
//        return iterator->cur_id;
//    }
//    NodeAddress IteratorLast(* tree, Iterator* iterator) {
//        RbBsTreeStackVectorInit(&iterator->stack);
//        iterator->cur_id = RbBsTreeIteratorLast((RbBsTree*)tree, &iterator->stack);
//        return iterator->cur_id;
//    }
//    NodeAddress IteratorNext(* tree, Iterator* iterator) {
//        if (iterator->cur_id == kInvalidAddress) {
//            IteratorFirst(tree, iterator);
//            return iterator->cur_id;
//        }
//        iterator->cur_id = RbBsTreeIteratorNext((RbBsTree*)tree, &iterator->stack, iterator->cur_id);
//        return iterator->cur_id;
//    }
//    NodeAddress IteratorPrev(* tree, Iterator* iterator) {
//        if (iterator->cur_id == kInvalidAddress) {
//            IteratorLast(tree, iterator);
//            return iterator->cur_id;
//        }
//        iterator->cur_id = RbBsTreeIteratorPrev((RbBsTree*)tree, &iterator->stack, iterator->cur_id);
//        return iterator->cur_id;
//    }
//    void IteratorCopy(Iterator* dst_iterator, const Iterator* src_iterator) {
//        dst_iterator->cur_id = src_iterator->cur_id;
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Offset count = RbBsTreeStackVectorGetCount((IteratorStack&)&src_iterator->stack);
//        RbBsTreeStackVectorSetCount(&dst_iterator->stack, count);
//        for (LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Offset i = 0; i < count; i++) {
//            *RbBsTreeStackVectorIndex(&dst_iterator->stack, i) = *RbBsTreeStackVectorIndex((IteratorStack&)&src_iterator->stack, i);
//        }
//    }

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
            Node* sibling = allocator_.reference(sibling_id);
            if (sibling_id != kInvalidAddress && sibling->GetColor() == kRed) {
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
    * 允许重复key，同一个节点重复插入时返回false
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
            if (cur_id == node_id) {
                success = false;
                break;
            }
            Node* cur = allocator_.reference(cur_id);
            Key& cur_key = cur->GetElement().key;
            Key& node_key = node->GetElement().key;

            if (cur_key < node_key) {
                if (cur->GetRight() == kInvalidAddress) {
                    cur->SetRight(node_id);
                    break;
                }
                cur_id = cur->GetRight();
            }
            else {
                if (cur_id == node_id) break;
                if (cur->GetLeft() == kInvalidAddress) {
                    cur->SetLeft(node_id);
                    break;
                }
                cur_id = cur->GetLeft();
            }
            allocator_.dereference(cur);
        }
        if (cur) allocator_.dereference(cur);
        allocator_.dereference(node);
        return success;
    }
    /*
    * 向树中插入节点
    * 允许重复key，但插入同一个节点时返回false
    */
    bool Insert(NodeAddress insert_node_id) {
        IteratorStack stack;
        if (!Insert(stack, insert_node_id)) return false;
        InsertFixup(stack, insert_node_id);
        return true;
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

            auto ordering = cur_key <=> node_key;
            if (ordering < 0) {
                if (cur->GetRight() == kInvalidAddress) {
                    cur->SetRight(node_id);
                    break;
                }
                parent_id = cur_id;
                cur_id = cur->GetRight();
            }
            else if (ordering > 0) {
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
    std::tuple<NodeAddress, std::strong_ordering> Find(IteratorStack& stack, const Key& key) {
        NodeAddress cur_id = root_;
        stack.clear();
        NodeAddress perv_id = kInvalidAddress;
        std::strong_ordering ordering;
        while (cur_id != kInvalidAddress) {
            perv_id = cur_id;
            Node* cur = allocator_.reference(cur_id);
            Key& cur_key = cur->GetElement().key;
            ordering = key <=> cur_key;
            if (ordering > 0) {
                cur_id = cur->GetRight();
            }
            else if (ordering < 0) {
                cur_id = cur->GetLeft();
            }
            else {
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
    NodeAddress root_;
};

} // namespace rbt

#endif // RBT_RB_TREE_HPP_