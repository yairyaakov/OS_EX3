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
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "proactor.hpp"
#include <sys/wait.h>
#define PORT "9034"

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
bool graph_updated = false;
size_t largest_scc_size = 0; // Stores the largest SCC size
bool flag50Percent = false;
vector<list<int>> adj;

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
    } else if (lowerCommand == "kosaraju") {
        return Command::Kosaraju;
    } else if (lowerCommand == "newedge") {
        return Command::Newedge;
    } else if (lowerCommand == "removeedge") {
        return Command::Removeedge;
    } else if (lowerCommand == "exit") {
        return Command::Exit;
    } else {
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
    List.push_front(v);
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

void kosaraju(vector<list<int>>& adj, int clientfd) {
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
            string response = "SCC: ";
            vector<int> component;
            DFS2(v, transposed_adj, visited, component);

            cout << "SCC: ";
            for (int i : component) {
                response.append(to_string(i + 1));
                response.append(" ");
            }
            response += "\n";

            send(clientfd, response.c_str(), response.size(), 0);
        }
    }
}

void Newgraph(vector<list<int>>& adj, int clientfd) {
    int vertex, edges;
    string message;
    message = "Please enter the number of vertices and edges: \n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    cin >> vertex >> edges;
    message = "Please enter the edges: \n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    adj.resize(vertex);
    for (int i = 0; i < edges; i++) {
        int u, v;
        cin >> u >> v;
        adj[u - 1].push_back(v - 1);
    }
    message = "The graph has created!\n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    return;
}

void Newedge(vector<list<int>>& adj, int clientfd) {
    int i, j;
    string message = "Please enter edge you wish to add\n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    cin >> i;
    cin >> j;
    adj[i-1].push_back(j-1);
   
    graph_updated = true;
    pthread_cond_signal(&cond); //wakes up
    message = "Edge has been added\n";
    send(clientfd, message.c_str(), message.size(), 0);
}

void Removeedge(vector<list<int>>& adj, int clientfd) {
    int i, j;
    string message = "Enter edge to remove (i j): ";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    cin >> i >> j;

    if ((i-1) >= 0 && (long unsigned int)(i-1) < adj.size() && (j-1) >= 0 && (long unsigned int)j < adj.size()) {
        bool found = false;

        for (auto it = adj[i-1].begin(); it != adj[i-1].end(); ++it) {
            if (*it == j-1) {
                adj[i-1].erase(it);
                found = true;
                graph_updated = true;
                
                pthread_cond_signal(&cond); //wakes up
                break;
            }
        }

        if (found) {
            message = "Edge: (" + to_string(i) + " , " + to_string(j) + " ) removed.\n";
            send(clientfd, message.c_str(), message.size(), 0);
        } else {
            message = "Edge: (" + to_string(i) + " , " + to_string(j) + " ) not found.\n";
            send(clientfd, message.c_str(), message.size(), 0);
        }
    } else {
        message = "Invalid vertices!\n";
        send(clientfd, message.c_str(), message.size(), 0);
    }
}

string handle_recieve_data(int client_fd) {
    char buf[256];
    int nbytes = recv(client_fd, buf, sizeof buf, 0);
    if (nbytes <= 0) {
        if (nbytes == 0) {
            printf("socket %d hung up\n", client_fd);
        } else
            cout << "recv\n";
        close(client_fd);
        return "exit";
    }
    buf[nbytes] = '\0';
    string input(buf);
    return input;
}

void* Command_Shift(void* client_socket) {
    
    int client_fd = *(int*)client_socket;
    dup2(client_fd, STDIN_FILENO);
    string input;
    Command command = Command::Invalid;
    while (command != Command::Exit) {
        input = handle_recieve_data(client_fd);
        command = getCommandFromString(input);

        switch (command) {
            case Command::Newgraph:
                pthread_mutex_lock(&mutex);
                Newgraph(adj, client_fd);
                pthread_mutex_unlock(&mutex);
                break;

            case Command::Kosaraju:
                kosaraju(adj, client_fd);
                break;

            case Command::Newedge:
                pthread_mutex_lock(&mutex);
                Newedge(adj, client_fd);
                pthread_mutex_unlock(&mutex);
                break;

            case Command::Removeedge:
                pthread_mutex_lock(&mutex);
                Removeedge(adj, client_fd);
                pthread_mutex_unlock(&mutex);
                break;

            case Command::Invalid:
                cout << "Invalid command!" << endl;
                break;

            case Command::Exit:
                break;
        }
    }
   
    return NULL;
}

void* get_in_addr(struct sockaddr* sa) {
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
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
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

int setup_client_connection(int server_fd) {
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);
    char remoteIP[INET6_ADDRSTRLEN];
    int new_fd;

    new_fd = accept(server_fd, (struct sockaddr*)&remoteaddr, &addrlen);
    if (new_fd == -1) {
        perror("accept");
        return -1;
    }

    if (remoteaddr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&remoteaddr;
        inet_ntop(AF_INET, &s->sin_addr, remoteIP, sizeof(remoteIP));
    } 
    else {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&remoteaddr;
        inet_ntop(AF_INET6, &s->sin6_addr, remoteIP, sizeof(remoteIP));
    }
    printf("New connection from %s on socket %d\n", remoteIP, new_fd);

    return new_fd;
}

void is_it_50Percent(vector<list<int>>& adj) {
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

            largest_scc_size = max(largest_scc_size, component.size());
        }
    }
}

void* monitorGraphChanges(void* arg) {
    while (true) {
        pthread_mutex_lock(&mutex);

        while (!graph_updated) { // wait for chenges
            pthread_cond_wait(&cond, &mutex);
        }

        graph_updated = false;

        largest_scc_size = 0;
        is_it_50Percent(adj); // claculate SCC

        size_t total_nodes = adj.size();
        if (largest_scc_size >= total_nodes / 2 ) {
            cout << "At Least 50% of the graph belongs to the same SCC\n";
            flag50Percent = true;
        } 
        else {
            if (flag50Percent) { //only if we reached to 50% in the past
                cout << "At Least 50% of the graph no longer belongs to the same SCC\n";
                flag50Percent = false; 
            }
        }

        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main() {
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitorGraphChanges, NULL);

    int server_fd = setup_server();
    if (server_fd < 0) {
        cerr << "Failed to set up server\n";
        return 1;
    }

    while (true) {
        int client_fd = setup_client_connection(server_fd);
        if (client_fd == -1) {
            return 1;
        }
        // Start proactor thread for each new client connection
        pthread_t proactor_tid = startProactor(client_fd, Command_Shift);
        if (proactor_tid == 0) {
            cout << "Failed to create proactor thread.\n";
            close(client_fd);
            continue;
        }
        // Example of stopping the Proactor thread manually if needed
        // stopProactor(proactor_tid);
    }

    return 0;
}
