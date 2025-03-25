#ifndef INTEGER_FACTORIZATIONS_H
#define INTEGER_FACTORIZATIONS_H

#include <span>
#include <vector>

#include <fmt/core.h>
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
class integer_factorizations
{

public:
    using value_type             = factors_view<T>;
    using reference              = value_type&;
    using const_reference        = value_type const&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = std::vector<value_type>::iterator;
    using const_iterator         = std::vector<value_type>::const_iterator;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr explicit integer_factorizations(T n, T cutoff = std::numeric_limits<T>::max())
        : factors_{},
          factorizations_{}
    {
        if(n < 0)
            throw std::invalid_argument{"integer_factorizations: n must be positive"};
        find_factorizations(n, cutoff);
    }

    [[nodiscard]] constexpr iterator       begin() noexcept { return factorizations_.begin(); }
    [[nodiscard]] constexpr iterator       end() noexcept { return factorizations_.end(); }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return factorizations_.begin(); }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return factorizations_.end(); }

    [[nodiscard]] constexpr reverse_iterator       rbegin() noexcept { return reverse_iterator(end()); }
    [[nodiscard]] constexpr reverse_iterator       rend() noexcept { return reverse_iterator(begin()); }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    [[nodiscard]] constexpr size_type size() const noexcept { return factorizations_.size(); }

    reference       operator[](size_type i) noexcept { return factorizations_[i]; }
    const_reference operator[](size_type i) const noexcept { return factorizations_[i]; }

private:
    std::vector<factor<T>>       factors_;
    std::vector<factors_view<T>> factorizations_;

    constexpr void find_factorizations(T n, T cutoff)
    {
        if(n < 2)
        {
            factors_        = {factor<T>{1, 1}};
            factorizations_ = {factors_view<T>{factors_.data(), factors_.size()}};
            return;
        }

        factors_.clear();
        factorizations_.clear();

        auto const trial_n = static_cast<size_t>(std::sqrt(n));

        std::vector<size_t> heads;
        heads.reserve(trial_n);
        heads.assign(1, 0);

        std::vector<T> current;
        current.reserve(trial_n);

        backtrack(n, 2, cutoff, current, heads);

        for(auto i = 1; i < heads.size(); ++i)
            factorizations_.emplace_back(factors_.data() + heads[i - 1], heads[i] - heads[i - 1]);
    }

    constexpr void backtrack(T n, T start, T cutoff, std::vector<T>& current, std::vector<size_t>& heads)
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
            heads.push_back(factors_.size());
            return;
        }

        for(int d = start; d * d <= n; ++d)
        {
            if(n % d == 0)
            {
                current.push_back(d);
                backtrack(n / d, d, cutoff, current, heads);
                current.pop_back();
            }
        }

        if(n >= start)
        {
            current.push_back(n);
            backtrack(1, n, cutoff, current, heads);
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


#endif // INTEGER_FACTORIZATIONS_H 
