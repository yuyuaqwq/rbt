#ifndef MEMORY_POOL_MEMORY_POOL_HPP_

#include <vector>

namespace memory_pool {

consteval size_t bit_shift(size_t base) {
    size_t index = 0;
    while (base >>= 1) {
        ++index;
    }
    return index;
}

consteval size_t bit_fill(size_t bit_count) {
    size_t val = 0;
    for (size_t i = 0; i < bit_count; i++) {
        val <<= 1;
        val |= 1;
    }
    return val;

}

//#define USE_PREV
template <class T, 
    size_t block_size = 4096>
class IndexPool {
    //static_assert(kSlotCount > 4 && kSlotCount <= 512, "each block can only have a maximum of 512 slots.");

public:
    using BlockId = uint32_t;
    using SlotId = uint32_t;

    static constexpr size_t kSlotCount = block_size / sizeof(T);
    static constexpr BlockId kBlockIdInvalid = bit_fill((sizeof(BlockId) * 8) - memory_pool::bit_shift(kSlotCount));

    struct SlotPos {
        BlockId block_id : (sizeof(BlockId) * 8) - memory_pool::bit_shift(kSlotCount);
        SlotId slot_id : memory_pool::bit_shift(kSlotCount);
    };
    union Slot {
        T element;
        struct {
#ifdef USE_PREV
            SlotPos prev;
#endif
            SlotPos next;
        };
    };
    struct BlockInfo {
        Slot* slot_array;
#ifdef USE_PREV
        size_t free_kSlotCount;
#endif
    };

public:
    IndexPool() noexcept {
        first_.block_id = kBlockIdInvalid;
        first_.slot_id = 0;
    }
    ~IndexPool() noexcept {
        for (auto& block : block_table_) {
            delete[] block.slot_array;
        }
    }

    IndexPool(const IndexPool&) = delete;
    void operator=(const IndexPool&) = delete;

    SlotPos allocate() {
        SlotPos res{};
        if (first_.block_id == kBlockIdInvalid) {
            // 已经没有空闲的slot了，创建新的block
            auto [_, block_id] = CreateBlock();
            first_.block_id = block_id;
            first_.slot_id = 0;
        }
        res.slot_id = first_.slot_id;
        res.block_id = first_.block_id;
#ifdef USE_PREV
        --block_table_[res.block_id].free_kSlotCount;
#endif
        auto& alloc_slot = block_table_[res.block_id].slot_array[res.slot_id];

        first_.block_id = alloc_slot.next.block_id;
        first_.slot_id = alloc_slot.next.slot_id;

#ifdef USE_PREV
        if (first_.block_id != kBlockIdInvalid) {
            // first的prev一定是无效的
            auto& new_first = block_table_[first_.block_id].slot_array[first_.slot_id];
            new_first.prev.block_id = kBlockIdInvalid;
            new_first.prev.slot_id = 0;
        }
#endif

        return res;
    }

    void deallocate(SlotPos& free_pos) {
        //if (block_table_[free_pos.block_id].free_kSlotCount == kSlotCount) {
        //    // 回收此block
        //    for (auto& slot : block_table_[free_pos.block_id].slot_array) {
        //        // 若prev指向当前的block，且next也指向当前的block，跳过此slot

        //        // 若prev指向其他的block。则循环next，找到第一个不指向当前block的slot，连接他们
        //        // 若next指向其他的block，循环prev，找到第一个不指向当前block的slot，连接他们

        //        if (slot.prev.block_id != free_pos.block_id) {

        //        }
        //    }
        //}
        auto& slot = block_table_[free_pos.block_id].slot_array[free_pos.slot_id];
#ifdef USE_PREV
        slot.prev.block_id = kBlockIdInvalid;
        slot.prev.slot_id = 0;
#endif
        slot.next.block_id = first_.block_id;
        slot.next.slot_id = first_.slot_id;
#ifdef USE_PREV
        if (first_.block_id != kBlockIdInvalid) {
            auto& old_first = block_table_[first_.block_id].slot_array[first_.slot_id];
            old_first.prev.block_id = kBlockIdInvalid;
            old_first.prev.slot_id = 0;
        }
#endif

        first_.block_id = free_pos.block_id;
        first_.slot_id = free_pos.slot_id;
        free_pos.block_id = kBlockIdInvalid;
        free_pos.slot_id = 0;
    }

private:
    std::tuple<Slot*, BlockId> CreateBlock() {
        auto new_block = new Slot[kSlotCount];
        auto block_id = block_table_.size();
#ifdef USE_PREV
        block_table_.push_back(BlockInfo{ new_block, kSlotCount });
        for (int32_t i = 1; i < kSlotCount; i++) {
            new_block[i].prev.block_id = block_id;
            new_block[i].prev.slot_id = i - 1;
            new_block[i].next.block_id = block_id;
            new_block[i].next.slot_id = i + 1;
        }
        auto& first_slot = new_block[0];
        first_slot.prev.block_id = kBlockIdInvalid;
        first_slot.prev.slot_id = 0;
        first_slot.next.block_id = block_id;
        first_slot.next.slot_id = 1;
        auto& tail_slot = new_block[kSlotCount - 1];
        tail_slot.prev.block_id = block_id;
        tail_slot.prev.slot_id = kSlotCount - 2;
#else
        block_table_.push_back(BlockInfo{ new_block });
        for (int32_t i = 0; i < kSlotCount; i++) {
            new_block[i].next.block_id = block_id;
            new_block[i].next.slot_id = i + 1;
        }
        auto& tail_slot = new_block[kSlotCount - 1];
#endif

        tail_slot.next.block_id = first_.block_id;
        tail_slot.next.slot_id = first_.slot_id;

        return std::tuple{ new_block, block_id };
    }


private:
    std::vector<BlockInfo> block_table_;
    SlotPos first_;
};

} // namespace memory_pool

#endif // MEMORY_POOL_MEMORY_POOL_HPP_