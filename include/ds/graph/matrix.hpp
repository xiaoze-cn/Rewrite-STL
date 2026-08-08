#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <stack>
#include <type_traits>
#include <vector>
#include <ds/common/error.hpp>
#include <ds/set/unionfind.hpp>

namespace ds {

template<typename V, typename E>
class AdjacencyMatrixGraph {
public:
    enum class GraphType {
        Directed,
        Undirected
    };
    struct Graph {
        std::vector<V> vertices;
        std::vector<std::vector<std::optional<E>>> edges;
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
    explicit AdjacencyMatrixGraph(GraphType type = GraphType::Directed) : graph{{}, {}, type} {}

    ~AdjacencyMatrixGraph() = default;

    bool isEmpty() const { return graph.vertices.empty(); }

    bool hasVertex(V vertex) const { return findVertexIndex(vertex).has_value(); }

    std::expected<bool, DataStructureError> hasEdge(V start, V end) const {
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
        return graph.edges[startIndex][endIndex].has_value();
    }

    std::expected<E, DataStructureError> getEdge(V start, V end) const {
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
        const auto& edge = graph.edges[startIndex][endIndex];
        if (!edge.has_value()) return std::unexpected(DataStructureError::ElementNotFound);
        return edge.value();
    }

    std::expected<std::vector<V>, DataStructureError> getVertices(std::function<void(V)> visitor) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        for (const auto& vertex : graph.vertices) visitor(vertex);
        return graph.vertices;
    }

    std::expected<std::vector<V>, DataStructureError> getNeighbours(V vertex, std::function<void(V)> visitor) const {
        DS_TRY(index, findVertexIndex(vertex));
        std::vector<V> neighbours;
        for (size_t i = 0; i < graph.vertices.size(); i++) if (graph.edges[index][i].has_value()) neighbours.push_back(graph.vertices[i]);
        for (const auto& neighbour : neighbours) visitor(neighbour);
        return neighbours;
    }

    std::expected<size_t, DataStructureError> getDegree(V vertex) const {
        DS_TRY(index, findVertexIndex(vertex));
        if (graph.graphType == GraphType::Undirected) {
            size_t degree = 0;
            for (size_t i = 0; i < graph.edges[index].size(); ++i) {
                if (graph.edges[index][i].has_value()) degree += (i == index ? 2 : 1);
            }
            return degree;
        }
        else {
            DS_TRY(inDegree, getInDegree(vertex));
            DS_TRY(outDegree, getOutDegree(vertex));
            return inDegree + outDegree;
        }
    }

    std::expected<size_t, DataStructureError> getInDegree(V vertex) const {
        if (graph.graphType == GraphType::Undirected) return getDegree(vertex);    // In undirected graph, degree is equal to in-degree and out-degree
        DS_TRY(index, findVertexIndex(vertex));
        size_t inDegree = 0;
        for(size_t i = 0; i < graph.vertices.size(); i++) if (graph.edges[i][index].has_value()) inDegree++;
        return inDegree;
    }

    std::expected<size_t, DataStructureError> getOutDegree(V vertex) const {
        if (graph.graphType == GraphType::Undirected) return getDegree(vertex);
        DS_TRY(index, findVertexIndex(vertex));
        size_t outDegree = 0;
        for (const auto& edge : graph.edges[index]) if (edge.has_value()) outDegree++;
        return outDegree;
    }

    std::expected<void, DataStructureError> addVertex(V vertex) {
        if (findVertexIndex(vertex).has_value()) return std::unexpected(DataStructureError::DuplicateValue);
        graph.vertices.push_back(vertex);
        for (auto& row : graph.edges) row.push_back(std::nullopt);
        graph.edges.push_back(std::vector<std::optional<E>>(graph.vertices.size(), std::nullopt));
        return {};
    }
    
    std::expected<void, DataStructureError> removeVertex(V vertex) {
        DS_TRY(index, findVertexIndex(vertex));
        graph.vertices.erase(graph.vertices.begin() + index);
        graph.edges.erase(graph.edges.begin() + index);
        for (auto& row : graph.edges) row.erase(row.begin() + index);
        return {};
    }

    std::expected<void, DataStructureError> addEdge(V start, V end, E edge) {
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
        if (graph.edges[startIndex][endIndex].has_value()) return std::unexpected(DataStructureError::DuplicateValue);
        graph.edges[startIndex][endIndex] = edge;
        if (graph.graphType == GraphType::Undirected) graph.edges[endIndex][startIndex] = edge;
        return {};
    }

    std::expected<void, DataStructureError> removeEdge(V start, V end) {
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
        if (!graph.edges[startIndex][endIndex].has_value()) return std::unexpected(DataStructureError::ElementNotFound);
        graph.edges[startIndex][endIndex] = std::nullopt;
        if (graph.graphType == GraphType::Undirected) graph.edges[endIndex][startIndex] = std::nullopt;
        return {};
    }

    std::expected<size_t, DataStructureError> getVertexCount() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return graph.vertices.size();
    }

    std::expected<size_t, DataStructureError> getEdgeCount() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        size_t edgeCount = 0;
        for (size_t i = 0; i < graph.edges.size(); ++i) {
            const size_t start = graph.graphType == GraphType::Undirected ? i : 0;
            for (size_t j = start; j < graph.edges[i].size(); ++j) {
                if (graph.edges[i][j].has_value()) edgeCount++;
            }
        }
        return edgeCount;
    }

    std::expected<std::vector<V>, DataStructureError> DFSRecursive(V start, std::function<void(V)> visitor) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        DS_TRY(startIndex, findVertexIndex(start));
        std::vector<V> visited;
        std::function<std::expected<void, DataStructureError>(size_t)> DFS = [&](size_t index) -> std::expected<void, DataStructureError> {
            visited.push_back(graph.vertices[index]);
            visitor(graph.vertices[index]);
            DS_TRY(vertexCount, getVertexCount());
            for (size_t i = 0; i < vertexCount; ++i) {
                if (graph.edges[index][i].has_value()) {
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
        DS_TRY(startIndex, findVertexIndex(start));
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
            DS_TRY(vertexCount, getVertexCount());
            for (size_t i = 0; i < vertexCount; i++) {
                if (graph.edges[currentIndex][i].has_value()) {
                    bool neighbourVisited = std::find(visited.begin(), visited.end(), graph.vertices[i]) != visited.end();
                    if (!neighbourVisited) unvisited.push(i);
                }
            }
        }
        return visited;
    }

    std::expected<std::vector<V>, DataStructureError> BFS(V start, std::function<void(V)> visitor) const {
    if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        DS_TRY(startIndex, findVertexIndex(start));
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
            DS_TRY(vertexCount, getVertexCount());
            for (size_t i = 0; i < graph.vertices.size(); i++) {
                if (graph.edges[currentIndex][i].has_value()) {
                    bool neighbourVisited = std::find(visited.begin(), visited.end(), graph.vertices[i]) != visited.end();
                    if (!neighbourVisited) unvisited.push(i);
                }
            }
        }
        return visited;
    }

    std::expected<bool, DataStructureError> hasPath(V start, V end) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
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
            DS_TRY(inDegree, getInDegree(vertex));
            inDegrees.push_back(inDegree);
            if (inDegree == 0) unvisited.push(vertex);
        }
        while (!unvisited.empty()) {
            V vertex = unvisited.front();
            unvisited.pop();
            sorted.push_back(vertex);
            DS_TRY(index, findVertexIndex(vertex));
            for (size_t i = 0; i < graph.vertices.size(); i++) {
                if (graph.edges[index][i].has_value()) {
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
        const size_t vertexCount = graph.vertices.size();
        if (graph.graphType == GraphType::Directed) {
            std::vector<int> state(vertexCount, 0);
            std::function<bool(size_t)> visit = [&](size_t vertex) {
                state[vertex] = 1;
                for (size_t neighbour = 0; neighbour < vertexCount; ++neighbour) {
                    if (!graph.edges[vertex][neighbour].has_value()) continue;
                    if (state[neighbour] == 1) return true;
                    if (state[neighbour] == 0 && visit(neighbour)) return true;
                }
                state[vertex] = 2;
                return false;
            };
            for (size_t vertex = 0; vertex < vertexCount; ++vertex) {
                if (state[vertex] == 0 && visit(vertex)) return true;
            }
            return false;
        }

        UnionFindSet<V> uf(graph.vertices);
        for (size_t i = 0; i < vertexCount; ++i) {
            if (graph.edges[i][i].has_value()) return true;
            for (size_t j = i + 1; j < vertexCount; ++j) {
                if (graph.edges[i][j].has_value()) {
                    DS_TRY(connected, uf.isConnected(graph.vertices[i], graph.vertices[j]));
                    if (connected) return true;
                    auto unionResult = uf.unionSet(graph.vertices[i], graph.vertices[j]);
                    if (!unionResult.has_value()) return std::unexpected(unionResult.error());
                }
            }
        }
        return false;
    }

    std::expected<std::vector<E>, DataStructureError> Dijkstra(V start) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        DS_TRY(startIndex, findVertexIndex(start));
        const size_t vertexCount = graph.vertices.size();
        if constexpr (std::is_arithmetic_v<E> && std::is_signed_v<E>) {
            for (const auto& row : graph.edges) {
                for (const auto& edge : row) {
                    if (edge.has_value() && edge.value() < E{}) return std::unexpected(DataStructureError::InvalidArgument);
                }
            }
        }
        std::vector<E> distances(graph.vertices.size(), std::numeric_limits<E>::max());
        std::vector<bool> processed(graph.vertices.size(), false);
        auto cmp = [&distances](size_t a, size_t b) { return distances[a] > distances[b]; };
        std::priority_queue<size_t, std::vector<size_t>, decltype(cmp)> unvisited(cmp);
        distances[startIndex] = E{};
        unvisited.push(startIndex);
        while (!unvisited.empty()) {
            size_t current = unvisited.top();
            unvisited.pop();
            if (processed[current]) continue;
            processed[current] = true;
            for (size_t neighbour = 0; neighbour < vertexCount; neighbour++) {
                if (graph.edges[current][neighbour].has_value()) {
                    const E& weight = graph.edges[current][neighbour].value();
                    if constexpr (std::is_arithmetic_v<E>) {
                        if (distances[current] > std::numeric_limits<E>::max() - weight) continue;
                    }
                    E newDistance = distances[current] + weight;
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
        DS_TRY(vertexCount, getVertexCount());
        std::vector<std::vector<E>> distances(vertexCount, std::vector<E>(vertexCount));
        for (size_t i = 0; i < vertexCount; i++) {
            for (size_t j = 0; j < vertexCount; j++) {
                if (i == j) distances[i][j] = 0;
                else if (graph.edges[i][j].has_value()) distances[i][j] = graph.edges[i][j].value();
                else distances[i][j] = std::numeric_limits<E>::max();
            }
        }
        for (size_t k = 0; k < vertexCount; k++) {
            for (size_t i = 0; i < vertexCount; i++) {
                for (size_t j = 0; j < vertexCount; j++) {
                    if (distances[i][k] != std::numeric_limits<E>::max() && distances[k][j] != std::numeric_limits<E>::max()) {
                        if constexpr (std::is_integral_v<E>) {
                            const E left = distances[i][k];
                            const E right = distances[k][j];
                            if (right > E{} && left > std::numeric_limits<E>::max() - right) continue;
                            if constexpr (std::is_signed_v<E>) {
                                if (right < E{} && left < std::numeric_limits<E>::lowest() - right) continue;
                            }
                        }
                        distances[i][j] = std::min(distances[i][j], static_cast<E>(distances[i][k] + distances[k][j]));
                    }
                }
            }
        }
        return distances;
    }
    
    std::expected<bool, DataStructureError> isConnected() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        const size_t vertexCount = graph.vertices.size();
        const auto reachableFrom = [&](size_t start) {
            std::vector<bool> visited(vertexCount, false);
            std::queue<size_t> pending;
            pending.push(start);
            visited[start] = true;
            size_t count = 0;
            while (!pending.empty()) {
                size_t current = pending.front();
                pending.pop();
                ++count;
                for (size_t neighbour = 0; neighbour < vertexCount; ++neighbour) {
                    if (graph.edges[current][neighbour].has_value() && !visited[neighbour]) {
                        visited[neighbour] = true;
                        pending.push(neighbour);
                    }
                }
            }
            return count;
        };
        if (graph.graphType == GraphType::Undirected) return reachableFrom(0) == vertexCount;
        for (size_t start = 0; start < vertexCount; ++start) {
            if (reachableFrom(start) != vertexCount) return false;
        }
        return true;
    }

    std::expected<Graph, DataStructureError> primMST(V start) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (graph.graphType != GraphType::Undirected) return std::unexpected(DataStructureError::InvalidOperation);
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(vertexCount, getVertexCount());
        Graph mst;
        mst.graphType = GraphType::Undirected;
        mst.vertices = graph.vertices;
        mst.edges = std::vector<std::vector<std::optional<E>>>(vertexCount, std::vector<std::optional<E>>(vertexCount, std::nullopt));
        std::vector<bool> inMST(vertexCount, false);
        std::vector<std::optional<E>> minEdge(vertexCount, std::nullopt);
        std::vector<std::optional<size_t>> parent(vertexCount, std::nullopt);
        minEdge[startIndex] = E{};
        for (size_t i = 0; i < vertexCount; i++) {
            size_t u = vertexCount;
            for (size_t j = 0; j < vertexCount; j++) {
                if (!inMST[j] && minEdge[j].has_value() &&
                    (u == vertexCount || minEdge[j].value() < minEdge[u].value())) {
                    u = j;
                }
            }
            if (u == vertexCount) return std::unexpected(DataStructureError::InvalidOperation);
            inMST[u] = true;
            for (size_t v = 0; v < vertexCount; v++) {
                if (graph.edges[u][v].has_value() && !inMST[v] &&
                    (!minEdge[v].has_value() || graph.edges[u][v].value() < minEdge[v].value())) {
                    minEdge[v] = graph.edges[u][v].value();
                    parent[v] = u;
                }
            }
        }
        for (size_t i = 0; i < vertexCount; i++) {
            if (parent[i].has_value()) {
                size_t parentIndex = parent[i].value();
                mst.edges[parentIndex][i] = minEdge[i].value();
                mst.edges[i][parentIndex] = minEdge[i].value();
            }
        }
        return mst;
    }

    std::expected<Graph, DataStructureError> kruskalMST() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (graph.graphType != GraphType::Undirected) return std::unexpected(DataStructureError::InvalidOperation);
        std::vector<Edge> allEdges;
        DS_TRY(vertexCount, getVertexCount());
        for (size_t i = 0; i < vertexCount; i++) {
            for (size_t j = i + 1; j < vertexCount; j++) {
                if (graph.edges[i][j].has_value()) allEdges.push_back({i, j, graph.edges[i][j].value()});
            }
        }
        std::sort(allEdges.begin(), allEdges.end(), [](const Edge& a, const Edge& b) { return a.weight < b.weight; });
        UnionFindSet<V> uf(graph.vertices);
        Graph mst;
        mst.graphType = GraphType::Undirected;
        mst.vertices = graph.vertices;
        mst.edges = std::vector<std::vector<std::optional<E>>>(vertexCount, std::vector<std::optional<E>>(vertexCount, std::nullopt));
        size_t edgesAdded = 0;
        size_t requiredEdges = vertexCount - 1;
        for (const auto& edge : allEdges) {
            V startVertex = graph.vertices[edge.start];
            V endVertex = graph.vertices[edge.end];
            DS_TRY(isConnected, uf.isConnected(startVertex, endVertex));
            if (!isConnected) {
                auto unionResult = uf.unionSet(startVertex, endVertex);
                if (!unionResult.has_value()) return std::unexpected(unionResult.error());
                DS_TRY(startIndex, findVertexIndex(startVertex));
                DS_TRY(endIndex, findVertexIndex(endVertex));
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
        std::cout << "Adjacency Matrix Graph:" << std::endl;
        std::cout << "Vertices: " << std::endl;
        for (const auto& vertex : graph.vertices) std::cout << vertex << " ";
        std::cout << std::endl;
        std::cout << "Edges:" << std::endl;
        for (size_t i = 0; i < graph.vertices.size(); i++) {
            for (size_t j = 0; j < graph.vertices.size(); j++) {
                if (graph.edges[i][j].has_value()) std::cout << graph.edges[i][j].value() << " ";
                else std::cout << ". ";
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

} // namespace ds
