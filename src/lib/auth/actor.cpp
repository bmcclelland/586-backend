#include "actor.h"

namespace mvc
{
    Actor::Actor(String unique_id, Set<Perm> perms)
        : _unique_id(std::move(unique_id))
        , _perms(std::move(perms))
    {}

    String const& Actor::unique_id() const
    {
        return _unique_id;
    }

    Set<Perm> const& Actor::perms() const
    {
        return _perms;
    }
}
