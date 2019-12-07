#pragma once

namespace mvc
{
    template <typename T>
    struct SingleIter
    {
        T* t = nullptr;
        
        SingleIter() = default;
        SingleIter(T* t_) : t(t_) {}

        T& operator*() { return *t; }
        T* operator->() { return t; }
        T const& operator*() const { return *t; }
        T const* operator->() const { return t; }
        SingleIter& operator++() { t = nullptr; return *this; }
        bool operator==(SingleIter o) const { return t == o.t; }
        bool operator!=(SingleIter o) const { return t != o.t; }
    };

    template <typename T>
    struct Single
    {
        T t;
        SingleIter<T> begin() { return SingleIter<T>(&t); }
        SingleIter<T> end()   { return SingleIter<T>(); }
    };
}
