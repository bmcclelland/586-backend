#include "test.h"
#include "util/typename.h"
#include "server/apiparse.h"
#include "data/data.h"
#include <iostream>

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

    //TEST(model,
    //    Model model = initial_model();
    //    Controller controller = new_controller<TestController>();
    //    TEST_TRUE(model.records.size() == 0);
    //    
    //    Request req = AddRecordRequest(
    //        Record(
    //            LastName("Sheckler"),
    //            FirstName("Ted")
    //            )
    //        );
    //
    //    controller->mutate(model, req);
    //    TEST_TRUE(model.records.size() == 1);
    //    TEST_EQ(model.records[0].last_name(), LastName("Sheckler"));
    //    TEST_EQ(model.records[0].first_name(), FirstName("Ted"));
    //)

    template <typename... Ts>
    static
    ApiPath test_path(Ts const&... ts)
    {
        return ApiPath{ApiSegment(ts)...};
    }

    TEST(odb,
    //    try
    //    {
    //        Unique<odb::core::database> db(
    //            new odb::sqlite::database(
    //                "test.sqlite",
    //                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
    //            )
    //        );
    //		
    //
    //        auto transact = [&db](auto&& f)
    //		{
    //			odb::transaction t(db->begin());
    //			f();
    //			t.commit();
    //		};
    //
    //
    //		transact([&]{ 
    //            odb::schema_catalog::create_schema(*db);
    //        });
    //
    //		int p1_id, p2_id;
    //        
    //        transact([&]{
    //            Project p1;
    //            p1.name = "Master Plan";
    //            Project p2;
    //            p2.name = "Side Project";
    //            p1_id = db->persist(p1);
    //            p2_id = db->persist(p2);
    //        });
                
    //Only T needs persisting. It's the forward relationship.
    //p->tasks.push_back(t);
    //db->persist(p);

    //		transact([&]{
    //			Shared<Project> p1 = db->find<Project>(p1_id);
    //			Shared<Project> p2(db->load<Project>(p2_id));
    //			
    //			Task t1;
    //			t1.name = "Task 1";
    //			t1.project = p1;
    //			db->persist(t1);
    //			
    //			Task t2;
    //			t2.name = "Task 2";
    //			t2.project = p1;
    //			db->persist(t2);
    //			
    //			Task t3;
    //			t3.name = "Task 3";
    //			t3.project = p2;
    //			db->persist(t3);
    //		});
    //
    //		transact([&]{
    //			for (auto&& row : db->query<Task>())
    //			{
    //				std::cout << "Task: ("
    //					<< row.id 
    //                    << "," 
    //                    << row.name;
    //				
    //				row.project.load();
    //				std::cout << "," << row.project->id << "\n";
    //			}
    //		});
                
    //		transact([&]{
    //			for (auto row : db->query<Project>())
    //			{
    //				std::cout << "PROJECT: " << row.name << "\n";
    //				for (auto&& t : row.tasks)
    //				{
    //					auto u = t.load();
    //					std::cout << "  " << u->name << "\n";
    //				}
    //			}
    //		});
    //    }
    //    catch (const odb::exception& e)
    //    {
    //        std::cerr << e.what () << std::endl;
    //    }
    )

    TEST(new_api,
    //    auto s = lol();
    //    TEST_EQ(s, "get_projects");
    )

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

    void main()
    {
        Config const config("config.test.json"); // TODO
        
        // NB: boost::di binds values by reference, leading to easy UB!
        // Giving them static lifetimes solves this.
        static HttpConfig   const http_config(config);
        static AuthConfig   const auth_config(config);
        static SqliteConfig const sqlite_config(config);
        
        DI::make_injector = []()
        {
            return di::make_injector(
                di::bind<IServer>.to<HttpServer>(),
                di::bind<IHandler>.to<HttpHandler>(),
                di::bind<IRouter>.to<ApiRouter>(),
                di::bind<IDatabase>.to<SqliteDatabase>(),
                di::bind<>.to(http_config),
                di::bind<>.to(sqlite_config),
                di::bind<>.to(auth_config)
            );
        };

        auto app = DI::make_injector().create<App>();
        (void)app;
        
        run_all();
//    show_view();
    }

    void debug()
    {
        Unique<IDatabase> db = DI::make_injector().create<Unique<IDatabase>>();

        RoleID rid1{"default"};
        RoleID rid2{"administrator"};
        UserID uid{"someuniqueauth0id"};
           
        db->transact([&]
        {
            Role r1(rid1, {Perm::view_project});
            db->persist(r1);

            Role r2(rid2, {Perm::administrate});
            db->persist(r2);
            
            User u(uid);
            u.roles.insert(std::make_shared<Role>(r2));
            db->persist(u);
        });

        Option<User> u = db->find_value(uid);

        if (u)
        {
            println(u->id);
            
            for (auto&& r : u->roles)
            {
                println("role: ", r->name);
            }
        }
    }
}

int main()
{
    mvc::test::main();
    mvc::test::debug();
    return 0;
}
