#include <algorithm>
#include <array>
#include <initializer_list>
#include <ranges>
#include <span>
#include <tuple>
#include <vector>

#include <fmt/core.h>

#include <spdlog/spdlog.h>

#include "integer_factorizations.h"


#define INLINE inline __attribute__((always_inline))


class mirror_grid;
class mirror_grid_solver;

enum mirror_type : uint8_t
{
    MirrorNone = 0,
    MirrorLR   = 1,
    MirrorRL   = 2,
};

auto format_as(mirror_type m)
{
    static constexpr std::array names{"None", "LR", "RL"};
    return names[static_cast<uint8_t>(m)];
}

enum direction : uint8_t
{
    Left       = 0,
    Top        = 1,
    Right      = 2,
    Bottom     = 3,
    InvalidDir = 4
};

auto format_as(direction d)
{
    static constexpr std::array names{"Left", "Top", "Right", "Bottom", "InvalidDir"};
    return names[static_cast<uint8_t>(d)];
}


class mirror_grid
{
public:
    using num_type = uint64_t;

    mirror_grid(size_t n)
        : nums_(4 * n, 0),
          mask_(n * n, false),
          mirrors_(n * n, MirrorNone),
          len_{n}
    {}

    mirror_grid(std::span<num_type const> left, std::span<num_type const> top, std::span<num_type const> right,
                std::span<num_type const> bottom)
        : mirror_grid(validate_sizes(left.size(), top.size(), right.size(), bottom.size()))
    {
        auto it = nums_.begin();
        for(auto&& s: {left, top, right, bottom})
            it = std::copy(s.begin(), s.end(), it);

        std::transform(nums_.begin(), nums_.end(), mask_.begin(), [](auto const& x) { return x == 0; });
    }

    mirror_grid(std::initializer_list<num_type> left, std::initializer_list<num_type> top,
                std::initializer_list<num_type> right, std::initializer_list<num_type> bottom)
        : mirror_grid(validate_sizes(left.size(), top.size(), right.size(), bottom.size()))
    {
        auto it = nums_.begin();
        for(auto&& s: {left, top, right, bottom})
            it = std::copy(s.begin(), s.end(), it);

        std::transform(nums_.begin(), nums_.end(), mask_.begin(), [](auto const& x) { return x == 0; });
    }

    constexpr auto length() const noexcept { return len_; }

    INLINE constexpr auto& operator()(int const row, int const col) noexcept { return mirrors_[row * len_ + col]; }
    INLINE constexpr auto& operator()(int const row, int const col) const noexcept
    {
        return mirrors_[row * len_ + col];
    }

    INLINE constexpr auto& num(direction const d, int const i) noexcept { return nums_[d * len_ + i]; }
    INLINE constexpr auto& num(direction const d, int const i) const noexcept { return nums_[d * len_ + i]; }

    INLINE constexpr mirror_type mirror(int const row, int const col) const noexcept
    {
        return static_cast<mirror_type>(1 * (operator()(row, col) > 0) + 2 * (operator()(row, col) < 0));
    }

    INLINE constexpr auto in_bounds(int const row, int const col) const noexcept
    {
        return row >= 0 && row < len_ && col >= 0 && col < len_;
    }
    INLINE constexpr auto in_border(int const row, int const col, int const offset = 0) const noexcept
    {
        return ((row == offset - 1 || row == len_ - offset) && (offset - 1 <= col) && (col <= len_ - offset)) ||
               ((col == offset - 1 || col == len_ - offset) && (offset - 1 <= row) && (row <= len_ - offset));
    }

    INLINE constexpr auto can_place_mirror(int const row, int const col) const noexcept
    {
        if(row > 0 && operator()(row - 1, col) != 0)
            return false;
        if(col > 0 && operator()(row, col - 1) != 0)
            return false;
        if(row < len_ - 1 && operator()(row + 1, col) != 0)
            return false;
        if(col < len_ - 1 && operator()(row, col + 1) != 0)
            return false;

        return true;
    }
    INLINE constexpr auto can_place_mirror(int const row, int const col, mirror_type const m) const noexcept
    {
        if(m != MirrorNone && mirror(row, col) == m)
            return true;
        return mirror(row, col) == MirrorNone && can_place_mirror(row, col);
    }

    INLINE constexpr auto add_mirror_counter(int const row, int const col, mirror_type const m) noexcept
    {
        return operator()(row, col) += (m == MirrorLR) - (m == MirrorRL);
    }
    INLINE constexpr auto remove_mirror_counter(int const row, int const col, mirror_type const m) noexcept
    {
        return operator()(row, col) -= (m == MirrorLR) - (m == MirrorRL);
    }

    struct result
    {
        num_type left;
        num_type top;
        num_type right;
        num_type bottom;
        num_type product;
    };

    constexpr result get_result() const noexcept
    {
        auto compute_clue_sum = [&](direction d)
        {
            num_type sum = 0;
            for(int i = 0; i < len_; ++i)
                sum += num(d, i) * mask_[d * len_ + i];
            return sum;
        };
        auto const left_sum   = compute_clue_sum(Left);
        auto const top_sum    = compute_clue_sum(Top);
        auto const right_sum  = compute_clue_sum(Right);
        auto const bottom_sum = compute_clue_sum(Bottom);
        auto const product    = left_sum * top_sum * right_sum * bottom_sum;

        return {left_sum, top_sum, right_sum, bottom_sum, product};
    }

private:
    friend class mirror_grid_solver;

    std::vector<num_type> nums_;
    std::vector<bool>     mask_;
    std::vector<int>      mirrors_;
    size_t                len_;

    INLINE static size_t validate_sizes(size_t const l, size_t const t, size_t const r, size_t const b)
    {
        if(l != t || t != r || r != b) [[unlikely]]
            throw std::invalid_argument{"All sides must have the same length"};
        return l;
    }
};

template<>
struct fmt::formatter<mirror_grid>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<class FormatContext>
    auto format(mirror_grid const& grid, FormatContext& ctx) const
    {
        constexpr int cell_width  = 5;
        constexpr int cell_height = 2;

        auto print_line = [&]()
        {
            fmt::format_to(ctx.out(), "{: ^{}}", "", cell_width);
            for(int i = 0; i < grid.length(); ++i)
                fmt::format_to(ctx.out(), "|{:-^{}}", "", cell_width);
            fmt::format_to(ctx.out(), "|\n");
        };

        auto print_header = [&](direction const placement)
        {
            fmt::format_to(ctx.out(), "{: ^{}}", "", cell_width);
            for(int i = 0; i < grid.length(); ++i)
                fmt::format_to(ctx.out(), " {: ^{}}",
                               grid.num(placement, i) > 0 ? fmt::to_string(grid.num(placement, i)) : "*", cell_width);
            fmt::format_to(ctx.out(), " ");
        };

        auto print_row = [&](size_t const r)
        {
            static constexpr std::array char_map{' ', '\\', '/'};

            auto const l_num = grid.num(Left, r) > 0 ? std::to_string(grid.num(Left, r)) : "*";
            auto const r_num = grid.num(Right, r) > 0 ? std::to_string(grid.num(Right, r)) : "*";

            auto const width = cell_width - 1;
            for(int h = 0; h < cell_height; ++h)
            {
                auto const pos = 1 + (cell_width / cell_height) * h + (cell_width % cell_height) / 2;

                fmt::format_to(ctx.out(), "{: >{}} ", h == cell_height / 2 ? l_num : "", width);
                for(int c = 0; c < grid.length(); ++c)
                {
                    auto const m    = grid.mirror(r, c);
                    auto const padd = (m < 2) ? width - pos : pos;
                    fmt::format_to(ctx.out(), "|{: ^{}}{}{: ^{}}", "", width - padd, char_map[m], "", padd);
                }
                fmt::format_to(ctx.out(), "| {: <{}}\n", h == 1 ? r_num : "", cell_width - 1);
            }
        };

        fmt::format_to(ctx.out(), "\n");
        print_header(Top);
        fmt::format_to(ctx.out(), "\n");
        for(int r = 0; r < grid.length(); ++r)
        {
            print_line();
            print_row(r);
        }
        print_line();
        print_header(Bottom);
        return fmt::format_to(ctx.out(), "\n");
    }
};


template<class T>
class restorer
{
public:
    // clang-format off
    constexpr restorer(T& value) noexcept
        : value_{value}, old_value_{value}, completed_{false}
    {}
    // clang-format on

    constexpr ~restorer() noexcept
    {
        if(completed_)
            restore();
    }

    constexpr void complete() noexcept { completed_ = true; }
    constexpr void restore() noexcept { value_ = old_value_; }

private:
    T&   value_;
    T    old_value_;
    bool completed_;
};


class mirror_grid_solver
{
public:
    using num_type = mirror_grid::num_type;

    mirror_grid_solver(mirror_grid& grid)
        : grid_{grid},
          factorizations_{}
    {}

    bool solve()
    {
        init_();
        return try_next_number_(0);
    }

private:
    mirror_grid&                                                      grid_;
    std::vector<std::tuple<integer_factorizations<num_type>, size_t>> factorizations_{};

    // CONSTANTS
    static constexpr auto dir_vec = std::array<std::array<int, 2>, 5>{{{0, -1}, {-1, 0}, {0, 1}, {1, 0}, {0, 0}}};
    static constexpr auto reverse_dir_map = std::array<direction, 5>{{Right, Bottom, Left, Top, InvalidDir}};
    static constexpr auto mirror_dir_map =
        std::array<std::array<direction, 5>, 3>{{{Left, Top, Right, Bottom, InvalidDir}, // None
                                                 {Top, Left, Bottom, Right, InvalidDir}, // LR
                                                 {Bottom, Right, Top, Left, InvalidDir}}}; // RL
    static constexpr auto mirror_border_placement =
        std::array<std::array<mirror_type, 5>, 5>{{{MirrorNone, MirrorLR, MirrorNone, MirrorRL, MirrorNone},
                                                   {MirrorLR, MirrorNone, MirrorRL, MirrorNone, MirrorNone},
                                                   {MirrorNone, MirrorRL, MirrorNone, MirrorLR, MirrorNone},
                                                   {MirrorRL, MirrorNone, MirrorLR, MirrorNone, MirrorNone},
                                                   {MirrorNone, MirrorNone, MirrorNone, MirrorNone, MirrorNone}}};

    struct grid_position
    {
        int       row;
        int       col;
        direction dir;

        constexpr grid_position next(mirror_type const m, int const dist) const noexcept
        {
            auto const new_dir = mirror_dir_map[m][dir];
            return {row + dir_vec[new_dir][0] * dist, col + dir_vec[new_dir][1] * dist, new_dir};
        }
    };

    void init_()
    {
        int const len = grid_.length();
        factorizations_.reserve(4 * len);

        for(int i = 0; i < grid_.nums_.size(); ++i)
        {
            auto const& x = grid_.nums_[i];
            if(x > 0)
                factorizations_.emplace_back(integer_factorizations<num_type>(x, len), i);
        }

        std::sort(factorizations_.begin(), factorizations_.end(), [&](auto const& a, auto const& b)
                  { return grid_.nums_[std::get<1>(a)] < grid_.nums_[std::get<1>(b)]; });

        spdlog::debug("Number order: {}",
                      factorizations_ | std::views::transform([&](auto& t) { return grid_.nums_[std::get<1>(t)]; }));
    }

    constexpr bool try_next_number_(size_t const number_idx)
    {
        if(number_idx == factorizations_.size())
        {
            spdlog::debug("Completed iterating input numbers. Trying to complete grid: \n{}", grid_);
            return try_complete_grid_();
        }

        spdlog::debug("STATE: \n{}", grid_);
        spdlog::debug("Trying number_idx={} out of {} numbers", number_idx, factorizations_.size());

        auto& [factorizations, i] = factorizations_[number_idx];

        auto const placement = static_cast<direction>(i / grid_.length());
        int const  loc       = i % grid_.length();
        int const  num       = grid_.num(placement, loc);

        spdlog::debug("Started with number {} on {}[{}]", num, placement, loc);

        int const  len = grid_.length();
        auto const start_pos =
            grid_position{(placement % 2 == 0) ? loc : (placement == Top ? 0 : len - 1),
                          (placement % 2 == 1) ? loc : (placement == Left ? 0 : len - 1), reverse_dir_map[placement]};

        for(auto it = factorizations.rbegin(); it != factorizations.rend(); ++it)
        {
            auto       factors       = *it;
            auto const total_factors = factors.total_factors();

            spdlog::debug("Trying factorization {} of num={} (total_factors={}). Starting at ({},{}), dir={}", factors,
                          num, total_factors, placement, loc, start_pos.row, start_pos.col, start_pos.dir);

            if(try_next_factor_(number_idx, factors, 0, total_factors, start_pos))
                return true;
        }

        return false;
    }

    constexpr bool try_complete_grid_()
    {
        restorer tx(grid_.nums_);

        for(int i = 0; i < grid_.nums_.size(); ++i)
        {
            auto const placement = static_cast<direction>(i / grid_.length());
            auto const loc       = i % grid_.length();

            int const len = grid_.length();
            int       row = (placement % 2 == 0) ? loc : (placement == Top ? -1 : len);
            int       col = (placement % 2 == 1) ? loc : (placement == Left ? -1 : len);
            auto      dir = reverse_dir_map[placement];

            int segment_len   = 0;
            int num_from_path = 1;

            do
            {
                row += dir_vec[dir][0];
                col += dir_vec[dir][1];
                ++segment_len;
                if(dir != mirror_dir_map[grid_.mirror(row, col)][dir])
                {
                    num_from_path *= segment_len;
                    segment_len = 0;
                    dir         = mirror_dir_map[grid_.mirror(row, col)][dir];
                }
            }
            while(grid_.in_bounds(row, col));

            num_from_path *= (segment_len == 0) ? 1 : segment_len;

            bool const is_valid_path = grid_.num(placement, loc) == 0 || grid_.num(placement, loc) == num_from_path;
            if(!is_valid_path)
            {
                spdlog::debug("Path from {}[{}], resulted in number={} but expected {}, arriving at ({},{}), dir={}",
                              placement, loc, num_from_path, grid_.num(placement, loc), row, col, dir);
                tx.restore();
                return false;
            }

            // auto const end_placement = static_cast<direction>(1 * (row == -1) + 2 * (col == len) + 3 * (row == len));
            // auto const end_loc       = (end_placement % 2 == 0) ? row : col;

            spdlog::debug("Path from {}[{}]={} arrived at pos=({},{}), dir={}. Setting to value {}.",
                          placement, loc, grid_.num(placement, loc), row, col, dir, num_from_path);
            grid_.num(placement, loc) = num_from_path;
        }

        spdlog::debug("COMPLETED GRID: \n{}", grid_);
        return true;
    };

    constexpr bool is_path_valid_(grid_position const& pos, grid_position const& new_pos) const noexcept
    {
        auto const dist = std::abs(new_pos.row - pos.row) + std::abs(new_pos.col - pos.col);

        int row = pos.row, col = pos.col;
        for(int k = 0; k < dist - 1; ++k)
        {
            row += dir_vec[new_pos.dir][0], col += dir_vec[new_pos.dir][1];
            if(grid_.mirror(row, col) != MirrorNone && grid_.in_bounds(row, col))
                return false;
        }
        row += dir_vec[new_pos.dir][0], col += dir_vec[new_pos.dir][1];
        return new_pos.col == col && new_pos.row == row;
    };

    constexpr bool try_next_factor_(size_t const number_idx, factors_view<num_type>& factors, size_t const factor_idx,
                                    size_t const total_factors, grid_position const& pos)
    {
        if(factor_idx == total_factors)
            return try_complete_factors_(number_idx, pos);

        // checks end of path is valid, checking last number can be placed on border
        auto is_pos_valid = [&](grid_position const& pos)
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

            int const dist = f.base;
            --f.count;

            auto const new_pos_lr = pos.next(MirrorLR, dist);
            if(is_pos_valid(new_pos_lr) && grid_.can_place_mirror(pos.row, pos.col, MirrorLR) &&
               is_path_valid_(pos, new_pos_lr))
            {
                spdlog::debug("Trying factor {} of {} and mirror={}, from ({},{}) to ({},{}), with dir={}", f.base,
                              factors, MirrorLR, pos.row, pos.col, new_pos_lr.row, new_pos_lr.col, new_pos_lr.dir);

                grid_.add_mirror_counter(pos.row, pos.col, MirrorLR);
                if(try_next_factor_(number_idx, factors, factor_idx + 1, total_factors, new_pos_lr))
                    return true;
                grid_.remove_mirror_counter(pos.row, pos.col, MirrorLR);
            }

            auto const new_pos_rl = pos.next(MirrorRL, dist);
            if(is_pos_valid(new_pos_rl) && grid_.can_place_mirror(pos.row, pos.col, MirrorRL) &&
               is_path_valid_(pos, new_pos_rl))
            {
                spdlog::debug("Trying factor {} of {} and mirror={}, from ({},{}) to ({},{}), with dir={}", f.base,
                              factors, MirrorRL, pos.row, pos.col, new_pos_rl.row, new_pos_rl.col, new_pos_rl.dir);

                grid_.add_mirror_counter(pos.row, pos.col, MirrorRL);
                if(try_next_factor_(number_idx, factors, factor_idx + 1, total_factors, new_pos_rl))
                    return true;
                grid_.remove_mirror_counter(pos.row, pos.col, MirrorRL);
            }

            auto const new_pos_none = pos.next(MirrorNone, dist - 1);
            if(factor_idx == 0 && is_pos_valid(new_pos_none) && is_path_valid_(pos, new_pos_none))
            {
                spdlog::debug(
                    "Trying factor {} of {} and no mirror (factor_idx==0), from ({},{}) to ({},{}) with dir={}", f.base,
                    factors, pos.row, pos.col, new_pos_none.row, new_pos_none.col, new_pos_none.dir);

                if(try_next_factor_(number_idx, factors, factor_idx + 1, total_factors, new_pos_none))
                    return true;
            }

            ++f.count;
        };

        return false;
    }

    constexpr bool try_complete_factors_(size_t const number_idx, grid_position const& pos)
    {
        int const len      = grid_.length();
        auto const& [_, i] = factorizations_[number_idx];
        auto const num     = grid_.nums_[i];

        // If pos is at the border
        if(grid_.in_border(pos.row, pos.col, 0))
        {
            auto const placement =
                static_cast<direction>(1 * (pos.row == -1) + 2 * (pos.col == len) + 3 * (pos.row == len));
            auto const loc = (placement % 2 == 0) ? pos.row : pos.col;

            bool const is_valid_endpoint = grid_.num(placement, loc) == 0 || grid_.num(placement, loc) == num;
            if(!is_valid_endpoint)
            {
                spdlog::debug("Invalid path reached border {}[{}]={} from dir={} (is_valid_endpoint={})", placement,
                              loc, grid_.num(placement, loc), pos.dir, is_valid_endpoint);
                return false;
            }

            spdlog::debug("Path reached border and setting {}[{}]={} -> {}", placement, loc, grid_.num(placement, loc),
                          num);

            auto const num_temp       = grid_.num(placement, loc);
            grid_.num(placement, loc) = num;

            if(try_next_number_(number_idx + 1))
                return true;

            grid_.num(placement, loc) = num_temp;
        }
        else // grid_.in_border(pos.row, pos.col, 1) == true
        {
            auto const placement =
                static_cast<direction>(1 * (pos.row == 0) + 2 * (pos.col == len - 1) + 3 * (pos.row == len - 1));
            auto const loc = (placement % 2 == 0) ? pos.row : pos.col;

            auto const m                 = mirror_border_placement[placement][pos.dir];
            bool const is_valid_endpoint = grid_.num(placement, loc) == 0 || grid_.num(placement, loc) == num;
            bool const can_place_mirror  = grid_.can_place_mirror(pos.row, pos.col, m);

            if(m == MirrorNone || !is_valid_endpoint || !can_place_mirror)
            {
                spdlog::debug("Invalid path reached adjacent to {}[{}]={} from dir={} "
                              "(is_valid_mirror={}, is_valid_endpoint={}, can_place_mirror={})",
                              placement, loc, grid_.num(placement, loc), pos.dir, m == MirrorNone, is_valid_endpoint,
                              can_place_mirror);
                return false;
            }

            spdlog::debug("Path reached border and setting {}[{}]={} -> {}, after placing final mirror={} at ({},{})",
                          placement, loc, grid_.num(placement, loc), num, m, pos.row, pos.col);

            auto const num_temp       = grid_.num(placement, loc);
            grid_.num(placement, loc) = num;
            grid_.add_mirror_counter(pos.row, pos.col, m);

            if(try_next_number_(number_idx + 1))
                return true;

            grid_.remove_mirror_counter(pos.row, pos.col, m);
            grid_.num(placement, loc) = num_temp;
        }

        return false;
    }
};
