#ifndef NUMBER_CROSS_GRID_H
#define NUMBER_CROSS_GRID_H

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <mach/mach_traps.h>
#include <ranges>
#include <span>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include "may-2025/number_cross_grid_predicates.h"


template<size_t MaxRegionSize, size_t MaxRegionNeighbors>
struct number_cross_grid_region
{
    constexpr number_cross_grid_region(int r = 0, int c = 0) noexcept
        : source_{r, c}
    {}

    constexpr bool add_neighbor(int region_idx) noexcept
    {
        if(neighbors_size_ >= MaxRegionNeighbors)
            return false;

        if(std::ranges::find(this->neighbors(), region_idx) != std::ranges::end(this->neighbors()))
            return false;

        neighbor_indexes_[neighbors_size_++] = region_idx;
        return true;
    }

    constexpr bool add_cell(int r, int c) noexcept
    {
        if(island_size_ >= MaxRegionSize)
            return false;

        cell_indexes_[island_size_++] = std::make_pair<uint8_t, uint8_t>(r, c);
        return true;
    }

    constexpr auto get_digit() const noexcept { return digit_; }
    constexpr void set_digit(int digit) noexcept { digit_ = digit; }

    constexpr auto get_source() const noexcept { return source_; }
    constexpr void set_source(int r, int c) noexcept { source_ = std::make_pair<uint8_t, uint8_t>(r, c); }

    constexpr auto get_allowed_digits() const noexcept -> std::bitset<10> { return allowed_mask_; }
    constexpr void set_allowed_digits(std::bitset<10> allowed) noexcept { allowed_mask_ = allowed.to_ulong(); }

    constexpr auto cells() const noexcept { return std::span{cell_indexes_}.subspan(0, island_size_); }
    constexpr auto neighbors() const noexcept { return std::span{neighbor_indexes_}.subspan(0, neighbors_size_); }

private:
    size_t island_size_{0};
    size_t neighbors_size_{0};

    uint16_t allowed_mask_{0b1111111110}; // 1-9 digits

    std::pair<uint8_t, uint8_t> source_{};

    std::array<std::pair<uint8_t, uint8_t>, MaxRegionSize> cell_indexes_{};
    std::array<uint8_t, MaxRegionNeighbors>                neighbor_indexes_{};

    uint8_t digit_{0};
};


template<CRowPredicate... Predicates>
class number_cross_grid
{
public:
    static constexpr size_t N = sizeof...(Predicates);

    static constexpr size_t kGridElements  = N * N;
    static constexpr size_t kMaxRegionSize = N * (N + 1) / 2;
    static constexpr size_t kMaxNeighbors  = N;

    using grid_region = number_cross_grid_region<kMaxRegionSize, kMaxNeighbors>;

    constexpr explicit number_cross_grid(std::tuple<Predicates...> const&             predicates,
                                         std::array<std::array<uint8_t, N>, N> const& region_index_map,
                                         std::array<std::array<bool, N>, N> const&    highlighted) noexcept
        : grid_regions_(std::ranges::max(region_index_map | std::views::join) + 1),
          highlighted_{},
          blocked_{},
          predicates_{predicates},
          allowed_digits_{},
          digits_{},
          region_index_{region_index_map}
    {
        for(int r = 0; r < N; ++r)
            for(int c = 0; c < N; ++c)
                highlighted_[to_idx_(r, c)] = highlighted[r][c];

        init_allowed_digits_();
        init_regions_();
    }

    constexpr auto& operator()(int r, int c) noexcept { return digits_[r][c]; }
    constexpr auto& operator()(int r, int c) const noexcept { return digits_[r][c]; }

    constexpr auto& regions() noexcept { return grid_regions_; }
    constexpr auto& regions() const noexcept { return grid_regions_; }

    constexpr auto& region_of(int r, int c) noexcept { return grid_regions_[region_index_[r][c]]; }
    constexpr auto& region_of(int r, int c) const noexcept { return grid_regions_[region_index_[r][c]]; }

    // clang-format off
    template<size_t Row>
    constexpr auto& predicate() const noexcept { return std::get<Row>(predicates_); }

    template<size_t Row>
    constexpr auto row() noexcept { return std::span{digits_[Row]}; }
    template<size_t Row>
    constexpr auto row() const noexcept { return std::span{digits_[Row]}; }
    // clang-format on


    constexpr auto altered(int r, int c) const noexcept
    {
        auto const digit = (*this)(r, c);
        return !this->blocked(r, c) && digit != 0 && digit != this->region_of(r, c).get_digit();
    }

    constexpr auto highlighted(int r, int c) const noexcept { return highlighted_[to_idx_(r, c)]; }

    constexpr auto blocked(int r, int c) noexcept { return blocked_[to_idx_(r, c)]; }
    constexpr auto blocked(int r, int c) const noexcept { return blocked_[to_idx_(r, c)]; }

    constexpr auto allowed_digits(int r) const noexcept { return std::bitset<10>{allowed_digits_[r]}; }

    // constexpr auto get_numbers() const noexcept
    // {
    //     std::vector<uint64_t> numbers;
    //     numbers.reserve(N * N / 3);

    //     for(auto const& row: digits_)
    //     {
    //         int start = row.front().is_blocked() ? 1 : 0;
    //         for(int end = start + 1; end <= N; ++end)
    //         {
    //             if((end != N && !row[end].is_blocked()) || (end == N && row.back().is_blocked()))
    //                 continue;

    //             auto const     cells = std::span(row).subspan(start, end - start);
    //             uint64_t const x     = std::ranges::fold_left(cells, 0ull, [](uint64_t const acc, grid_cell const& c)
    //                                                           { return 10 * acc + c.get_digit(); });

    //             numbers.push_back(x);

    //             start = end + 1;
    //         }
    //     }

    //     return numbers;
    // }

    constexpr auto& digits_array() const noexcept { return digits_; }
    constexpr auto& region_index_array() const noexcept { return region_index_; }

private:
    template<CRowPredicate... Preds>
    friend class number_cross_grid_solver;

    std::vector<grid_region> grid_regions_{};

    std::bitset<N * N> highlighted_{};
    std::bitset<N * N> blocked_{};

    std::tuple<Predicates...> predicates_{};

    std::array<uint16_t, N> allowed_digits_{};

    std::array<std::array<uint8_t, N>, N> digits_{};
    std::array<std::array<uint8_t, N>, N> region_index_{};


    constexpr auto to_idx_(int r, int c) const noexcept { return r * N + c; }

    template<int Row = 0>
    constexpr void init_allowed_digits_() noexcept
    {
        if constexpr(Row < N)
        {
            allowed_digits_[Row] = this->predicate<Row>().allowed_digits().to_ulong();
            init_allowed_digits_<Row + 1>();
        }
    }

    constexpr void init_regions_() noexcept
    {
        std::bitset<N * N> visited;

        for(int r = 0; r < N; ++r)
        {
            for(int c = 0; c < N; ++c)
            {
                if(visited[to_idx_(r, c)])
                    continue;

                SPDLOG_DEBUG("Starting visiting new island starting at ({}, {})", r, c);

                uint32_t const region_index = region_index_[r][c];

                grid_region& region = grid_regions_[region_index];
                region.set_allowed_digits(allowed_digits_[r]);

                fill_region_dfs_(r, c, region_index, region, visited);

                SPDLOG_INFO("Island idx: {}, size: {}, source: {}, allowed_digits: {}, neighbors: {}", region_index,
                            region.cells().size(), region.get_source(), region.get_allowed_digits().to_string(),
                            region.neighbors());
            }
        }
    }

    constexpr void fill_region_dfs_(int row, int col, int region_index, grid_region& region,
                                    std::bitset<N * N>& visited) noexcept
    {
        auto const in_range = [](std::pair<int, int> p)
        { return p.first >= 0 && p.first < N && p.second >= 0 && p.second < N; };

        if(!in_range({row, col}) || visited[to_idx_(row, col)] || region_index_[row][col] != region_index)
            return;

        visited[to_idx_(row, col)] = true;

        region.add_cell(row, col);
        auto const [sr, sc] = region.get_source();

        bool const is_curr_highlighted   = this->highlighted(row, col);
        bool const is_source_highlighted = this->highlighted(sr, sc);

        if((!is_source_highlighted & is_curr_highlighted) ||
           ((is_source_highlighted & is_curr_highlighted) &&
            this->allowed_digits(row).count() < this->allowed_digits(sr).count()))
        {
            region.set_allowed_digits(this->allowed_digits(row));
            region.set_source(row, col);
        }

        auto valid_positions =
            std::array<std::pair<int, int>, 4>{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}} |
            std::views::transform([&](auto const& d) { return std::make_pair(row + d.first, col + d.second); }) |
            std::views::filter(in_range);

        for(auto [nr, nc]: valid_positions)
        {
            auto const neighbor_region_index = region_index_[nr][nc];

            if(neighbor_region_index == region_index)
                fill_region_dfs_(nr, nc, region_index, region, visited);
            else
                region.add_neighbor(neighbor_region_index);
        }
    }
};


template<CRowPredicate... Predicates>
struct fmt::formatter<number_cross_grid<Predicates...>>
{
    using value_type = number_cross_grid<Predicates...>;

    static constexpr size_t N          = sizeof...(Predicates);
    static constexpr auto   kValidOpts = std::array<char, 6>{{'d', 'r', 'o', 'D', 'R', 'O'}};

    constexpr auto parse(format_parse_context& ctx)
    {
        auto       it  = ctx.begin();
        auto const end = ctx.end();

        if(it != end && std::ranges::contains(kValidOpts, *it))
        {
            presentation_ = to_lower_mp_[static_cast<unsigned char>(*it)];
            extra_flags_  = (*it != presentation_);
            it++;
        }

        if(it != end && *it != '}')
            throw fmt::format_error("invalid format");
        return it;
    }

    template<typename FormatContext>
    auto format(value_type const& grid, FormatContext& ctx) const
    {
        using namespace std::literals;

        auto const cell_sep_line  = "---"sv;
        auto const cell_sep_empty = "   "sv;
        auto const cross_ch       = "+"sv;

        auto const blocked_ch     = '#';
        auto const altered_ch     = extra_flags_ ? '\'' : ' ';
        auto const highlighted_ch = extra_flags_ ? '@' : ' ';

        auto print_digit_to = [&](auto& ctx, int i, int j)
        {
            auto const dig_ch = [&]() -> char
            {
                switch(presentation_)
                {
                    case 'r':
                        return '0' + grid.region_index_array()[i][j];
                    case 'o':
                        return grid.region_of(i, j).get_digit() == 0 ? ' ' : '0' + grid.region_of(i, j).get_digit();
                    default:
                        return grid(i, j) == 0 ? ' ' : '0' + grid(i, j);
                }
            }();

            auto const [ch, flags_ch] = [&]() -> std::pair<char, char>
            {
                auto const flags = (grid.blocked(i, j) & (presentation_ == 'd')) * 1 + grid.highlighted(i, j) * 2 +
                                   (grid.altered(i, j) & (presentation_ == 'd')) * 3;
                switch(flags)
                {
                    case 1:
                        return {blocked_ch, ' '};
                    case 2:
                        return {dig_ch, highlighted_ch};
                    case 3:
                        return {dig_ch, altered_ch};
                    default:
                        return {dig_ch, ' '};
                }
            }();

            fmt::format_to(ctx.out(), " {}{}"sv, ch, flags_ch);
        };

        std::array<std::string_view, N + 2> row_sep{};

        row_sep.fill(cell_sep_line);
        row_sep.front() = row_sep.back() = ""sv;
        fmt::format_to(ctx.out(), "{}\n"sv, fmt::join(row_sep, cross_ch));

        for(size_t i = 0; i < N; ++i)
        {
            if constexpr(N > 1)
            {
                fmt::format_to(ctx.out(), "|"sv);
                print_digit_to(ctx, i, 0);
            }

            for(size_t j = 1; j < N; ++j)
            {
                bool const is_vertical_sep = grid.region_index_array()[i][j] == grid.region_index_array()[i][j - 1];
                fmt::format_to(ctx.out(), "{}"sv, is_vertical_sep ? ' ' : '|');
                print_digit_to(ctx, i, j);
            }

            if(i < N - 1)
            {
                for(size_t j = 0; j < N; ++j)
                {
                    bool const is_horizontal_sep =
                        grid.region_index_array()[i][j] == grid.region_index_array()[i + 1][j];
                    row_sep[j + 1] = is_horizontal_sep ? cell_sep_empty : cell_sep_line;
                }
                fmt::format_to(ctx.out(), "|\n{}\n"sv, fmt::join(row_sep, cross_ch));
            }
        }

        row_sep.fill(cell_sep_line);
        row_sep.front() = row_sep.back() = ""sv;
        fmt::format_to(ctx.out(), "|\n{}"sv, fmt::join(row_sep, cross_ch));

        return ctx.out();
    }

private:
    char presentation_ = 'd'; // digit + altered + highlighted
    bool extra_flags_  = true;

    static constexpr auto to_lower_mp_ = []()
    {
        std::array<char, 256> table{};
        for(size_t i = 0; i < 256; ++i)
        {
            auto const c = static_cast<unsigned char>(i);
            table[i]     = (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
        }
        return table;
    }();
};

#endif // NUMBER_CROSS_GRID_H
