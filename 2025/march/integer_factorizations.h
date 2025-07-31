#ifndef INTEGER_FACTORIZATIONS_H
#define INTEGER_FACTORIZATIONS_H

#include <ranges>
#include <span>
#include <vector>

#include <fmt/core.h>
#include <fmt/ranges.h>


class integer_factorizations
{
public:
    using num_type = uint32_t;
    struct factor
    {
        num_type base;
        uint32_t count;
    };

    using value_type             = std::span<factor>;
    using reference              = value_type&;
    using const_reference        = value_type const&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = std::vector<value_type>::iterator;
    using const_iterator         = std::vector<value_type>::const_iterator;
    using reverse_iterator       = std::vector<value_type>::reverse_iterator;
    using const_reverse_iterator = std::vector<value_type>::const_reverse_iterator;

    constexpr explicit integer_factorizations(num_type n, num_type cutoff = std::numeric_limits<num_type>::max())
        : factors_{},
          factorizations_{},
          number_{n}
    {
        if(n < 0)
            throw std::invalid_argument{"integer_factorizations: n must be positive"};
        compute_factorizations_(n, cutoff);
    }

    constexpr num_type number() const noexcept { return number_; }

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
    std::vector<factor>            factors_;
    std::vector<std::span<factor>> factorizations_;
    num_type                       number_;

    constexpr void compute_factorizations_(num_type n, num_type cutoff)
    {
        if(n < 2)
        {
            factors_        = {factor{1, 1}};
            factorizations_ = {std::span<factor>{factors_}};
            return;
        }

        factors_.clear();
        factorizations_.clear();

        auto const trial_n = static_cast<size_t>(std::sqrt(n)) + 1;

        std::vector<size_t> ends;
        ends.reserve(trial_n);
        ends.assign(1, 0);

        std::vector<num_type> curr_factors;
        curr_factors.reserve(trial_n);

        find_next_(n, 2, cutoff, curr_factors, ends);

        for(auto i = 1; i < ends.size(); ++i)
            factorizations_.emplace_back(factors_.data() + ends[i - 1], ends[i] - ends[i - 1]);
    }

    constexpr void find_next_(num_type n, num_type start_factor, num_type cutoff_factor,
                              std::vector<num_type>& curr_factors, std::vector<size_t>& ends)
    {
        if(start_factor > cutoff_factor)
            return;

        if(n < 2)
        {
            factor f{curr_factors.front(), 0};
            for(auto&& x: curr_factors)
            {
                if(x != f.base)
                {
                    factors_.push_back(f);
                    f = factor{x, 0};
                }
                ++f.count;
            }
            factors_.push_back(f);
            ends.push_back(factors_.size());
            return;
        }

        auto divisors = std::views::iota(start_factor, n + 1) |
                        std::views::filter([&](auto d) { return d * d <= n && n % d == 0; });
        for(int d: divisors)
        {
            curr_factors.push_back(d);
            find_next_(n / d, d, cutoff_factor, curr_factors, ends);
            curr_factors.pop_back();
        }

        if(n >= start_factor)
        {
            curr_factors.push_back(n);
            find_next_(1, n, cutoff_factor, curr_factors, ends);
            curr_factors.pop_back();
        }
    }
};


template<>
struct fmt::formatter<integer_factorizations::factor>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<class FormatContext>
    auto format(integer_factorizations::factor const& f, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}^{}", f.base, f.count);
    }
};


#endif // INTEGER_FACTORIZATIONS_H
