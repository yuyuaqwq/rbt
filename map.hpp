#ifndef RBT_MAP_HPP_
#define RBT_MAP_HPP_

/*
* ��ʵ�ϣ�����������������洢int���͵ĳ������������˽���������ݽṹ�У�art������ѡ(�ۺ�ʱ����ռ俪��)��b�������
* ��10M�ڵ�ĳ����£�art������rbtree��8x-15x����btree��������rbtree��3x-5x(btree�ڽڵ��ڲ�ʹ��˳����Ƕ��֣��������û���)
* ���ռ俪����btree�����ܳ����ҡ�
    - btree > art > rb
* 
* ��btree�����rb��ȱ����ɾ��ʱ��ʹ����������ʧЧ��
* 
* ��ʵ�ֵ�rb�����ڲ�ʹ�ø��ڵ㣬ͨ��ջ������·��������������ڵ㣬��ʹÿ���ڵ�Ŀռ俪����Ϊ20bytes����Ȼ�������ڳ����׼��ʵ�֣����ܹ�����Щ�ź���
* ���ڵ�����ʧЧ��Ӱ�첢����̫�󣬹����ɲ��������ڵ㡣
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