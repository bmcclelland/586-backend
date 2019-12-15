#pragma once

#include "util/string.h"

namespace mvc
{
    using Perm = String;
    
    namespace perms
    {
        inline Perm const read_projects{"read:projects"};
        inline Perm const read_tasks{"read:tasks"};
        inline Perm const read_workers{"read:workers"};
        
        inline Perm const create_projects{"create:projects"};
        inline Perm const create_tasks{"create:tasks"};
        inline Perm const create_workers{"create:workers"};
        
        inline Perm const modify_projects{"modify:projects"};
        inline Perm const modify_tasks{"modify:tasks"};
        inline Perm const modify_workers{"modify:workers"};

        inline Perm const delete_projects{"delete:projects"};
        inline Perm const delete_workers{"delete:workers"};
        inline Perm const delete_tasks{"delete:tasks"};
    }
}
