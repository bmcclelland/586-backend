#include "requirements.h"

namespace mvc
{
    Requirements::Requirements(Set<Perm> reqs)
        : _reqs(std::move(reqs))
    {}

    bool Requirements::met_by(Option<Actor> const& actor) const
    {
        if (!actor)
        {
            // Requires nothing.
            return _reqs.empty();
        }
        else
        {
            for (Perm const& req : _reqs)
            {
                if (!actor->perms().count(req))
                {
                    // Lacking a required permission.
                    return false;
                }
            }

            // Has all requires permissions.
            return true;
        }
    }
}
