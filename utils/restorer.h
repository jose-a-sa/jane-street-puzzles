#ifndef RESTORER_H
#define RESTORER_H

#include <array>
#include <cstddef>
#include <memory>
#include <tuple>
#include <type_traits>

namespace qs
{
    struct defer_restore_t
    {};
    inline constexpr defer_restore_t defer_restore{};

    template<class T>
    class restorer_bind;

    template<class... Ts>
    class restorer;

    namespace details
    {
        template<class T>
        struct restorer_inplace_binder
        {
            using value_type = std::remove_reference_t<T>;

            constexpr restorer_inplace_binder(T& ref)
                : value_{ref},
                  ref_{ref} {};

            constexpr void restore() { ref_ = value_; }

        private:
            value_type  value_;
            value_type& ref_;
        };

        template<class T>
        struct restorer_external_binder
        {
            using value_type = std::remove_reference_t<T>;

            constexpr restorer_external_binder(restorer_bind<T> bind)
                : ref_{bind.ref_},
                  external_{bind.external_}
            {
                external_ = ref_;
            }

            constexpr void restore() { ref_ = external_; }

        private:
            value_type& ref_;
            value_type& external_;
        };


        template<class T, class = void>
        struct is_restorer_bind : std::false_type
        {};

        template<class T>
        struct is_restorer_bind<T, std::void_t<typename T::value_type>>
            : std::is_same<std::decay_t<T>, restorer_bind<typename T::value_type>>
        {};

        template<class T>
        static constexpr bool is_restorer_bind_v = is_restorer_bind<T>::value;

        template<class Arg, class = void>
        struct restored_binder_selector : std::type_identity<restorer_inplace_binder<std::remove_reference_t<Arg>>>
        {};

        template<class Arg>
        struct restored_binder_selector<Arg, std::void_t<typename std::remove_reference_t<Arg>::value_type>>
            : std::conditional<is_restorer_bind_v<std::remove_reference_t<Arg>>,
                               restorer_external_binder<typename std::remove_reference_t<Arg>::value_type>,
                               restorer_inplace_binder<std::remove_reference_t<Arg>>>
        {};

        template<class Arg>
        using restored_binder_selector_t = restored_binder_selector<Arg>::type;
    } // namespace details


    template<class T>
    class restorer_bind
    {
    public:
        using value_type = std::remove_reference_t<T>;

        constexpr restorer_bind(T& ref, T& external)
            : ref_{ref},
              external_{external}
        {}

    private:
        value_type& ref_;
        value_type& external_;

        template<class U>
        friend class details::restorer_external_binder;
    };

    template<class... Ts>
    class restorer
    {
    public:
        using bindings_tuple = std::tuple<details::restored_binder_selector_t<Ts>...>;

        template<class... Args>
        constexpr restorer(Args&&... args) noexcept
            : bindings_(bind_(std::forward<Args>(args))...),
              completed_{true}
        {}

        template<class... Args>
        constexpr restorer(defer_restore_t, Args&&... args) noexcept
            : bindings_(bind_(std::forward<Args>(args))...),
              completed_{false}
        {}

        restorer(restorer const&)            = delete;
        restorer& operator=(restorer const&) = delete;

        restorer(restorer&&)            = default;
        restorer& operator=(restorer&&) = default;

        constexpr ~restorer() noexcept
        {
            if(completed_)
                restore();
        }

        constexpr auto& complete() noexcept
        {
            completed_ = true;
            return *this;
        }

        constexpr void restore() noexcept
        {
            std::apply([](auto&... b) { (b.restore(), ...); }, bindings_);
        }

    private:
        bindings_tuple bindings_;
        bool           completed_;

        template<class Arg>
        static constexpr auto bind_(Arg&& arg) -> decltype(auto)
        {
            if constexpr(details::is_restorer_bind_v<std::remove_reference_t<Arg>>)
                return details::restorer_external_binder(std::forward<Arg>(arg));
            else
                return details::restorer_inplace_binder(std::forward<Arg>(arg));
        }
    };

    template<class... Ts>
    restorer(Ts&&...) -> restorer<std::remove_reference_t<Ts>...>;

    template<class... Ts>
    restorer(defer_restore_t, Ts&&...) -> restorer<std::remove_reference_t<Ts>...>;


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
        {
            static_assert(sizeof...(refs) <= N, "Too many references for restorer_array");
        }

        template<class... Refs>
        constexpr restorer_array(defer_restore_t, Refs&... refs) noexcept
            : size_{sizeof...(refs)},
              values_{{refs...}},
              ptrs_{{std::addressof(refs)...}},
              completed_{false}
        {
            static_assert(sizeof...(refs) <= N, "Too many references for restorer_array");
        }

        restorer_array(restorer_array const&)            = delete;
        restorer_array& operator=(restorer_array const&) = delete;

        constexpr ~restorer_array() noexcept
        {
            if(completed_)
                restore();
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

        constexpr bool try_push_back(reference ref) noexcept
        {
            if(size_ >= N)
                return false;

            this->unchecked_push_back(ref);
            return true;
        }

        constexpr void unchecked_push_back(reference ref) noexcept
        {
            ptrs_[size_]   = std::addressof(ref);
            values_[size_] = ref;
            ++size_;
        }

        constexpr void reset(bool defer_restore = false) noexcept
        {
            size_      = 0;
            completed_ = !defer_restore;
        }

    private:
        size_t                    size_ = 0;
        std::array<value_type, N> values_;
        std::array<pointer, N>    ptrs_;
        bool                      completed_ = true;
    };

    template<class... Ts>
    restorer_array(Ts&...) -> restorer_array<std::tuple_element_t<0, std::tuple<Ts...>>, sizeof...(Ts)>;

    template<class... Ts>
    restorer_array(defer_restore_t, Ts&...)
        -> restorer_array<std::tuple_element_t<0, std::tuple<Ts...>>, sizeof...(Ts)>;

} // namespace qs

#endif // RESTORER_H
