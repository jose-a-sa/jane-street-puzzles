#ifndef NUMBER_CROSS_PRED_H
#define NUMBER_CROSS_PRED_H

#include <algorithm>
#include <array>
#include <bitset>
#include <ranges>
#include <span>

#include <fmt/core.h>

#include "may-2025/number_cross_cell.h"


using int128_t  = __int128_t;
using uint128_t = __uint128_t;


template<class T>
concept CRowPredicate = requires(T t, int64_t x, std::span<grid_cell const> cells) {
    { t.check(x, cells) } -> std::same_as<bool>;
    { t.get_allowed_digits() } -> std::same_as<std::bitset<10> const&>;
};


template<class Derived>
struct row_predicate
{
    constexpr explicit row_predicate(std::bitset<10> allowed_digits = 0b1111111110) noexcept
        : allowed_digits_{allowed_digits}
    {}

    constexpr auto check(std::span<grid_cell const> const cells) const noexcept -> bool
    {
        if(cells.size() < 2)
            return false;

        bool const all_digit_valid =
            std::ranges::all_of(cells, [&](grid_cell const& c) { return allowed_digits_.test(c.get_digit()); });
        if(!all_digit_valid)
            return false;

        int64_t x = std::ranges::fold_left(cells, 0, [](int64_t const acc, grid_cell const& c)
                                           { return 10 * acc + c.get_digit(); });
         
        return self()(x, cells);
    }

    constexpr auto check(int64_t const x, std::span<grid_cell const> const cells) const noexcept -> bool
    {
        if(cells.size() < 2)
            return false;

        bool const all_digit_valid =
            std::ranges::all_of(cells, [&](grid_cell const& c) { return allowed_digits_.test(c.get_digit()); });
        if(!all_digit_valid)
            return false;

        return self()(x, cells);
    }

    constexpr auto& self() const noexcept { return static_cast<Derived const&>(*this); }

    constexpr auto& get_allowed_digits() const noexcept { return allowed_digits_; }

protected:
    std::bitset<10> allowed_digits_;
};


struct is_square : row_predicate<is_square>
{
    using row_predicate<is_square>::row_predicate;

private:
    friend class row_predicate<is_square>;

    constexpr auto operator()(int64_t const x, std::span<grid_cell const> const) const noexcept -> bool
    {
        auto const s = static_cast<int64_t>(std::sqrt(x));
        return s * s == x || (s + 1) * (s + 1) == x;
    }
};


struct is_odd_palindrome : row_predicate<is_odd_palindrome>
{
    using row_predicate<is_odd_palindrome>::row_predicate;

private:
    friend class row_predicate<is_odd_palindrome>;

    constexpr auto operator()(int64_t const, std::span<grid_cell const> const digits) const noexcept -> bool
    {
        if((digits.back().get_digit() & 1) == 0 || (digits.front().get_digit() & 1) == 0)
            return false;

        auto const mid         = digits.size() / 2;
        auto const first_half  = digits.subspan(0, mid);
        auto const second_half = digits.subspan(digits.size() - mid, mid);

        return std::ranges::equal(first_half, std::views::reverse(second_half),
                                  [](grid_cell const& a, grid_cell const& b)
                                  { return a.get_digit() == b.get_digit(); });
    }
};


struct is_fibonacci : row_predicate<is_fibonacci>
{
    using row_predicate<is_fibonacci>::row_predicate;

private:
    friend class row_predicate<is_fibonacci>;

    constexpr auto operator()(int64_t const x, std::span<grid_cell const> const) const noexcept -> bool
    {
        return std::ranges::binary_search(numbers, x);
    }

private:
    static constexpr auto numbers = std::array<int64_t, 56>{
        0,          1,          1,           2,           3,           5,           8,           13,
        21,         34,         55,          89,          144,         233,         377,         610,
        987,        1597,       2584,        4181,        6765,        10946,       17711,       28657,
        46368,      75025,      121393,      196418,      317811,      514229,      832040,      1346269,
        2178309,    3524578,    5702887,     9227465,     14930352,    24157817,    39088169,    63245986,
        102334155,  165580141,  267914296,   433494437,   701408733,   1134903170,  1836311903,  2971215073,
        4807526976, 7778742049, 12586269025, 20365011074, 32951280099, 53316291173, 86267571272, 139583862445};
};

struct is_prime : row_predicate<is_prime>
{
    using row_predicate<is_prime>::row_predicate;

private:
    friend class row_predicate<is_prime>;

    constexpr auto operator()(int64_t const x, std::span<grid_cell const> const) const noexcept -> bool
    {
        if(x < 2)
            return false;

        // Handle small primes directly
        for(auto const p: {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47})
        {
            if(x == p)
                return true;
            if(x % p == 0 && x != p)
                return false;
        }

        // Deterministic Miller-Rabin bases for 64-bit integers
        for(auto const b: {2, 325, 9375, 28178, 450775, 9780504, 1795265022})
        {
            if(b % x == 0)
                return true;
            if(!miller_rabin_test(x, b))
                return false;
        }
        return true;
    }

    constexpr uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) const noexcept
    {
        uint64_t result = 1;
        uint64_t cur    = base % mod;
        while(exp > 0)
        {
            if(exp & 1)
                result = (static_cast<uint128_t>(result) * cur) % mod;
            cur = (static_cast<uint128_t>(cur) * cur) % mod;
            exp >>= 1;
        }
        return result;
    }

    // Miller-Rabin test for a single base
    constexpr bool miller_rabin_test(uint64_t n, uint64_t a) const noexcept
    {
        if(n % a == 0)
            return false;
        uint64_t d = n - 1;
        int      r = 0;
        while((d & 1) == 0)
        {
            d >>= 1;
            r++;
        }
        uint64_t x = mod_pow(a, d, n);
        if(x == 1 || x == n - 1)
            return true;
        for(int i = 1; i < r; i++)
        {
            x = (static_cast<uint128_t>(x) * x) % n;
            if(x == n - 1)
                return true;
        }
        return false;
    }
};


template<int64_t N>
struct is_multiple_of : row_predicate<is_multiple_of<N>>
{
    using row_predicate<is_multiple_of<N>>::row_predicate;
    // using row_predicate<is_multiple_of<N>>::check;

private:
    friend class row_predicate<is_multiple_of<N>>;

    constexpr auto operator()(int64_t const x, std::span<grid_cell const> const) const noexcept -> bool
    {
        return x % N == 0;
    }
};


template<int64_t N>
struct product_of_digits_matches : row_predicate<product_of_digits_matches<N>>
{
    using row_predicate<product_of_digits_matches<N>>::row_predicate;

    constexpr product_of_digits_matches() noexcept
        : row_predicate<product_of_digits_matches<N>>(compute_allowed_<N>())
    {}

private:
    friend class row_predicate<product_of_digits_matches<N>>;

    constexpr auto operator()(int64_t const, std::span<grid_cell const> const cells) const noexcept
    {
        auto const mult = std::ranges::fold_left(cells, int64_t{1}, [](int64_t const acc, grid_cell const& c)
                                                 { return acc * c.get_digit(); });
        return mult == N;
    }

    template<int64_t M>
    static constexpr std::bitset<10> compute_allowed_() noexcept
    {
        int64_t m = M;

        std::array<int, 10> factor_count{};
        for(auto const f: {2, 3, 5, 7})
        {
            while(m % f == 0)
            {
                ++factor_count[f];
                m /= f;
            }
        }

        if(m == 0)
            return std::bitset<10>{0b1111111111};

        std::bitset<10> allowed_digits;
        allowed_digits[1] = true;
        allowed_digits[2] = factor_count[2] > 0;
        allowed_digits[3] = factor_count[3] > 0;
        allowed_digits[4] = factor_count[2] > 1;
        allowed_digits[5] = factor_count[5] > 0;
        allowed_digits[6] = factor_count[2] > 0 && factor_count[3] > 0;
        allowed_digits[7] = factor_count[7] > 0;
        allowed_digits[8] = factor_count[2] > 2;
        allowed_digits[9] = factor_count[3] > 1;
        return allowed_digits;
    }
};

struct is_divisible_by_its_digits : row_predicate<is_divisible_by_its_digits>
{
    using row_predicate<is_divisible_by_its_digits>::row_predicate;
    using row_predicate<is_divisible_by_its_digits>::check;

private:
    friend class row_predicate<is_divisible_by_its_digits>;

    constexpr auto operator()(int64_t const x, std::span<grid_cell const> const cells) const noexcept -> bool
    {
        return std::ranges::all_of(cells, [&](grid_cell const& c) { return x % c.get_digit() == 0; });
    }
};


#endif // NUMBER_CROSS_PRED_H
