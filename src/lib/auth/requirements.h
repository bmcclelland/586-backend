#pragma once

#include "util/containers.h"
#include "perm.h"
#include "actor.h"

namespace mvc
{
    class Requirements
    {
        public:
            Requirements(Set<Perm>);
            bool met_by(Option<Actor> const&) const;

        private:
            Set<Perm> _reqs;
    };

    template <typename... Ts>
    Requirements make_reqs(Ts&&... ts)
    {
        return Requirements({ts...});
    }
}
