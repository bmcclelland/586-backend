# ORM

Since C++ lacks reflection, ODB uses its own pre-compiler to generate code from your model. Metadata for each class/member is specified with `#pragma` directives. Class members are automatically turned into database columns. 

Because of C++'s memory management style, references are done explicitly. Some types:
- `Unique<T>` is a single-owner smart pointer which automatically deletes when it is destroyed.
- `Shared<T>` is a reference-counted smart pointer that deletes when the last reference is destroyed.
- `LazyShared<T>` is a reference-counted ODB smart pointer which won't pull data from the database until you call its `load()` member.
- `LazyWeak<T>` is an ODB weak pointer to a `Lazy<T>` (necessary with cyclic references).

For example, Project contains a vector of LazyWeak<Task>, so a Project can be retrieved without necessarily loading all of its tasks.

### One-to-many

[link](https://github.com/bmcclelland/586-backend/blob/master/src/lib/data/new.h#L117)
```
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
`
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

            // The unique id for the class, auto-incrementing.
            #pragma db id auto
            int id;

            String name;

            // A non-null reference to its Project.
            #pragma db not_null
            #pragma db on_delete(cascade)
            LazyShared<Project> project;

            // A maybe-null reference to its assigned Worker.
            #pragma db on_delete(set_null)
            LazyShared<Worker> worker;
    };
```    
    
Each Project has many Tasks. A Task has a lazy shared reference to its Project, and a Project has a vector of lazy weak references to its Tasks. Example of adding a Task to a Project:

[link](https://github.com/bmcclelland/586-backend/blob/master/src/lib/iendpoint/addtask.cpp#L17)
```
    // tx is a transaction guard. When it goes out of scope, the transaction is rolled back if it has not been committed.
    auto tx = _db->transaction();
 
    Shared<Project> project = tx.find_shared(project_id);
        
    if (!project)
    {
        return nullopt;
    }

    // To add a Task to a Project, create a new Task with a reference to the Project and persist it.
    Task t(name, project);
    TaskID task_id{ tx.persist(t) };
    tx.commit();
```

### Views

Views can be created that include multiple DB classes. Here is the most complex one:

[link](https://github.com/bmcclelland/586-backend/blob/master/src/lib/data/new.h#L204)
```
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
```

This class is a view of 3 objects: Task, Project, and Worker. The members need pragmas to disambiguate because all 3 object have `id` and `name` fields. The pragma says "column", but the idea of a column and an object member are interchangeable here. `Project::name` is the C++ way to refer to the name member of Project.

Example of retrieving this view:

[link](https://github.com/bmcclelland/586-backend/blob/master/src/lib/iendpoint/gettask.cpp#L17)
```
    auto const tx = _db->transaction();
    
    // This is a type alias to a query<T> type that allows you to write queries in natural C++.
    // It contains all of the viewed object types and their fields, such as Q::Task::id or Q::Project::name.
    using Q = odb::query<TaskDetails>;

    auto query = tx.query<TaskDetails>(
        Q::Task::id == _path.task_id.val
        );

    // Queries are iterable for their results.
    // There should be one result, so we just return the first one we see.
    for (auto&& result : query)
    {
        return EndpointOutput(result);
    }

    // There was no result. Return a null-optional value.
    return nullopt;
```
