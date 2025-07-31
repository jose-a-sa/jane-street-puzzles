#ifndef PARTRIDGE_TILING_H
#define PARTRIDGE_TILING_H

#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>
#include <optional>
#include <ranges>
#include <span>
#include <utility>

#include <fmt/ranges.h>
#include <fmt/std.h>

#include <spdlog/spdlog.h>

#include "utils/base.h"

struct square_tile
{
    uint32_t side;
    int      row;
    int      col;
};

template<>
struct fmt::formatter<square_tile>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    constexpr auto format(square_tile const& t, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "Tile(size={}, pos=({}, {}))", t.side, t.row, t.col);
    }
};


template<size_t N>
class partridge_square_tiling
{
public:
    static constexpr size_t kGridSide = N * (N + 1) / 2;
    static constexpr size_t kGridArea = kGridSide * kGridSide;

    static constexpr std::pair<int, int> kUnusedPosition = {-1, -1};
    static constexpr uint8_t             kUnuserArea     = -1;


    constexpr explicit partridge_square_tiling()
        : tile_positions_{},
          tiles_count_{}
    {
        tile_positions_.fill(kUnusedPosition);
    }

    constexpr explicit partridge_square_tiling(std::span<square_tile const> tiles)
        : partridge_square_tiling()
    {
        for(auto const& t: tiles)
        {
            this->try_push_tile(t);
        }
    }

    constexpr auto tile_count(uint32_t const side) const noexcept -> uint32_t { return tiles_count_[side]; }

    constexpr auto try_push_tile(square_tile const& t) noexcept -> bool
    {
        if(t.side == 0 || t.side > N || t.row < 0 || t.row + t.side > kGridSide || t.col < 0 ||
           t.col + t.side > kGridSide)
            return false;

        if(this->tile_count(t.side) >= t.side)
            return false;

        if(this->overlaps_with_placed(t))
            return false;

        this->unchecked_push_tile(t);
        return true;
    }

    constexpr auto unchecked_push_tile(square_tile const& t) noexcept
    {
        auto const row_mask = get_row_mask_(t);
        for(auto& row_bits: filled_pos_ | std::views::drop(t.row) | std::views::take(t.side))
            row_bits |= row_mask;

        auto const idx       = size_offset_(t.side - 1) + tiles_count_[t.side];
        tile_positions_[idx] = {t.row, t.col};
        ++tiles_count_[t.side];
    }

    constexpr auto pop_tile(uint32_t const side) noexcept -> std::optional<square_tile>
    {
        if(side == 0 || side > N || tiles_count_[side] == 0)
            return std::nullopt;

        --tiles_count_[side];
        auto const idx    = size_offset_(side - 1) + tiles_count_[side];
        auto const [r, c] = std::exchange(tile_positions_[idx], kUnusedPosition);

        square_tile t{side, r, c};
        auto const  row_mask = get_row_mask_(t);
        for(auto& row_bits: filled_pos_ | std::views::drop(t.row) | std::views::take(t.side))
            row_bits &= ~row_mask;

        return t;
    }

    INLINE constexpr auto overlaps_with_placed(square_tile const& t) const noexcept -> bool
    {
        auto overlaps = false;

        auto const row_mask = get_row_mask_(t);
        for(auto& row_bits: filled_pos_ | std::views::drop(t.row) | std::views::take(t.side))
            overlaps |= (row_bits & row_mask).any();

        return overlaps;
    }

    constexpr auto tile_counts() const noexcept { return std::span{tiles_count_}.subspan(1); }

    constexpr auto& filled_mask_array() const noexcept { return filled_pos_; }

    constexpr auto& tile_positions() const noexcept { return tile_positions_; }

private:
    std::array<std::pair<int, int>, kGridSide> tile_positions_{};
    std::array<uint32_t, N + 1>                tiles_count_{};

    std::array<std::bitset<kGridSide>, kGridSide> filled_pos_{};

    constexpr auto get_row_mask_(square_tile const& t) const noexcept -> std::bitset<kGridSide>
    {
        return (std::bitset<kGridSide>{}.flip() >> (kGridSide - t.side)) << t.col;
    }

    constexpr auto size_offset_(uint32_t const side) const noexcept { return side * (side + 1) / 2; }

public:
    static constexpr auto kSideSequence = []()
    {
        std::array<uint32_t, kGridSide> result = {};

        uint32_t k = 1;
        for(int const i: std::views::iota(0u, kGridSide))
        {
            while(k * (k + 1) / 2 <= i)
                ++k;
            result[i] = k;
        }

        return result;
    }();
};


template<size_t N>
struct fmt::formatter<partridge_square_tiling<N>>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    constexpr auto format(partridge_square_tiling<N> const& tilling, FormatContext& ctx) const
    {
        using partridge_square_tiling<N>::kGridSide;
        using partridge_square_tiling<N>::kSideSequence;
        using partridge_square_tiling<N>::kUnusedPosition;

        std::array<std::array<uint8_t, kGridSide>, kGridSide> grid{};
        for(auto& row: grid)
            row.fill(-1);

        auto draw_tile = [&](uint32_t side, int row, int col, uint8_t index)
        {
            for(size_t r = row; r < row + side; ++r)
                for(size_t c = col; c < col + side; ++c)
                    grid[r][c] = index;
        };

        auto tiles = std::views::zip(kSideSequence, tilling.tile_positions()) |
                     std::views::filter([](auto const& t) { return std::get<1>(t) != kUnusedPosition; });

        for(auto [idx, pos]: std::views::zip(std::views::iota(0u), tiles))
        {
            auto const& [side, p] = pos;
            draw_tile(side, p.first, p.second, idx);
        }

        auto const& filled_mask = tilling.filled_mask_array();
        fmt::format_to(ctx.out(), "{:-^{}}\n", "", kGridSide * 3 + 2);
        for(size_t r = 0; r < kGridSide; ++r)
        {
            for(size_t c = 0; c < kGridSide; ++c)
            {
                auto const placed = filled_mask[r][c] ? '*' : ' ';
                if(c == 0)
                    fmt::format_to(ctx.out(), "|");
                if(grid[r][c] == uint8_t(-1))
                    fmt::format_to(ctx.out(), "  {:1} ", placed);
                else
                    fmt::format_to(ctx.out(), "{:2}{:1} ", grid[r][c], placed);
            }
            fmt::format_to(ctx.out(), "|\n");
        }
        fmt::format_to(ctx.out(), "{:-^{}}", "", kGridSide * 3 + 2);

        return ctx.out();
    }
};


#endif // PARTRIDGE_TILING_H
