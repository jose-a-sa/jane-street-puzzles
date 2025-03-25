#ifndef RESTORER_H
#define RESTORER_H

#include <initializer_list>
#include <tuple>
#include <type_traits>

template<class... Ts>
class restorer
{
public:
    using value_type = std::tuple<Ts...>;
    using reference  = std::tuple<Ts&...>;

    constexpr restorer(Ts&... refs) noexcept
        : refs_(refs...),
          values_(refs...),
          completed_{true}
    {}

    restorer(restorer const&)            = delete;
    restorer& operator=(restorer const&) = delete;

    constexpr ~restorer() noexcept
    {
        if(completed_)
            restore();
    }

    constexpr auto& wait() noexcept
    {
        completed_ = false;
        return *this;
    }
    constexpr auto& complete() noexcept
    {
        completed_ = true;
        return *this;
    }
    constexpr void restore() noexcept { refs_ = values_; }

    template<class... Restorters>
    friend constexpr auto restorer_combine(Restorters&&... restorers) noexcept;

private:
    reference  refs_;
    value_type values_;
    bool       completed_;
};


template<class R>
struct is_restorer : std::false_type
{};

template<class... Ts>
struct is_restorer<restorer<Ts...>> : std::true_type
{};

template<class R>
inline constexpr bool is_restorer_v = is_restorer<R>::value;


template<class... Restorters> // require all to be restorer and rvalue reference
    requires(
        std::conjunction_v<is_restorer<std::remove_reference_t<Restorters>>, std::is_rvalue_reference<Restorters>> &&
        ...)
constexpr auto restorer_combine(Restorters&&... restorers) noexcept
{
    bool const all_completed = (std::forward<Restorters>(restorers).completed_ && ...);

    using swallow = std::initializer_list<int>;
    (void)swallow{(std::forward<Restorters>(restorers).wait(), 0)...};

    auto res       = std::apply([](auto&... r) { return restorer(r...); },
                          std::tuple_cat((std::forward<Restorters>(restorers).refs_)...));
    res.values_    = std::tuple_cat(std::forward<Restorters>(restorers).values_...);
    res.completed_ = all_completed;
    return res;
}

#endif // RESTORER_H
