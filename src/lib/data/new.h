#pragma once

#include "util/containers.h"
#include "util/string.h"
#include "util/lazy.h"
#include "auth/perm.h"
//#include "util/newtype.h"

#pragma db model version(1, 1)

#pragma db namespace pointer(std::shared_ptr)
namespace mvc 
{
    class Project;
    class Worker;
    class User;
    class Role;

    struct ProjectName
    {
        String val;
    };

    struct ProjectID
    {
        using Type = Project;

        int val;
    };
    
    struct UserID
    {
        using Type = User;

        String val;
    };
    
    struct RoleID
    {
        using Type = Role;

        String val;
    };

    #pragma db object
    class Role
    {
        friend class odb::access;

        Role() = default;

        public:
            Role(RoleID id_, Set<Perm> perms_)
                : name(std::move(id_.val))
                , perms(std::move(perms_))
            {}

            #pragma db id
            String name;

            Set<Perm> perms;
    };

    #pragma db object
    class User
    {
        friend class odb::access;

        User() = default;

        public:
            User(UserID id_)
                : id(std::move(id_.val))
            {}

            #pragma db id
            String id;

            #pragma db value_not_null
            Set<Shared<Role>> roles;
    };

    #pragma db object
    class Task
    {
        friend class odb::access;
        
        public:
            #pragma db id auto
            int id;

            String name;

            #pragma db not_null
            LazyShared<Project> project;

            LazyShared<Worker> worker;
    };

    #pragma db object
    class Project
    {
        friend class odb::access;
        
        public:
            #pragma db id auto
            int id;

            String name;

            #pragma db value_not_null inverse(project)
            Vec<LazyWeak<Task>> tasks;
    };

    #pragma db object
    class Worker
    {
        friend class odb::access;
        
        public:
            #pragma db id auto
            int id;

            String name;
            
            #pragma db value_not_null inverse(worker)
            Vec<LazyWeak<Task>> tasks;
    };
}

