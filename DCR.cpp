#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <chrono>
#include <windows.h>
#include <psapi.h>
#include <climits>

class Graph {

public:

    struct FullEdge {
        int from;
        int to;
        int weight;
    };

private:

    struct Edge {
        int to;
        int weight;
    };

    int n = 0;
    int m = 0;

    char type = 'u';

    std::vector<std::vector<Edge>> adj;

    std::vector<FullEdge> edges;

public:

    void readGraph(const std::string& filename)
    {
        std::ifstream stream(filename);

        if (!stream.is_open()) {

            std::cout << "File error\n";

            return;
        }

        stream >> type >> n >> m;

        adj.resize(n + 1);

        edges.reserve(m);

        int from, to, weight;

        for (int i = 0; i < m; ++i) {

            stream >> from >> to >> weight;

            adj[from].push_back({ to, weight });

            adj[to].push_back({ from, weight });
        }

        stream.close();
    }

    Graph getSpaingTreePrima()
    {
        Graph tree;

        tree.n = n;

        tree.type = type;

        std::vector<int> minEdge(n + 1, 1e9);

        std::vector<int> parent(n + 1, -1);

        std::vector<bool> used(n + 1, false);

        std::priority_queue<
            std::pair<int, int>,
            std::vector<std::pair<int, int>>,
            std::greater<std::pair<int, int>>
        > q;

        minEdge[1] = 0;

        q.push({ 0, 1 });

        while (!q.empty()) {

            int v = q.top().second;

            q.pop();

            if (used[v])
                continue;

            used[v] = true;

            for (const Edge& edge : adj[v]) {

                int to = edge.to;

                int weight = edge.weight;

                if (!used[to] && weight < minEdge[to]) {

                    minEdge[to] = weight;

                    parent[to] = v;

                    q.push({ weight, to });
                }
            }
        }

        for (int i = 1; i <= n; ++i) {

            if (!used[i]) {

                std::cout << "Graph is disconnected\n";

                return tree;
            }
        }

        for (int i = 2; i <= n; ++i) {

            tree.edges.push_back({
                parent[i],
                i,
                minEdge[i]
                });
        }

        tree.m = tree.edges.size();

        return tree;
    }

    void writeGraph(const std::string& filename)
    {
        std::ofstream stream(filename);

        stream << type << " "
            << n << " "
            << edges.size() << "\n";

        for (int i = 0; i < edges.size(); ++i) {

            stream << edges[i].from << " "
                << edges[i].to << " "
                << edges[i].weight << "\n";
        }

        stream.close();
    }

    int getN()
    {
        return n;
    }

    int getM()
    {
        return m;
    }

    const std::vector<FullEdge>& getEdges()
    {
        return edges;
    }

    size_t getMemory()
    {
        size_t memory = 0;

        for (int i = 0; i < adj.size(); ++i) {

            memory += adj[i].size() * sizeof(Edge);
        }

        memory += edges.size() * sizeof(FullEdge);

        return memory;
    }
};

SIZE_T getMemoryUsage()
{
    PROCESS_MEMORY_COUNTERS memory;

    GetProcessMemoryInfo(
        GetCurrentProcess(),
        &memory,
        sizeof(memory)
    );

    return memory.WorkingSetSize;
}

int main()
{
    std::ios::sync_with_stdio(false);

    std::cin.tie(nullptr);

    setlocale(LC_ALL, "Russian");

    auto start = std::chrono::high_resolution_clock::now();

    Graph g;

    // менять тест здесь
    g.readGraph("MST05in.txt");

    Graph gg = g.getSpaingTreePrima();

    gg.writeGraph("output5.txt");

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;

    long long weight = 0;

    const std::vector<Graph::FullEdge>& result = gg.getEdges();

    for (int i = 0; i < result.size(); ++i) {

        weight += result[i].weight;
    }

    std::cout << "Вершин: "
        << g.getN()
        << ", Рёбер: "
        << g.getM()
        << "\n\n";

    std::cout << "Память под граф: ~"
        << g.getMemory() / 1024.0
        << " KB\n\n";

    std::cout << "Вершин в дереве: "
        << gg.getN()
        << "\n\n";

    std::cout << "Рёбер в дереве: "
        << gg.getM()
        << "\n\n";

    std::cout << "Вес дерева: "
        << weight
        << "\n\n";

    std::cout << "Время работы: "
        << elapsed.count()
        << " сек\n\n";

    std::cout << "Использовано памяти: ~"
        << getMemoryUsage() / 1024.0
        << " KB\n";

    return 0;
}
