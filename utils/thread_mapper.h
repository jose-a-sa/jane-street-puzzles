#ifndef THREAD_MAPPER_H
#define THREAD_MAPPER_H

#include <cstddef>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

class thread_mapper
{
public:
    constexpr static uint64_t get_this_thread_id()
    {
        auto local_tid = tid_;
        if(local_tid == kUnsetThreadId)
        {
            std::lock_guard lock(mtx_);
            local_tid = unsafe_get_thread_id_(std::this_thread::get_id());
            tid_      = local_tid;
        }
        return local_tid;
    }

    constexpr static uint64_t get_thread_id(std::thread::id const& th)
    {
        std::lock_guard lock(mtx_);
        return unsafe_get_thread_id_(th);
    }

    constexpr static uint64_t set_this_thread_id(uint64_t tid)
    {
        auto local_tid = tid_;
        if(local_tid == kUnsetThreadId)
        {
            std::lock_guard lock(mtx_);
            local_tid = unsafe_set_thread_id_(std::this_thread::get_id(), tid);
            tid_      = local_tid;
        }
        return local_tid;
    }

    constexpr static uint64_t set_thread_id(std::thread::id const& th, uint64_t tid)
    {
        std::lock_guard lock(mtx_);
        return unsafe_set_thread_id_(th, tid);
    }

private:
    thread_mapper()  = delete;
    ~thread_mapper() = default;

    thread_mapper(thread_mapper const&)            = delete;
    thread_mapper& operator=(thread_mapper const&) = delete;

    static constexpr uint64_t kUnsetThreadId = -1;

    constexpr static uint64_t unsafe_get_thread_id_(std::thread::id const& th)
    {
        auto it = thread_map_.find(th);
        if(it == thread_map_.end())
        {
            while(used_ids_.contains(next_thread_id_))
                ++next_thread_id_;

            it = thread_map_.emplace(th, next_thread_id_).first;
            used_ids_.emplace(next_thread_id_);
            ++next_thread_id_;
        }
        return it->second;
    }

    constexpr static uint64_t unsafe_set_thread_id_(std::thread::id const& th, uint64_t tid)
    {
        auto it = thread_map_.find(th);
        if(it == thread_map_.end())
            it = thread_map_.emplace(th, tid).first;
        return it->second;
    }

    static inline std::mutex mtx_{};

    thread_local static inline uint64_t tid_ = kUnsetThreadId;

    static inline uint64_t next_thread_id_ = 0;

    static inline std::unordered_map<std::thread::id, uint64_t> thread_map_{};
    static inline std::unordered_set<uint64_t>                  used_ids_{};
};

#endif // THREAD_MAPPER_H
