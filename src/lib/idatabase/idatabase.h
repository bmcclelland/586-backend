#pragma once

#include "data/data.h"
#include "util/single.h"
#include <functional>

namespace mvc
{
    class MyTx : public odb::transaction
    {
        public:
            MyTx(MyTx&&) = default;
    };
    
    struct Transaction
    {
        MyTx tx;
        odb::database& db;

        template <typename T>
        auto persist(Shared<T>& t)
        {
            using traits = odb::object_traits<std::decay_t<T>>;
            using id_type = typename traits::id_type;

            if constexpr(traits::auto_id)
            {
                return id_type{db.persist(t)};
            }
            else
            {
                try
                {
                    return Option<id_type>{db.persist(t)};
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
                return id_type{db.persist(std::forward<T>(t))};
            }
            else
            {
                try
                {
                    return Option<id_type>{db.persist(std::forward<T>(t))};
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
            return db.update(std::forward<T>(t));
        }

        template <typename ID>
        void erase(ID const& id)
        {
            using T = typename ID::Type;
            db.erase<T>(id.val);
        }
        
        template <typename T, typename Q>
        void erase_query(Q&& q)
        {
            db.erase_query<T>(std::forward<Q>(q));
        }
        
        template <typename T>
        odb::result<T> query() const
        {
            return db.query<T>();
        }
        
        template <typename T>
        odb::result<T> query(odb::query<T> const& q) const
        {
            return db.query(q);
        }
        
        template <typename T>
        Option<T> find_value(int id) const
        {
            if (auto p = db.find<T>(id))
            {
                return *p;
            }
            else
            {
                return nullopt;
            }
        }
        
        template <typename ID>
        auto find_value(ID const& id) const
            -> Option<typename ID::Type>
        {
            using T = typename ID::Type;

            if (auto p = db.find<T>(id.val))
            {
                return *p;
            }
            else
            {
                return nullopt;
            }
        }
        
        template <typename T>
        Shared<T> find_shared(int id) const
        {
            return db.find<T>(id);
        }
        
        template <typename ID>
        auto find_shared(ID const& id) const
            -> Shared<typename ID::Type>
        {
            using T = typename ID::Type;

            return db.find<T>(id.val);
        }

        template <typename T>
        Vec<T> collect() const
        {
            Vec<T> result;
           
            for (T const& t : query<T>())
            {
                result.push_back(t);
            }
            
            return result;
        }

        template <typename T>
        Vec<T> collect(odb::query<T> const& q) const
        {
            Vec<T> result;

            for (T const& t : query(q))
            {
                result.push_back(t);
            }
            
            return result;
        }

        void commit()
        {
            tx.commit();
        }
    };

    class IDatabase
    {
        public:
            virtual ~IDatabase() = default;
            
        protected:
            virtual odb::database& get() = 0;
            
        public:
            [[nodiscard]]
            Transaction transaction()
            {
                return Transaction{
                    odb::transaction(get().begin()),
                    get()
                };
            }
//            Single<Transaction> transaction()
//            {
//                return Single<Transaction>{Transaction{
//                    odb::transaction(get().begin()),
//                    get()
//                }};
//            }
    };
}
