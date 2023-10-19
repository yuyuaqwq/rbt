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
    * ��֤���������
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

//    /*
//    * ���������
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
            Node* sibling = allocator_.reference(sibling_id);
            if (sibling_id != kInvalidAddress && sibling->GetColor() == kRed) {
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
    * �����ظ�key��ͬһ���ڵ��ظ�����ʱ����false
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
    * �����в���ڵ�
    * �����ظ�key��������ͬһ���ڵ�ʱ����false
    */
    bool Insert(NodeAddress insert_node_id) {
        IteratorStack stack;
        if (!Insert(stack, insert_node_id)) return false;
        InsertFixup(stack, insert_node_id);
        return true;
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
    NodeAddress root_;
};

} // namespace rbt

#endif // RBT_RB_TREE_HPP_