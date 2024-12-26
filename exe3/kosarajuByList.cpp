#include <iostream>
#include <vector>
#include <stack>
#include <sstream>
#include <string>

using namespace std;

class Graph {
    int V; // מספר הקודקודים
    vector<vector<int>> adj; // מטריצת השכנות

    void DFSUtil(int v, vector<bool> &visited, vector<int> &component);
    void fillOrder(int v, vector<bool> &visited, stack<int> &Stack);

public:
    Graph(int V);
    void addEdge(int v, int w);
    void removeEdge(int v, int w);
    void kosarajuSCC();
    Graph getTranspose();
};

Graph::Graph(int V) {
    this->V = V;
    adj.resize(V + 1, vector<int>(V + 1, 0)); // אתחול מטריצה של 0
}

void Graph::addEdge(int v, int w) {
    adj[v][w] = 1; // הוספת קשת מ-v ל-w
}

void Graph::removeEdge(int v, int w) {
    adj[v][w] = 0; // מחיקת קשת מ-v ל-w
}

void Graph::DFSUtil(int v, vector<bool> &visited, vector<int> &component) {
    visited[v] = true;
    component.push_back(v);

    for (int i = 1; i <= V; ++i) {
        if (adj[v][i] == 1 && !visited[i]) {
            DFSUtil(i, visited, component);
        }
    }
}

Graph Graph::getTranspose() {
    Graph g(V);
    for (int v = 1; v <= V; v++) {
        for (int i = 1; i <= V; i++) {
            if (adj[v][i] == 1) {
                g.addEdge(i, v);
            }
        }
    }
    return g;
}

void Graph::fillOrder(int v, vector<bool> &visited, stack<int> &Stack) {
    visited[v] = true;

    for (int i = 1; i <= V; ++i) {
        if (adj[v][i] == 1 && !visited[i]) {
            fillOrder(i, visited, Stack);
        }
    }

    Stack.push(v);
}

void Graph::kosarajuSCC() {
    stack<int> Stack;
    vector<bool> visited(V + 1, false);

    for (int i = 1; i <= V; i++)
        if (!visited[i])
            fillOrder(i, visited, Stack);

    Graph gr = getTranspose();

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

// פונקציה שמבצעת ניהול פקודות מהמשתמש
void handleCommands() {
    Graph *graph = nullptr; // מצביע לגרף שנבנה
    string line;

    while (getline(cin, line)) {
        istringstream iss(line);
        string command;
        iss >> command;

        if (command == "Newgraph") {
            int n, m;
            iss >> n >> m;
            delete graph; // מוחק גרף קודם אם קיים
            graph = new Graph(n);
            for (int i = 0; i < m; ++i) {
                int u, v;
                cin >> u >> v;
                graph->addEdge(u, v);
            }
        } else if (command == "Kosaraju") {
            if (graph != nullptr) {
                graph->kosarajuSCC();
            } else {
                cout << "No graph available!" << endl;
            }
        } else if (command == "Newedge") {
            int u, v;
            iss >> u >> v;
            if (graph != nullptr) {
                graph->addEdge(u, v);
            } else {
                cout << "No graph available!" << endl;
            }
        } else if (command == "Removeedge") {
            int u, v;
            iss >> u >> v;
            if (graph != nullptr) {
                graph->removeEdge(u, v);
            } else {
                cout << "No graph available!" << endl;
            }
        }
    }
    delete graph;
}

int main() {
    handleCommands();
    return 0;
}