#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <stack>
#include <vector>
#include <containerium/core/error.hpp>
#include <containerium/structures/unionfind.hpp>

namespace containerium {

template<typename V, typename E>
class AdjacencyMatrixGraph {
public:
    enum class GraphType {
        Directed,
        Undirected
    };
    struct Graph {
        std::vector<V> vertices;
        std::vector<std::vector<E>> edges;
        GraphType graphType;
    };
    struct Edge {
        size_t start;
        size_t end;
        E weight;
    };

protected:
    Graph graph;

    std::expected<size_t, DataStructureError> findVertexIndex(V vertex) const {
        for (size_t i = 0; i < graph.vertices.size(); i++) if (graph.vertices[i] == vertex) return i;
        return std::unexpected(DataStructureError::ElementNotFound);
    }

public:
    explicit AdjacencyMatrixGraph(GraphType type = GraphType::Directed) : graph{.graphType = type} {}

    ~AdjacencyMatrixGraph() = default;

    bool isEmpty() const { return graph.vertices.empty(); }

    bool hasVertex(V vertex) const { return findVertexIndex(vertex).has_value(); }

    bool hasEdge(V start, V end) const {
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        CONTAINERIUM_TRY(endIndex, findVertexIndex(end));
        return graph.edges[startIndex][endIndex] != E{};
    }

    E getEdge(V start, V end) const {
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        CONTAINERIUM_TRY(endIndex, findVertexIndex(end));
        return graph.edges[startIndex][endIndex];
    }

    std::expected<std::vector<V>, DataStructureError> getVertices(std::function<void(V)> visitor) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        for (const auto& vertex : graph.vertices) visitor(vertex);
        return graph.vertices;
    }

    std::expected<std::vector<V>, DataStructureError> getNeighbours(V vertex, std::function<void(V)> visitor) const {
        CONTAINERIUM_TRY(index, findVertexIndex(vertex));
        std::vector<V> neighbours;
        for (size_t i = 0; i < graph.vertices.size(); i++) if (graph.edges[index][i] != E{}) neighbours.push_back(graph.vertices[i]);
        for (const auto& neighbour : neighbours) visitor(neighbour);
        return neighbours;
    }

    std::expected<size_t, DataStructureError> getDegree(V vertex) const {
        CONTAINERIUM_TRY(index, findVertexIndex(vertex));
        if (graph.graphType == GraphType::Undirected) {
            size_t degree = 0;
            for (const auto& edge : graph.edges[index]) if (edge != E{}) degree++;
            return degree;
        }
        else {
            CONTAINERIUM_TRY(inDegree, getInDegree(vertex));
            CONTAINERIUM_TRY(outDegree, getOutDegree(vertex));
            return inDegree + outDegree;
        }
    }

    std::expected<size_t, DataStructureError> getInDegree(V vertex) const {
        if (graph.graphType == GraphType::Undirected) return getDegree(vertex);    // In undirected graph, degree is equal to in-degree and out-degree
        CONTAINERIUM_TRY(index, findVertexIndex(vertex));
        size_t inDegree = 0;
        for(size_t i = 0; i < graph.vertices.size(); i++) if (graph.edges[i][index] != E{}) inDegree++;
        return inDegree;
    }

    std::expected<size_t, DataStructureError> getOutDegree(V vertex) const {
        if (graph.graphType == GraphType::Undirected) return getDegree(vertex);
        CONTAINERIUM_TRY(index, findVertexIndex(vertex));
        size_t outDegree = 0;
        for (const auto& edge : graph.edges[index]) if (edge != E{}) outDegree++;
        return outDegree;
    }

    std::expected<void, DataStructureError> addVertex(V vertex) {
        if (findVertexIndex(vertex).has_value()) return std::unexpected(DataStructureError::DuplicateValue);
        graph.vertices.push_back(vertex);
        for (auto& row : graph.edges) row.push_back(E{});
        graph.edges.push_back(std::vector<E>(graph.vertices.size(), E{}));
        return {};
    }
    
    std::expected<void, DataStructureError> removeVertex(V vertex) {
        CONTAINERIUM_TRY(index, findVertexIndex(vertex));
        graph.vertices.erase(graph.vertices.begin() + index);
        graph.edges.erase(graph.edges.begin() + index);
        for (auto& row : graph.edges) row.erase(row.begin() + index);
        return {};
    }

    std::expected<void, DataStructureError> addEdge(V start, V end, E edge) {
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        CONTAINERIUM_TRY(endIndex, findVertexIndex(end));
        if (graph.edges[startIndex][endIndex] != E{}) return std::unexpected(DataStructureError::DuplicateValue);
        graph.edges[startIndex][endIndex] = edge;
        if (graph.graphType == GraphType::Undirected) graph.edges[endIndex][startIndex] = edge;
        return {};
    }

    std::expected<void, DataStructureError> removeEdge(V start, V end) {
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        CONTAINERIUM_TRY(endIndex, findVertexIndex(end));
        graph.edges[startIndex][endIndex] = E{};
        if (graph.graphType == GraphType::Undirected) graph.edges[endIndex][startIndex] = E{};
        return {};
    }

    std::expected<size_t, DataStructureError> getVertexCount() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return graph.vertices.size();
    }

    std::expected<size_t, DataStructureError> getEdgeCount() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        size_t edgeCount = 0;
        for (const auto& row : graph.edges) for (const auto& edge : row) if (edge != E{}) edgeCount++;
        return edgeCount;
    }

    std::expected<std::vector<V>, DataStructureError> DFSRecursive(V start, std::function<void(V)> visitor) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        std::vector<V> visited;
        std::function<std::expected<void, DataStructureError>(size_t)> DFS = [&](size_t index) -> std::expected<void, DataStructureError> {
            visited.push_back(graph.vertices[index]);
            visitor(graph.vertices[index]);
            CONTAINERIUM_TRY(vertexCount, getVertexCount());
            for (size_t i = 0; i < vertexCount; ++i) {
                if (graph.edges[index][i] != E{}) {
                    bool isVisited = std::find(visited.begin(), visited.end(), graph.vertices[i]) != visited.end();
                    if (!isVisited) {
                        auto res = DFS(i);
                        if (!res.has_value()) return res;
                    }
                }
            }
            return {};
        };
        auto dfsRes = DFS(startIndex);
        if (!dfsRes.has_value()) return std::unexpected(dfsRes.error());
        return visited;
    }

    std::expected<std::vector<V>, DataStructureError> DFSIterative(V start, std::function<void(V)> visitor) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        std::vector<V> visited;
        std::stack<size_t> unvisited;
        unvisited.push(startIndex);
        while (!unvisited.empty()) {
            size_t currentIndex = unvisited.top();
            unvisited.pop();
            bool isVisited = std::find(visited.begin(), visited.end(), graph.vertices[currentIndex]) != visited.end();
            if (isVisited) continue;
            visited.push_back(graph.vertices[currentIndex]);
            visitor(graph.vertices[currentIndex]);
            CONTAINERIUM_TRY(vertexCount, getVertexCount());
            for (size_t i = 0; i < vertexCount; i++) {
                if (graph.edges[currentIndex][i] != E()) {
                    bool neighbourVisited = std::find(visited.begin(), visited.end(), graph.vertices[i]) != visited.end();
                    if (!neighbourVisited) unvisited.push(i);
                }
            }
        }
        return visited;
    }

    std::expected<std::vector<V>, DataStructureError> BFS(V start, std::function<void(V)> visitor) const {
    if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        std::vector<V> visited;
        std::queue<size_t> unvisited;
        unvisited.push(startIndex);
        while (!unvisited.empty()) {
            size_t currentIndex = unvisited.front();
            unvisited.pop();
            bool isVisited = std::find(visited.begin(), visited.end(), graph.vertices[currentIndex]) != visited.end();
            if (isVisited) continue;
            visited.push_back(graph.vertices[currentIndex]);
            visitor(graph.vertices[currentIndex]);
            CONTAINERIUM_TRY(vertexCount, getVertexCount());
            for (size_t i = 0; i < graph.vertices.size(); i++) {
                if (graph.edges[currentIndex][i] != E{}) {
                    bool neighbourVisited = std::find(visited.begin(), visited.end(), graph.vertices[i]) != visited.end();
                    if (!neighbourVisited) unvisited.push(i);
                }
            }
        }
        return visited;
    }

    std::expected<bool, DataStructureError> hasPath(V start, V end) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        CONTAINERIUM_TRY(endIndex, findVertexIndex(end));
        bool found = false;
        if (startIndex == endIndex) found = true;
        auto result = BFS(start, [&found, end, this](V vertex) { if (vertex == end) found = true; });
        return found;
    }

    std::expected<std::vector<V>, DataStructureError> topologicalSort() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (graph.graphType != GraphType::Directed) return std::unexpected(DataStructureError::InvalidOperation);
        std::vector<V> sorted;
        std::vector<size_t> inDegrees;
        std::queue<V> unvisited;
        for (V vertex : graph.vertices) {
            CONTAINERIUM_TRY(inDegree, getInDegree(vertex));
            inDegrees.push_back(inDegree);
            if (inDegree == 0) unvisited.push(vertex);
        }
        while (!unvisited.empty()) {
            V vertex = unvisited.front();
            unvisited.pop();
            sorted.push_back(vertex);
            CONTAINERIUM_TRY(index, findVertexIndex(vertex));
            for (size_t i = 0; i < graph.vertices.size(); i++) {
                if (graph.edges[index][i] != E{}) {
                    inDegrees[i]--;
                    if (inDegrees[i] == 0) unvisited.push(graph.vertices[i]);
                }
            }
        }
        if (sorted.size() != graph.vertices.size()) return std::unexpected(DataStructureError::CycleDetected);
        return sorted;
    }

    std::expected<bool, DataStructureError> hasCycle() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        CONTAINERIUM_TRY(vertexCount, getVertexCount());
        if (graph.graphType == GraphType::Directed) {
            UnionFindSet<V> uf(graph.vertices);
            for (size_t i = 0; i < vertexCount; i++) {
                for (size_t j = 0; j < vertexCount; j++) {
                    if (graph.edges[i][j] != E{}) {
                        CONTAINERIUM_TRY(isConnected, uf.isConnected(graph.vertices[i], graph.vertices[j]));
                        if (isConnected) return true;
                        CONTAINERIUM_TRY(_, uf.unionSet(graph.vertices[i], graph.vertices[j]));
                    }
                }
            }
            return false;
        }
        else {
            CONTAINERIUM_TRY(_, topologicalSort());
            return false;
        }
    }

    std::expected<std::vector<E>, DataStructureError> Dijkstra(V start) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (graph.graphType != GraphType::Directed) return std::unexpected(DataStructureError::InvalidOperation);
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        const size_t vertexCount = graph.vertices.size();
        std::vector<E> distances(graph.vertices.size(), std::numeric_limits<E>::max());
        std::vector<bool> processed(graph.vertices.size(), false);
        auto cmp = [&distances](size_t a, size_t b) { return distances[a] > distances[b]; };
        std::priority_queue<size_t, std::vector<size_t>, decltype(cmp)> unvisited(cmp);
        distances[startIndex] = 0;
        unvisited.push(startIndex);
        while (!unvisited.empty()) {
            size_t current = unvisited.top();
            unvisited.pop();
            if (processed[current]) continue;
            processed[current] = true;
            for (size_t neighbour = 0; neighbour < vertexCount; neighbour++) {
                if (graph.edges[current][neighbour] != E{}) {
                    E newDistance = distances[current] + graph.edges[current][neighbour];
                    if (newDistance < distances[neighbour]) {
                        distances[neighbour] = newDistance;
                        unvisited.push(neighbour);
                    }
                }
            }
        }
        return distances;
    }

    std::expected<std::vector<std::vector<E>>, DataStructureError> floyd() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        CONTAINERIUM_TRY(vertexCount, getVertexCount());
        std::vector<std::vector<E>> distances(vertexCount, std::vector<E>(vertexCount));
        for (size_t i = 0; i < vertexCount; i++) {
            for (size_t j = 0; j < vertexCount; j++) {
                if (i == j) distances[i][j] = 0;
                else if (graph.edges[i][j] != E{}) distances[i][j] = graph.edges[i][j];
                else distances[i][j] = std::numeric_limits<E>::max();
            }
        }
        for (size_t k = 0; k < vertexCount; k++) {
            for (size_t i = 0; i < vertexCount; i++) {
                for (size_t j = 0; j < vertexCount; j++) {
                    if (distances[i][k] != std::numeric_limits<E>::max() && distances[k][j] != std::numeric_limits<E>::max()) {
                        distances[i][j] = std::min(distances[i][j], distances[i][k] + distances[k][j]);
                    }
                }
            }
        }
        return distances;
    }
    
    std::expected<bool, DataStructureError> isConnected() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        V start = graph.vertices[0];
        size_t visitedCount = 0;
        auto result = BFS(start, [&visitedCount](V vertex) { visitedCount++; });
        CONTAINERIUM_TRY(vertexCount, getVertexCount());
        return visitedCount == vertexCount;
    }

    std::expected<Graph, DataStructureError> primMST(V start) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (graph.graphType != GraphType::Undirected) return std::unexpected(DataStructureError::InvalidOperation);
        CONTAINERIUM_TRY(startIndex, findVertexIndex(start));
        CONTAINERIUM_TRY(vertexCount, getVertexCount());
        Graph mst;
        mst.graphType = GraphType::Undirected;
        mst.vertices = graph.vertices;
        mst.edges = std::vector<std::vector<E>>(graph.vertices.size(), std::vector<E>(graph.vertices.size(), E{}));
        std::vector<bool> inMST(vertexCount, false);
        std::vector<E> minEdge(vertexCount, std::numeric_limits<E>::max());
        std::vector<V> parent(vertexCount, std::numeric_limits<V>::max());
        minEdge[startIndex] = 0;
        for (size_t i = 0; i < vertexCount; i++) {
            size_t u = vertexCount;
            E minWeight = std::numeric_limits<E>::max();
            for (size_t j = 0; j < vertexCount; j++) {
                if (!inMST[j] && minEdge[j] < minWeight) {
                    minWeight = minEdge[j];
                    u = j;
                }
            }
            if (u == vertexCount) return std::unexpected(DataStructureError::InvalidOperation);
            inMST[u] = true;
            for (size_t v = 0; v < vertexCount; v++) {
                if (graph.edges[u][v] != E{} && !inMST[v] && graph.edges[u][v] < minEdge[v]) {
                    minEdge[v] = graph.edges[u][v];
                    parent[v] = graph.vertices[u];
                }
            }
        }
        for (size_t i = 0; i < vertexCount; i++) {
            if (parent[i] != std::numeric_limits<V>::max()) {
                CONTAINERIUM_TRY(parentIndex, findVertexIndex(parent[i]));
                mst.edges[parentIndex][i] = minEdge[i];
                mst.edges[i][parentIndex] = minEdge[i];
            }
        }
        return mst;
    }

    std::expected<Graph, DataStructureError> kruskalMST() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (graph.graphType != GraphType::Undirected) return std::unexpected(DataStructureError::InvalidOperation);
        std::vector<Edge> allEdges;
        CONTAINERIUM_TRY(vertexCount, getVertexCount());
        for (size_t i = 0; i < vertexCount; i++) {
            for (size_t j = i + 1; j < vertexCount; j++) {
                if (graph.edges[i][j] != E{}) allEdges.push_back({i, j, graph.edges[i][j]});
            }
        }
        std::sort(allEdges.begin(), allEdges.end(), [](const Edge& a, const Edge& b) { return a.weight < b.weight; });
        UnionFindSet<V> uf(graph.vertices);
        Graph mst;
        mst.graphType = GraphType::Undirected;
        mst.vertices = graph.vertices;
        mst.edges = std::vector<std::vector<E>>(graph.vertices.size(), std::vector<E>(graph.vertices.size(), E{}));
        int edgesAdded = 0;
        int requiredEdges = vertexCount - 1;
        for (const auto& edge : allEdges) {
            V startVertex = graph.vertices[edge.start];
            V endVertex = graph.vertices[edge.end];
            CONTAINERIUM_TRY(isConnected, uf.isConnected(startVertex, endVertex));
            if (!isConnected) {
                auto unionResult = uf.unionSet(startVertex, endVertex);
                if (!unionResult.has_value()) return std::unexpected(unionResult.error());
                CONTAINERIUM_TRY(startIndex, findVertexIndex(startVertex));
                CONTAINERIUM_TRY(endIndex, findVertexIndex(endVertex));
                mst.edges[startIndex][endIndex] = edge.weight;
                mst.edges[endIndex][startIndex] = edge.weight;
                edgesAdded++;
                if (edgesAdded == requiredEdges) break;
            }
        }
        if (edgesAdded != requiredEdges) return std::unexpected(DataStructureError::InvalidOperation);
        return mst;
    }

    std::expected<void, DataStructureError> printAdjacencyMatrixGraph() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        Graph printGraph;
        std::cout << "Adjacency Matrix Graph:" << std::endl;
        std::cout << "Vertices: " << std::endl;
        for (const auto& vertex : graph.vertices) std::cout << vertex << " ";
        std::cout << std::endl;
        std::cout << "Edges:" << std::endl;
        for (size_t i = 0; i < graph.vertices.size(); i++) {
            for (size_t j = 0; j < graph.vertices.size(); j++) {
                std::cout << graph.edges[i][j] << " ";
            }
            std::cout << std::endl;
        }
        return {};
    }

    void clear() {
        graph.vertices.clear();
        graph.edges.clear();
    }
};

} // namespace containerium
