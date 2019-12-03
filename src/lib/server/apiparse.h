#pragma once

#include "server/apipath.h"
#include "di.h"
#include "util/void.h"
#include "util/variant.h"
#include "util/containers.h"
#include "util/string.h"
#include <tuple>
#include <functional>

namespace mvc::api_parsers::detail 
{
    class State
    {
        public:
            State(ApiPath const&);
        
            bool at_end() const;
            ApiSegment const& segment() const;
            State next() const;

        private:
            State(ApiPath::Iter, ApiPath::Iter);
            
            ApiPath::Iter _iter;
            ApiPath::Iter _end;
    };

    template <typename... Ts>
    struct Result
    {
        Result(State ns, std::tuple<Ts...> r)
            : next_state(ns)
            , result(r)
        {}
        State next_state;
        std::tuple<Ts...> result;
    };

    template <typename... Ts>
    Result(State, std::tuple<Ts...>) -> Result<Ts...>;

    template <typename T>
    struct Flatten;

    template <typename... Ts>
    struct Flatten<std::tuple<Ts...>>
    {
        using type = std::tuple<Ts...>;
    };

    template <typename T>
    struct Flatten<std::tuple<T>>
    {
        using type = T;
    };

    template <typename... Ts>
    std::tuple<Ts...> flatten(std::tuple<Ts...> const& t)
    {
        return t;
    }

    template <typename T>
    T flatten(std::tuple<T> const& t)
    {
        return std::get<0>(t);
    }

    template <typename...>
    struct All;

    template <>
    struct All<>
    {
        template <typename... Xs>
        static 
        Option<Result<Xs...>> parse(
            std::tuple<Xs...> const& cur_tuple, 
            State cur_state
            )
        {
            return Result<Xs...>(
                cur_state, 
                cur_tuple
                );
        }
    };

    template <typename T, typename... Ts>
    struct All<T, Ts...>
    {
        template <typename... Xs>
        static 
        auto parse(std::tuple<Xs...> const& cur_tuple, State cur_state)
            -> decltype(All<Ts...>::parse(
                    std::tuple_cat(cur_tuple, T::parse(cur_state)->result),
                    T::parse(cur_state)->next_state
                    )
                )
        {
            auto r = T::parse(cur_state);

            if (r)
            {
                return All<Ts...>::parse(
                    std::tuple_cat(cur_tuple, r->result),
                    r->next_state
                    );
            }
            else
            {
                return nullopt;
            }
        }
    };

    template <typename R, typename...>
    struct Any;

    template <typename R>
    struct Any<R>
    {
        static 
        Option<Result<R>> parse(State)
        {
            return nullopt;
        }
    };

    template <typename R, typename T, typename... Ts>
    struct Any<R, T, Ts...>
    {
        static 
        Option<Result<R>> parse(State cur_state)
        {
            auto r = T::parse(cur_state);

            if (r)
            {
                return Result<R>(
                    r->next_state,
                    flatten(r->result)
                    );
            }
            else
            {
                return Any<R,Ts...>::parse(cur_state);
            }
        }
    };

    template <typename R, typename...>
    struct Dyn;

    template <typename R>
    struct Dyn<R>
    {
        static 
        Option<Result<Shared<R>>> parse(State)
        {
            return nullopt;
        }
    };

    template <typename R, typename T, typename... Ts>
    struct Dyn<R, T, Ts...>
    {
        static 
        Option<Result<Shared<R>>> parse(State cur_state)
        {
            auto r = T::parse(cur_state);

            if (r)
            {
                return Result<Shared<R>>(
                    r->next_state,
                    flatten(r->result)
                    );
            }
            else
            {
                return Dyn<R,Ts...>::parse(cur_state);
            }
        }
    };

    template <char... Cs> 
    struct Lit
    {
        static 
        constexpr char const k[sizeof...(Cs) + 1] = { Cs..., '\0' };
        
        static 
        Option<detail::Result<>> parse(detail::State cur_state)
        {
            if (cur_state.at_end())
            {
                return nullopt;
            }
            else if (cur_state.segment().val() == k)
            {
                return detail::Result<>(
                    cur_state.next(), 
                    std::tuple<>()
                    );
            }
            else
            {
                return nullopt;
            }
        }
    };
} // end mvc::api_parsers::detail

namespace mvc::api_parsers 
{
    struct Int 
    {
        static 
        Option<detail::Result<int>> parse(detail::State cur_state)
        {
            if (cur_state.at_end())
            {
                return nullopt;
            }
            else
            {
                try
                {
                    return detail::Result<int>(
                        cur_state.next(), 
                        std::make_tuple(
                            std::stoi(cur_state.segment())
                            )
                        );
                }
                catch (std::invalid_argument const&)
                {
                    return nullopt;
                }
                catch (std::out_of_range const&)
                {
                    return nullopt;
                }
            }
        }
    };

    struct String
    {
        static 
        Option<detail::Result<mvc::String>> parse(detail::State cur_state)
        {
            if (cur_state.at_end())
            {
                return nullopt;
            }
            else
            {
                return detail::Result<mvc::String>(
                    cur_state.next(),
                    std::make_tuple(cur_state.segment().val())
                    );
            }
        }
    };

    template <typename T, T... Cs>
    detail::Lit<Cs...> operator""_Lit();

#define Lit(s) decltype(s##_Lit)

    template <typename... Ts>
    struct Any
    {
        using R = Variant<
            typename detail::Flatten<
                decltype(
                    Ts::parse(
                        std::declval<detail::State>()
                        )->result
                    )
                >::type
                ...
            >;

        static 
        Option<detail::Result<R>> parse(detail::State cur_state)
        {
            return detail::Any<R, Ts...>::parse(cur_state);
        }
    };

    template <typename R, typename... Ts>
    struct Dyn
    {
        static 
        Option<detail::Result<Shared<R>>> parse(detail::State cur_state)
        {
            return detail::Dyn<R, Ts...>::parse(cur_state);
        }
    };

    template <typename... Ts>
    struct All
    {
        static 
        auto parse(detail::State cur_state)
        {
            return detail::All<Ts...>::parse(std::tuple<>(), cur_state);
        }
    };

    template <typename T, typename... Ts>
    struct Cons
    {
        static 
        Option<detail::Result<T>> parse(detail::State cur_state)
        {
            auto r = detail::All<Ts...>::parse(std::tuple<>(), cur_state);

            if (r)
            {
                return detail::Result<T>(
                    r->next_state,
                    std::make_tuple(
                        std::make_from_tuple<T>(r->result)
                        )
                    );
            }
            else
            {
                return nullopt;
            }
        }
    };

    template <typename T, typename... Ts>
    struct New
    {
        static 
        Option<detail::Result<Shared<T>>> parse(detail::State cur_state)
        {
            auto r = detail::All<Ts...>::parse(std::tuple<>(), cur_state);

            if (r)
            {
                return detail::Result<Shared<T>>(
                    r->next_state,
                    std::make_tuple(
                        std::apply(std::make_unique<T>, r->result)
                        )
                    );
            }
            else
            {
                return nullopt;
            }
        }
    };

    template <typename T, typename... Ts>
    struct Endpoint
    {
        using R = detail::Result<Shared<T>>;
        
        static 
        Option<R> parse(detail::State cur_state)
        {
            auto r = detail::All<Ts...>::parse(std::tuple<>(), cur_state);

            if (r)
            {
                auto const path_args = std::make_from_tuple<typename T::PathArgs>(r->result);

                auto injector = di::make_injector(
                    DI::make_injector(),
                    di::bind<typename T::PathArgs>.to(path_args)
                );  

                return R(
                    r->next_state,
                    std::make_tuple(
                        Shared<T>(injector.template create<Unique<T>>())
                        )
                    );
            }
            else
            {
                return nullopt;
            }
        }
    };

    template <typename... Ts>
    struct Root
    {
        using Result = decltype(
            detail::flatten(
                detail::All<Ts...>::parse(
                    std::tuple<>(), 
                    std::declval<detail::State>()
                    )->result
                )
            );

        static 
        Option<Result> parse(ApiPath const& path) 
        { 
            detail::State init_state(path);

            auto r = detail::All<Ts...>::parse(std::tuple<>(), init_state);

            if (r && r->next_state.at_end())
            {
                return detail::flatten(r->result);
            }
            else
            {
                return nullopt;
            }
        }
    };
} // end mvc::api_parsers
