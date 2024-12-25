#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sstream>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/wait.h>
#include "reactor.hpp"
#include <poll.h>
#define PORT "9034"

using namespace std;

vector<list<int>> adj;
Reactor * reactor = static_cast<Reactor *>(startReactor());

enum class Command {
    Newgraph,
    Kosaraju,
    Newedge,
    Removeedge,
    Exit,
    Invalid
};

Command getCommandFromString(const string& commandStr) {
    string lowerCommand = commandStr;
    transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);

    lowerCommand.erase(remove_if(lowerCommand.begin(), lowerCommand.end(),
    [](unsigned char c) { return c == '\n' || c == '\r'; }), lowerCommand.end());

    if (lowerCommand == "newgraph") {
        return Command::Newgraph;
    } 
    else if (lowerCommand == "kosaraju") {
        return Command::Kosaraju;
    } 
    else if (lowerCommand == "newedge") {
        return Command::Newedge;
    } 
    else if (lowerCommand == "removeedge") {
        return Command::Removeedge;
    } 
    else if (lowerCommand == "Exit") {
        return Command::Exit;
    }
    else {
        return Command::Invalid;
    }
}

void DFS1(int v, vector<list<int>>& adj, vector<bool>& visited, list<int>& List) {
    visited[v] = true;
    for (int i : adj[v]) {
        if (!visited[i]) {
            DFS1(i, adj, visited, List);
        }
    }
    List.push_front(v);  // use list (deque) to store the finish time order
}

void DFS2(int v, vector<list<int>>& adj, vector<bool>& visited, vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int i : adj[v]) {
        if (!visited[i]) {
            DFS2(i, adj, visited, component);
        }
    }
}

void kosaraju(vector<list<int>>& adj) {
    cout << "Kabab\n";
    int n = adj.size();
    list<int> List;
    vector<bool> visited(n, false);
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            DFS1(i, adj, visited, List);
        }
    }

    vector<list<int>> transposed_adj(n);
    for (int v = 0; v < n; v++) {
        for (int i : adj[v]) {
            transposed_adj[i].push_front(v);
        }
    }

    fill(visited.begin(), visited.end(), false);
    for (int v : List) {
        if (!visited[v]) {
            vector<int> component;
            DFS2(v, transposed_adj, visited, component);
            cout << "SCC: ";
            for (int i : component) {
                cout << i + 1 << " ";
            }
            cout << endl;
        }
    }
}

vector<list<int>> Newgraph(vector<list<int>> &adj) {
    int vertex, edges;
    cout << "Please enter the number of vertices and edges: \n";
    cin >> vertex >> edges;
    cout << "Please enter the edges: " << endl;
    adj.resize(vertex);
    for (int i = 0; i < edges; i++) {
        int u, v;
        cin >> u >> v;
        adj[u - 1].push_back(v - 1);
    }
    cout << "The graph has created!\n";
    return adj;
}

void Newedge(vector<list<int>> &adj) {
    int i, j;
    cout << "Please enter edge you wish to add\n";
    cin >> i;
    cin >> j;
    adj[i].push_back(j);
}

void Removeedge(vector<list<int>> &adj) {
    int i, j;
    cout << "Enter edge to remove (i j): ";
    cin >> i >> j;

    if (i >= 0 && (long unsigned int)i < adj.size() && j >= 0 && (long unsigned int)j < adj.size()) {
        bool found = false;

        for (auto it = adj[i].begin(); it != adj[i].end(); ++it) {
            if (*it == j) {
                adj[i].erase(it);
                found = true;
                break;
            }
        }

        if (found) {
            cout << "Edge (" << i << ", " << j << ") removed.\n" << endl;
        } else {
            cout << "Edge (" << i << ", " << j << ") not found.\n" << endl;
        }
    } else {
        cout << "Invalid vertices!\n" << endl;
    }
}

string handle_recieve_data(int index) {
    char buf[256];
    int nbytes = recv(index, buf, sizeof buf, 0);
    if (nbytes <= 0) {
        removeFdFromReactor(reactor, index);
        return "fail";
    }   
    buf[nbytes] = '\0';
    string input(buf);
    return input;
}

void *Command_Shift(int index) {
    string input;
    Command command = Command::Invalid;

    if (command != Command::Exit) {
        input = handle_recieve_data(index);
        command = getCommandFromString(input);

        switch (command) {
            case Command::Newgraph:
                Newgraph(adj);
                break;

            case Command::Kosaraju:
                kosaraju(adj);
                break;

            case Command::Newedge:
                Newedge(adj);
                break;

            case Command::Removeedge:
                Removeedge(adj);
                break;

            case Command::Invalid:
                cout << "Invalid command!" << endl;
                break;

            case Command::Exit:
                break;
        }
    }   
    return (void*)0;
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int setup_server() {
    int server_fd;
    struct sockaddr_in address;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9034);
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        return -1;
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", 9034);
    return server_fd;
}

int setup_poll_connection(int server_fd) { 
    int sock;   
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof remoteaddr;
    char remoteIP[INET6_ADDRSTRLEN];

    if ((sock = accept(server_fd, (struct sockaddr *)&remoteaddr, &addrlen)) < 0) {
        perror("accept");
        return -1;
    } else {
        printf("pollserver: new connection from %s on socket %d\n",
               inet_ntop(remoteaddr.ss_family,
                         get_in_addr((struct sockaddr*)&remoteaddr),
                         remoteIP, INET6_ADDRSTRLEN),
               sock);
        return sock;
    }
}

void * handle_hot_fd(int fd) {
    if (fd == reactor->pfds[0].fd) {
        int newfd = setup_poll_connection(fd);
        if (newfd == -1) return (int)(intptr_t)0;
        int res = addFdToReactor(reactor, newfd, Command_Shift);
        if (res < 0) return nullptr;
        else printf("%d\n", res);
        return (void*)1;
    } else {  
        return nullptr;
    }
}

int main() {
    int server_fd = setup_server();
    if (server_fd < 0) {
        cerr << "Failed to set up server\n";
        return 1;
    }

    reactor->pfds[0].fd = server_fd;
    reactor->pfds[0].events = POLLIN;
    reactor->curr_index = 1;

    vector<list<int>> adj;

    while (true) {
        int poll_count = poll(reactor->pfds, reactor->curr_index, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for (int i = 0; i < reactor->curr_index; i++) {
            if (reactor->pfds[i].revents & POLLIN) {
               if (handle_hot_fd(reactor->pfds[i].fd) == nullptr) {
                    dup2(reactor->pfds[i].fd, STDIN_FILENO);
                    reactor->funcs[i](reactor->pfds[i].fd);
               }
            }
        }
    }
    return 0;
}
