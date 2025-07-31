#ifndef BASE_H
#define BASE_H

#include <array>
#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#if defined(INLINE)
// do nothing
#elif defined(__GNUC__) || defined(__clang__)
#define INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define INLINE __forceinline
#else
#define INLINE inline
#endif


namespace qs
{
    /**
     * @brief Constexpr version of std::tolower with the same precomputed table of characters as GCC implementation.
     * @param c
     * @return char
     */
    INLINE constexpr auto tolower(char c) noexcept -> char
    {
        static constexpr auto to_lower_map_ = []()
        {
            constexpr int kCharLo = std::numeric_limits<char>::lowest();
            constexpr int kCharHi = std::numeric_limits<char>::max();

            std::array<char, kCharHi - kCharLo + 1> table{};
            for(int ch = kCharLo; ch <= kCharHi; ++ch)
            {
                table[static_cast<unsigned char>(ch)] =
                    (ch >= 'A' && ch <= 'Z') ? static_cast<char>(ch + ('a' - 'A')) : ch;
            }
            return table;
        }();

        return to_lower_map_[static_cast<unsigned char>(c)];
    }

    template<std::size_t First, std::size_t Last, class Tuple>
        requires(First <= Last && Last <= std::tuple_size_v<std::decay_t<Tuple>>)
    constexpr auto tuple_slice(Tuple&& t)
    {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>)
        {
            return std::make_tuple(std::get<First + Is>(std::forward<Tuple>(t))...);
        }(std::make_index_sequence<Last - First>{});
    }

} // namespace qs

#endif // BASE_H
