#ifndef MIRROR_GRID_SOLVER_H
#define MIRROR_GRID_SOLVER_H

#include <algorithm>
#include <array>
#include <ranges>
#include <span>
#include <sys/syslimits.h>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <spdlog/spdlog.h>

#include "2025/march/integer_factorizations.h"
#include "2025/march/mirror_grid.h"
#include "utils/restorer.h"


class mirror_grid_solver
{
public:
    using num_type       = mirror_grid::num_type;
    using laser_position = mirror_grid::laser_position;

    mirror_grid_solver(mirror_grid& grid)
        : grid_{grid},
          factorizations_{}
    {}

    bool solve()
    {
        init_factorizations_();
        return try_next_number_();
    }

    constexpr auto init() { init_factorizations_(); }

    constexpr auto& factorizations() const noexcept { return factorizations_; }

private:
    static constexpr auto kPlacements =
        std::array<direction, 4>{direction::Left, direction::Top, direction::Right, direction::Bottom};

    mirror_grid& grid_;

    std::vector<std::tuple<integer_factorizations, direction, int>> factorizations_{};

    std::vector<uint32_t> numbers_storage_;

    // Branchless way to determine which mirror should be placed to terminate the path when is at the border `loc`
    // approaching from the direaction `dir`
    static constexpr mirror_type mirror_border_placement_(direction loc, direction dir) noexcept
    {
        using enum mirror_type;
        static constexpr auto border_placement_map =
            std::array<std::array<mirror_type, 5>, 5>{{{None, LR, None, RL, None},
                                                       {LR, None, RL, None, None},
                                                       {None, RL, None, LR, None},
                                                       {RL, None, LR, None, None},
                                                       {None, None, None, None, None}}};
        return border_placement_map[std::to_underlying(loc)][std::to_underlying(dir)];
    }

    void init_factorizations_()
    {
        int const grid_len = grid_.length();
        factorizations_.reserve(4 * grid_len);

        // auto cartesian = std::views::cartesian_product(kPlacements, std::views::iota(0, grid_len));
        for(auto const placement: kPlacements)
        {
            for(auto const loc: std::views::iota(0, grid_len))
            {
                auto const x = grid_.boundary_number(placement, loc);
                if(x == 0)
                    continue;

                factorizations_.emplace_back(integer_factorizations(x, grid_len), placement, loc);
            }
        }

        std::ranges::sort(factorizations_,
                          [](auto const& a, auto const& b) { return std::get<0>(a).size() < std::get<0>(b).size(); });

        spdlog::debug("Number order: {}",
                      factorizations_ | std::views::transform([](auto& t) { return std::get<0>(t).number(); }));
    }

    constexpr bool try_next_number_(size_t const number_idx = 0)
    {
        if(number_idx >= factorizations_.size())
        {
            spdlog::debug("Completed iterating input numbers. Trying to complete grid: \n{}", grid_);
            return try_complete_grid_();
        }

        spdlog::debug("CURRENT STATE: \n{}", grid_);
        spdlog::debug("Trying number_idx={} out of {} numbers", number_idx, factorizations_.size());

        auto& [factorizations, placement, loc] = factorizations_[number_idx];
        spdlog::debug("Started with number {} on {}[{}]", factorizations.number(), placement, loc);

        auto const start_pos = laser_position::start_position(placement, loc, grid_.length()).advance();

        for(auto factors: factorizations | std::views::reverse)
        {
            auto const total_factors =
                std::ranges::fold_left(factors, uint32_t{0}, [](uint32_t acc, auto const& f) { return acc + f.count; });

            spdlog::debug("Trying factorization {} of {}[{}]={} (total_factors={}). Starting at ({},{}), dir={}",
                          factors, placement, loc, factorizations.number(), total_factors, start_pos.row, start_pos.col,
                          start_pos.dir);

            if(try_next_factor_(number_idx, factors, 0, total_factors, start_pos))
                return true;
        }

        return false;
    }

    constexpr bool try_complete_grid_()
    {
        // qs::restorer_bind(ref, external) avoids allocating a new vector to backup grid_.numbers_array() per call
        // This only works because not more recursive calls are made for the function.
        qs::restorer rx(qs::defer_restore, qs::restorer_bind(grid_.numbers_array(), numbers_storage_));

        int const grid_len = grid_.length();

        for(auto const placement: kPlacements)
        {
            for(auto const loc: std::views::iota(0, grid_len))
            {
                auto       pos       = laser_position::start_position(placement, loc, grid_len);
                auto const start_num = grid_.boundary_number(placement, loc);

                spdlog::trace("Starting path from {}[{}] = {}, at ({}, {}), dir={}", placement, loc, start_num, pos.row,
                              pos.col, pos.dir);

                int segment_len   = 0;
                int num_from_path = 1;

                do
                {
                    pos.advance();
                    ++segment_len;

                    auto const curr_mirror = grid_.mirror(pos.row, pos.col);
                    auto const next_dir    = direction_after_mirror(curr_mirror, pos.dir);
                    if(pos.dir != next_dir)
                    {
                        num_from_path *= segment_len;
                        segment_len = 0;
                        pos.dir     = next_dir;
                    }
                }
                while(grid_.in_bounds(pos.row, pos.col));

                if(segment_len > 0)
                    num_from_path *= segment_len;

                bool const is_valid_endpoint = start_num == 0 || start_num == num_from_path;
                if(!is_valid_endpoint)
                {
                    spdlog::debug(
                        "Ending path from {}[{}], arriving at ({},{}), dir={}. Resulted in number={}, but expected {}.",
                        placement, loc, pos.row, pos.col, pos.dir, num_from_path, start_num);

                    rx.restore();
                    return false;
                }

                spdlog::debug("Ending path from {}[{}] = {}, arriving at ({},{}), dir={}. Setting to value {}.",
                              placement, loc, start_num, pos.row, pos.col, pos.dir, num_from_path);
                grid_.boundary_number(placement, loc) = num_from_path;
            }
        }

        spdlog::debug("COMPLETED GRID: \n{}", grid_);
        return true;
    };

    constexpr bool is_laser_path_valid_(laser_position const& start_pos, laser_position const& end_pos) const noexcept
    {
        auto       pos  = laser_position{start_pos.row, start_pos.col, end_pos.dir};
        auto const dist = std::abs(end_pos.row - start_pos.row) + std::abs(end_pos.col - start_pos.col);

        for(auto const k: std::views::iota(0, dist - 1))
        {
            pos.advance();
            if(grid_.mirror(pos.row, pos.col) != mirror_type::None && grid_.in_bounds(pos.row, pos.col))
                return false;
        }
        pos.advance();

        return end_pos.col == pos.col && end_pos.row == pos.row;
    };

    constexpr bool try_next_factor_(size_t const number_idx, std::span<integer_factorizations::factor>& factors,
                                    size_t const factor_idx, size_t const total_factors, laser_position const& pos)
    {
        if(factor_idx >= total_factors)
            return try_complete_factors_(number_idx, pos);

        // checks end of path is valid, checking last number can be placed on border
        auto is_pos_valid_ = [&](laser_position const& pos)
        {
            if(factor_idx + 1 < total_factors)
                return grid_.in_bounds(pos.row, pos.col);
            else
                return grid_.in_border(pos.row, pos.col, 0) || grid_.in_border(pos.row, pos.col, 1);
        };

        // try to place mirror and continue pat
        for(auto&& f: factors)
        {
            if(f.count == 0)
                continue;

            qs::restorer rx(qs::defer_restore, f.count);

            // We start the laser inside the grid (in_bounds == true), so that we can immediately place a mirror, which
            // has not cost to the product.
            for(auto const m: {mirror_type::LR, mirror_type::RL})
            {
                auto const pos_after_mirror = laser_position::next_after_mirror(pos, m, f.base);

                if(is_pos_valid_(pos_after_mirror) && grid_.can_place_mirror(pos.row, pos.col, m) &&
                   is_laser_path_valid_(pos, pos_after_mirror))
                {
                    spdlog::debug("Trying factor {} of {} and mirror={}, from ({},{}) to ({},{}), with dir={}.", f.base,
                                  factors, m, pos.row, pos.col, pos_after_mirror.row, pos_after_mirror.col,
                                  pos_after_mirror.dir);

                    grid_.add_mirror_counter(pos.row, pos.col, m);

                    if(try_next_factor_(number_idx, factors, factor_idx + 1, total_factors, pos_after_mirror))
                        return true;

                    grid_.remove_mirror_counter(pos.row, pos.col, m);
                }
            }

            // We can allow laser going perpendiculat ot the number placement. Since the laser starts on the inside
            // of the grid we reduce the number of square to move by 1.
            auto const pos_after_none = laser_position::next_after_mirror(pos, mirror_type::None, f.base - 1);
            if(factor_idx == 0 && is_pos_valid_(pos_after_none) && is_laser_path_valid_(pos, pos_after_none))
            {
                spdlog::debug(
                    "Trying factor {} of {} and no mirror (factor_idx==0), from ({},{}) to ({},{}) with dir={}.",
                    f.base, factors, pos.row, pos.col, pos_after_none.row, pos_after_none.col, pos_after_none.dir);

                if(try_next_factor_(number_idx, factors, factor_idx + 1, total_factors, pos_after_none))
                    return true;
            }

            rx.restore();
        };

        return false;
    }

    constexpr bool try_complete_factors_(size_t const number_idx, laser_position const& end_pos)
    {
        auto const& factorization = std::get<0>(factorizations_[number_idx]);
        auto const  start_num     = factorization.number();

        auto const conditions = std::array<std::tuple<direction, bool, bool>, 4>{
            {{direction::Left, end_pos.col == -1, end_pos.col == 0},
             {direction::Top, end_pos.row == -1, end_pos.row == 0},
             {direction::Right, end_pos.col == grid_.length(), end_pos.col + 1 == grid_.length()},
             {direction::Bottom, end_pos.row == grid_.length(), end_pos.row + 1 == grid_.length()}}};

        auto const outside_border_count =
            std::ranges::fold_left(conditions, 0u, [](auto acc, auto const& t) { return acc + std::get<1>(t); });

        if(outside_border_count == 1) // grid_.in_border(pos.row, pos.col, 0)
        {
            auto const to_direction_from_cond = [](auto acc, auto&& t)
            { return acc + std::to_underlying(std::get<0>(t)) * std::get<1>(t); };

            auto const end_placement =
                static_cast<direction>(std::ranges::fold_left(conditions, 0u, to_direction_from_cond));
            auto const end_loc = (std::to_underlying(end_placement) % 2 == 0) ? end_pos.row : end_pos.col;
            auto const end_num = grid_.boundary_number(end_placement, end_loc);

            bool const is_valid_endpoint = (end_num == 0) | (end_num == start_num);
            if(!is_valid_endpoint)
            {
                spdlog::debug("Invalid path reached border {}[{}]={} from dir={} (is_valid_endpoint={})", end_placement,
                              end_loc, end_num, end_pos.dir, is_valid_endpoint);
                return false;
            }

            grid_.boundary_number(end_placement, end_loc) = start_num;

            if(try_next_number_(number_idx + 1))
                return true;

            grid_.boundary_number(end_placement, end_loc) = end_num;
        }
        else // grid_.in_border(pos.row, pos.col, 1)
        {
            auto const placement_finder = [&](auto const& t)
            { return std::get<2>(t) && mirror_border_placement_(std::get<0>(t), end_pos.dir) != mirror_type::None; };

            auto const it = std::ranges::find_if(conditions, placement_finder);
            if(it == std::ranges::end(conditions))
            {
                spdlog::debug("Path reached at border position ({},{}) from invalid dir={}", end_pos.row, end_pos.col,
                              end_pos.dir);
                return false;
            }

            auto const end_placement = std::get<0>(*it);
            auto const end_loc       = (std::to_underlying(end_placement) % 2 == 0) ? end_pos.row : end_pos.col;
            auto const end_num       = grid_.boundary_number(end_placement, end_loc);

            auto const required_mirror = mirror_border_placement_(end_placement, end_pos.dir);

            bool const is_valid_endpoint = (end_num == 0) | (end_num == start_num);
            bool const can_place_mirror  = (required_mirror != mirror_type::None) &&
                                          grid_.can_place_mirror(end_pos.row, end_pos.col, required_mirror);
            if(!(is_valid_endpoint & can_place_mirror))
            {
                spdlog::debug("Invalid path reached adjacent to {}[{}]={} from dir={} "
                              "(required_mirror={}, is_valid_endpoint={}, can_place_mirror={})",
                              end_placement, end_loc, end_num, end_pos.dir, required_mirror, is_valid_endpoint,
                              can_place_mirror);
                return false;
            }

            grid_.boundary_number(end_placement, end_loc) = start_num;
            grid_.add_mirror_counter(end_pos.row, end_pos.col, required_mirror);

            if(try_next_number_(number_idx + 1))
                return true;

            grid_.remove_mirror_counter(end_pos.row, end_pos.col, required_mirror);
            grid_.boundary_number(end_placement, end_loc) = end_num;
        }

        return false;
    }
};


#endif // MIRROR_GRID_SOLVER_H
