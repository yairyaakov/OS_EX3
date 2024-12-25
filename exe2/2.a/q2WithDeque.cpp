#include <iostream>
#include <deque>
#include <stack>
#include <vector>
#include <chrono>
#include <algorithm> // For sort()

using namespace std;

class Graph {
    int V;
    deque<int> *adj;

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
    adj = new deque<int>[V + 1];  // Initialize deque for adjacency list
}

Graph::~Graph() {
    delete[] adj;
}


void Graph::addEdge(int v, int w) {
    adj[v].push_back(w);  // Add an edge from v to w
}

void Graph::DFSUtil(int v, vector<bool> &visited, vector<int> &component) {
    visited[v] = true;
    component.push_back(v);  // Add node to current SCC

    for (int i : adj[v])
        if (!visited[i])
            DFSUtil(i, visited, component);
}

Graph Graph::getTranspose() {
    Graph g(V);
    for (int v = 1; v <= V; v++) {
        for (int i : adj[v]) {
            g.addEdge(i, v);  // Reverse the direction of all edges
        }
    }
    return g;
}

void Graph::fillOrder(int v, vector<bool> &visited, stack<int> &Stack) {
    visited[v] = true;

    for (int i : adj[v])
        if (!visited[i])
            fillOrder(i, visited, Stack);

    Stack.push(v);  // Push to stack when finished with all adjacent nodes
}

void Graph::kosarajuSCC() {
    stack<int> Stack;
    vector<bool> visited(V + 1, false);

    // Step 1: Fill vertices in stack according to their finishing times
    for (int i = 1; i <= V; i++)
        if (!visited[i])
            fillOrder(i, visited, Stack);

    // Step 2: Get the transpose of the graph
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

            // Sort the nodes within the component to match expected order
            sort(component.begin(), component.end());

            // Print the current SCC
            for (int node : component) {
                cout << node << " ";
            }
            cout << endl;  // Ensure each SCC is printed on a new line
        }
    }
}

int main() {
    int n, m;
    cin >> n >> m;  // Read number of vertices (n) and edges (m)

    Graph g(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;  // Read each edge
        g.addEdge(u, v);
    }

    g.kosarajuSCC();  // Find and print SCCs

    return 0;
}
