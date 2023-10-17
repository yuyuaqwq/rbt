#ifndef MEMORY_POOL_MEMORY_POOL_HPP_

#include <vector>

namespace spoo {

namespace detail {
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
}

template <class T, size_t block_size = 4096>
class MemoryPool {
public:
    using SlotPos = uint32_t;
    using BlockId = uint32_t;
    using SlotId = uint32_t;
	
	union Slot {
        SlotPos next;
        T element;
    };
    struct BlockInfo {
        Slot* slot_array;
    };

    static constexpr size_t kBlockSlotCount = block_size / sizeof(T);
    static constexpr size_t kBlockShift = detail::bit_shift(kBlockSlotCount);
    static constexpr size_t kSlotMark = kBlockSlotCount - 1;
    static constexpr SlotPos kSlotPosInvalid = detail::bit_fill(sizeof(SlotPos));
  
public:
    MemoryPool() noexcept {
        free_slot_ = kSlotPosInvalid;
        begin_slot_ = kSlotPosInvalid;
        end_slot_ = kSlotPosInvalid;
    }
    ~MemoryPool() noexcept {
        for (auto& block : block_table_) {
            delete[] block.slot_array;
        }
    }

    MemoryPool(const MemoryPool&) = delete;
    void operator=(const MemoryPool&) = delete;

    SlotPos allocate() {
        SlotPos res{};
        if (free_slot_ != kSlotPosInvalid) {
            res = free_slot_;
            auto [block_id, slot_id] = SplitId(res);
            auto& alloc_slot = block_table_[block_id].slot_array[slot_id];
            free_slot_ = alloc_slot.next;
            return res;
        }
        else {
            if (begin_slot_ >= end_slot_) {
                CreateBlock();
            }
            return begin_slot_++;
        }
    }

    void deallocate(SlotPos& free_pos) {
        if (free_pos != kSlotPosInvalid) {
            auto [block_id, slot_id] = SplitId(free_pos);
            auto& slot = block_table_[block_id].slot_array[slot_id];
            slot.next = free_slot_;
            free_slot_ = free_pos;
            free_pos = kSlotPosInvalid;
        }
    }

    T* get_ptr(const SlotPos& pos) noexcept {
        auto [block_id, slot_id] = SplitId(pos);
        auto& alloc_slot = block_table_[block_id].slot_array[slot_id];
        return &alloc_slot.element;
    }

private:
    std::tuple<BlockId, SlotId> SplitId(const SlotPos& slot_pos) const noexcept {
        BlockId block_id = slot_pos / kBlockSlotCount;
        SlotId slot_id = slot_pos % kBlockSlotCount;
        //BlockId block_id = slot_pos >> kBlockShift;
        //SlotId slot_id = slot_pos & kSlotMark;
        return std::tuple{ block_id, slot_id };
    }

    void CreateBlock() {
        auto new_block = new Slot[kBlockSlotCount];
        auto slot_pos = block_table_.size() * kBlockSlotCount;
        block_table_.push_back(BlockInfo{ new_block });

        begin_slot_ = slot_pos;
        end_slot_ = slot_pos + kBlockSlotCount;
    }

private:
    std::vector<BlockInfo> block_table_;

    SlotPos free_slot_;

    SlotPos begin_slot_;
    SlotPos end_slot_;
};

} // namespace spoo

#endif // MEMORY_POOL_MEMORY_POOL_HPP_