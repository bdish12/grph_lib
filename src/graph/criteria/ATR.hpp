#ifndef GRPH_ATR_HPP
#define GRPH_ATR_HPP

#include <vector>
#include <set>
#include <list>
#include <queue>

#include "../common/AdjacencyMatrix.hpp"
#include "../common/EdgesList.hpp"
#include "../common/types.hpp"
#include "../../exceptions/graph/exceptions.h"
#include "../common/DegreesVector.hpp"
#include "../random/undirected_graph/UndirectedRandomGraph.hpp"
#include "../random/IRandomGraph.hpp"

#define COMPUTABLE_DIMENSION 5

namespace grph::graph::random {

    class ATR {
    public:
        static double calculateATR(const AdjacencyMatrix<EdgeProbability> &adjacencyMatrix) {
            IRandomGraph &&graph = UndirectedRandomGraph(adjacencyMatrix.copy());

            if (graph.isTree()) {
                return graph.calculateTreeATR();
            }

            const auto &foundConnectivityComponent = findConnectivityComponent(graph);
            if (!isConnectedGraph(graph, foundConnectivityComponent)) {
                return 0;
            }

            if (graph.isCycle()) {
                return graph.calculateCycleATR();
            }

            if (graph.getNumVertexes() <= COMPUTABLE_DIMENSION) {
                return calculateComputableDimensionGraph(graph);
            }

            double R = prepareForReduction(graph);
            if (graph.getNumVertexes() <= COMPUTABLE_DIMENSION) {
                return R * calculateComputableDimensionGraph(graph);
            }

            return R * removeEdgeATRStep(graph);
        }

        static std::unordered_set<int> findConnectivityComponent(const IRandomGraph &graph) {
            return bfs(graph.getAdjacencyMatrix());
        }

        static Chain findResolvingChain(const IRandomGraph &graph) {
            std::list<int> foundChain;
            double sumInverseP = 0;
            double pProduct = 1;

            const auto begin = graph.getDegreesVector()->findFirstVertexThatDegreeEqualTwo();
            if (begin == VERTEX_NOT_FOUND) {
                return {};
            }

            auto prevVertex = begin;
            auto curVertex = begin;
            do {
                foundChain.push_front(graph.getAdjacencyMatrix()->findNeighborWithLowestNumFor(curVertex,
                                                                                               {prevVertex}));
                prevVertex = curVertex;
                curVertex = foundChain.front();

                auto curEdgeVal = graph.at(prevVertex, curVertex);
                sumInverseP += 1 / curEdgeVal;
                pProduct *= curEdgeVal;
            } while (graph.getVertexDegree(curVertex) == 2);

            prevVertex = begin;
            curVertex = begin;
            do {
                foundChain.push_back(graph.getAdjacencyMatrix()->findNeighborWithBiggestNumFor(curVertex,
                                                                                               {prevVertex}));
                prevVertex = curVertex;
                curVertex = foundChain.back();

                auto curEdgeVal = graph.at(prevVertex, curVertex);
                sumInverseP += 1 / curEdgeVal;
                pProduct *= curEdgeVal;
            } while (graph.getVertexDegree(curVertex) == 2);

            return Chain(foundChain, sumInverseP, pProduct, begin);
        }

    private:
        static double removeEdgeATRStep(IRandomGraph &graph) {
            const auto &removeEdgeAction = graph.removeRandomEdge();

            auto foundConnectivityComponent = findConnectivityComponent(graph);
            bool isRemovedEdgeBridge = !isConnectedGraph(graph, foundConnectivityComponent);

            double R = 0.;
            if (isRemovedEdgeBridge) {
                const auto &inverseComponent = graph.getInverseComponent(foundConnectivityComponent);
                R = removeEdgeAction->getEdgeProbability()
                    * calculateComponentATR(graph, foundConnectivityComponent)
                    * calculateComponentATR(graph, inverseComponent);
            } else {
                R = (1 - removeEdgeAction->getEdgeProbability()) * calculateInternalATR(graph);

                const auto &pullAction = graph.pullEdge(
                        removeEdgeAction->getRemovedEdge().minVertex(),
                        removeEdgeAction->getRemovedEdge().maxVertex()
                );
                R += removeEdgeAction->getEdgeProbability() * calculateInternalATR(graph);
                pullAction->rollback();
            }
            removeEdgeAction->rollback();
            return R;
        }

        static double prepareForReduction(IRandomGraph &graph) {
            double R = 1;
            R *= removeHangingVertexes(graph);

            Chain foundChain;
            while (graph.getNumVertexes() > COMPUTABLE_DIMENSION && !(foundChain = findResolvingChain(graph)).empty()) {
                R *= graph.removeChain(foundChain);

                if (graph.containsHangingVertex()) {
                    throw std::runtime_error("ERROR: have hanging vertex");
                }
            }
            return R;
        }

        static double removeHangingVertexes(IRandomGraph &graph) {
            double R = 1;
            while (graph.containsHangingVertex()) {
                R *= graph.removeHangingVertexWithLowestIndex();
            }
            return R;
        }

        static std::unordered_set<int> bfs(const std::shared_ptr<AdjacencyMatrix<EdgeProbability>> &adjacencyMatrix) {
            if (adjacencyMatrix->getDimension() == 0) {
                return {};
            }
            std::unordered_set<int> connectivityComponent;
            std::vector<bool> processed(adjacencyMatrix->getDimension(), false);
            std::queue<int> vertexesToProcess;

            connectivityComponent.insert(0);
            vertexesToProcess.push(0);
            while (!vertexesToProcess.empty()) {
                const auto curVertexToProcess = vertexesToProcess.front();
                vertexesToProcess.pop();
                for (int vertex = 0; vertex < adjacencyMatrix->getDimension(); ++vertex) {
                    if (!processed[vertex]
                        && adjacencyMatrix->isConnected(curVertexToProcess, vertex)) {
                        connectivityComponent.insert(vertex);
                        vertexesToProcess.push(vertex);
                    }
                }
                processed[curVertexToProcess] = true;
            }
            return connectivityComponent;
        }

        static double calculateComponentATR(IRandomGraph &graph,
                                            const std::unordered_set<int> &foundConnectivityComponent) {
            const auto prevMatrix = graph.getAdjacencyMatrix();
            const auto prevDegreesVector = graph.getDegreesVector();

            const auto subgraphMatrix = graph.getSubgraphAdjacencyMatrix(foundConnectivityComponent);
            const auto subgraphDegreesVector = DegreesVector::make(subgraphMatrix);

            graph.setMatrixAndVector(subgraphMatrix, subgraphDegreesVector);
            double R = calculateInternalATR(graph);
            graph.setMatrixAndVector(prevMatrix, prevDegreesVector);

            return R;
        }

        static double calculateInternalATR(IRandomGraph &graph) {
            if (graph.getNumVertexes() <= COMPUTABLE_DIMENSION) {
                return calculateComputableDimensionGraph(graph);
            }

            double R = 0.;
            const Chain &trivialChain = findResolvingChain(graph);
            if (!trivialChain.empty()) { // Тут не появится висячая вершина
                const auto &removeTrivialChainAction = graph.removeTrivialChain(trivialChain);
                R = trivialChain.getChainFactor() * calculateInternalATR(graph);
                removeTrivialChainAction->rollback();
            } else {
                R = removeEdgeATRStep(graph);
            }
            return R;
        }

        static double calculateComputableDimensionGraph(const IRandomGraph &graph) {
            if (graph.getNumVertexes() == 0 || graph.getNumVertexes() == 1) {
                return 1.;
            } else if (graph.getNumVertexes() == 2) {
                return graph.at(0, 1);
            } else if (graph.getNumVertexes() == 3) {
                double p12 = graph.at(0, 1);
                double p13 = graph.at(0, 2);
                double p23 = graph.at(1, 2);
                return p12 * p13 * p23
                       + (1 - p12) * p13 * p23
                       + p12 * (1 - p13) * p23
                       + p12 * p13 * (1 - p23);
            } else if (graph.getNumVertexes() == 4) {
                double _a = (1 - graph.at(0, 1));
                double _b = (1 - graph.at(1, 2));
                double _c = (1 - graph.at(2, 3));
                double _d = (1 - graph.at(0, 3));
                double _e = (1 - graph.at(1, 3));
                double _f = (1 - graph.at(0, 2));
                return 1
                       - 6 * _a * _b * _c * _d * _e * _f
                       - _a * _b * _e
                       - _a * _d * _f
                       - _b * _c * _f
                       - _c * _d * _e
                       + 2 * (_b * _d * _e * _f * (_a + _c - 0.5)
                              + _a * _c * _e * _f * (_b + _d - 0.5)
                              + _a * _b * _c * _d * (_e + _f - 0.5));
            } else if (graph.getNumVertexes() == 5) {
                double a = graph.at(0, 1);
                double b = graph.at(0, 2);
                double c = graph.at(0, 3);
                double d = graph.at(0, 4);
                double e = graph.at(1, 2);
                double f = graph.at(1, 3);
                double g = graph.at(1, 4);
                double h = graph.at(2, 3);
                double u = graph.at(2, 4);
                double v = graph.at(3, 4);

                double _a = 1 - a;
                double _b = 1 - b;
                double _c = 1 - c;
                double _d = 1 - d;
                double _e = 1 - e;
                double _f = 1 - f;
                double _g = 1 - g;
                double _h = 1 - h;
                double _u = 1 - u;
                double _v = 1 - v;

                double k_1 = 1 - _e * (_f * _g + _h * _u);
                double k_2 = 1 - _h * (_b * _u + _c * _v);
                double k_3 = 1 - _v * (_c * _f + _d * _g);
                double k_4 = 1 - _d * (_a * _b + _g * _u);
                double k_5 = 1 - _a * (_b * _c + _e * _f);
                double k_6 = a * h * u + a * v * (h * _u + _h * u) + _a * _h * _u * (1 - 4 * _v);
                double k_7 = c * d * e + e * v * (c * _d + _c * d) + _d * _e * _v;
                double k_8 = a * d * h + g * h * (a * _d + _a * d) + _a * _h * _g;
                double k_9 = a * b * v + e * v * (a * _b + _a * b) + _a * _e * _v;
                double k_10 = e * d * f + d * h * (e * _f + _e * f) + _d * _f * _h;
                double k_11 = b * f * g + b * v * (f * _g + _f * g) + _b * _g * _v;
                double k_12 = c * e * g + c * u * (e * _g + _e * g) + _c * _e * _u;
                double k_13 = b * d * f + f * u * (b * _d + _b * d) + _b * _d * _f;
                double k_14 = b * c * g + g * h * (b * _c + _b * c) + _b * _c * _g;
                double k_15 = a * c * u + f * u * (a * _c + _a * c) + _c * _f * _u;

                return 1
                       - _b * _c * (_a * _d * k_1 + _f * _e * (_d * _g * k_6 + _u * _v * k_8))
                       - _f * _g * (_a * _e * k_2 + _h * _u * (_a * _b * k_7 + _c * _d * k_9))
                       - _b * _h * (_e * _u * k_3 + _d * _v * (_a * _f * k_12 + _e * _g * k_15))
                       - _c * _v * (_f * _h * k_4 + _a * _g * (_b * _u * k_10 + _e * _h * k_13))
                       - _d * _u * (_g * _v * k_5 + _a * _e * (_c * _h * k_11 + _f * _v * k_14));
            } else {
                throw std::runtime_error("ERROR: try calculate non_computable graph");
            }
        }

        static bool isConnectedGraph(const IRandomGraph &graph,
                                     const std::unordered_set<int> &foundConnectivityComponent) {
            return graph.getAdjacencyMatrix()->getDimension() == foundConnectivityComponent.size();
        }
    };

}

#endif //GRPH_ATR_HPP
