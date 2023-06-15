
#include "Server.hpp"
#include "Kqueue.hpp"
#include "EventHandler.hpp"

int main()
{
    Server server;
    server.startServer(); 
    Kqueue eventQueue;

    eventQueue.addEvent(server.getSocket());
    std::cout << "echo server started" << std::endl;

    EventHandler eventHandler(server.getSocket());

    while (1)
    {
        int eventCount = eventQueue.newEvents();
        for (int i = 0; i < eventCount; ++i)
        {
            eventHandler.setCurrentEvent(i);
            eventHandler.checkStatus();
        }
    }
    return (0);
}
