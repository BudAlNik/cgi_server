#include <iostream>
#include <sys/epoll.h>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <cassert>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <memory>
#include <signal.h>
#include "parser.h"
#include "executor.h"


const int EPOLL_MAX_EVENTS_NUMBER = 10;
const time_t TIMEOUT_CONSTANT = 30;

void epoll_stop(const int evfd, epoll_event& event, const int clientfd);

struct client_handler;

static std::unordered_map<int, std::shared_ptr<client_handler>> clients;

struct client_handler {
    client_handler(const int evfd_, const int fd_) : evfd(evfd_), fd(fd_), data(0), last_run(time(0)), forks(0) {}

    int get_fd() {
        return fd;
    }

    time_t get_last_run() {
        return last_run;
    }

    void handle(epoll_event& event) {
        if (event.events & EPOLLIN) {
            last_run = time(0);
            int read_ = read(fd, buf, BUFFERSIZE);
            if (read_ == 0) {
                printf("removing client %d\n", fd);
                close(fd);
                clients.erase(fd);
                return;
            }
            if (data + read_>= BUFFERSIZE) {
                event.events |= EPOLLOUT;
                return;
            }
            //*****
            std::string cur = "";
            for (int i = 0; i < data + read_; i++) {
                if (buf[i] != '\n') {
                    cur += buf[i];
                } else {
                    commands.push_back(cur);
                    cur = "";
                }
            }
            for (int i = 0; i < (int)cur.size(); i++) {
                buf[i] = cur[i];
            }
            data = cur.size();
            for (auto c : commands) {
                auto res = parse_path(c);
                printf("handling client %d\n", fd);
                while (true) {
                    if (forks == FORKLIMIT) 
                        continue;
                    forks++;
                    on_request_recieved(res, fd);
                    forks--;
                    break;
                }
            }
            //*****
            return;
        }
        if ((event.events & EPOLLOUT) && (data < BUFFERSIZE)) {
            event.events |= EPOLLIN;
        }   
    }

private:
    const static int BUFFERSIZE = 2048;
    const static int FORKLIMIT = 64;
    char buf[BUFFERSIZE];
    const int fd, evfd;
    int data;
    int forks;
    int state;
    time_t last_run;
    std::vector<string> commands;
};


void error(const std::string &msg, int error_code) {
    std::cerr << msg << "\n";
    exit(1);
}

void bind_socket(int socketfd, sockaddr_in &server_addr) {
    if (bind(socketfd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
        error("Can't bind server_addr", 2);
    }
}

void build_socket(int &socketfd, sockaddr_in &server_addr, const int port) {
    if ((socketfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0)) < 0) {
        error("Can't open socket", 1);
    }
    int temp = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &temp, sizeof(temp)) < 0) {
        error("setsockopt(SO_REUSEADDR) failed", 2);
    }


    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
}

void create_epoll(int &evfd, const int socketfd) {
    if ((evfd = epoll_create1(EPOLL_CLOEXEC)) == -1) {
        error("Can't create epoll", 2);
    }
    epoll_event event;
    event.data.fd = socketfd;
    event.events = EPOLLIN;
    epoll_ctl(evfd, EPOLL_CTL_ADD, socketfd, &event);
}

void epoll_stop(const int evfd, epoll_event& event, const int clientfd) {
    printf("break connection with %d\n", clientfd);
    epoll_ctl(evfd, EPOLL_CTL_DEL, clientfd, &event);
    clients.erase(clientfd);
}

void epoll_add(const int evfd, const int socketfd, sockaddr_in &client_addr, socklen_t socklen) {
    int clientfd = accept4(socketfd, reinterpret_cast<sockaddr *>(&client_addr), &socklen,
                           SOCK_NONBLOCK | SOCK_CLOEXEC);
    printf("adding client %d\n", clientfd);
    if (clientfd >= 0) {
        epoll_event event;
        event.data.fd = clientfd;
        event.events = EPOLLIN;
        epoll_ctl(evfd, EPOLL_CTL_ADD, clientfd, &event);
        std::shared_ptr<client_handler> hanlder_ptr = std::make_shared<client_handler>(evfd, clientfd);
        clients.insert({clientfd, hanlder_ptr});
    }
}

void handle_client(epoll_event& event) {
    auto it = clients.find(event.data.fd);
    if (it == clients.end()) {
        return;
    }
    it->second->handle(event);
    return;
}

int main() {
    int socketfd;
    int evfd;
    int port = 8004;
    cerr << port << "\n";
    signal(SIGPIPE, SIG_IGN);
    if (signal(SIGINT, [](int signo) {
                   if (signo == SIGINT) {
                       std::cout << "SIGINT\n";
                       exit(0);
                   }
               }
    ) == SIG_ERR) {
        std::cout << "Can't handle signals\n";
        exit(0);
    }
    sockaddr_in server_addr, client_addr;
    socklen_t socklen = sizeof(client_addr);
    build_socket(socketfd, server_addr, port);
    bind_socket(socketfd, server_addr);
    listen(socketfd, 1);
    create_epoll(evfd, socketfd);

    while (true) {
        static epoll_event events[EPOLL_MAX_EVENTS_NUMBER];
        int event_num = epoll_wait(evfd, events, EPOLL_MAX_EVENTS_NUMBER, TIMEOUT_CONSTANT);
        for (size_t i = 0; i < event_num; i++) {
            if (events[i].data.fd == socketfd) {
                if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLRDHUP)) {
                    epoll_stop(evfd, events[i], events[i].data.fd);
                } else {
                    epoll_add(evfd, socketfd, client_addr, socklen);
                }
            } else {
                handle_client(events[i]);
            }
        }
    }

    return 0;
}