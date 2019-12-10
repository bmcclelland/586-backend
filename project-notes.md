# SPA

The backend is a standalone C++ application implementing an HTTP REST API. Results are returned as JSON.

The frontend is Rust code that compiles to webassembly. It includes a JavaScript loader and an HTML index, so it is served normally like any website. 

The frontend uses an SPA framework based on a message loop. Messages are inserted into the loop (from user actions, etc), which when handled can update the model and rerender if necessary. In most cases, the messages make GET or POST requests to the backend and the model is updated with the result.

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
```C++
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
```C++
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
```C++
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

This class is a view of 3 objects: Task, Project, and Worker. The members need pragmas to disambiguate because all 3 objects have `id` and `name` fields. The pragma says "column", but the idea of a column and an object member are interchangeable here.

Example of retrieving this view:

[link](https://github.com/bmcclelland/586-backend/blob/master/src/lib/iendpoint/gettask.cpp#L17)
```C++
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

# MVC

### Backend

[Models and views](https://github.com/bmcclelland/586-backend/blob/master/src/lib/data/new.h) (model objects are labeled `#pragma db object` and views are labeled `#pragma db view`). Model classes are mapped to tables. Some endpoints just return the model objects when they contain the data needed, but they could trivially be replaced with view classes.

Views, once fetched by an endpoint, are serialized into JSON (this must be written manually since C++ lacks reflection). Example:

[link](https://github.com/bmcclelland/586-backend/blob/master/src/lib/data/json.h#L151)
```C++
void to_json(Json& j, WorkerNameID const& x) 
{
    j = Json{
        {"id",    x.id},
        {"name",  x.name},
    };
}
```

[Controllers](https://github.com/bmcclelland/586-backend/tree/master/src/lib/iendpoint). Controllers inherit from `IEndpoint`, which takes some input and produces some output (or nothing if it fails). Typically (always) they contain a reference to an `IDatabase` to interact with the model. They're selected by the [router](https://github.com/bmcclelland/586-backend/blob/master/src/lib/irouter/apirouter.cpp), which parses an API path. The API itself is constructed as a type (I wrote this as an experiment but it's probably irrelevant to the project). API paths come from the [request handler](https://github.com/bmcclelland/586-backend/blob/master/src/lib/ihandler/httphandler.cpp), which also handles authentication and authorization.

### Frontend

[Model](https://github.com/bmcclelland/586-frontend/blob/master/src/model.rs). This class contains the state for the frontend business logic. It inherits from a `Component` class which contains an `update` method which handles `Msg` message classes from the event loop. Messages can be inserted into the loop through `model.link.send_back()`.

For views, `Model` contains a `Scene` enum (a tagged union made of multiple types) which collects together different view types. Example of a view type:

[link](https://github.com/bmcclelland/586-frontend/blob/master/src/views.rs#L26)
```Rust
#[derive(Template)]
#[template(path = "assign_task.html")]
pub struct AssignTaskView {
    pub task: TaskDetails,
    pub workers: Vec<ListWorker>,
}
```

In Rust, `#[derive(...)]` is a way to automatically generate code for a type. `Template` and the `#[template(...)]` directive say that this class is represented by a static template with a specific path (so it can check the templates at compile time). The class's members (and only them) will be available to use inside the template. This specific template is located [here](https://github.com/bmcclelland/586-frontend/blob/master/templates/assign_task.html).

The frontend controller is just a handler for the different `Msg` subtypes. Here is the AssignTask handler:

[link](https://github.com/bmcclelland/586-frontend/blob/master/src/msg.rs#L448)
```Rust
// Msgs only allow one parameter, so this takes a tuple of (TaskId, WorkerId).
Msg::AssignTask((task_id, worker_id)) => {
    // Build the POST body.
    let params = AssignTaskParams { task_id, worker_id };
    
    // Build the POST request with path and body.
    let req = model.post("assign_task", &params);
    
    // A macro which does the actual request, passing a closure to handle a successful response.
    // The response handler sends a new message into the frontend controller for what to do next.
    fetch!(model, req, move |_: ()| {
        Msg::GetTask(task_id) // Refresh the page
    });
}
```
(model.post() and fetch!() are located in the same file. Both actions are too ugly to do inline.)

# Dependency Injection

This uses the boost::di library. Here is the "production" injector and its use:

[link](https://github.com/bmcclelland/586-backend/blob/master/src/mvc/main.cpp)
```C++
DI::make_injector = []()
{
    return di::make_injector(
        di::bind<IServer>.to<HttpServer>().in(di::unique),
        di::bind<IHandler>.to<HttpHandler>().in(di::unique),
        di::bind<IRouter>.to<ApiRouter>().in(di::unique),
        di::bind<IDatabase>.to<SqliteDatabase>().in(di::unique),
        di::bind<>.to(ConfigPath("config.json"))
    );
};

auto app = DI::make_injector().create<App>();
app.start();
```

`DI` is a static class with a `make_injector` member whose type is a function object returning an injector. This sets that function to be a closure that returns the given injector. The `in` call specifies the scope; here `unique` is used, which creates a new object each time `injector.create<T>()` is called.

The "testing" injector is defined [here](https://github.com/bmcclelland/586-backend/blob/master/src/test/main.cpp#L241). Other implementation classes could be bound here, though as it happened I only changed the `ConfigPath` binding. This determines where the `Config` class (see [here](https://github.com/bmcclelland/586-backend/tree/master/src/lib/config)) reads its JSON data, which is then used by other configuration classes (such as [AuthConfig](https://github.com/bmcclelland/586-backend/blob/master/src/lib/config/auth.h)).

Another use of the injector is where IEndpoints are parsed:

[link](https://github.com/bmcclelland/586-backend/blob/master/src/lib/server/apiparse.h#L364)
```C++
auto const path_args = std::make_from_tuple<typename T::PathArgs>(r->result);

auto injector = di::make_injector(
    DI::make_injector(),
    di::bind<typename T::PathArgs>.to(path_args)
    );  

return R(
    r->next_state,
    std::make_tuple(Shared<T>(
        injector.template create<Unique<T>>())
        )
    );
```
This code is not easy to read, but what happens is that every IEndpoint subclass has a PathArgs public type (here, `T` is the endpoint type). When arguments come from the URL path (such as the `1` in `api/get_task/1`), the endpoint's PathArgs are constructed from them and then that is injected into the endpoint's constructor along with its other dependencies. This code binds the PathArgs in a new injector made from the existing injector (ex: [gettask.h](https://github.com/bmcclelland/586-backend/blob/master/src/lib/iendpoint/gettask.h)).

# Authentication

This app uses third-party authentication through Auth0. Most of the relevant code is [here](https://github.com/bmcclelland/586-frontend/blob/master/src/authservice.rs). Most of the work is done in a `js!` macro which allows running inline JavaScript. The result of the auth attempt is returned through a captured Rust callback which sends `Msg::AuthReady` with the auth state (handled [here](https://github.com/bmcclelland/586-frontend/blob/master/src/msg.rs#L311)). If successful, the auth state contains the JWT.

The JWT is inserted into request header inside this code. It is a new interface I added onto the library's `http::Request::Builder` class:

[link](https://github.com/bmcclelland/586-frontend/blob/master/src/msg.rs#L84)
```Rust
impl Authable for http::request::Builder
{
    fn add_auth(&mut self, auth: &AuthState) -> &mut Self
    {
        match auth {
            AuthState::Yes(user) => {
                self.header("Authorization", format!("Bearer {}", user.token))
            }
            _ => {
                self
            }
        }
    }
}
```
Which can be used while building a web request like:
```Rust
Request::get(format!("{}/api/{}", remote_host(), action))
    .add_auth(&self.auth_state) // &self here is a reference to our Model.
    .body(Nothing)
    .unwrap()
```

The JWT is pulled out of the request on the backend [here](https://github.com/bmcclelland/586-backend/blob/master/src/lib/ihandler/getjwt.cpp) and verified [here](https://github.com/bmcclelland/586-backend/blob/master/src/lib/auth/authenticator.cpp). Once we have the subject, it is used the user's unique ID.

# Hosting on AWS S3

The frontend is hosted [here](http://586-frontend.s3-website-us-east-1.amazonaws.com/)

# Deployment scripts

Only for the frontend, located [here](https://github.com/bmcclelland/586-frontend/blob/master/tools/deploy-to-s3). It builds the app, replaces the backend address with the production one, syncs with the bucket, and then sets the correct Content-Type of the .wasm file.
