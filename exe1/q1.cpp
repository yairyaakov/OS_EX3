#include <iostream>
#include <list>
#include <stack>
#include <vector>
#include <chrono>
#include <algorithm>  // For sorting the components

using namespace std;

class Graph {
    int V; // מספר הקודקודים
    list<int> *adj; // רשימת השכנים שאליהם יוצאת קשת

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
    delete[] adj;  // Deallocate memory for adjacency list
}

void Graph::addEdge(int v, int w) {
    adj[v].push_back(w);  // Add directed edge v -> w
}

void Graph::DFSUtil(int v, vector<bool> &visited, vector<int> &component) {
    visited[v] = true;
    component.push_back(v);

    // Explore all unvisited neighbors of v
    for (int i : adj[v]) {
        if (!visited[i]) {
            DFSUtil(i, visited, component);
        }
    }
}

Graph Graph::getTranspose() {
    Graph g(V);  // Create a new graph for the transposed version
    for (int v = 1; v <= V; v++) {
        // Reverse the edges of the original graph
        for (int i : adj[v]) {
            g.addEdge(i, v);  // Add edge i -> v in the transposed graph
        }
    }
    return g;
}

void Graph::fillOrder(int v, vector<bool> &visited, stack<int> &Stack) {
    visited[v] = true;

    // Recursively visit all unvisited neighbors
    for (int i : adj[v]) {
        if (!visited[i]) {
            fillOrder(i, visited, Stack);
        }
    }

    Stack.push(v);  // Push vertex v to stack once its neighbors are processed
}

void Graph::kosarajuSCC() {
    stack<int> Stack;
    vector<bool> visited(V + 1, false);

    // Step 1: Fill the stack with vertices in the order of their finishing times
    for (int i = 1; i <= V; i++) {
        if (!visited[i]) {
            fillOrder(i, visited, Stack);
        }
    }

    // Step 2: Get the transpose of the graph
    Graph gr = getTranspose();

    // Reset visited array for second DFS pass
    fill(visited.begin(), visited.end(), false);

    // Step 3: Process the vertices in the order of the stack
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        // If vertex v is not visited, start DFS from it on the transposed graph
        if (!visited[v]) {
            vector<int> component;
            gr.DFSUtil(v, visited, component);

            // Sort the component to ensure order of output is as expected
            sort(component.begin(), component.end());

            // Print the current SCC
            cout << "The SCC's are:" << endl;
            for (int node : component) {
                cout << node << " ";
            }
            cout << endl;
        }
    }
}

int main() {
    int n, m;
    cin >> n >> m;

    Graph g(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);  // Build the graph by adding edges
    }

    // Find strongly connected components (SCCs) using Kosaraju's algorithm
    g.kosarajuSCC();

    return 0;
}
