#ifndef NUMBER_CROSS_CELL_H
#define NUMBER_CROSS_CELL_H

#include <array>
#include <bitset>

#include <fmt/core.h>

class grid_cell
{
public:
    constexpr grid_cell() noexcept
        : value_(allowed_mask)
    {}

    constexpr grid_cell(int const value) noexcept
        : value_((value & digit_mask) | allowed_mask)
    {}

    constexpr auto get_digit() const noexcept { return value_ & digit_mask; }
    constexpr void set_digit(int d) noexcept { value_ = (value_ & ~digit_mask) | (d & digit_mask); }

    constexpr auto get_allowed_digits() const noexcept { return std::bitset<10>{(value_ & allowed_mask) >> 4}; }
    constexpr void set_allowed_digits(std::bitset<10> allowed) noexcept
    {
        value_ = (value_ & ~allowed_mask) | (allowed.to_ulong() << 4);
    }

    constexpr void set_highlighted(bool flag = true) { value_ |= (flag << 14); }
    constexpr bool is_highlighted() const { return value_ & highlighted_flag; }
    constexpr void clear_highlighted() { value_ &= ~highlighted_flag; }

    constexpr void set_blocked(bool flag = true) noexcept { value_ |= (flag << 15); }
    constexpr bool is_blocked() const noexcept { return value_ & blocked_flag; }
    constexpr void clear_blocked() noexcept { value_ &= ~blocked_flag; }

private:
    uint16_t value_;

    static constexpr uint32_t digit_mask   = 0b1111;
    static constexpr uint32_t allowed_mask = 0b0011111111110000;

    static constexpr uint32_t highlighted_flag = 1u << 14;
    static constexpr uint32_t blocked_flag     = 1u << 15;
    static constexpr auto     allowed_flags =
        std::array{1u << 4, 1u << 5, 1u << 6, 1u << 7, 1u << 8, 1u << 9, 1u << 10, 1u << 11, 1u << 12};
};


template<>
struct fmt::formatter<grid_cell>
{
    constexpr auto parse(format_parse_context& ctx) const { return ctx.begin(); }

    template<typename FormatContext>
    auto format(grid_cell const& cell, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "{}", cell.get_digit());
    }
};


struct cell_info
{
    uint8_t island;
    bool    highlighted;

    constexpr cell_info(int island, bool highlighted = false) noexcept
        : island(island),
          highlighted(highlighted)
    {}
};

template<>
struct fmt::formatter<cell_info>
{
    constexpr auto parse(format_parse_context& ctx) const { return ctx.begin(); }

    template<typename FormatContext>
    auto format(cell_info const& c, FormatContext& ctx) const
    {
        auto const ch = c.highlighted ? '\'' : ' ';
        return format_to(ctx.out(), "{}{}{}", ch, c.island, ch);
    }
};


#endif // NUMBER_CROSS_CELL_H
