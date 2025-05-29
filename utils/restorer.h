#ifndef RESTORER_H
#define RESTORER_H

#include <array>
#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <memory>
#include <tuple>
#include <type_traits>


struct wait_restore_t
{};
inline constexpr wait_restore_t wait_restore{};

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

    constexpr restorer(wait_restore_t, Ts&... refs) noexcept
        : refs_(refs...),
          values_(refs...),
          completed_{false}
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

template<class T, size_t N>
class restorer_array
{
public:
    using value_type = std::remove_reference_t<T>;
    using reference  = value_type&;
    using pointer    = value_type*;

    constexpr restorer_array() noexcept = default;

    template<class... Refs>
    constexpr restorer_array(Refs&... refs) noexcept
        : size_{sizeof...(refs)},
          values_{{refs...}},
          ptrs_{{std::addressof(refs)...}},
          completed_{true}
    {}

    template<class... Refs>
    constexpr restorer_array(wait_restore_t, Refs&... refs) noexcept
        : size_{sizeof...(refs)},
          values_{{refs...}},
          ptrs_{{std::addressof(refs)...}},
          completed_{false}
    {}

    restorer_array(restorer_array const&)            = delete;
    restorer_array& operator=(restorer_array const&) = delete;

    constexpr ~restorer_array() noexcept
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
    constexpr void restore() const noexcept
    {
        for(size_t i = 0; i < size_; ++i)
            *ptrs_[i] = values_[i];
    }

    constexpr bool try_track(reference ref) noexcept
    {
        if(size_ >= N)
            return false;

        this->unchecked_track(ref);
        return true;
    }

    constexpr void unchecked_track(reference ref) noexcept
    {
        ptrs_[size_]   = std::addressof(ref);
        values_[size_] = ref;
        ++size_;
    }

    template<size_t... Ns>
    friend constexpr auto restorer_combine(restorer_array<T, Ns>&&... restorers) noexcept;

private:
    size_t                    size_ = 0;
    std::array<value_type, N> values_;
    std::array<pointer, N>    ptrs_;
    bool                      completed_ = true;
};

template<class... Ts>
restorer_array(Ts&...) -> restorer_array<std::tuple_element_t<0, std::tuple<Ts...>>, sizeof...(Ts)>;

template<class... Ts>
restorer_array(wait_restore_t, Ts&...) -> restorer_array<std::tuple_element_t<0, std::tuple<Ts...>>, sizeof...(Ts)>;


// int a ,b, c;
// auto rx = restorer_array(a, b, c);


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
