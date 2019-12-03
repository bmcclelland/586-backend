#pragma once

#include "util/string.h"
#include "util/containers.h"
#include "perm.h"

namespace mvc
{
    class Actor
    {
        public:
            Actor(String, Set<Perm>);
            String const& unique_id() const;
            Set<Perm> const& perms() const;

        private:
            String _unique_id;
            Set<Perm> _perms;
    };
}
