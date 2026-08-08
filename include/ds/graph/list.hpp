#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <queue>
#include <stack>
#include <vector>

#include <ds/common/error.hpp>

namespace ds {

template<typename V, typename E>
class AdjacencyListGraph {
public:
    enum class GraphType {
        Directed,
        Undirected
    };

    struct Edge {
        size_t end;
        E weight;
    };

    struct Graph {
        std::vector<V> vertices;
        std::vector<std::vector<Edge>> edges;
        GraphType graphType;
    };

protected:
    Graph graph;

    std::expected<size_t, DataStructureError> findVertexIndex(V vertex) const {
        for (size_t i = 0; i < graph.vertices.size(); i++) {
            if (graph.vertices[i] == vertex) return i;
        }
        return std::unexpected(DataStructureError::ElementNotFound);
    }

    std::optional<size_t> findEdgeIndex(size_t start, size_t end) const {
        const auto& edges = graph.edges[start];
        for (size_t i = 0; i < edges.size(); i++) {
            if (edges[i].end == end) return i;
        }
        return std::nullopt;
    }

public:
    explicit AdjacencyListGraph(GraphType type = GraphType::Directed) : graph{{}, {}, type} {}

    ~AdjacencyListGraph() = default;

    bool isEmpty() const { return graph.vertices.empty(); }

    bool hasVertex(V vertex) const { return findVertexIndex(vertex).has_value(); }

    std::expected<bool, DataStructureError> hasEdge(V start, V end) const {
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
        return findEdgeIndex(startIndex, endIndex).has_value();
    }

    std::expected<E, DataStructureError> getEdge(V start, V end) const {
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
        auto edgeIndex = findEdgeIndex(startIndex, endIndex);
        if (!edgeIndex.has_value()) return std::unexpected(DataStructureError::ElementNotFound);
        return graph.edges[startIndex][edgeIndex.value()].weight;
    }

    std::expected<std::vector<V>, DataStructureError> getVertices(std::function<void(V)> visitor) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        for (const auto& vertex : graph.vertices) visitor(vertex);
        return graph.vertices;
    }

    std::expected<std::vector<V>, DataStructureError> getNeighbours(V vertex, std::function<void(V)> visitor) const {
        DS_TRY(index, findVertexIndex(vertex));
        std::vector<V> neighbours;
        neighbours.reserve(graph.edges[index].size());
        for (const auto& edge : graph.edges[index]) neighbours.push_back(graph.vertices[edge.end]);
        for (const auto& neighbour : neighbours) visitor(neighbour);
        return neighbours;
    }

    std::expected<size_t, DataStructureError> getDegree(V vertex) const {
        if (graph.graphType == GraphType::Undirected) {
            DS_TRY(index, findVertexIndex(vertex));
            size_t degree = 0;
            for (const auto& edge : graph.edges[index]) degree += edge.end == index ? 2 : 1;
            return degree;
        }
        DS_TRY(inDegree, getInDegree(vertex));
        DS_TRY(outDegree, getOutDegree(vertex));
        return inDegree + outDegree;
    }

    std::expected<size_t, DataStructureError> getInDegree(V vertex) const {
        if (graph.graphType == GraphType::Undirected) return getDegree(vertex);
        DS_TRY(index, findVertexIndex(vertex));
        size_t degree = 0;
        for (const auto& edges : graph.edges) {
            for (const auto& edge : edges) if (edge.end == index) degree++;
        }
        return degree;
    }

    std::expected<size_t, DataStructureError> getOutDegree(V vertex) const {
        if (graph.graphType == GraphType::Undirected) return getDegree(vertex);
        DS_TRY(index, findVertexIndex(vertex));
        return graph.edges[index].size();
    }

    std::expected<void, DataStructureError> addVertex(V vertex) {
        if (findVertexIndex(vertex).has_value()) return std::unexpected(DataStructureError::DuplicateValue);
        graph.vertices.push_back(vertex);
        graph.edges.emplace_back();
        return {};
    }

    std::expected<void, DataStructureError> removeVertex(V vertex) {
        DS_TRY(index, findVertexIndex(vertex));
        graph.vertices.erase(graph.vertices.begin() + static_cast<std::ptrdiff_t>(index));
        graph.edges.erase(graph.edges.begin() + static_cast<std::ptrdiff_t>(index));
        for (auto& edges : graph.edges) {
            edges.erase(std::remove_if(edges.begin(), edges.end(), [index](const Edge& edge) { return edge.end == index; }), edges.end());
            for (auto& edge : edges) if (edge.end > index) edge.end--;
        }
        return {};
    }

    std::expected<void, DataStructureError> addEdge(V start, V end, E edge) {
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
        if (findEdgeIndex(startIndex, endIndex).has_value()) return std::unexpected(DataStructureError::DuplicateValue);
        graph.edges[startIndex].push_back(Edge{endIndex, edge});
        if (graph.graphType == GraphType::Undirected && startIndex != endIndex) {
            graph.edges[endIndex].push_back(Edge{startIndex, edge});
        }
        return {};
    }

    std::expected<void, DataStructureError> removeEdge(V start, V end) {
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
        auto edgeIndex = findEdgeIndex(startIndex, endIndex);
        if (!edgeIndex.has_value()) return std::unexpected(DataStructureError::ElementNotFound);
        auto& startEdges = graph.edges[startIndex];
        startEdges.erase(startEdges.begin() + static_cast<std::ptrdiff_t>(edgeIndex.value()));
        if (graph.graphType == GraphType::Undirected && startIndex != endIndex) {
            auto reverseIndex = findEdgeIndex(endIndex, startIndex);
            if (reverseIndex.has_value()) {
                auto& endEdges = graph.edges[endIndex];
                endEdges.erase(endEdges.begin() + static_cast<std::ptrdiff_t>(reverseIndex.value()));
            }
        }
        return {};
    }

    std::expected<size_t, DataStructureError> getVertexCount() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        return graph.vertices.size();
    }

    std::expected<size_t, DataStructureError> getEdgeCount() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        size_t edgeCount = 0;
        for (size_t start = 0; start < graph.edges.size(); start++) {
            for (const auto& edge : graph.edges[start]) {
                if (graph.graphType == GraphType::Directed || start <= edge.end) edgeCount++;
            }
        }
        return edgeCount;
    }

    std::expected<std::vector<V>, DataStructureError> DFSRecursive(V start, std::function<void(V)> visitor) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        DS_TRY(startIndex, findVertexIndex(start));
        std::vector<V> result;
        std::vector<bool> visited(graph.vertices.size(), false);
        std::function<void(size_t)> visit = [&](size_t current) {
            visited[current] = true;
            result.push_back(graph.vertices[current]);
            visitor(graph.vertices[current]);
            for (const auto& edge : graph.edges[current]) {
                if (!visited[edge.end]) visit(edge.end);
            }
        };
        visit(startIndex);
        return result;
    }

    std::expected<std::vector<V>, DataStructureError> DFSIterative(V start, std::function<void(V)> visitor) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        DS_TRY(startIndex, findVertexIndex(start));
        std::vector<V> result;
        std::vector<bool> visited(graph.vertices.size(), false);
        std::stack<size_t> pending;
        pending.push(startIndex);
        while (!pending.empty()) {
            size_t current = pending.top();
            pending.pop();
            if (visited[current]) continue;
            visited[current] = true;
            result.push_back(graph.vertices[current]);
            visitor(graph.vertices[current]);
            for (const auto& edge : graph.edges[current]) {
                if (!visited[edge.end]) pending.push(edge.end);
            }
        }
        return result;
    }

    std::expected<std::vector<V>, DataStructureError> BFS(V start, std::function<void(V)> visitor) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        DS_TRY(startIndex, findVertexIndex(start));
        std::vector<V> result;
        std::vector<bool> visited(graph.vertices.size(), false);
        std::queue<size_t> pending;
        pending.push(startIndex);
        visited[startIndex] = true;
        while (!pending.empty()) {
            size_t current = pending.front();
            pending.pop();
            result.push_back(graph.vertices[current]);
            visitor(graph.vertices[current]);
            for (const auto& edge : graph.edges[current]) {
                if (!visited[edge.end]) {
                    visited[edge.end] = true;
                    pending.push(edge.end);
                }
            }
        }
        return result;
    }

    std::expected<bool, DataStructureError> hasPath(V start, V end) const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        DS_TRY(startIndex, findVertexIndex(start));
        DS_TRY(endIndex, findVertexIndex(end));
        std::vector<bool> visited(graph.vertices.size(), false);
        std::queue<size_t> pending;
        pending.push(startIndex);
        visited[startIndex] = true;
        while (!pending.empty()) {
            size_t current = pending.front();
            pending.pop();
            if (current == endIndex) return true;
            for (const auto& edge : graph.edges[current]) {
                if (!visited[edge.end]) {
                    visited[edge.end] = true;
                    pending.push(edge.end);
                }
            }
        }
        return false;
    }

    std::expected<std::vector<V>, DataStructureError> topologicalSort() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (graph.graphType != GraphType::Directed) return std::unexpected(DataStructureError::InvalidOperation);
        std::vector<size_t> inDegrees(graph.vertices.size(), 0);
        for (const auto& edges : graph.edges) for (const auto& edge : edges) inDegrees[edge.end]++;
        std::queue<size_t> pending;
        for (size_t i = 0; i < inDegrees.size(); i++) if (inDegrees[i] == 0) pending.push(i);
        std::vector<V> sorted;
        while (!pending.empty()) {
            size_t current = pending.front();
            pending.pop();
            sorted.push_back(graph.vertices[current]);
            for (const auto& edge : graph.edges[current]) {
                inDegrees[edge.end]--;
                if (inDegrees[edge.end] == 0) pending.push(edge.end);
            }
        }
        if (sorted.size() != graph.vertices.size()) return std::unexpected(DataStructureError::CycleDetected);
        return sorted;
    }

    std::expected<bool, DataStructureError> hasCycle() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        if (graph.graphType == GraphType::Directed) {
            std::vector<int> state(graph.vertices.size(), 0);
            std::function<bool(size_t)> visit = [&](size_t current) {
                state[current] = 1;
                for (const auto& edge : graph.edges[current]) {
                    if (state[edge.end] == 1) return true;
                    if (state[edge.end] == 0 && visit(edge.end)) return true;
                }
                state[current] = 2;
                return false;
            };
            for (size_t i = 0; i < graph.vertices.size(); i++) {
                if (state[i] == 0 && visit(i)) return true;
            }
            return false;
        }

        std::vector<bool> visited(graph.vertices.size(), false);
        std::function<bool(size_t, std::optional<size_t>)> visit = [&](size_t current, std::optional<size_t> parent) {
            visited[current] = true;
            for (const auto& edge : graph.edges[current]) {
                if (edge.end == current) return true;
                if (!visited[edge.end]) {
                    if (visit(edge.end, current)) return true;
                }
                else if (!parent.has_value() || edge.end != parent.value()) return true;
            }
            return false;
        };
        for (size_t i = 0; i < graph.vertices.size(); i++) {
            if (!visited[i] && visit(i, std::nullopt)) return true;
        }
        return false;
    }

    std::expected<bool, DataStructureError> isConnected() const {
        if (isEmpty()) return std::unexpected(DataStructureError::ContainerIsEmpty);
        const auto reachableCount = [&](size_t start) {
            std::vector<bool> visited(graph.vertices.size(), false);
            std::queue<size_t> pending;
            pending.push(start);
            visited[start] = true;
            size_t count = 0;
            while (!pending.empty()) {
                size_t current = pending.front();
                pending.pop();
                count++;
                for (const auto& edge : graph.edges[current]) {
                    if (!visited[edge.end]) {
                        visited[edge.end] = true;
                        pending.push(edge.end);
                    }
                }
            }
            return count;
        };
        if (graph.graphType == GraphType::Undirected) return reachableCount(0) == graph.vertices.size();
        for (size_t i = 0; i < graph.vertices.size(); i++) {
            if (reachableCount(i) != graph.vertices.size()) return false;
        }
        return true;
    }

    void clear() {
        graph.vertices.clear();
        graph.edges.clear();
    }
};

} // namespace ds
