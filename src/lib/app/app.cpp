#include "app.h"
#include "util/print.h"
#include <thread>
#include <chrono>

namespace mvc
{
    App::App(Unique<IServer> server)
        : _server(std::move(server))
    {
        println("App::App");
    }
    
    void App::start()
    {
        println("App::start");
        _server->run();

        while (true)
        {
            String input;
            std::cin >> input;  
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}
