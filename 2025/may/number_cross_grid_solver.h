#ifndef NUMBER_CROSS_SOLVER_H
#define NUMBER_CROSS_SOLVER_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <optional>
#include <ranges>
#include <span>
#include <tuple>
#include <unordered_set>

#include "2025/may/number_cross_cell_partitions.h"
#include "2025/may/number_cross_grid.h"
#include "spdlog/spdlog.h"
#include "utils/restorer.h"


template<CRowPredicate... Predicates>
class number_cross_grid_solver
{
public:
    static constexpr size_t N = sizeof...(Predicates);
    using grid_type           = number_cross_grid<Predicates...>;

    constexpr explicit number_cross_grid_solver(grid_type& grid) noexcept
        : grid_{grid},
          unique_numbers_{}
    {
        unique_numbers_.reserve(N * N);
    }

    constexpr bool solve()
    {
        SPDLOG_INFO("Started solving grid with N={}", N);

        if(try_region_configuration_())
        {
            SPDLOG_INFO("Found solution for grid with N={}:\n{}", N, grid_);
            return true;
        }

        SPDLOG_INFO("No solution found for grid with N={}", N);
        return false;
    }

    constexpr bool solve_with_region_digits(std::span<uint8_t const> region_digits)
    {
        auto const grid_regions_sz  = grid_.regions().size();
        auto const input_regions_sz = region_digits.size();

        if(grid_regions_sz != input_regions_sz)
        {
            SPDLOG_ERROR("Number of islands {} does not match number of digits {}", grid_regions_sz, input_regions_sz);
            return false;
        }

        for(auto&& [idx, region]: std::views::zip(std::views::iota(0), grid_.regions()))
        {
            auto const reg_digit = region_digits[idx];
            if(reg_digit < 1 || reg_digit > 9)
            {
                SPDLOG_ERROR("Invalid region_digits[{}] = {}", idx, reg_digit);
                return false;
            }

            if(!region.get_allowed_digits()[reg_digit])
            {
                SPDLOG_ERROR("Digit {} is not allowed for region {}", reg_digit, idx);
                return false;
            }

            region.set_digit(reg_digit);
            for(auto [r, c]: region.cells())
                grid_(r, c) = reg_digit;
        }

        if(try_grid_configuration_())
        {
            SPDLOG_INFO("Found solution for grid with N={}, region_digits={}:\n{}", N, region_digits, grid_);
            return true;
        }

        SPDLOG_INFO("No solution found for grid with N={}, region_digits={}", N, region_digits);
        return false;
    }

    constexpr std::unordered_set<int64_t> const& get_unique_numbers() const noexcept { return unique_numbers_; }

private:
    grid_type&                  grid_;
    std::unordered_set<int64_t> unique_numbers_{};

    constexpr bool try_region_configuration_(int const region_idx = 0)
    {
        if(region_idx >= grid_.regions().size())
        {
            auto const region_config =
                std::views::transform(grid_.regions(), [](auto const& reg) { return reg.get_digit(); });
            SPDLOG_INFO("Trying region configuration: {}", region_config);

            if(try_grid_configuration_())
            {
                return true;
            }
            return false;
        }

        SPDLOG_DEBUG("Trying setting region {} cells digit", region_idx);

        auto& region = grid_.regions()[region_idx];

        std::bitset<10> const region_allowed_digits = region.get_allowed_digits();

        for(int curr_digit = 1; curr_digit < 10; ++curr_digit)
        {
            auto const is_digit_allowed = region_allowed_digits.test(curr_digit);
            SPDLOG_DEBUG("Trying digit {} for region {}, allowed: {}", curr_digit, region_idx, is_digit_allowed);
            if(!is_digit_allowed)
                continue;

            bool valid_digit = true;
            for(auto neighbor_idx: region.neighbors())
            {
                auto const& neighbor_region = grid_.regions()[neighbor_idx];
                auto const  neighbor_digit  = neighbor_region.get_digit();
                if(neighbor_digit == 0)
                    continue;

                if(neighbor_digit == curr_digit)
                {
                    SPDLOG_DEBUG("Digit {} is already used by neighbor region {}", curr_digit, neighbor_idx);
                    valid_digit = false;
                    break;
                }
            }

            if(!valid_digit)
                continue;

            SPDLOG_DEBUG("Setting digit {} for region {}, allowed", curr_digit, region_idx);

            region.set_digit(curr_digit);
            for(auto [r, c]: region.cells())
                grid_(r, c) = curr_digit;

            if(try_region_configuration_(region_idx + 1))
                return true;
        }

        region.set_digit(0);
        for(auto [r, c]: region.cells())
            grid_(r, c) = 0;

        return false;
    }

    template<size_t Row>
    constexpr auto get_previous_number_digits_(int end_col = N) const noexcept
        -> std::optional<std::span<uint8_t const>>
    {
        end_col = std::min(end_col, static_cast<int>(N));
        // pre-condition
        if(!(end_col == N || (end_col > 0 && end_col < N - 1 && grid_.blocked(Row, end_col))))
            return std::nullopt;

        if(end_col == N && grid_.blocked(Row, N - 1))
            --end_col;

        int start_col = end_col;
        while(start_col > 0 && !grid_.blocked(Row, start_col - 1))
            --start_col;

        auto const cell_row = std::span<uint8_t const>(grid_.template row<Row>());
        return cell_row.subspan(start_col, end_col - start_col);
    }

    template<size_t Row = 0>
    constexpr bool try_grid_configuration_(int const col = 0, int const prev_tile_col = -1)
    {
        if constexpr(Row == 0)
        {
            if(col >= N)
                return try_grid_configuration_<1>();

            if(try_put_tile_<0>(col, prev_tile_col))
                return true;

            return try_grid_configuration_<0>(col + 1, prev_tile_col);
        }
        else if constexpr(Row < N)
        {
            auto const prev_number = get_previous_number_digits_<Row - 1>(col);
            if(prev_number.has_value())
            {
                auto const& predicate    = grid_.template predicate<Row - 1>();
                auto const [is_valid, x] = predicate(prev_number.value());
                if(!is_valid)
                    return false;

                auto [_, is_inserted] = unique_numbers_.insert(x);
                if(!is_inserted)
                    return false;

                if(col >= N && try_grid_configuration_<Row + 1>())
                    return true;
                else if(try_grid_configuration_<Row>(col + 1, prev_tile_col))
                    return true;

                unique_numbers_.erase(x);
            }
            else
            {
                if(!grid_.blocked(Row - 1, col) && try_put_tile_<Row>(col, prev_tile_col))
                    return true;

                if(try_grid_configuration_<Row>(col + 1, prev_tile_col))
                    return true;
            };

            return false;
        }
        else // constexpr(Row >= N)
        {
            SPDLOG_DEBUG("Row={}: Verifying last row of completed grid:\n{}", N, grid_);

            static constexpr size_t             kMaxRowNumbers = (N + 1) / 3;
            std::array<int64_t, kMaxRowNumbers> row_numbers_buffer{};
            size_t                              row_numbers_size = 0;

            for(int col = N; col >= 0; --col)
            {
                auto const prev_number = get_previous_number_digits_<N - 1>(col);
                if(prev_number.has_value())
                {
                    auto const& predicate    = grid_.template predicate<N - 1>();
                    auto const [is_valid, x] = predicate(prev_number.value());

                    if(!is_valid)
                        return false;

                    row_numbers_buffer[row_numbers_size++] = x;
                }
            }

            auto const row_numbers = std::span<int64_t const>(row_numbers_buffer.data(), row_numbers_size);

            auto const it =
                std::ranges::find_if(row_numbers, [&](auto const& x) { return !unique_numbers_.insert(x).second; });
            if(it != std::ranges::end(row_numbers))
            {
                auto const m = std::distance(row_numbers.begin(), it);
                for(auto const& x: row_numbers.subspan(0, m))
                    unique_numbers_.erase(x);
                return false;
            }

            return true;
        }
    }

    template<size_t Row>
    constexpr bool try_put_tile_(int const col, int const prev_tile_col)
    {
        SPDLOG_DEBUG("Row={}, col={}, prev_tile_col={}: Trying to place tile at ({}, {})", Row, col, prev_col, Row, col,
                     prev_col, Row, col);

        if((col != 0 && col - prev_tile_col < 3) || (col != N - 1 && N - col < 3) || grid_.highlighted(Row, col))
        {
            SPDLOG_DEBUG("Row={}, col={}, prev_tile_col={}: Skipping column. Highlighted or too close to previous.",
                         Row, col, prev_col);
            return false;
        }

        auto const digit        = grid_(Row, col);
        auto const region_digit = grid_.region_of(Row, col).get_digit();

        grid_(Row, col)         = 0;
        grid_.blocked(Row, col) = true;

        for(auto const& partition: grid_cross_partitions::get(digit))
        {
            bool const valid_partition = is_valid_partition_<Row>(col, partition);
            if(!valid_partition)
                continue;

            qs::restorer_array<uint8_t, 4> rxs(qs::defer_restore);

            if constexpr(Row > 0)
            {
                auto& top = grid_(Row - 1, col);
                rxs.unchecked_push_back(top);
                top += partition.top;
            }
            if constexpr(Row + 1 < N)
            {
                auto& bottom = grid_(Row + 1, col);
                rxs.unchecked_push_back(bottom);
                bottom += partition.bottom;
            }
            if(col > 0)
            {
                auto& left = grid_(Row, col - 1);
                rxs.unchecked_push_back(left);
                left += partition.left;
            }
            if(col + 1 < N)
            {
                auto& right = grid_(Row, col + 1);
                rxs.unchecked_push_back(right);
                right += partition.right;
            }

#ifndef NDEBUG
            check_digit_value_invariants_();
#endif

            if(try_grid_configuration_<Row>(col + 1, col))
                return true;

            rxs.complete();
        }

        grid_(Row, col)         = digit;
        grid_.blocked(Row, col) = false;

        SPDLOG_DEBUG("Row={}, col={}, prev_tile_col={}: Failed to place tile at ({}, {})", Row, col, prev_col, Row, col,
                     prev_col, Row, col);

        return false;
    }

    constexpr void check_digit_value_invariants_() const noexcept
    {
        auto const is_valid_digit = [](auto const d) { return d >= 0 && d < 10; };
        auto const valid          = std::ranges::all_of(grid_.digits_array() | std::views::join, is_valid_digit);
        if(!valid)
        {
            SPDLOG_CRITICAL("Invalid digit in grid: \n{}", grid_);
            std::terminate();
        }
    }

    template<size_t Row>
    constexpr bool is_valid_partition_(int const col, grid_cross_partition const& part) const noexcept
    {
        auto is_valid_sum = [&](int r, int c, int const val)
        { return (val == 0 || !(grid_.highlighted(r, c) || grid_.blocked(r, c))) && (grid_(r, c) + val < 10); };

        if constexpr(Row == 0)
        {
            if(part.top > 0)
                return false;
        }
        else
        {
            if(!is_valid_sum(Row - 1, col, part.top))
                return false;
        }

        if constexpr(Row == N - 1)
        {
            if(part.bottom > 0)
                return false;
        }
        else
        {
            if(!is_valid_sum(Row + 1, col, part.bottom))
                return false;
        }

        if((col == 0 && part.left > 0) || !is_valid_sum(Row, col - 1, part.left))
            return false;

        if((col == N - 1 && part.right > 0) || !is_valid_sum(Row, col + 1, part.right))
            return false;

        return true;
    }
};


#endif // NUMBER_CROSS_SOLVER_H
