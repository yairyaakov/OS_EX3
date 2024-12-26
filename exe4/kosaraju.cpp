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
#define PORT "9034"

using namespace std;

enum class Command{
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

void DFS1(int v, vector<list<int>>& adj, vector<bool>& visited, list<int>& List)
{
    visited[v] = true;
    for (int i : adj[v])
    {
        if (!visited[i])
        {
            DFS1(i, adj, visited, List);
        }
    }
    List.push_front(v);  // use list (deque) to store the finish time order
}

void DFS2(int v, vector<list<int>>& adj, vector<bool>& visited, vector<int>& component)
{
    visited[v] = true;
    component.push_back(v);
    for (int i : adj[v])
    {
        if (!visited[i])
        {
            DFS2(i, adj, visited, component);
        }
    }
}

void kosaraju(vector<list<int>>& adj)
{

    int n=adj.size();
    list<int> List;
    vector<bool> visited(n, false);
    for (int i = 0; i < n; i++)
    {
        if (!visited[i])
        {
            DFS1(i, adj, visited, List);
        }
    }

    vector<list<int>> transposed_adj(n);
    for (int v = 0; v < n; v++)
    {
        for (int i : adj[v])
        {
            transposed_adj[i].push_front(v);
        }
    }

    fill(visited.begin(), visited.end(), false);
    for (int v : List)
    {
        if (!visited[v])
        {
            vector<int> component;
            DFS2(v, transposed_adj, visited, component);
            cout << "SCC: ";
            for (int i : component)
            {
                cout << i + 1 << " ";
            }
            cout << endl;
        }
    }
}

vector<list<int>> Newgraph(vector<list<int>> &adj){
    int vertex,edges;
    cout << "Please enter the number of vertices and edges: \n";
    cin >> vertex >> edges;
    cout << "Please enter the edges: " << endl;
    adj.resize(vertex);
    for (int i = 0; i < edges; i++)
    {
        int u, v;
        cin >>u >> v;
        adj[u - 1].push_back(v - 1);
    }
    cout<<"The graph has created!\n";
    return adj;
}

void Newedge(vector<list<int>> &adj){

    int i,j;
    cout<<"Please enter edge you wish to add\n";
    cin>>i;
    cin>>j;
    adj[i-1].push_back(j-1);
}

void Removeedge(vector<list<int>> &adj){
    int i, j;
    cout << "Enter edge to remove (i j): ";
    cin >> i >> j;

    // Check if vertex i exists in the adjacency list and if j is in its list
    if ((i-1) >= 0 && (long unsigned int)(i-1) < adj.size() && (j-1) >= 0 && (long unsigned int)j < adj.size()) {
        bool found = false;

        // Traverse the list at index i to find and remove vertex j
        for (auto it = adj[i-1].begin(); it != adj[i-1].end(); ++it) {
            if (*it == (j-1)) {
                adj[i-1].erase(it);
                found = true;
                break;
            }
        }

        if (found) {
            cout << "Edge (" << i  << ", " << j  << ") removed.\n" << endl;
        } else {
            cout << "Edge (" << i  << ", " << j  << ") not found.\n" << endl;
        }
    } else {
        cout << "Invalid vertices!\n" << endl;
    }
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    close(pfds[i].fd);
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];
    (*fd_count)--;
}

string handle_recieve_data(struct pollfd pfds[],int index, int *fd_count){
// If not the listener, w int sender_fd = poll_fds[index].fd;e're just a regular client
    char buf[256];
    int nbytes = recv(pfds[index].fd, buf, sizeof buf, 0);
    int sender_fd = pfds[index].fd;

    if (nbytes <= 0) {
     // Got error or connection closed by client
        if (nbytes == 0) {
                            // Connection closed
          printf("pollserver: socket %d hung up\n", sender_fd);
                 } else {
                        perror("recv");
                        }
                        close(pfds[index].fd); // Bye!
                        del_from_pfds(pfds,index,fd_count);   

    }   
        buf[nbytes]='\0';
        string input(buf);
        return input;
}

void Command_Shift(struct pollfd pfds[], int index, int *fd_count, vector<list<int>>& adj) {
    string input = handle_recieve_data(pfds, index, fd_count);
    Command command = getCommandFromString(input);

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


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int setup_server() {

    int server_fd;
    struct sockaddr_in  address;

                            
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9034);

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
// Function to add a client.
int setup_poll_connection(int server_fd,struct pollfd pfds[]) {
    int sock;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof remoteaddr;
    char remoteIP[INET6_ADDRSTRLEN];

    // Create socket
    if((sock = accept(server_fd, (struct sockaddr *)&remoteaddr, &addrlen)) < 0){ // Accepts a new client connection and creates a new socket for that connection
        perror("accept");
        return -1;
    }else{
     
    printf("pollserver: new connection from %s on ""socket %d\n",
             inet_ntop(remoteaddr.ss_family,
              get_in_addr((struct sockaddr*)&remoteaddr),
                  remoteIP, INET6_ADDRSTRLEN),
                            sock);


    return sock;
    }
}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

         *pfds = (pollfd *) realloc(*pfds, sizeof(**pfds) * (*fd_size));

    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;	
}

int main()
{
    // Set up server
    int server_fd = setup_server();
    if (server_fd < 0) {
        cerr << "Failed to set up server\n";
        return 1;
    }
    vector<list<int>> adj;
    int fd_size = 5;
    struct pollfd *pfds = (pollfd *)malloc(sizeof *pfds * fd_size);
    pfds[0].fd = server_fd;
    pfds[0].events = POLLIN;
    int fd_count = 1;

    while (true) {
        int poll_count = poll(pfds, fd_count, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

         for(int i = 0; i < fd_count; i++) {

            if (pfds[i].revents & POLLIN) {

                if (pfds[i].fd == server_fd) {
                    // Accept new connection
                    int newfd=setup_poll_connection(server_fd,pfds);
                    if(newfd==-1) perror("accept");else{
                    add_to_pfds(&pfds, newfd, &fd_count, &fd_size);
                     }                     
                } else {
                    dup2(pfds[i].fd,STDIN_FILENO);
                    // Handle incoming message from a client
                    Command_Shift(pfds,i,&fd_count,adj);
                }
                
             }
        }
    }
    return 0;
}
