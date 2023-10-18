#ifndef RBT_RB_TREE_HPP_
#define RBT_RB_TREE_HPP_

#include <utility>

namespace rbt {

template <class Traits>
class RbTree {
private:
    using ValueType = typename Traits::ValueType;
    using AllocatorType = typename Traits::AllocatorType;
    
    enum Color {
        kBlack,
        kRed,
    };
    struct Node {
        
    };
    using NodeAllocatorType = typename std::allocator_traits<AllocatorType>::template rebind_alloc<Node>;
    using Address = decltype(std::declval<NodeAllocatorType>().allocate());



private:
    Address root_;

public:
//    static LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbRotateRight(RbTree* tree, RbEntry* sub_root_parent, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id sub_root_id, RbEntry* sub_root) {
//        return (LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id)RbBsRotateRight(&tree->bs_tree, (RbBsEntry*)sub_root_parent, sub_root_id, (RbBsEntry*)sub_root);
//    }
//
//    static LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbRotateLeft(RbTree* tree, RbEntry* sub_root_parent, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id sub_root_id, RbEntry* sub_root) {
//        return (LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id)RbBsRotateLeft(&tree->bs_tree, (RbBsEntry*)sub_root_parent, sub_root_id, (RbBsEntry*)sub_root);
//    }
//
//
//    /*
//    * ȡ�ֵܽڵ�
//    */
//    static LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id GetSiblingEntry(RbTree* tree, RbEntry* entry_parent, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id entry_id, RbEntry* entry) {
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id ret;
//        if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, entry_parent) == entry_id) {
//            ret = LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, entry_parent);
//        }
//        else {
//            assert(LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, entry_parent) == entry_id);
//            ret = LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, entry_parent);
//        }\
//            return ret;
//    }
//
//    /*
//    * �����в���ڵ���ƽ�����
//    */
//    static void RbTreeInsertFixup(RbTree* tree, RbBsTreeStackVector* stack, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id ins_entry_id) {
//        RbEntry* ins_entry = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, ins_entry_id);
//        LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, ins_entry, kRbBlack);
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id* cur_id = RbBsTreeStackVectorPopTail(stack);
//        if (cur_id == NULL) {
//            LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, ins_entry, kRbBlack);
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, ins_entry);
//            return;
//        }
//        LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, ins_entry, kRbRed);
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, ins_entry);
//
//        RbEntry* cur = NULL;
//        /* ��ʼ����ά�� */
//        while (cur_id != NULL) {
//            cur = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, *cur_id);
//            if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, cur) == kRbBlack) {
//                /* ��ǰ�ڵ�(����ڵ�ĸ��ڵ�)�Ǻ�ɫ��ɶ��������(��2�ڵ� / 3�ڵ�Ĳ��룬ֱ�Ӻϲ�) */
//                break;
//            }
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id* parent_id = RbBsTreeStackVectorPopTail(stack);
//            if (parent_id == NULL) {
//                /* û�и��ڵ㣬���ݵ����ڵ��ˣ�ֱ��Ⱦ�� */
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, cur, kRbBlack);
//                break;
//            }
//            RbEntry* parent = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, *parent_id);
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id sibling_id = GetSiblingEntry(tree, parent, *cur_id, cur);
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, parent);
//            RbEntry* sibling = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, sibling_id);
//            if (sibling_id != LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId && LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, sibling) == kRbRed) {
//                /* �ֵܽڵ��Ǻ�ɫ��˵����4�ڵ�Ĳ��룬����(����������־��Ǳ�ɫ)�����ڵ����ϲ��룬�������� */
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, cur, kRbBlack);
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, sibling, kRbBlack);
//                ins_entry_id = *parent_id;         /* ����Ϊ�ýڵ����ϲ��� */
//                ins_entry = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, ins_entry_id);
//                if (RbBsTreeStackVectorGetTail(stack) == NULL) {
//                    LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, ins_entry, kRbBlack);     /* ���ڵ㣬Ⱦ�ڲ����� */
//                    break;
//                }
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, ins_entry, kRbRed);
//                parent_id = RbBsTreeStackVectorPopTail(stack);
//                /* ��parent��������ڵ� */
//            }
//            else {
//                /* û���ֵܽڵ���ֵܽڵ��Ǻ�ɫ��˵����3�ڵ�Ĳ��룬���Բ��룬����Ҫ������ת�����Ϊ4�ڵ�
//                    *        10b                                5b
//                    *     5r     20b    ->     !2r         10r
//                    * !2r                                                 20b
//                */
//                assert(sibling_id == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId || LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, sibling) == kRbBlack);
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id new_sub_root_id;
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id old_sub_root_id = *parent_id;
//                RbEntry* old_sub_root = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, old_sub_root_id);
//                RbEntry* new_sub_root_parent = NULL;
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id* new_sub_root_parent_id = RbBsTreeStackVectorGetTail(stack);
//                if (new_sub_root_parent_id) new_sub_root_parent = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, *new_sub_root_parent_id);
//                if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, old_sub_root) == *cur_id) {
//                    if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, cur) == ins_entry_id) {
//                        RbRotateLeft(tree, old_sub_root, *cur_id, cur);
//                    }
//                    new_sub_root_id = RbRotateRight(tree, new_sub_root_parent, old_sub_root_id, old_sub_root);
//                }
//                else {
//                    if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, cur) == ins_entry_id) {
//                        RbRotateRight(tree, old_sub_root, *cur_id, cur);
//                    }
//                    new_sub_root_id = RbRotateLeft(tree, new_sub_root_parent, old_sub_root_id, old_sub_root);
//                }
//                if (new_sub_root_parent_id) LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, new_sub_root_parent);
//                RbEntry* new_sub_root = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, new_sub_root_id);
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, new_sub_root, kRbBlack);
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, old_sub_root, kRbRed);
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, new_sub_root);
//
//                if (tree->root == old_sub_root_id) {        /* ԭ�����������ڵ�������������ĸ��ڵ㣬���Ҫ������ */
//                    tree->root = new_sub_root_id;
//                }
//                break;        /* ֻ�ǲ��룬δ���ѣ�����û�иı���ɫ��������Ҫ���� */
//            }
//            cur_id = parent_id;
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, cur);
//        }
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, cur);
//    }
//    /*
//    * ������ɾ���ڵ���ƽ�����
//    */
//    static void RbTreeDeleteFixup(RbTree* tree, RbBsTreeStackVector* stack, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id del_entry_id, bool is_parent_left) {
//        RbEntry* del_entry = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, del_entry_id);
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id* parent_id = NULL;
//
//        RbColor del_color = LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, del_entry);
//        if (del_color == kRbRed) { /* �Ǻ�ɫ�ģ���3/4�ڵ㣬��Ϊ��ʱһ����Ҷ�ӽڵ�(��ڵ㲻����ֻ��һ���ӽڵ�)��ֱ���Ƴ� */ }
//        /* �Ǻ�ɫ�ģ�������һ���ӽڵ㣬˵����3�ڵ㣬��Ϊ2�ڵ㼴�� */
//        else if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, del_entry) != LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) {
//            RbEntry* del_entry_left = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, del_entry));
//            LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, del_entry_left, kRbBlack);
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, del_entry_left);
//        }
//        else if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, del_entry) != LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) {
//            RbEntry* del_entry_right = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, del_entry));
//            LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, del_entry_right, kRbBlack);
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, del_entry_right);
//        }
//        else { parent_id = RbBsTreeStackVectorPopTail(stack); }
//
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id new_sub_root_id;
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id* grandpa_id = NULL;
//        /* ����ά��ɾ����ɫ�ڵ㣬��û���ӽڵ�(2�ڵ�)����� */
//        RbEntry* parent = NULL, * sibling = NULL, * grandpa = NULL;
//        if (parent_id) parent = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, *parent_id);
//        while (parent_id != NULL) {
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id sibling_id = is_parent_left ? LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, parent) : LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, parent);
//            sibling = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, sibling_id);
//            grandpa_id = RbBsTreeStackVectorPopTail(stack);
//            if (grandpa_id) grandpa = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, *grandpa_id);
//            else grandpa = NULL;
//            /* ��ɫ�ڵ�һ�����ֵܽڵ� */
//            if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, sibling) == kRbRed) {
//                /* �ֵܽڵ�Ϊ�죬˵���ֵܽڵ��븸�ڵ��γ�3�ڵ㣬�������ֵܽڵ�Ӧ���Ǻ��ֵܽڵ���ӽڵ�
//                    ��ת����ʱֻ��ʹ���ֵܽڵ�͸��ڵ��γɵ�3�ڵ��ɫ����λ�õ�������ǰ�ڵ���ֵܽڵ��Ϊԭ�ֵܽڵ���ӽڵ� */
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id old_sub_root_id = *parent_id;
//                RbEntry* old_sub_root = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, old_sub_root_id);
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, old_sub_root, kRbRed);
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, sibling, kRbBlack);
//                if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, old_sub_root) == sibling_id) {
//                    new_sub_root_id = RbRotateRight(tree, grandpa, old_sub_root_id, old_sub_root);
//                    sibling_id = LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, old_sub_root);     /* �½���ҽӹ����Ľڵ� */
//                    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, sibling);
//                    sibling = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, sibling_id);
//                }
//                else {
//                    new_sub_root_id = RbRotateLeft(tree, grandpa, old_sub_root_id, old_sub_root);
//                    sibling_id = LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, old_sub_root);     /* �½���ҽӹ����Ľڵ� */
//                    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, sibling);
//                    sibling = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, sibling_id);
//                }
//                if (tree->root == old_sub_root_id) {
//                    tree->root = new_sub_root_id;
//                }
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, old_sub_root);
//                /* grandpa��Ϊ�¸��ڵ� */
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, grandpa);
//                grandpa_id = &new_sub_root_id;
//                grandpa = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, *grandpa_id);
//            }
//
//            /* �����ֵܽڵ�һ��Ϊ�� */
//
//            /* ֶ�ӽڵ�Ϊ�죬���ֵܽڵ���3 / 4�ڵ����������ֵܽ�ڵ�(�����ֵܽڵ㣬�½����׽ڵ�) */
//            RbEntry* sibling_right = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, sibling));
//            RbEntry* sibling_left = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, sibling));
//            if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, sibling) != LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId && LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, sibling_right) == kRbRed ||
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, sibling) != LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId && LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, sibling_left) == kRbRed) {
//                RbColor parent_color = LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, parent);
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, parent, kRbBlack);
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id old_sub_root_id = *parent_id;
//                RbEntry* new_sub_root_parent = NULL;
//                if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, parent) == sibling_id) {
//                    if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, sibling) == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId || LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, sibling_left) == kRbBlack) {
//                        LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, sibling_right, kRbBlack);
//                        sibling_id = RbRotateLeft(tree, parent, sibling_id, sibling);
//                    }
//                    else {
//                        LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, sibling_left, kRbBlack);
//                    }
//                    new_sub_root_id = RbRotateRight(tree, grandpa, *parent_id, parent);
//                }
//                else {
//                    if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, sibling) == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId || LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, sibling_right) == kRbBlack) {
//                        LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, sibling_left, kRbBlack);
//                        sibling_id = RbRotateRight(tree, parent, sibling_id, sibling);
//                    }
//                    else {
//                        LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, sibling_right, kRbBlack);
//                    }
//                    new_sub_root_id = RbRotateLeft(tree, grandpa, *parent_id, parent);
//                }
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, sibling);
//                sibling = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, sibling_id);
//                /* �ýڵ�����ԭ�ȵ��Ӹ��ڵ㣬ҲҪ������ɫ */
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, sibling, parent_color);
//                if (tree->root == old_sub_root_id) {
//                    tree->root = new_sub_root_id;
//                }
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, sibling_right);
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, sibling_left);
//                break;
//            }
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, sibling_right);
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, sibling_left);
//
//            if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, parent) == kRbRed) {
//                /* ���ڵ�Ϊ�죬�����ڵ���3/4�ڵ㣬�����½����ֵܽڵ�ϲ�
//                    |5|8|                 |5|
//                        /    |        ->        /
//                    3     6    -9-            3     |6|8| */
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, sibling, kRbRed);
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, parent, kRbBlack);
//                break;
//            }
//            else {
//                /* ���ڵ�Ϊ�ڣ������ڵ���2�ڵ㣬�ֵܽڵ�Ҳ��2�ڵ㣬�½����ڵ����ֵܽڵ�ϲ����൱������ɾ�����ڵ㣬��������
//                    Ϊʲô����3/4�ڵ㣿��Ϊ�ڸ��ڵ������3���ֵܽڵ��Ǻ죬4�Ļ�����ʱ���ڵ��Ǻ� */
//                LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, sibling, kRbRed);
//            }
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id child_id = *parent_id;
//            parent_id = grandpa_id;
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, parent);
//            parent = grandpa;
//            if (parent != NULL) {
//                is_parent_left = LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, parent) == child_id;
//            }
//        }
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, sibling);
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, parent);
//
//        RbEntry* root = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, tree->root);
//        if (root && LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, root) == kRbRed) {
//            /* ���ڵ�Ⱦ�� */
//            LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, root, kRbBlack);
//        }
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, root);
//    }
//    /*
//    * ��ʼ����
//    */
//    void RbTreeInit(RbTree* tree) {
//        RbBsTreeInit(&tree->bs_tree);
//    }
//    /*
//    * �����в��ҽڵ�
//    * ���ڷ��ز��ҵ��Ľڵ��Ӧ�Ķ��󣬲����ڷ���NULL/���һ�β��ҵĽڵ�
//    */
//    static LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbTreeFindInternal(RbTree* tree, RbBsTreeStackVector* stack, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Key* key) {
//        return RbBsTreeFind(&tree->bs_tree, stack, key);
//    }
//    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbTreeFind(RbTree* tree, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Key* key) {
//        RbBsTreeStackVector stack;
//        RbBsTreeStackVectorInit(&stack);
//        return RbTreeFindInternal(tree, &stack, key);
//    }
//    /*
//    * �����в���ڵ�
//    * �����ظ�key��������ͬһ���ڵ�ʱ����false
//    */
//    bool RbTreeInsert(RbTree* tree, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id insert_entry_id) {
//        RbBsTreeStackVector stack;
//        RbBsTreeStackVectorInit(&stack);
//        if (!RbBsTreeInsert(&tree->bs_tree, &stack, insert_entry_id)) return false;
//        RbTreeInsertFixup(tree, &stack, insert_entry_id);
//        return true;
//    }
//    /*
//    * ����������ڵ�
//    * �������ظ�key�����ر����ǵ�ԭentry������InvalidId�����put_entry_id�Ѿ���������ˣ�Ҳ�ᱻ����(����ֵ == put_entry_id)
//    */
//    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbTreePut(RbTree* tree, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id put_entry_id) {
//        RbBsTreeStackVector stack;
//        RbBsTreeStackVectorInit(&stack);
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id old_id = RbBsTreePut(&tree->bs_tree, &stack, put_entry_id);
//        if (old_id == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) RbTreeInsertFixup(tree, &stack, put_entry_id);
//        return old_id;
//    }
//    /*
//    * ɾ������ָ���ڵ�
//    */
//    static bool RbTreeDeleteInternal(RbTree* tree, RbBsTreeStackVector* stack, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id del_entry_id) {
//        assert(del_entry_id != LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId);
//        bool is_parent_left;
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id del_min_entry_id = RbBsTreeDelete(&tree->bs_tree, stack, del_entry_id, &is_parent_left);
//        if (del_min_entry_id == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) {
//            return false;
//        }
//        if (del_min_entry_id != del_entry_id) {
//            RbEntry* del_entry = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, del_entry_id);
//            RbEntry* del_min_entry = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, del_min_entry_id);
//            /* ��Ҫ������ɫ */;
//            RbColor old_color = LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, del_min_entry);
//            LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, del_min_entry, LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, del_entry));
//            LIBYUC_CONTAINER_RB_TREE_ACCESSOR_SetColor(tree, del_entry, old_color);
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, del_min_entry);
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, del_entry);
//        }
//        RbTreeDeleteFixup(tree, stack, del_entry_id, is_parent_left);
//        return true;
//    }
//    bool RbTreeDelete(RbTree* tree, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Key* key) {
//        RbBsTreeStackVector stack;
//        RbBsTreeStackVectorInit(&stack);
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id del_entry_id = RbTreeFindInternal(tree, &stack, key);
//        if (del_entry_id == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) return false;
//        return RbTreeDeleteInternal(tree, &stack, del_entry_id);
//    }
//    bool RbTreeDeleteByIterator(RbTree* tree, RbTreeIterator* iterator) {
//        if (iterator->cur_id == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) return false;
//        bool success = RbTreeDeleteInternal(tree, &iterator->stack, iterator->cur_id);
//        iterator->cur_id = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId;
//        return success;
//    }
//    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Offset RbTreeGetCount(RbTree* tree) {
//        RbBsTreeStackVector stack;
//        RbBsTreeStackVectorInit(&stack);
//        return RbBsTreeGetCount((RbBsTree*)tree, &stack);
//    }
//    /*
//    * ���������
//    */
//    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbTreeIteratorLocate(RbTree* tree, RbTreeIterator* iterator, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Key* key, LIBYUC_CONTAINER_RB_TREE_COMPARER_Type_Diff* cmp_diff) {
//        RbBsTreeStackVectorInit(&iterator->stack);
//        iterator->cur_id = RbBsTreeIteratorLocate((RbBsTree*)tree, &iterator->stack, key, cmp_diff);
//        return iterator->cur_id;
//    }
//    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbTreeIteratorFirst(RbTree* tree, RbTreeIterator* iterator) {
//        RbBsTreeStackVectorInit(&iterator->stack);
//        iterator->cur_id = RbBsTreeIteratorFirst((RbBsTree*)tree, &iterator->stack);
//        return iterator->cur_id;
//    }
//    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbTreeIteratorLast(RbTree* tree, RbTreeIterator* iterator) {
//        RbBsTreeStackVectorInit(&iterator->stack);
//        iterator->cur_id = RbBsTreeIteratorLast((RbBsTree*)tree, &iterator->stack);
//        return iterator->cur_id;
//    }
//    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbTreeIteratorNext(RbTree* tree, RbTreeIterator* iterator) {
//        if (iterator->cur_id == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) {
//            RbTreeIteratorFirst(tree, iterator);
//            return iterator->cur_id;
//        }
//        iterator->cur_id = RbBsTreeIteratorNext((RbBsTree*)tree, &iterator->stack, iterator->cur_id);
//        return iterator->cur_id;
//    }
//    LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id RbTreeIteratorPrev(RbTree* tree, RbTreeIterator* iterator) {
//        if (iterator->cur_id == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) {
//            RbTreeIteratorLast(tree, iterator);
//            return iterator->cur_id;
//        }
//        iterator->cur_id = RbBsTreeIteratorPrev((RbBsTree*)tree, &iterator->stack, iterator->cur_id);
//        return iterator->cur_id;
//    }
//    void RbTreeIteratorCopy(RbTreeIterator* dst_iterator, const RbTreeIterator* src_iterator) {
//        dst_iterator->cur_id = src_iterator->cur_id;
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Offset count = RbBsTreeStackVectorGetCount((RbBsTreeStackVector*)&src_iterator->stack);
//        RbBsTreeStackVectorSetCount(&dst_iterator->stack, count);
//        for (LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Offset i = 0; i < count; i++) {
//            *RbBsTreeStackVectorIndex(&dst_iterator->stack, i) = *RbBsTreeStackVectorIndex((RbBsTreeStackVector*)&src_iterator->stack, i);
//        }
//    }
//    /*
//    * ��֤���������
//    */
//    static bool RbTreeCheckPath(RbTree* tree, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id parent_id,
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id entry_id, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Offset cur_high,
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Offset max_high) {
//        if (entry_id == LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) {
//            return cur_high == max_high;
//        }
//        bool correct = false;
//        RbEntry* entry = NULL;
//        RbEntry* parent = NULL;
//        do {
//            entry = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, entry_id);
//            if (parent_id != LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) {
//                parent = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, parent_id);
//                if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, entry) == kRbRed && LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, parent) == kRbRed) {
//                    break;
//                }
//            }
//            if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, entry) == kRbBlack) { cur_high++; }
//            correct = RbTreeCheckPath(tree, entry_id, LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, entry), cur_high, max_high) && RbTreeCheckPath(tree, entry_id, LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetRight(tree, entry), cur_high, max_high);
//        } while (false);
//        if (entry) LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, entry);
//        if (parent) LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, parent);
//        return correct;
//    }
//    bool RbTreeVerify(RbTree* tree) {
//        RbEntry* entry = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, tree->root);
//        if (!entry) return true;
//        bool correct = false;
//        do {
//            if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, entry) != kRbBlack) break;
//            LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Offset high = 1;
//            while (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, entry) != LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId) {
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Type_Id entry_id = LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetLeft(tree, entry);
//                LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, entry);
//                entry = LIBYUC_CONTAINER_RB_TREE_REFERENCER_Reference(tree, entry_id);
//                /* ��ɫ�ڵ㲻��ʾ�߶� */
//                if (LIBYUC_CONTAINER_RB_TREE_ACCESSOR_GetColor(tree, entry) == kRbBlack) {
//                    high++;
//                }
//            }
//            correct = RbTreeCheckPath(tree, LIBYUC_CONTAINER_RB_TREE_REFERENCER_Const_InvalidId, tree->root, 0, high);
//        } while (false);
//        LIBYUC_CONTAINER_RB_TREE_REFERENCER_Dereference(tree, entry);
//        return correct;
//    }

};

} // namespace rbt

#endif // RBT_RB_TREE_HPP_