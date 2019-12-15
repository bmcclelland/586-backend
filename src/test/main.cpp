#include "test.h"
#include "util/typename.h"
#include "server/apiparse.h"
#include "data/data.h"
#include <iostream>
#include <filesystem>

#include "app/app.h"
#include "di.h"
#include "iserver/httpserver.h"
#include "ihandler/httphandler.h"
#include "irouter/apirouter.h"
#include "idatabase/sqlitedatabase.h"

namespace mvc::test 
{
    template <typename T, typename Tag>
    std::ostream& operator<<(std::ostream& s, NewType<T,Tag> const& x)
    {
        return s << type_name<Tag> << "(" << x.val() << ")";
    }
    
    static
    void clear_db()
    {
        static SqliteConfig const config(
            DI::make_injector().create<Config>()
            );

        static String const db_path(
            config.db_path().val()
            );

        std::filesystem::remove(db_path);
    }

    template <typename... Ts>
    static
    ApiPath test_path(Ts const&... ts)
    {
        return ApiPath{ApiSegment(ts)...};
    }

    //Only T needs persisting. It's the forward relationship.
    //p->tasks.push_back(t);
    //db->persist(p);

    TEST(path1, // Normal path
        using namespace mvc::api_parsers;
        using Api = Root<
            Lit("api"),
            Lit("get"),
            Int,
            Lit("get"),
            Int
            >;
        ApiPath path = test_path("api", "get", "14", "get", "15");
        Option<Tuple<int, int>> result = Api::parse(path);
        TEST_TRUE(result != nullopt);
        auto [n1, n2] = *result;
        TEST_EQ(n1, 14);
        TEST_EQ(n2, 15);
    )

    TEST(path2, // Dataless path
        using namespace mvc::api_parsers;
        using Api = Root<Lit("api")>;
        ApiPath path = test_path("api");
        Option<Tuple<>> result = Api::parse(path);
        TEST_TRUE(result != nullopt);
    )

    TEST(path3, // Bad single-token path
        using namespace mvc::api_parsers;
        using Api = Root<Lit("api")>;
        ApiPath path = test_path();
        Option<Tuple<>> result = Api::parse(path);
        TEST_TRUE(result == nullopt);
    )

    TEST(path4, // Bad empty path
        using namespace mvc::api_parsers;
        using Api = Root<>;
        ApiPath path = test_path("api");
        Option<Tuple<>> result = Api::parse(path);
        TEST_TRUE(result == nullopt);
    )

    TEST(path5, // Good empty path
        using namespace mvc::api_parsers;
        using Api = Root<>;
        ApiPath path = test_path();
        Option<Tuple<>> result = Api::parse(path);
        TEST_TRUE(result != nullopt);
    )
    
    TEST(project,
        auto injector = DI::make_injector();
        auto rtr = injector.create<Unique<IRouter>>();
        Actor admin("test|admin", {
            perms::read_projects,
            perms::read_workers,
            perms::read_tasks,
            perms::create_projects,
            perms::create_workers,
            perms::create_tasks,
            perms::modify_projects,
            perms::modify_workers,
            perms::modify_tasks,
            perms::delete_projects,
            perms::delete_workers,
            perms::delete_tasks
            });
    
        auto endpoint = [&](Json&& json, auto const&... path)
        {
            EndpointInput input(std::move(json), admin);
            auto output = rtr->route(test_path("api", path...), input);

            if (output)
            {
                return output->payload;
            }
            else
            {
                return Json{};
            }
        };
        
        [[maybe_unused]]
        auto print_endpoint = [&](Json&& json, auto const&... path)
        {
            auto result = endpoint(std::move(json), path...);
            println(result.dump());
            return result;
        };
        
        clear_db();
        
        { // Add a project -- project looks ok
            endpoint({{"name", "p"}}, "add_project");
            endpoint({{"name", "t"}, {"project_id", 1}}, "add_task");
            endpoint({}, "get_projects");
            Json actual = endpoint({}, "get_project", "1");
            Json ideal_task{
                {"id", 1}, 
                {"name", "t"}, 
                {"project", 1}, 
                {"worker", nullptr}
            };
            Json ideal{
                {"id", 1},
                {"name", "p"},
                {"tasks", { ideal_task } } 
            };
            TEST_EQ(actual, ideal);
        }

        { // Add a task -- task looks ok
            Json actual = endpoint({}, "get_task", "1");
            Json ideal{
                {"id", 1},
                {"name", "t"},
                {"project", {
                    {"id", 1},
                    {"name", "p"},
                }},
                {"worker", nullptr}
            };
            TEST_EQ(actual, ideal);
        }
       
        { // Add a worker, assign to task -- worker looks ok
            endpoint({{"name", "w"}}, "add_worker");
            endpoint({{"task_id", 1}, {"worker_id", 1}}, "assign_task");
            Json actual = endpoint({}, "get_worker", "1");
            Json ideal_task{
                {"id", 1}, 
                {"name", "t"}, 
                {"project", 1}, 
                {"worker", 1}
            };
            Json ideal{
                {"id", 1},
                {"name", "w"},
                {"tasks", { ideal_task } } 
            };
            TEST_EQ(actual, ideal);
        }
        
        { // Task has a worker now
            Json actual = endpoint({}, "get_task", "1");
            Json ideal{
                {"id", 1},
                {"name", "t"},
                {"project", {
                    {"id", 1},
                    {"name", "p"},
                }},
                {"worker", {
                    {"id", 1},
                    {"name", "w"},
                }}
            };
            TEST_EQ(actual, ideal);
        }

        { // Add a project -- getting both projects looks ok
            endpoint({{"name", "p2"}}, "add_project");
            Json actual = endpoint({}, "get_projects");
            Json ideal_p{
                {"id", 1}, 
                {"name", "p"}
            };
            Json ideal_p2{
                {"id", 2}, 
                {"name", "p2"}
            };
            Json ideal{ ideal_p, ideal_p2 };
            TEST_EQ(actual, ideal);
        }
        
        { // Add a worker -- getting both workers looks ok
            endpoint({{"name", "w2"}}, "add_worker");
            Json actual = endpoint({}, "get_workers");
            Json ideal_w{
                {"id", 1}, 
                {"name", "w"}
            };
            Json ideal_w2{
                {"id", 2}, 
                {"name", "w2"}
            };
            Json ideal{ ideal_w, ideal_w2 };
            TEST_EQ(actual, ideal);
        }

        { // Delete a project -- project and task are gone, worker lacks task
            endpoint({{"id", 1}}, "del_project");
            TEST_EQ(endpoint({}, "get_project", "1"), Json{});
            TEST_EQ(endpoint({}, "get_task", "1"), Json{});
            Json actual = endpoint({}, "get_worker", "1");  
            Json ideal{
                {"id", 1},
                {"name", "w"},
                {"tasks", Json::array()}
            };
            TEST_EQ(actual, ideal);
        }
    )

    void main()
    {
        DI::make_injector = []()
        {
            return di::make_injector(
                di::bind<IServer>.to<HttpServer>().in(di::unique),
                di::bind<IHandler>.to<HttpHandler>().in(di::unique),
                di::bind<IRouter>.to<ApiRouter>().in(di::unique),
                di::bind<IDatabase>.to<SqliteDatabase>().in(di::unique),
                di::bind<>.to(ConfigPath("config.test.json"))
            );
        };
        
        auto app = DI::make_injector().create<App>();
        (void)app;
        run_all();
    }

}

int main()
{
    mvc::test::main();
    return 0;
}
