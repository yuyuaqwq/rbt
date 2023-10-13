#ifndef MEMORY_POOL_MEMORY_POOL_HPP_

#include <vector>

namespace memory_pool {

template <class T, 
    int16_t block_element_count = 4096 / sizeof(T)>
class Pool {
public:
    using BlockId = int16_t;
    using SlotId = int16_t;
    struct SlotPos {
        BlockId block_id;
        SlotId slot_id;
    };

    constexpr kBlockIdInvalid = -1;
    constexpr kSlotIdInvalid = -1;

public:
    Pool() noexcept {

    }
    ~Pool() noexcept {

    }

    Pool(const Pool&) = delete;
    void operator=(const Pool&) = delete;

    SlotPos Alloc() {
        if (first_.block_id == kBlockIdInvalid) {
            auto new_block = new Slot[block_element_count];
            block_table.push_back(new_block);
            first_.block_id_ = block_table.size() - 1;
            first_.slot_id_ = 0;
        }
        block_table_[first.block_id_];
    }

    void Free(SlotPos& alloc_pos) {

    }

private:
    union Slot {
        T element;
        SlotPos next;
    };
    std::vector<Slot*> block_table_;
    SlotPos first_;
};

} // namespace memory_pool

#endif // MEMORY_POOL_MEMORY_POOL_HPP_