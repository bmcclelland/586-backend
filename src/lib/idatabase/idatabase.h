#pragma once

#include "data/data.h"
#include <functional>

namespace mvc
{
    class IDatabase
    {
        class TxGuard
        {
            public:
                TxGuard(bool& flag) 
                    : _flag(flag) 
                {
                    if (_flag)
                    {
                        throw std::logic_error("TxGuard(&true)");
                    }

                    _flag = true;
                }

                ~TxGuard() 
                { 
                    _flag = false;
                }

            private:
                bool& _flag;
        };
        
        public:
            virtual ~IDatabase() = default;
            
            template <typename T>
            auto persist(Shared<T>& t)
            {
                using traits = odb::object_traits<std::decay_t<T>>;
                using id_type = typename traits::id_type;

                if constexpr(traits::auto_id)
                {
                    return id_type{ 
                        get().persist(t)
                    };
                }
                else
                {
                    try
                    {
                        return Option<id_type>{
                            get().persist(t)
                        };
                    }
                    catch (odb::object_already_persistent&)
                    {
                        return Option<id_type>{};
                    }
                }
            }
            
            template <typename T>
            auto persist(T&& t)
            {
                using traits = odb::object_traits<std::decay_t<T>>;
                using id_type = typename traits::id_type;

                if constexpr(traits::auto_id)
                {
                    return id_type{ 
                        get().persist(std::forward<T>(t))
                    };
                }
                else
                {
                    try
                    {
                        return Option<id_type>{
                            get().persist(std::forward<T>(t))
                        };
                    }
                    catch (odb::object_already_persistent&)
                    {
                        return Option<id_type>{};
                    }
                }
            }
            
            template <typename T>
            auto update(T&& t)
            {
                return get().update(std::forward<T>(t));
            }
            
            template <typename T>
            odb::result<T> query()
            {
                return get().query<T>();
            }
            
            template <typename T>
            odb::result<T> query(odb::query<T> const& q)
            {
                return get().query(q);
            }

            void transact(std::function<void()> const& f)
            {
                if (!_in_transaction)
                {
                    auto [guard, tx] = begin_transaction();
                    f();
                    tx.commit();
                }
                else
                {
                    f();
                }
            };
            
            template <typename R>
            R transact(std::function<R()> const& f)
            {
                if (!_in_transaction)
                {
                    auto [guard, tx] = begin_transaction();
                    R r = f();
                    tx.commit();
                    return r;
                }
                else
                {
                    return f();
                }
            }
            
            template <typename T>
            Option<T> find_value(int id)
            {
                return transact<Option<T>>([&]() -> Option<T>
                {
                    if (auto p = get().find<T>(id))
                    {
                        return *p;
                    }
                    else
                    {
                        return nullopt;
                    }
                });
            }
            
            template <typename ID>
            auto find_value(ID const& id)
                -> Option<typename ID::Type>
            {
                using T = typename ID::Type;

                return transact<Option<T>>([&]() -> Option<T>
                {
                    if (auto p = get().find<T>(id.val))
                    {
                        return *p;
                    }
                    else
                    {
                        return nullopt;
                    }
                });
            }
            
            template <typename T>
            Shared<T> find_shared(int id)
            {
                return transact<Shared<T>>([&]() -> Shared<T>
                {
                    return get().find<T>(id);
                });
            }
            
            template <typename ID>
            auto find_shared(ID const& id)
                -> Shared<typename ID::Type>
            {
                using T = typename ID::Type;

                return transact<Shared<T>>([&]() -> Shared<T>
                {
                    return get().find<T>(id.val);
                });
            }

            template <typename T>
            Vec<T> collect()
            {
                Vec<T> result;
               
                transact([&]
                {
                    for (T const& t : query<T>())
                    {
                        result.push_back(t);
                    }
                });
                
                return result;
            }

            template <typename T>
            Vec<T> collect(odb::query<T> const& q)
            {
                Vec<T> result;

                transact([&]
                {
                    for (T const& t : query(q))
                    {
                        result.push_back(t);
                    }
                });
                
                return result;
            }
        
        protected:
            virtual odb::database& get() = 0;
            
        private:
            struct Tx
            {
                TxGuard guard;
                odb::transaction tx;
            };

            [[nodiscard]]
            Tx begin_transaction()
            {
                return {
                    TxGuard(_in_transaction),
                    odb::transaction(get().begin())
                };
            }

            bool _in_transaction = false;
    };
}
