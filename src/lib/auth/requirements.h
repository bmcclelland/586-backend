#pragma once

#include "util/containers.h"
#include "perm.h"
#include "actor.h"
//#include "util/variant.h"
#include "data/new.h"

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


//// TODO working on
//namespace mvc
//{
//    struct ParaRole
//    {
//        
//    };
//
//    namespace perms
//    {
//        struct AddProject {};
//        struct AddTask { ProjectID id; };
//        struct AssignTask { ProjectID id; };
//        struct UnassignTask { ProjectID id; };
//    }
//
//    using ParaPerm = Variant<
//        perms::AddProject,
//        perms::AddTask,
//        perms::AssignTask,
//        perms::UnassignTask
//        >;
//}
