#include <iostream>
#include <list>
#include <stack>
#include <vector>
#include <chrono>
#include <algorithm> // For sort()

using namespace std;

class Graph {
    int V; // Number of vertices
    list<int> *adj; // Adjacency list

    void DFSUtil(int v, vector<bool> &visited, vector<int> &component);
    void fillOrder(int v, vector<bool> &visited, stack<int> &Stack);

public:
    Graph(int V);
    ~Graph();
    void addEdge(int v, int w);
    void kosarajuSCC();
    Graph getTranspose();
};

Graph::Graph(int V) {
    this->V = V;
    adj = new list<int>[V + 1];  // Initialize adjacency list for V vertices
}

Graph::~Graph() {
    delete[] adj;  // Deallocate memory for the adjacency list
}

void Graph::addEdge(int v, int w) {
    adj[v].push_back(w);  // Add edge from v to w
}

void Graph::DFSUtil(int v, vector<bool> &visited, vector<int> &component) {
    visited[v] = true;
    component.push_back(v);  // Add node to the current SCC

    for (int i : adj[v]) {
        if (!visited[i]) {
            DFSUtil(i, visited, component);  // Explore all neighbors
        }
    }
}

Graph Graph::getTranspose() {
    Graph g(V);
    for (int v = 1; v <= V; v++) {
        for (int i : adj[v]) {
            g.addEdge(i, v);  // Reverse the direction of each edge
        }
    }
    return g;
}

void Graph::fillOrder(int v, vector<bool> &visited, stack<int> &Stack) {
    visited[v] = true;

    for (int i : adj[v]) {
        if (!visited[i]) {
            fillOrder(i, visited, Stack);  // Perform DFS to get the fill order
        }
    }

    Stack.push(v);  // Push vertex to stack when all adjacent vertices are processed
}

void Graph::kosarajuSCC() {
    stack<int> Stack;
    vector<bool> visited(V + 1, false);

    // Step 1: Fill vertices in stack according to their finishing times
    for (int i = 1; i <= V; i++) {
        if (!visited[i]) {
            fillOrder(i, visited, Stack);
        }
    }

    // Step 2: Get the transpose (reverse all edges)
    Graph gr = getTranspose();

    // Step 3: Mark all vertices as not visited for the second DFS
    fill(visited.begin(), visited.end(), false);

    // Step 4: Process vertices in the order defined by the stack
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        // If the vertex is not visited in the transposed graph, it's a new SCC
        if (!visited[v]) {
            vector<int> component;
            gr.DFSUtil(v, visited, component);

            // Sort the nodes within the component to ensure they are in ascending order
            sort(component.begin(), component.end());

            // Print the nodes of the SCC in sorted order
            for (int node : component) {
                cout << node << " ";
            }
            cout << endl;
        }
    }
}

int main() {
    int n, m;
    cin >> n >> m;  // Read the number of vertices (n) and edges (m)

    Graph g(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;  // Read each edge (u, v)
        g.addEdge(u, v);
    }

    g.kosarajuSCC();  // Call Kosaraju's algorithm to find and print SCCs

    return 0;
}
