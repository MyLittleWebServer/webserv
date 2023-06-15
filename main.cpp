
#include "Server.hpp"
#include "Kqueue.hpp"
#include "EventHandler.hpp"

int main()
{
    Server server;
    server.startServer(); 

    /* init kqueue */
    Kqueue eventQueue;

    // std::map<int, std::string> clients;     // map for client socket:data

    /* add event for server socket */
    eventQueue.addEvent(server.getSocket());
    std::cout << "echo server started" << std::endl;

    EventHandler eventHandler(server.getSocket());

    /* main loop */
    while (1)
    {
        int eventCount = eventQueue.newEvents();
        for (int i = 0; i < eventCount; ++i)
        {
            // const struct kevent &currEvent = eventQueue.getEvent(i);
            eventHandler.setCurrentEvent(i);
            eventHandler.errorOccuration();
            eventHandler.branchCondition();
        }
    }
    return (0);
}
