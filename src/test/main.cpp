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
        Actor admin("test|admin", {Perm::administrate});
    
        auto run_test = [&](Json&& json, auto const&... path)
        {
            EndpointInput input(std::move(json), admin);
            auto output = rtr->route(test_path("api", path...), input);

            if (output)
            {
                return output->payload;
            }
            else
            {
                return Json{"nullopt"};
            }
        };

        clear_db();
        run_test({{"name", "p"}}, "add_project");
        run_test({{"name", "t"}, {"project_id", 1}}, "add_task");
        run_test({}, "get_projects");
        Json actual = run_test({}, "get_project", "1");
        
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
