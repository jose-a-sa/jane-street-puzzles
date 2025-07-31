#ifndef MIRROR_GRID_H
#define MIRROR_GRID_H

#include <algorithm>
#include <array>
#include <initializer_list>
#include <limits>
#include <ranges>
#include <span>
#include <sys/types.h>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <spdlog/spdlog.h>

#include "utils/base.h"


enum class mirror_type : uint8_t
{
    None = 0,
    LR   = 1,
    RL   = 2
};

auto format_as(mirror_type m)
{
    using namespace std::literals;
    static constexpr auto mirror_type_str = std::array{"None"sv, "LR"sv, "RL"sv};
    return mirror_type_str[std::to_underlying(m)];
}

enum class direction : uint8_t
{
    Left    = 0,
    Top     = 1,
    Right   = 2,
    Bottom  = 3,
    Invalid = 4
};

auto format_as(direction d)
{
    using namespace std::literals;
    static constexpr auto direction_str = std::array{"Left"sv, "Top"sv, "Right"sv, "Bottom"sv, "Invalid"sv};
    return direction_str[std::to_underlying(d)];
}

constexpr std::array<int, 2> direction_to_vector(direction dir) noexcept
{
    static constexpr auto dir_vec = std::array<std::array<int, 2>, 5>{{{0, -1}, {-1, 0}, {0, 1}, {1, 0}, {0, 0}}};
    return dir_vec[std::to_underlying(dir)];
}

constexpr direction reverse_direction(direction dir) noexcept
{
    using enum direction;
    static constexpr auto reverse_dir_map = std::array<direction, 5>{{Right, Bottom, Left, Top, Invalid}};
    return reverse_dir_map[std::to_underlying(dir)];
}

constexpr direction direction_after_mirror(mirror_type m, direction dir) noexcept
{
    using enum direction;
    using enum mirror_type;
    static constexpr auto mirror_dir_map =
        std::array<std::array<direction, 5>, 3>{{{Left, Top, Right, Bottom, Invalid}, // None
                                                 {Top, Left, Bottom, Right, Invalid}, // LR
                                                 {Bottom, Right, Top, Left, Invalid}}}; // RL
    return mirror_dir_map[std::to_underlying(m)][std::to_underlying(dir)];
}

class mirror_grid
{
public:
    using num_type = uint32_t;

    struct laser_position
    {
        int       row{};
        int       col{};
        direction dir = direction::Invalid;

        constexpr laser_position& advance(int const dist = 1) noexcept
        {
            auto const dir_vec = direction_to_vector(dir);
            row += dir_vec[0];
            col += dir_vec[1];
            return *this;
        }

        static constexpr laser_position next_after_mirror(laser_position const& p, mirror_type const m, int const dist) noexcept
        {
            auto const new_dir = direction_after_mirror(m, p.dir);
            auto const dir_vec = direction_to_vector(new_dir);
            return {p.row + dir_vec[0] * dist, p.col + dir_vec[1] * dist, new_dir};
        }

        static constexpr laser_position start_position(direction const placement, int const loc,
                                                       int const length) noexcept
        {
            using enum direction;
            direction const dir = reverse_direction(placement);

            switch(placement)
            {
            case Top:
                return {-1, loc, dir};
            case Right:
                return {loc, length, dir};
            case Bottom:
                return {length, loc, dir};
            case Left:
                return {loc, -1, dir};
            default:
                // spdlog::error("Invalid placement direction: {}", placement);
                return {std::numeric_limits<int>::lowest(), std::numeric_limits<int>::lowest(), Invalid};
            }
        }

        static constexpr std::tuple<direction, int> to_border_placement(laser_position const& p,
                                                                        int const             length) noexcept
        {
            using enum direction;

            bool const is_left   = p.col <= 0;
            bool const is_top    = p.row <= 0;
            bool const is_right  = p.col >= length - 1;
            bool const is_bottom = p.row >= length - 1;

            if(is_top + is_bottom + is_left + is_right == 1)
            {
                auto const placement = static_cast<direction>(
                    std::to_underlying(Left) * is_left + std::to_underlying(Top) * is_top +
                    std::to_underlying(Right) * is_right + std::to_underlying(Bottom) * is_bottom);
                auto const loc = (std::to_underlying(placement) % 2 == 0) ? p.row : p.col;
                return std::make_tuple(placement, loc);
            }
            else
                return std::make_tuple(direction::Invalid, -1);
        }
    };

    struct result
    {
        num_type left;
        num_type top;
        num_type right;
        num_type bottom;
        num_type product;
    };

    constexpr mirror_grid(size_t n)
        : numbers_(4 * n, 0),
          number_mask_(n * n, false),
          mirrors_(n * n, 0),
          length_{n}
    {}

    constexpr mirror_grid(std::span<num_type const> left, std::span<num_type const> top,
                          std::span<num_type const> right, std::span<num_type const> bottom)
        : mirror_grid(validate_sizes_(left.size(), top.size(), right.size(), bottom.size()))
    {
        auto const inputs = std::array{left, top, right, bottom};
        std::ranges::copy(inputs | std::views::join, numbers_.begin());
        std::ranges::transform(numbers_, number_mask_.begin(), [](auto const& x) { return x == 0; });
    }

    constexpr mirror_grid(std::initializer_list<num_type> left, std::initializer_list<num_type> top,
                          std::initializer_list<num_type> right, std::initializer_list<num_type> bottom)
        : mirror_grid(validate_sizes_(left.size(), top.size(), right.size(), bottom.size()))
    {
        auto const inputs = std::array{left, top, right, bottom};
        std::ranges::copy(inputs | std::views::join, numbers_.begin());
        std::ranges::transform(numbers_, number_mask_.begin(), [](auto const& x) { return x == 0; });
    }

    constexpr num_type& boundary_number(direction const dir, int const i) noexcept
    {
        return numbers_[to_num_idx_(dir, i)];
    }

    constexpr num_type const& boundary_number(direction const dir, int const i) const noexcept
    {
        return numbers_[to_num_idx_(dir, i)];
    }

    constexpr mirror_type mirror(int const row, int const col) const noexcept
    {
        bool const is_LR = mirrors_[to_idx_(row, col)] > 0;
        bool const is_RL = mirrors_[to_idx_(row, col)] < 0;
        return static_cast<mirror_type>(std::to_underlying(mirror_type::LR) * is_LR +
                                        std::to_underlying(mirror_type::RL) * is_RL);
    }

    constexpr bool in_bounds(int const row, int const col) const noexcept
    {
        return row >= 0 && row < length_ && col >= 0 && col < length_;
    }

    constexpr bool in_border(int const row, int const col, int const offset = 0) const noexcept
    {
        return ((row == offset - 1 || row == length_ - offset) && (offset - 1 <= col) && (col <= length_ - offset)) ||
               ((col == offset - 1 || col == length_ - offset) && (offset - 1 <= row) && (row <= length_ - offset));
    }

    constexpr bool can_place_mirror(int const row, int const col) const noexcept
    {
        static constexpr auto dirs = std::array<std::pair<int, int>, 4>{{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}};

        auto const check_adjacent_mirror = [&](std::pair<int, int> const& dir)
        {
            auto const nr = row + dir.first;
            auto const nc = col + dir.second;
            return this->in_bounds(nr, nc) && this->mirror(nr, nc) != mirror_type::None;
        };

        return std::ranges::none_of(dirs, check_adjacent_mirror);
    }

    constexpr bool can_place_mirror(int const row, int const col, mirror_type const m) const noexcept
    {
        // precondition: m != mirror_type::None
        if(!this->in_bounds(row, col))
            return false;
        if(this->mirror(row, col) == m)
            return true;
        return this->mirror(row, col) == mirror_type::None && this->can_place_mirror(row, col);
    }

    constexpr auto add_mirror_counter(int const row, int const col, mirror_type const m) noexcept
    {
        return mirrors_[to_idx_(row, col)] += (m == mirror_type::LR) - (m == mirror_type::RL);
    }

    constexpr auto remove_mirror_counter(int const row, int const col, mirror_type const m) noexcept
    {
        return mirrors_[to_idx_(row, col)] -= (m == mirror_type::LR) - (m == mirror_type::RL);
    }

    constexpr result compute_result() const noexcept
    {
        auto compute_clue_sum = [&](direction const dir) -> num_type
        {
            return std::ranges::fold_left(
                std::views::iota(0u, length_), num_type{0}, [&](auto acc, auto i)
                { return acc + this->boundary_number(dir, i) * number_mask_[to_num_idx_(dir, i)]; });
        };

        auto const left_sum   = compute_clue_sum(direction::Left);
        auto const top_sum    = compute_clue_sum(direction::Top);
        auto const right_sum  = compute_clue_sum(direction::Right);
        auto const bottom_sum = compute_clue_sum(direction::Bottom);
        auto const product    = left_sum * top_sum * right_sum * bottom_sum;

        return {left_sum, top_sum, right_sum, bottom_sum, product};
    }

    constexpr auto length() const noexcept { return length_; }

    constexpr std::vector<num_type> const& numbers_array() const noexcept { return numbers_; }
    constexpr std::vector<num_type>&       numbers_array() noexcept { return numbers_; }

private:
    std::vector<num_type> numbers_;
    std::vector<bool>     number_mask_;
    std::vector<int>      mirrors_;
    size_t                length_;

    INLINE static size_t validate_sizes_(size_t const l, size_t const t, size_t const r, size_t const b)
    {
        if(l != t || t != r || r != b) [[unlikely]]
            throw std::invalid_argument{"All sides must have the same length"};
        return l;
    }

    INLINE constexpr int to_idx_(int const row, int const col) const noexcept { return row * length_ + col; }

    INLINE constexpr int to_num_idx_(direction const dir, int const i) const noexcept
    {
        return std::to_underlying(dir) * length_ + i;
    }
};


template<>
struct fmt::formatter<mirror_grid>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<class FormatContext>
    auto format(mirror_grid const& grid, FormatContext& ctx) const
    {
        using namespace std::literals;
        using enum direction;

        constexpr int cell_width    = 5;
        constexpr int cell_height   = 2;
        constexpr int cell_wh_ratio = (cell_width / cell_height);
        constexpr int cell_wh_mod   = (cell_width % cell_height);

        auto get_number_str = [&](direction placement, int loc)
        {
            auto const x = grid.boundary_number(placement, loc);
            return x > 0 ? fmt::to_string(x) : "*";
        };

        auto print_line = [&]()
        {
            fmt::format_to(ctx.out(), "{: ^{}}", "", cell_width);
            for(int i = 0; i < grid.length(); ++i)
                fmt::format_to(ctx.out(), "+{:-^{}}", "", cell_width);
            fmt::format_to(ctx.out(), "+\n");
        };

        auto print_header = [&](direction const placement)
        {
            fmt::format_to(ctx.out(), "{: ^{}}", "", cell_width);
            for(int i = 0; i < grid.length(); ++i)
                fmt::format_to(ctx.out(), " {: ^{}}", get_number_str(placement, i), cell_width);
            fmt::format_to(ctx.out(), " \n");
        };

        auto print_row = [&](size_t const r)
        {
            static constexpr std::array char_map{' ', '\\', '/'};

            auto const left_num  = get_number_str(Left, r);
            auto const right_num = get_number_str(Right, r);

            auto const width = cell_width - 1;
            for(int h = 0; h < cell_height; ++h)
            {
                auto const pos = 1 + cell_wh_ratio * h + cell_wh_mod / 2;

                fmt::format_to(ctx.out(), "{: >{}} ", (h == cell_height / 2) ? left_num : "", width);
                for(int c = 0; c < grid.length(); ++c)
                {
                    auto const m    = grid.mirror(r, c);
                    auto const padd = (m != mirror_type::RL) ? pos : width - pos;
                    fmt::format_to(ctx.out(), "|{: ^{}}{}{: ^{}}", "", padd, char_map[std::to_underlying(m)], "",
                                   width - padd);
                }
                fmt::format_to(ctx.out(), "| {: <{}}\n", (h == 1) ? right_num : "", width);
            }
        };

        fmt::format_to(ctx.out(), "\n");
        print_header(Top);
        for(int r = 0; r < grid.length(); ++r)
        {
            print_line();
            print_row(r);
        }
        print_line();
        print_header(Bottom);
        return ctx.out();
    }
};


#endif // MIRROR_GRID_H
