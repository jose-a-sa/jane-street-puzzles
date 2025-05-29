#ifndef NUMBER_CROSS_GRID_H
#define NUMBER_CROSS_GRID_H

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <span>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <utility>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <vector>

#include "may-2025/number_cross_cell.h"
#include "may-2025/number_cross_cell_partitions.h"
#include "may-2025/number_cross_grid_predicates.h"

#include "number_cross_cell_partitions.h"
#include "utils/restorer.h"


template<size_t MaxIslandSize, size_t MaxNeighbors>
struct grid_cell_island
{
    constexpr grid_cell_island(int r = 0, int c = 0) noexcept
        : neighbor_indexes_{},
          cell_indexes_{},
          island_digit_{0},
          island_size_{0},
          neighbors_size_{0},
          source_{r, c},
          allowed_digits_{}
    {}

    constexpr bool add_neighbor(int idx) noexcept
    {
        if(neighbors_size_ >= MaxNeighbors)
            return false;

        auto neighbors = get_neighbors();
        if(neighbors.end() != std::find(neighbors.begin(), neighbors.end(), idx))
            return false;

        neighbor_indexes_[neighbors_size_++] = idx;
        return true;
    }

    constexpr bool add_cell_index(int idx) noexcept
    {
        if(island_size_ >= MaxIslandSize)
            return false;
        cell_indexes_[island_size_++] = idx;
        return true;
    }

    constexpr auto get_island_digit() const noexcept { return island_digit_; }
    constexpr void set_island_digit(int digit) noexcept { island_digit_ = digit; }

    constexpr auto get_source() const noexcept { return source_; }
    constexpr void set_source(int r, int c) noexcept { source_ = std::make_pair(r, c); }

    constexpr auto get_allowed_digits() const noexcept { return allowed_digits_; }
    constexpr void set_allowed_digits(std::bitset<10> allowed) noexcept { allowed_digits_ = allowed; }

    constexpr auto get_cell_indexes() const noexcept { return std::span{cell_indexes_.data(), island_size_}; }
    constexpr auto get_neighbors() const noexcept { return std::span{neighbor_indexes_.data(), neighbors_size_}; }

private:
    std::array<uint8_t, MaxNeighbors>   neighbor_indexes_{};
    std::array<uint16_t, MaxIslandSize> cell_indexes_{};
    uint16_t                            island_digit_{0};

    size_t island_size_{0};
    size_t neighbors_size_{0};

    std::pair<int, int> source_{};
    std::bitset<10>     allowed_digits_{};
};


template<CRowPredicate... Predicates>
class number_cross_grid
{
public:
    static constexpr size_t N = sizeof...(Predicates);

    static constexpr size_t GridElements  = N * N;
    static constexpr size_t MaxIslandSize = N * (N + 1) / 2;
    static constexpr size_t MaxNeighbors  = N;

    using cell_island = grid_cell_island<MaxIslandSize, MaxNeighbors>;

    constexpr explicit number_cross_grid(std::tuple<Predicates...> const&               predicates,
                                         std::array<std::array<cell_info, N>, N> const& cell_data) noexcept
        : cells_{},
          island_map_{},
          predicates_(std::move(predicates)),
          cell_islands_{}
    {
        init_allowed_digits_<0>();


        uint32_t max_island_index = 0;
        for(int r = 0; r < N; ++r)
        {
            for(int c = 0; c < N; ++c)
            {
                island_map_[to_idx_(r, c)] = cell_data[r][c].island;
                cells_[r][c].set_highlighted(cell_data[r][c].highlighted);

                max_island_index = std::max<uint32_t>(max_island_index, cell_data[r][c].island);
            }
        }

        cell_islands_.resize(max_island_index + 1);
        std::bitset<N * N> visited{};


        for(int r = 0; r < N; ++r)
        {
            for(int c = 0; c < N; ++c)
            {
                auto const idx = to_idx_(r, c);
                if(visited[idx])
                    continue;

                SPDLOG_DEBUG("Starting visiting new island starting at ({}, {})", r, c);

                uint32_t const island_index = cell_data[r][c].island;

                cell_island& isl = cell_islands_[island_index];
                isl.set_allowed_digits(cells_[r][c].get_allowed_digits());
                init_islands_(r, c, island_index, isl, visited);

                SPDLOG_INFO("Island idx: {}, size: {}, source: {}, allowed_digits: {}, neighbors: {}", island_index,
                            isl.get_cell_indexes().size(), isl.get_source(), isl.get_allowed_digits().to_string(),
                            isl.get_neighbors());
            }
        }
    }

    constexpr bool in_range(int r, int c) const noexcept { return r >= 0 && r < N && c >= 0 && c < N; }

    constexpr auto& operator[](int idx) noexcept { return cells_[idx / N][idx % N]; }
    constexpr auto& operator[](int idx) const noexcept { return cells_[idx / N][idx % N]; }

    constexpr auto& operator()(int r, int c) noexcept { return cells_[r][c]; }
    constexpr auto& operator()(int r, int c) const noexcept { return cells_[r][c]; }

    constexpr auto island_index(int r, int c) const noexcept { return island_map_[to_idx_(r, c)]; }

    constexpr auto& island(int island_idx) noexcept { return cell_islands_[island_idx]; }
    constexpr auto& island(int island_idx) const noexcept { return cell_islands_[island_idx]; }

    constexpr auto& island_of(int r, int c) noexcept { return island(island_index(r, c)); }
    constexpr auto& island_of(int r, int c) const noexcept { return island(island_index(r, c)); }

    constexpr auto islands_total() noexcept { return cell_islands_.size(); }

    template<size_t R>
    constexpr auto& predicate() const noexcept
    {
        return std::get<R>(predicates_);
    }

    // clang-format off
    template<size_t R>
    constexpr auto& row() noexcept { return cells_[R]; }
    template<size_t R>
    constexpr auto& row() const noexcept { return cells_[R]; }
    // clang-format on

    constexpr auto& row(size_t i) noexcept { return cells_[i]; }
    constexpr auto& row(size_t i) const noexcept { return cells_[i]; }

    constexpr auto is_altered(int r, int c) const noexcept
    {
        return !cells_[r][c].is_blocked() && cells_[r][c].get_digit() != 0 &&
               cells_[r][c].get_digit() != island_of(r, c).get_island_digit();
    }

    constexpr auto get_numbers() const noexcept
    {
        std::vector<uint64_t> numbers;
        numbers.reserve(N * N / 3);

        for(auto const& row: cells_)
        {
            int start = row.front().is_blocked() ? 1 : 0;
            for(int end = start + 1; end <= N; ++end)
            {
                if((end != N && !row[end].is_blocked()) || (end == N && row.back().is_blocked()))
                    continue;

                auto const     cells = std::span(row).subspan(start, end - start);
                uint64_t const x     = std::ranges::fold_left(cells, 0ull, [](uint64_t const acc, grid_cell const& c)
                                                              { return 10 * acc + c.get_digit(); });

                numbers.push_back(x);

                start = end + 1;
            }
        }

        return numbers;
    }

private:
    template<CRowPredicate... Preds>
    friend class number_cross_grid_solver;

    std::array<std::array<grid_cell, N>, N> cells_{};
    std::array<uint8_t, N * N>              island_map_{};
    std::tuple<Predicates...>               predicates_{};

    std::vector<cell_island> cell_islands_;

    constexpr auto to_idx_(int r, int c) const noexcept { return r * N + c; }

    template<size_t I = 0>
    constexpr void init_allowed_digits_()
    {
        if constexpr(I < N)
        {
            auto row_allowed = this->predicate<I>().get_allowed_digits();

            for(auto& cell: this->row<I>())
                cell.set_allowed_digits(row_allowed);

            this->init_allowed_digits_<I + 1>();
        }
    }

    constexpr void init_islands_(int r, int c, int group_idx, cell_island& island, std::bitset<N * N>& visited)
    {
        auto const idx = to_idx_(r, c);
        if(!this->in_range(r, c) || visited[idx] || this->island_index(r, c) != group_idx)
            return;

        visited[idx] = true;

        island.add_cell_index(idx);
        auto [sr, sc] = island.get_source();
        auto& source  = (*this)(sr, sc);
        auto& current = (*this)(r, c);

        if((!source.is_highlighted() && current.is_highlighted()) ||
           (source.is_highlighted() && current.is_highlighted() &&
            current.get_allowed_digits().count() < source.get_allowed_digits().count()))
        {
            island.set_allowed_digits(current.get_allowed_digits());
            island.set_source(r, c);
        }

        for(auto [dr, dc]: {std::pair{1, 0}, {-1, 0}, {0, 1}, {0, -1}})
        {
            auto const new_r = r + dr;
            auto const new_c = c + dc;

            if(!this->in_range(new_r, new_c))
                continue;

            if(this->island_index(new_r, new_c) == group_idx)
                this->init_islands_(new_r, new_c, group_idx, island, visited);
            else
                island.add_neighbor(this->island_index(new_r, new_c));
        }

        if(current.is_highlighted())
            current.set_allowed_digits(island.get_allowed_digits());
    }
};

#endif // NUMBER_CROSS_GRID_H


template<CRowPredicate... Predicates>
struct fmt::formatter<number_cross_grid<Predicates...>>
{
    using value_type          = number_cross_grid<Predicates...>;
    static constexpr size_t N = sizeof...(Predicates);

    constexpr auto parse(format_parse_context& ctx) const { return ctx.begin(); }

    template<typename FormatContext>
    auto format(value_type const& grid, FormatContext& ctx) const
    {
        std::array<std::string_view, N> row_sep{};
        row_sep.fill("---");
        fmt::format_to(ctx.out(), "+{}+\n", fmt::join(row_sep, "+"));

        for(size_t i = 0; i < N; ++i)
        {
            if constexpr(N > 1)
            {
                auto const dig     = grid(i, 0).get_digit();
                char const ch      = grid(i, 0).is_blocked() ? '#' : (dig == 0 ? ' ' : static_cast<char>('0' + dig));
                char const flag_ch = grid(i, 0).is_highlighted() ? '@' : (grid.is_altered(i, 0) ? '\'' : ' ');
                fmt::format_to(ctx.out(), "| {}{}", ch, flag_ch);
            }

            for(size_t j = 1; j < N; ++j)
            {
                auto const dig     = grid(i, j).get_digit();
                char const ch      = grid(i, j).is_blocked() ? '#' : (dig == 0 ? ' ' : static_cast<char>('0' + dig));
                char const flag_ch = grid(i, j).is_highlighted() ? '@' : (grid.is_altered(i, j) ? '\'' : ' ');
                auto const vsep    = grid.island_index(i, j) == grid.island_index(i, j - 1) ? ' ' : '|';
                fmt::format_to(ctx.out(), "{} {}{}", vsep, ch, flag_ch);
            }

            if(i < N - 1)
            {
                for(size_t j = 0; j < N; ++j)
                    row_sep[j] = grid.island_index(i, j) == grid.island_index(i + 1, j) ? "   " : "---";
                fmt::format_to(ctx.out(), "|\n+{}+\n", fmt::join(row_sep, "+"));
            }
        }

        row_sep.fill("---");
        fmt::format_to(ctx.out(), "|\n+{}+", fmt::join(row_sep, "+"));
        return ctx.out();
    }
};


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

        if(try_island_configuration_())
        {
            SPDLOG_INFO("FINAL:\n{}", grid_);
            return true;
        }

        SPDLOG_INFO("No solution found");
        return false;
    }

    constexpr bool solve_with_islands(std::vector<int> const& digits)
    {
        if(grid_.islands_total() != digits.size())
        {
            SPDLOG_ERROR("Number of islands {} does not match number of digits {}", grid_.islands_total(),
                         digits.size());
            return false;
        }

        for(int i = 0; i < grid_.islands_total(); ++i)
        {
            auto& island = grid_.island(i);
            if(digits[i] < 1 || digits[i] > 9)
            {
                SPDLOG_ERROR("Invalid digit[i] = {}", digits[i], i);
                return false;
            }

            if(!island.get_allowed_digits()[digits[i]])
            {
                SPDLOG_ERROR("Digit {} is not allowed for island {}", digits[i], i);
                return false;
            }

            island.set_island_digit(digits[i]);
            for(auto idx: island.get_cell_indexes())
                grid_[idx].set_digit(digits[i]);
        }

        SPDLOG_INFO("Trying to solve grid: \n{}", grid_);

        return try_grid_configuration_<0>(0);
    }


private:
    grid_type&                  grid_;
    std::unordered_set<int64_t> unique_numbers_{};

    constexpr bool try_island_configuration_(int const island_idx = 0)
    {
        if(island_idx >= grid_.islands_total())
        {
            auto const island_config =
                grid_.cell_islands_ | std::views::transform([](auto const& isl) { return isl.get_island_digit(); });
            SPDLOG_INFO("Trying island configuration: {}", island_config);

            if(try_grid_configuration_<0>(0))
            {
                return true;
            }
            return false;
        }

        SPDLOG_DEBUG("Trying setting island {} cells digit", island_idx);

        auto&                 island                = grid_.island(island_idx);
        std::bitset<10> const island_allowed_digits = island.get_allowed_digits();

        for(int curr_digit = 1; curr_digit < 10; ++curr_digit)
        {
            SPDLOG_DEBUG("Trying digit {} for island {}, allowed: {}", curr_digit, island_idx,
                         island_allowed_digits.test(curr_digit));
            if(!island_allowed_digits.test(curr_digit))
                continue;

            bool allowed_digit = true;
            for(auto neighbor_idx: island.get_neighbors())
            {
                auto [ng_source_row, ng_source_col] = grid_.island(neighbor_idx).get_source();
                auto& neighbor_cell                 = grid_(ng_source_row, ng_source_col);

                if(neighbor_cell.get_digit() == 0)
                    continue;

                if(neighbor_cell.get_digit() == curr_digit)
                {
                    SPDLOG_DEBUG("Digit {} is already used in neighbor {}", curr_digit, neighbor_idx);
                    allowed_digit = false;
                    break;
                }
            }

            if(!allowed_digit)
                continue;

            SPDLOG_DEBUG("Setting digit {} for island {}, allowed", curr_digit, island_idx,
                         static_cast<bool>(island_allowed_digits[curr_digit]));

            island.set_island_digit(curr_digit);
            for(auto idx: island.get_cell_indexes())
                grid_[idx].set_digit(curr_digit);

#ifndef NDEBUG
            check_island_invariants_();
#endif

            if(try_island_configuration_(island_idx + 1))
                return true;
        }

        island.set_island_digit(0);
        for(auto idx: island.get_cell_indexes())
            grid_[idx].set_digit(0);

        return false;
    }

    constexpr auto to_number_(std::span<grid_cell const> const cells) const noexcept
    {
        return std::ranges::fold_left(cells, int64_t{0},
                                      [](int64_t const acc, grid_cell const& c) { return 10 * acc + c.get_digit(); });
    }

    template<int R>
    constexpr auto get_previous_number_(int end_col = N) const noexcept
    {
        auto const cell_row = std::span<grid_cell const>{grid_.template row<R>()};

        end_col = std::min<int64_t>(end_col, N);
        if(end_col == N && cell_row.back().is_blocked())
            --end_col;

        while(end_col >= 0 && end_col < N && !cell_row[end_col].is_blocked())
            --end_col;

        if(end_col == 0)
            return std::make_tuple(int64_t{0}, std::span<grid_cell const>{});

        int start_col = end_col - 1;
        while(start_col >= 0 && !cell_row[start_col].is_blocked())
            --start_col;

        auto const cells = cell_row.subspan(start_col + 1, end_col - start_col - 1);

        return std::make_tuple(to_number_(cells), cells);
    }

    template<int R>
    constexpr auto validate_previous_number_(int end_col = N) const noexcept
    {
        end_col = std::min<int64_t>(end_col, N);

        auto const& cell_row = grid_.template row<R>();
        if((end_col == 0 && cell_row.front().is_blocked()) || (end_col == N && cell_row.back().is_blocked()))
            return std::make_tuple(true, int64_t{0});

        auto const& predicate = grid_.template predicate<R>();
        auto const [x, cells] = get_previous_number_<R>(end_col);

        bool const predicate_satisfied = predicate.check(x, cells);
        SPDLOG_DEBUG("validate_previous_number_<R={}>(end_col={}) Previous number {}={} is {}", R, end_col, cells, x,
                     predicate_satisfied ? "SATISFIED" : "INVALID");
        if(!predicate_satisfied)
            return std::make_tuple(false, x);

        return std::make_tuple(true, x);
    }

    template<int R>
    constexpr auto validate_grid_uniqueness_(int end_col = N) noexcept
    {
        if constexpr(R >= 0)
        {
            auto const& cell_row = grid_.template row<R>();
            while(end_col > 1)
            {
                auto const [x, cells] = get_previous_number_<R>(end_col);

                if(unique_numbers_.contains(x))
                {
                    SPDLOG_DEBUG("validate_grid_uniqueness_<R={}>(end_col={}) Numbers {} is not unique: {}", R, end_col,
                                 x, unique_numbers_);
                    unique_numbers_.clear();
                    return false;
                }

                if(x == 0 || cells.empty())
                {
                    SPDLOG_ERROR("validate_grid_uniqueness_<R={}>(end_col={}) Zero has been found: {}", R, end_col,
                                 cell_row);
                }
                unique_numbers_.insert(x);

                end_col -= cells.size() + 1;
            }

            return validate_grid_uniqueness_<R - 1>(N);
        }
        else
        {
            SPDLOG_DEBUG("validate_grid_uniqueness_<R={}>(end_col={}) Numbers are unique: {}", R, end_col,
                         unique_numbers_);
            unique_numbers_.clear();
        }

        return true;
    }


    template<int R = 0>
    constexpr bool try_grid_configuration_(int const curr_col = 0, int const prev_col = -1)
    {
        if(curr_col >= N)
        {
            auto const [valid_number, x] = validate_previous_number_<R - 1>(N);
            if(!valid_number)
                return false;

            auto const numbers_are_unique = validate_grid_uniqueness_<R - 1>(N);
            if(!numbers_are_unique)
                return false;

            if(try_grid_configuration_<R + 1>())
                return true;

            return false;
        }

        auto const& top = grid_.template row<R - 1>()[curr_col];
        if(top.is_blocked())
        {
            SPDLOG_DEBUG("<R: {}, col: {}, prev_col: {}> Top cell is blocked:\n{}", R, curr_col, prev_col, grid_);

            if(curr_col > 0)
            {
                auto const [valid_number, x] = validate_previous_number_<R - 1>(curr_col);
                if(!valid_number)
                    return false;
            }
        }
        else
        {
            if(try_tile_configuration_<R>(curr_col, prev_col))
                return true;
        }

        return try_grid_configuration_<R>(curr_col + 1, prev_col);
    }

    template<>
    constexpr bool try_grid_configuration_<0>(int const curr_col, int const prev_col)
    {
        if(curr_col >= N)
        {
            SPDLOG_TRACE("<R: {}, col: {}, prev_col: {}> First row, no checks needed", 0, curr_col, prev_col);
            return try_grid_configuration_<1>();
        }

        if(try_tile_configuration_<0>(curr_col, prev_col))
            return true;

        return try_grid_configuration_<0>(curr_col + 1, prev_col);
    }

    template<>
    constexpr bool try_grid_configuration_<N>(int const, int const)
    {
        SPDLOG_INFO("try_grid_configuration_<R=N>() Verifying last row of completed grid:\n{}", N, grid_);

        auto const& predicate = grid_.template predicate<N - 1>();
        auto const  cell_row  = std::span<grid_cell const>{grid_.template row<N - 1>()};

        int start_col = cell_row.front().is_blocked() ? 1 : 0;
        for(int end_col = start_col + 1; end_col < N; ++end_col)
        {
            if(!cell_row[end_col].is_blocked())
                continue;

            auto const cells = cell_row.subspan(start_col, end_col - start_col);
            auto const x     = to_number_(cells);

            bool const predicate_satisfied = predicate.check(x, cells);
            SPDLOG_DEBUG("try_grid_configuration_<R=N>() Last row number {}={} is {}, unique_numbers_: {}", N, cells, x,
                         predicate_satisfied ? "SATISFIED" : "INVALID", unique_numbers_);
            if(!predicate_satisfied)
                return false;

            start_col = end_col + 1;
        }

        auto const numbers_are_unique = validate_grid_uniqueness_<N - 1>(N);
        SPDLOG_INFO("try_grid_configuration_<R=N>() Grid uniqueness check: {}", N,
                    numbers_are_unique ? "PASSED" : "FAILED");
        if(!numbers_are_unique)
            return false;

        return true;
    }

    template<int R>
    constexpr bool try_tile_configuration_(int const curr_col, int const prev_col)
    {
        SPDLOG_DEBUG("try_tile_configuration_<R={}>(col={}, prev_col={}) Trying to place tile at ({}, {})", R, curr_col,
                     prev_col, R, curr_col, prev_col, R, curr_col);

        auto& curr_row  = grid_.template row<R>();
        auto& curr_cell = curr_row[curr_col];

        if((curr_col != 0 && curr_col - prev_col < 3) || (curr_col != N - 1 && N - curr_col < 3) ||
           curr_cell.is_highlighted())
        {
            SPDLOG_TRACE("try_tile_configuration_<R={}>(col={}, prev_col={}) Skipping column. Either highlighted, or "
                         "to close to previous.",
                         R, curr_col, prev_col);
            return false;
        }

        auto const saved_curr_cell = curr_cell;
        auto const island_digit    = saved_curr_cell.get_digit();

        curr_cell.set_blocked();
        curr_cell.set_digit(0);

        for(cell_partition const& partition: cell_partitions::get(island_digit))
        {
            bool const valid_partition = valid_partition_<R>(curr_col, partition);
            if(!valid_partition)
                continue;

            restorer_array<grid_cell, 4> rxs;
            rxs.wait();

            if constexpr(R - 1 >= 0)
            {
                auto& top = grid_.template row<R - 1>()[curr_col];
                rxs.unchecked_track(top);
                top.set_digit(top.get_digit() + partition.top);
            }
            if constexpr(R + 1 < N)
            {
                auto& bottom = grid_.template row<R + 1>()[curr_col];
                rxs.unchecked_track(bottom);
                bottom.set_digit(bottom.get_digit() + partition.bottom);
            }
            if(curr_col - 1 >= 0)
            {
                auto& left = curr_row[curr_col - 1];
                rxs.unchecked_track(left);
                left.set_digit(left.get_digit() + partition.left);
            }
            if(curr_col + 1 < N)
            {
                auto& right = curr_row[curr_col + 1];
                rxs.unchecked_track(right);
                right.set_digit(right.get_digit() + partition.right);
            }

#ifndef NDEBUG
            check_digit_value_invariants_();
#endif

            if(try_grid_configuration_<R>(curr_col + 1, curr_col))
                return true;

            rxs.restore();
        }

        curr_cell = saved_curr_cell;

        SPDLOG_DEBUG("try_tile_configuration_<R={}>(col={}, prev_col={}) Failed to place tile at ({}, {})", R, curr_col,
                     prev_col, R, curr_col, prev_col, R, curr_col);

        return false;
    }

    constexpr void check_digit_value_invariants_() const
    {
        bool valid = true;
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
                valid &= grid_(i, j).get_digit() >= 0 && grid_(i, j).get_digit() < 10;
        }

        if(!valid)
        {
            SPDLOG_CRITICAL("Invalid digit in grid: \n{}", grid_);
            std::terminate();
        }
    }

    constexpr void check_island_invariants_() const
    {
        bool valid = true;
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                valid &= grid_(i, j).get_digit() == grid_.island_of(i, j).get_island_digit();
            }
        }

        if(!valid)
        {
            SPDLOG_CRITICAL("Invalid island-digit pair in grid: \n{}", grid_);
            std::terminate();
        }
    }

    template<int R>
    constexpr bool valid_partition_(int const col, cell_partition const& part) const noexcept
    {
        auto is_valid_sum = [](grid_cell const& cell, int const val)
        { return (val == 0 || (!cell.is_highlighted() && !cell.is_blocked())) && (cell.get_digit() + val < 10); };

        if constexpr(R == 0)
        {
            if(part.top > 0)
                return false;
        }
        else
        {
            auto const& top = grid_.template row<R - 1>()[col];
            if(!is_valid_sum(top, part.top))
                return false;
        }

        if constexpr(R == N - 1)
        {
            if(part.bottom > 0)
                return false;
        }
        else
        {
            auto const& bottom = grid_.template row<R + 1>()[col];
            if(!is_valid_sum(bottom, part.bottom))
                return false;
        }

        auto const& cell_row = grid_.template row<R>();
        if((col == 0 && part.left > 0) || !is_valid_sum(cell_row[col - 1], part.left))
        {
            return false;
        }
        if((col == N - 1 && part.right > 0) || !is_valid_sum(cell_row[col + 1], part.right))
        {
            return false;
        }

        return true;
    }
};
