#include <iostream>
#include <stack>
#include <vector>
#include <chrono>

using namespace std;

class GraphMatrix {
    int V;
    vector<vector<int>> adjMatrix;  // מטריצת שכנויות

    void DFSUtil(int v, vector<bool> &visited, vector<int> &component);
    void fillOrder(int v, vector<bool> &visited, stack<int> &Stack);

public:
    GraphMatrix(int V);
    void addEdge(int v, int w);
    void kosarajuSCC();
    GraphMatrix getTranspose();
};

GraphMatrix::GraphMatrix(int V) {
    this->V = V;
    adjMatrix = vector<vector<int>>(V + 1, vector<int>(V + 1, 0));  // מטריצה של n*n
}

void GraphMatrix::addEdge(int v, int w) {
    adjMatrix[v][w] = 1;
}

void GraphMatrix::DFSUtil(int v, vector<bool> &visited, vector<int> &component) {
    visited[v] = true;
    component.push_back(v);

    for (int i = 1; i <= V; i++)
        if (adjMatrix[v][i] && !visited[i])
            DFSUtil(i, visited, component);
}

GraphMatrix GraphMatrix::getTranspose() {
    GraphMatrix g(V);
    for (int v = 1; v <= V; v++) {
        for (int i = 1; i <= V; i++) {
            if (adjMatrix[v][i])
                g.addEdge(i, v);
        }
    }
    return g;
}

void GraphMatrix::fillOrder(int v, vector<bool> &visited, stack<int> &Stack) {
    visited[v] = true;

    for (int i = 1; i <= V; i++)
        if (adjMatrix[v][i] && !visited[i])
            fillOrder(i, visited, Stack);

    Stack.push(v);
}

void GraphMatrix::kosarajuSCC() {
    stack<int> Stack;
    vector<bool> visited(V + 1, false);

    for (int i = 1; i <= V; i++)
        if (!visited[i])
            fillOrder(i, visited, Stack);

    GraphMatrix gr = getTranspose();

    fill(visited.begin(), visited.end(), false);

    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            vector<int> component;
            gr.DFSUtil(v, visited, component);

            for (int node : component)
                cout << node << " ";
            cout << endl;
        }
    }
}

int main() {
    int n, m;
    cin >> n >> m;

    GraphMatrix g(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);
    }

    // מדידת זמן עבור מטריצת שכנויות
    auto start = chrono::high_resolution_clock::now();
    g.kosarajuSCC();
    auto stop = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Duration with adjacency matrix: " << duration.count() << " microseconds" << endl;

    return 0;
}