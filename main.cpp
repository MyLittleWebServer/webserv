#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <vector>

#include "Server.hpp"

void exit_with_perror(const std::string &msg)
{
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}

void change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

void disconnect_client(int client_fd, std::map<int, std::string>& clients)
{
    std::cout << "client disconnected: " << client_fd << std::endl;
    close(client_fd);
    clients.erase(client_fd);
}

int main()
{
    Server server;
    server.startServer(); 

    /* init kqueue */
    int kq;
    if ((kq = kqueue()) == -1)
        exit_with_perror("kqueue() error\n" + std::string(strerror(errno)));

    std::map<int, std::string> clients;     // map for client socket:data
    std::vector<struct kevent> change_list; // 변경 이벤트 저장
    struct kevent event_list[8]; // 이벤트를 받을 배열, 최대 8개의 이벤트 동시 처리

    /* add event for server socket */
    change_events(change_list, server.getSocket(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    std::cout << "echo server started" << std::endl;

    /* main loop */
    int new_events;
    struct kevent* curr_event;
    while (1)
    {
        /*  apply changes and return new events(pending events) */
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
        if (new_events == -1)
            exit_with_perror("kevent() error\n" + std::string(strerror(errno)));

        change_list.clear(); // clear change_list for new changes

        for (int i = 0; i < new_events; ++i)
        {
            curr_event = &event_list[i];

            /* check error event return */
            if (curr_event->flags & EV_ERROR)
            {
                if (curr_event->ident == server.getSocket())
                    exit_with_perror("server socket error");
                else
                {
                    std::cerr << "client socket error" << std::endl;
                    disconnect_client(curr_event->ident, clients);
                }
            }
            else if (curr_event->filter == EVFILT_READ)
            {
                if (curr_event->ident == server.getSocket())
                {
                    /* accept new client */
                    int client_socket;
                    if ((client_socket = accept(server.getSocket(), NULL, NULL)) == -1)
                        exit_with_perror("accept() error\n" + std::string(strerror(errno)));
                    std::cout << "accept new client: " << client_socket << std::endl;
                    fcntl(client_socket, F_SETFL, O_NONBLOCK);

                    /* add event for client socket - add read && write event */
                    change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clients[client_socket] = "";
                }
                else if (clients.find(curr_event->ident) != clients.end())
                {
                    /* read data from client */
                    char buf[1024];
                    int n = read(curr_event->ident, buf, sizeof(buf));
                    if (n <= 0)
                    {
                        if (n < 0)
                            std::cerr << "client read error!" << std::endl;
                        disconnect_client(curr_event->ident, clients);
                    }
                    else
                    {
                        buf[n] = '\0';
                        clients[curr_event->ident] += buf;
                        std::cout << "received data from " << curr_event->ident << ": " << clients[curr_event->ident] << std::endl;
                    }
                }
            }
            else if (curr_event->filter == EVFILT_WRITE)
            {
                /* send data to client */
                // std::cout << "쏠쑤있어 " << curr_event->ident << ": " << clients[curr_event->ident] << std::endl;
                std::map<int, std::string>::iterator it = clients.find(curr_event->ident);
                if (it != clients.end() && clients[curr_event->ident] != "")
                {
                    if (write(curr_event->ident, clients[curr_event->ident].c_str(),
                                    clients[curr_event->ident].size()) == -1)
                    {
                        std::cerr << "client write error!" << std::endl;
                        disconnect_client(curr_event->ident, clients);  
                    }
                    else
                        clients[curr_event->ident].clear();
                }
            }
        }
    }
    return (0);
}
