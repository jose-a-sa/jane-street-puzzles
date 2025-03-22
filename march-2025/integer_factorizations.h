#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>
#include <span>
#include <type_traits>
#include <vector>
#include "spdlog/spdlog.h"

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>


template<class T>
struct factor
{
    T      base;
    size_t count;
};

template<class T>
class factors_view : public std::span<factor<T>>
{
public:
    using std::span<factor<T>>::span;

    [[nodiscard]] size_t unique_factors() const noexcept { return this->size(); }

    [[nodiscard]] size_t total_factors() const noexcept
    {
        size_t total = 0;
        for(auto&& f: *this)
            total += f.count;
        return total;
    }
};

template<class T>
class factors_const_view : public std::span<factor<T> const>
{
public:
    using std::span<factor<T> const>::span;

    factors_const_view(factors_view<T> const& other) noexcept
        : std::span<factor<T> const>{other.data(), other.size()}
    {}

    [[nodiscard]] size_t unique_factors() const noexcept { return this->size(); }

    [[nodiscard]] size_t total_factors() const noexcept
    {
        size_t total = 0;
        for(auto&& f: *this)
            total += f.count;
        return total;
    }
};


template<class T>
class integer_factorizations
{
public:
    // class integer_factor<T>;
    class iterator;
    class const_iterator;

    using value_type             = factors_view<T>;
    using reference              = factors_view<T>;
    using const_reference        = factors_const_view<T>;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    class iterator
    {
        using factor_iterator = typename std::vector<factor<T>>::iterator;
        using head_iterator   = typename std::vector<size_t>::const_iterator;

    public:
        using value_type        = factors_view<T>;
        using reference         = factors_view<T>;
        using const_reference   = factors_const_view<T>;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        constexpr iterator(head_iterator head, factor_iterator factors_begin) noexcept
            : head_{head},
              factors_begin_{factors_begin}
        {}

        const_reference operator*() const noexcept
        {
            return {factors_begin_ + static_cast<difference_type>(*head_),
                    factors_begin_ + static_cast<difference_type>(*(head_ + 1))};
        }

        reference operator*() noexcept
        {
            return {factors_begin_ + static_cast<difference_type>(*head_),
                    factors_begin_ + static_cast<difference_type>(*(head_ + 1))};
        }
        const_reference operator[](difference_type n) const noexcept { return *(*this + n); }
        reference       operator[](difference_type n) noexcept { return *(*this + n); }

        // clang-format off
        constexpr iterator& operator++() noexcept { ++head_; return *this; }
        constexpr iterator  operator++(int) noexcept { iterator const copy = *this; ++(*this); return copy; }
        constexpr iterator& operator--() noexcept { --head_; return *this; }
        constexpr iterator  operator--(int) noexcept { iterator const copy = *this; --(*this); return copy; }

        constexpr iterator& operator+=(difference_type n) noexcept { head_ += n; return *this; }
        constexpr iterator  operator+(difference_type n) const noexcept { iterator copy = *this; return copy += n; }
        constexpr iterator& operator-=(difference_type n) noexcept { head_ -= n; return *this; }
        constexpr iterator  operator-(difference_type n) const noexcept { iterator copy = *this; return copy -= n; }
        // clang-format on

        constexpr difference_type operator-(const iterator& other) const noexcept { return head_ - other.head_; }

        constexpr bool operator==(const iterator& other) const noexcept { return head_ == other.head_; }
        constexpr bool operator!=(const iterator& other) const noexcept { return head_ != other.head_; }

    private:
        head_iterator   head_;
        factor_iterator factors_begin_;
    };

    class const_iterator
    {
        using factor_iterator = typename std::vector<factor<T>>::const_iterator;
        using head_iterator   = typename std::vector<size_t>::const_iterator;

    public:
        using value_type        = factors_const_view<T>;
        using reference         = factors_const_view<T>;
        using const_reference   = factors_const_view<T>;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        constexpr const_iterator(head_iterator head, factor_iterator factors_begin) noexcept
            : head_{head},
              factors_begin_{factors_begin}
        {}

        constexpr const_iterator(iterator const& other) noexcept
            : head_{other.head_},
              factors_begin_{other.factors_begin_}
        {}

        const_reference operator*() const noexcept
        {
            return {factors_begin_ + static_cast<difference_type>(*head_),
                    factors_begin_ + static_cast<difference_type>(*(head_ + 1))};
        }

        reference operator*() noexcept
        {
            return {factors_begin_ + static_cast<difference_type>(*head_),
                    factors_begin_ + static_cast<difference_type>(*(head_ + 1))};
        }
        const_reference operator[](difference_type n) const noexcept { return *(*this + n); }
        reference       operator[](difference_type n) noexcept { return *(*this + n); }

        // clang-format off
        constexpr const_iterator& operator++() noexcept { ++head_; return *this; }
        constexpr const_iterator  operator++(int) noexcept { const_iterator const copy = *this; ++(*this); return copy; }
        constexpr const_iterator& operator--() noexcept { --head_; return *this; }
        constexpr const_iterator  operator--(int) noexcept { const_iterator const copy = *this; --(*this); return copy; }

        constexpr const_iterator& operator+=(difference_type n) noexcept { head_ += n; return *this; }
        constexpr const_iterator  operator+(difference_type n) const noexcept { const_iterator copy = *this; return copy += n; }
        constexpr const_iterator& operator-=(difference_type n) noexcept { head_ -= n; return *this; }
        constexpr const_iterator  operator-(difference_type n) const noexcept { const_iterator copy = *this; return copy -= n; }
        // clang-format on

        constexpr difference_type operator-(const const_iterator& other) const noexcept { return head_ - other.head_; }

        constexpr bool operator==(const const_iterator& other) const noexcept { return head_ == other.head_; }
        constexpr bool operator!=(const const_iterator& other) const noexcept { return head_ != other.head_; }

    private:
        head_iterator   head_;
        factor_iterator factors_begin_;
    };


    constexpr explicit integer_factorizations(T n, T cutoff = std::numeric_limits<T>::max())
        : factors_{},
          heads_{}
    {
        if(n < 0)
            throw std::invalid_argument{"integer_factorizations: n must be positive"};
        find_factorizations(n, cutoff);
    }

    [[nodiscard]] constexpr iterator       begin() noexcept { return {heads_.begin(), factors_.begin()}; }
    [[nodiscard]] constexpr iterator       end() noexcept { return {heads_.end() - 1, factors_.begin()}; }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return {heads_.begin(), factors_.begin()}; }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return {heads_.end() - 1, factors_.begin()}; }

    [[nodiscard]] constexpr reverse_iterator       rbegin() noexcept { return reverse_iterator(end()); }
    [[nodiscard]] constexpr reverse_iterator       rend() noexcept { return reverse_iterator(begin()); }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    [[nodiscard]] constexpr size_type size() const noexcept { return heads_.size() - 1; }

    const_reference operator[](size_type i) const noexcept
    {
        return {factors_.begin() + static_cast<ptrdiff_t>(heads_[i]),
                factors_.begin() + static_cast<ptrdiff_t>(heads_[i + 1])};
    }

    reference operator[](size_type i) noexcept
    {
        return {factors_.begin() + static_cast<ptrdiff_t>(heads_[i]),
                factors_.begin() + static_cast<ptrdiff_t>(heads_[i + 1])};
    }

    void erase(const_iterator it) noexcept
    {
        auto const view = *it;
        auto const sz   = view.size();
        factors_.erase(view.begin(), view.end());
        auto const head_it = heads_.begin() + (it - begin());
        std::transform(head_it, heads_.end(), head_it, [&](auto& x) { return x - sz; });
        heads_.erase(head_it);
    }

    void erase(value_type x) noexcept
    {
        size_t const head    = x.begin() - factors_.begin();
        auto const   head_it = std::find(heads_.begin(), heads_.end(), head);
        if(head_it == heads_.end())
            return;
        erase(iterator{head_it, factors_.begin()});
    }

private:
    std::vector<factor<T>> factors_;
    std::vector<size_t>    heads_;

    constexpr void find_factorizations(T n, T cutoff)
    {
        if(n < 2)
        {
            factors_ = {factor<T>{1, 1}};
            heads_   = {0, 1};
            return;
        }

        factors_.clear();
        heads_.assign(1, 0);

        std::vector<T> current;
        current.reserve(static_cast<size_t>(std::sqrt(n)));

        backtrack(n, 2, cutoff, current);
    }

    constexpr void backtrack(T n, T start, T cutoff, std::vector<T>& current)
    {
        if(start > cutoff)
            return;

        if(n == 1)
        {
            factor<T> f{current.front(), 0};
            for(auto&& x: current)
            {
                if(x != f.base)
                {
                    factors_.push_back(f);
                    f.base  = x;
                    f.count = 0;
                }
                ++f.count;
            }
            factors_.push_back(f);
            heads_.push_back(factors_.size());
            return;
        }

        for(int d = start; d * d <= n; ++d)
        {
            if(n % d == 0)
            {
                current.push_back(d);
                backtrack(n / d, d, cutoff, current);
                current.pop_back();
            }
        }

        if(n >= start)
        {
            current.push_back(n);
            backtrack(1, n, cutoff, current);
            current.pop_back();
        }
    }
};


template<class T>
struct fmt::formatter<factor<T>>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<class FormatContext>
    auto format(factor<T> const& f, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "{}^{}", f.base, f.count);
    }
};
