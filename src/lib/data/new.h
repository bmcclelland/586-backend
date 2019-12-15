#pragma once

#include "util/containers.h"
#include "util/string.h"
#include "util/lazy.h"
#include "auth/perm.h"
#include <odb/nullable.hxx>

#pragma db model version(1, 1)

#pragma db namespace pointer(std::shared_ptr)
namespace mvc 
{
    class Project;
    class Worker;
    class Task;

    #pragma db value 
    struct ProjectName
    {
        String val;
    };

    #pragma db value 
    struct ProjectID
    {
        using Type = Project;

        int val;
    };
    
    #pragma db value 
    struct TaskName
    {
        String val;
    };
    
    #pragma db value 
    struct TaskID
    {
        using Type = Task;

        int val;
    };
    
    #pragma db value 
    struct WorkerName
    {
        String val;
    };
    
    #pragma db value 
    struct WorkerID
    {
        using Type = Worker;

        int val;
    };
    
    #pragma db object
    class Task
    {
        friend class odb::access;
        
        Task() = default;
        
        public:
            Task(TaskName name_, Shared<Project> const& project_)
                : id(-1)
                , name(std::move(name_.val))
                , project(project_)
                , worker()
            {}

            #pragma db id auto
            int id;

            String name;

            #pragma db not_null
            #pragma db on_delete(cascade)
            LazyShared<Project> project;

            #pragma db on_delete(set_null)
            LazyShared<Worker> worker;
    };

    #pragma db object
    class Project
    {
        friend class odb::access;

        Project() = default;
        
        public:
            Project(ProjectName name_)
                : id()
                , name(std::move(name_.val))
                , tasks()
            {}

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

        Worker() = default;
        
        public:
            Worker(WorkerName name_)
                : name(std::move(name_.val))
                , tasks()
            {}

            #pragma db id auto
            int id;

            String name;
            
            #pragma db value_not_null inverse(worker)
            Vec<LazyWeak<Task>> tasks;
    };
    
    #pragma db view object(Project)
    struct ProjectNameID
    {
        String name;
        int    id;
    };
    
    #pragma db view object(Worker)
    struct WorkerNameID
    {
        String name;
        int    id;
    };

    #pragma db view object(Task) object(Project) object(Worker)
    struct TaskDetails
    {
        #pragma db column(Task::name)
        String name;
        
        #pragma db column(Task::id)
        int id;

        #pragma db column(Project::name)
        String project_name;
        
        #pragma db column(Project::id)
        int project_id;
        
        #pragma db column(Worker::name)
        odb::nullable<String> worker_name;
        
        #pragma db column(Worker::id)
        odb::nullable<int> worker_id;
    };
}

