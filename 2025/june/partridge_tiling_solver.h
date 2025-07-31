#ifndef PARTRIDGE_TILING_SOLVER_H
#define PARTRIDGE_TILING_SOLVER_H


#include <array>
#include <cstdint>
#include <vector>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include "2025/june/partridge_tiling.h"


template<size_t N, bool Reversed = true>
class partridge_square_tiling_solver
{
public:
    constexpr partridge_square_tiling_solver(partridge_square_tiling<N>& tiling)
        : tiling_(tiling),
          solutions_{}
    {}

    constexpr auto& find_all() noexcept
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

    std::array<size_t, 1> optimization_counts_{};

    constexpr auto try_placing_tile_(uint32_t const            side     = (Reversed ? N : 1),
                                     std::pair<int, int> const last_pos = {0, -1}) noexcept
    {
        auto const [reached_solution, next_side] = [&]
        {
            if constexpr(Reversed)
            {
                if(side == 0)
                    return std::make_pair(true, 0u);
                else
                    return std::make_pair(false, side - 1);
            }
            else
            {
                if(side == N)
                    return std::make_pair(true, N);
                else
                    return std::make_pair(false, side + 1);
            }
        }();

        if(reached_solution)
        {
            auto tiles_view = std::views::zip(partridge_square_tiling<9>::kSideSequence, tiling_.tile_positions());
            spdlog::info("Found the solution: {}", tiles_view);
            solutions_.emplace_back(tiling_.tile_positions());
            return;
        }

        if(tiling_.tile_count(side) >= side)
        {
            try_placing_tile_(next_side);
            return;
        }

        auto const [last_r, last_c] = last_pos;
        auto const& filled_mask     = tiling_.filled_mask_array();

        auto const max_pos = static_cast<int>(kGridSide - side);
        for(int r = last_r; r <= max_pos; ++r)
        {
            int const c_start = (r == last_r) ? (last_c + 1) : 0;
            for(int c = c_start; c <= max_pos; ++c)
            {
                if(filled_mask[r][c])
                    continue;

                // If the tile is large enough that the gaps cannot be filled with smaller tiles then
                // ignore this position
                std::array<int, 4> const gaps{r, c, max_pos - r, max_pos - c};
                if(std::ranges::fold_left(gaps, false, [&](bool acc, auto dx) { return acc | ((1 <= dx) & (dx <= 3) & (dx * dx < side)); }))
                    continue;

                square_tile const t{side, r, c};

                if(tiling_.overlaps_with_placed(t))
                    continue;

                tiling_.unchecked_push_tile(t);

                try_placing_tile_(side, {r, c});

                tiling_.pop_tile(side);
            }
        }
    }
};


#endif // PARTRIDGE_TILING_SOLVER_H
