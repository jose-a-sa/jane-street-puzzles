#ifndef NUMBER_CROSS_PRED_H
#define NUMBER_CROSS_PRED_H

#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>
#include <functional>
#include <ranges>
#include <span>

#include <fmt/core.h>
#include <tuple>

#include "utils/base.h"


template<class Pred>
concept CRowPredicate = requires(Pred pred, std::span<uint8_t const> digits) {
    { pred(digits) } -> std::same_as<std::tuple<bool, int64_t>>;
    { pred.allowed_digits() } -> std::same_as<std::bitset<10>>;
};

template<class Tuple>
concept CTupleRowPredicates = requires {
    typename std::tuple_size<Tuple>::type; // Must be a tuple

    requires([]<size_t... I>(std::index_sequence<I...>)
             { return (CRowPredicate<std::tuple_element_t<I, Tuple>> && ...); })(
        std::make_index_sequence<std::tuple_size_v<Tuple>>{});
};


template<class Derived>
struct row_predicate
{
    constexpr auto operator()(std::span<uint8_t const> digits) const noexcept -> std::tuple<bool, int64_t>
    {
        if(digits.size() < 2)
            return {false, 0};

        bool const all_digit_valid =
            std::ranges::all_of(digits, [&](uint8_t const& c) { return allowed_digits().test(c); });
        if(!all_digit_valid)
            return {false, 0};

        auto const x = std::ranges::fold_left(digits, int64_t{}, [](auto acc, auto c) { return 10 * acc + c; });

        return {self().check(x, digits), x};
    }

    constexpr auto allowed_digits() const noexcept -> std::bitset<10> { return 0b1111111110; }

protected:
    constexpr auto& self() const noexcept { return static_cast<Derived const&>(*this); }
};


struct is_perfect_square : row_predicate<is_perfect_square>
{
private:
    friend class row_predicate<is_perfect_square>;

    constexpr auto check(int64_t const x, std::span<uint8_t const>) const noexcept -> bool
    {
        auto const s = static_cast<int64_t>(std::sqrt(x));
        return s * s == x || (s + 1) * (s + 1) == x;
    }
};


struct is_odd_palindrome : row_predicate<is_odd_palindrome>
{
private:
    friend class row_predicate<is_odd_palindrome>;

    constexpr auto check(int64_t const, std::span<uint8_t const> digits) const noexcept -> bool
    {
        auto const mid         = digits.size() / 2;
        auto const first_half  = digits.subspan(0, mid);
        auto const second_half = digits.subspan(digits.size() - mid, mid);

        return (digits.back() & 1) && (digits.front() & 1) &&
               std::ranges::equal(first_half, second_half | std::views::reverse);
    }
};


struct fibonacci_sequence
{
    template<size_t SeqSize>
    static constexpr auto compute() noexcept -> std::array<int64_t, SeqSize>
    {
        std::array<int64_t, SeqSize> seq{};
        if constexpr(SeqSize > 0)
            seq[0] = 0;
        if constexpr(SeqSize > 1)
            seq[1] = 1;
        for(size_t i = 2; i < SeqSize; ++i)
            seq[i] = seq[i - 1] + seq[i - 2];
        return seq;
    }
};

struct is_fibonacci : row_predicate<is_fibonacci>
{
private:
    friend class row_predicate<is_fibonacci>;

    constexpr auto check(int64_t const x, std::span<uint8_t const>) const noexcept -> bool
    {
        // Precompute Fibonacci sequence array up to 93-th term.
        // Fibonacci(93) = 12200160415121876738, which is the largest Fibonacci number that fits in int64_t.
        static constexpr auto fibonacci_seq_ = fibonacci_sequence::compute<93>();
        return std::ranges::binary_search(fibonacci_seq_, x);
    }
};


struct is_prime : row_predicate<is_prime>
{
private:
    friend class row_predicate<is_prime>;

    constexpr auto check(int64_t const x, std::span<uint8_t const>) const noexcept -> bool
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

    // Modular multiplication using 128-bit arithmetic to avoid overflow (platform-dependent)
    INLINE constexpr auto mul_mod(uint64_t x, uint64_t y, uint64_t m) const noexcept -> uint64_t
    {
        return (static_cast<__uint128_t>(x) * y) % m;
    }

    constexpr auto pow_mod(uint64_t base, uint64_t exp, uint64_t mod) const noexcept -> uint64_t
    {
        uint64_t result = 1;
        uint64_t cur    = base % mod;
        while(exp > 0)
        {
            if(exp & 1)
                result = mul_mod(result, cur, mod);
            cur = mul_mod(cur, cur, mod);
            exp >>= 1;
        }
        return result;
    }

    // Miller-Rabin test for a single base
    constexpr auto miller_rabin_test(uint64_t n, uint64_t a) const noexcept -> bool
    {
        if(n % a == 0)
            return false;
        uint64_t d = n - 1;
        int      r = 0;
        while((d & 1) == 0)
        {
            d >>= 1;
            ++r;
        }
        uint64_t x = pow_mod(a, d, n);
        if(x == 1 || x == n - 1)
            return true;
        for(int i = 1; i < r; ++i)
        {
            x = mul_mod(x, x, n);
            if(x == n - 1)
                return true;
        }
        return false;
    }
};


template<int64_t N>
struct is_multiple_of : row_predicate<is_multiple_of<N>>
{
private:
    friend class row_predicate<is_multiple_of<N>>;

    constexpr auto check(int64_t const x, std::span<uint8_t const>) const noexcept -> bool { return x % N == 0; }
};


template<int64_t N>
struct product_of_digits_matches : row_predicate<product_of_digits_matches<N>>
{
    constexpr auto allowed_digits() const noexcept -> std::bitset<10>
    {
        int64_t m = N;

        std::array<int, 10> factor_count{};
        for(auto const f: {2, 3, 5, 7})
        {
            while(m % f == 0)
            {
                ++factor_count[f];
                m /= f;
            }
        }

        std::bitset<10> allowed_digits;
        allowed_digits[1] = true;
        allowed_digits[2] = factor_count[2] >= 1;
        allowed_digits[3] = factor_count[3] >= 1;
        allowed_digits[4] = factor_count[2] >= 2;
        allowed_digits[5] = factor_count[5] >= 1;
        allowed_digits[6] = factor_count[2] >= 1 && factor_count[3] >= 1;
        allowed_digits[7] = factor_count[7] >= 1;
        allowed_digits[8] = factor_count[2] >= 3;
        allowed_digits[9] = factor_count[3] >= 2;
        return allowed_digits;
    }

private:
    friend class row_predicate<product_of_digits_matches<N>>;

    constexpr auto check(int64_t const, std::span<uint8_t const> digits) const noexcept -> bool
    {
        return std::ranges::fold_left(digits, int64_t{1}, std::multiplies{}) == N;
    }
};

struct is_divisible_by_its_digits : row_predicate<is_divisible_by_its_digits>
{
private:
    friend class row_predicate<is_divisible_by_its_digits>;

    constexpr auto check(int64_t const x, std::span<uint8_t const> digits) const noexcept -> bool
    {
        return std::ranges::all_of(digits, [&](auto d) { return x % d == 0; });
    }
};


#endif // NUMBER_CROSS_PRED_H
