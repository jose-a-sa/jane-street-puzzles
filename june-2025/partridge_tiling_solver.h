#ifndef PARTRIDGE_TILING_SOLVER_H
#define PARTRIDGE_TILING_SOLVER_H

#include <array>
#include <bitset>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include "june-2025/partridge_tiling.h"


template<size_t N>
class partridge_square_tiling_solver
{
public:
    constexpr partridge_square_tiling_solver(partridge_square_tiling<N>& tiling)
        : tiling_(tiling),
          solutions_{}
    {}

    constexpr auto& solve() noexcept
    {
        solutions_.reserve(N);
        solutions_.clear();
        try_placing_tile_();
        return solutions_;
    }

private:
    static constexpr size_t kGridSide     = N * (N + 1) / 2;
    static constexpr size_t kGridArea     = kGridSide * kGridSide;
    static constexpr auto   kSideSequence = partridge_square_tiling<N>::kSideSequence;

    partridge_square_tiling<N>& tiling_;

    std::vector<std::array<std::pair<int, int>, kGridSide>> solutions_;

    constexpr auto try_placing_tile_(uint32_t side = 1) noexcept
    {
        if(side > N)
        {
            spdlog::info("Found complete:\n{}", tiling_.tile_positions_);
            solutions_.push_back(tiling_.tile_positions_);
            return;
        }

        auto const curr_count = tiling_.tile_count(side);
        if(curr_count >= side)
        {
            try_placing_tile_(side + 1);
            return;
        }

        for(int r = 0; r < kGridSide - side; ++r)
        {
            for(int c = 0; c < kGridSide - side; ++c)
            {
                if(tiling_.filled_pos_[r][c])
                    continue;

                if(tiling_.try_push_tile({side, r, c}))
                {
                    auto const next_side = curr_count == side ? (side + 1) : side;
                    try_placing_tile_(next_side);

                    tiling_.pop_tile(side);
                }
            }
        }
    }
};


template<size_t N>
class partridge_square_tiling_solver_reversed
{
public:
    constexpr partridge_square_tiling_solver_reversed(partridge_square_tiling<N>& tiling)
        : tiling_(tiling),
          solutions_{}
    {}

    constexpr auto& solve() noexcept
    {
        solutions_.reserve(N);
        solutions_.clear();
        try_placing_tile_();
        return solutions_;
    }

private:
    static constexpr size_t kGridSide     = N * (N + 1) / 2;
    static constexpr size_t kGridArea     = kGridSide * kGridSide;
    static constexpr auto   kSideSequence = partridge_square_tiling<N>::kSideSequence;

    partridge_square_tiling<N>& tiling_;

    std::vector<std::array<std::pair<int, int>, kGridSide>> solutions_;

    constexpr auto try_placing_tile_(uint32_t side = N, std::pair<int, int> last_pos = {0, -1}) noexcept
    {
        if(side == 0)
        {
            solutions_.push_back(tiling_.tile_positions_);
            return;
        }

        if(tiling_.tile_count(side) >= side)
        {
            try_placing_tile_(side - 1);
            return;
        }

        auto const [last_r, last_c] = last_pos;

        for(int r = last_r; r < kGridSide - side + 1; ++r)
        {
            int const c_start = (r == last_r) ? last_c + 1 : 0;
            for(int c = c_start; c < kGridSide - side + 1; ++c)
            {
                if(tiling_.filled_pos_[r][c])
                    continue;

                if(!tiling_.try_push_tile({side, r, c}))
                    continue;

                // needs work to prune the search space

                try_placing_tile_(side, {r, c});

                tiling_.pop_tile(side);
            }
        }
    }
};


#endif // PARTRIDGE_TILING_SOLVER_H
