#ifndef GRIDSNORT_TT_H
#define GRIDSNORT_TT_H

#define PREFETCH(addr) __builtin_prefetch(addr, 0, 3)
#include <cstdint>
#include <cstring>

constexpr size_t HASH_SIZE = 8;      // MB
constexpr int    BUCKET_SIZE = 4;

// Each entry is forced to 64 bytes (cache line size)
struct alignas(64) Entry {
    uint64_t hash = 0;
    int      value = 0;
    int      flag = 0;
    uint64_t best_move = 0;

    bool operator==(const Entry& other) const = default;
};

static_assert(sizeof(Entry) == 64, "Entry size must be 64 bytes");

// Memory budget: HASH_SIZE MB
constexpr size_t TOTAL_ENTRIES = (HASH_SIZE * 1024ULL * 1024ULL) / sizeof(Entry);
// Number of buckets = total entries / entries per bucket, rounded down to power of two
constexpr size_t BUCKET_COUNT = 1ULL << (64 - __builtin_clzll((TOTAL_ENTRIES / BUCKET_SIZE) - 1));
static_assert((BUCKET_COUNT & (BUCKET_COUNT - 1)) == 0, "Bucket count must be power of two");
constexpr uint64_t MASK = BUCKET_COUNT - 1;

struct TT {
    Entry table[BUCKET_COUNT][BUCKET_SIZE]{};
    uint8_t replace_idx[BUCKET_COUNT]{};   // per‑bucket round‑robin counter

    const Entry* get_bucket(uint64_t hash) const {
        uint64_t idx = hash & MASK;
        return table[idx];
    }

    void clear() {
        for (auto& bucket : table)
            for (auto& entry : bucket)
                entry = Entry();
        std::memset(replace_idx, 0, sizeof(replace_idx));
    }

    Entry& operator[](uint64_t hash) {
        uint64_t idx = hash & MASK;
        Entry* bucket = table[idx];

        // 1) Try to find an existing entry with the same hash
        for (int i = 0; i < BUCKET_SIZE; ++i) {
            if (bucket[i].hash == hash)
                return bucket[i];
        }

        // 2) Not found – replace using round‑robin (no depth needed)
        int replace = replace_idx[idx];
        replace_idx[idx] = (replace_idx[idx] + 1) % BUCKET_SIZE;
        return bucket[replace];
    }
};

inline auto tt = TT();

#endif // GRIDSNORT_TT_H