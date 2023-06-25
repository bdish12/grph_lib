#ifndef GRPH_UNDIRECTEDRANDOMGRAPH_HPP
#define GRPH_UNDIRECTEDRANDOMGRAPH_HPP

#include <memory>
#include <vector>
#include <set>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "../../common/AdjacencyMatrix.hpp"
#include "../../../exceptions/graph/exceptions.h"
#include "../../common/DegreesVector.hpp"
#include "../../common/types.hpp"
#include "../IRandomGraph.hpp"
#include "../Chain.hpp"

namespace grph::graph::random {

    class UndirectedRandomGraph : public IRandomGraph {
    public:
        explicit UndirectedRandomGraph()
                : _adjacencyMatrix(std::make_shared<AdjacencyMatrix<EdgeProbability>>(0)),
                  _degreesVector(DegreesVector::make(_adjacencyMatrix)) {}

        explicit UndirectedRandomGraph(const std::shared_ptr<AdjacencyMatrix<EdgeProbability>> &adjacencyMatrix)
                : _adjacencyMatrix(adjacencyMatrix),
                  _degreesVector(DegreesVector::make(adjacencyMatrix)) {};

        int getNumVertexes() const override;

        int getVertexDegree(int vertexNum) const override;

        const std::shared_ptr<DegreesVector> &getDegreesVector() const override;

        const std::shared_ptr<AdjacencyMatrix<EdgeProbability>> &getAdjacencyMatrix() const override;

        EdgeProbability at(int firstVertexNum, int secondVertexNum) const override;

        bool containsHangingVertex() const override;

        EdgeProbability removeHangingVertexWithLowestIndex() override;

        void restoreHangingVertexWithLowestIndex(int prevRemovedVertexNum);

        int findFirstVertexThatDegreeEqualTwo() const override;

        bool containsChain() const override;

        /// @return: chainFactor
        double removeChain(const Chain &chain) override;

        int getNumEdges() const override;

        bool isTree() const override;

        bool isCycle() const override;

        bool isConnected(int vertex1, int vertex2) const;

        double calculateTreeATR() const override;

        double calculateCycleATR() const override;

        std::shared_ptr<RemoveRandomEdgeAction> removeRandomEdge() override;

        // TODO: проверить правильность работы, если вершина с наибольшим номером
        // является частью окружения или одной из стягиваемых
        // TODO: а что происходит с удаляемым ребром?
        std::shared_ptr<PullEdgeAction> pullEdge(int remainingVertexNum, int vertexToRemoveNum) override;

        // TODO: проверить правильность работы, если вершина с наибольшим номером
        // является частью цепи
        std::shared_ptr<RemoveTrivialChainAction> removeTrivialChain(const Chain &trivialChain) override;

        std::shared_ptr<AdjacencyMatrix<EdgeProbability>> getSubgraphAdjacencyMatrix(
                const std::unordered_set<int> &subgraphVertexes
        ) const override;

        void setMatrixAndVector(
                const std::shared_ptr<AdjacencyMatrix<EdgeProbability>> &matrix,
                const std::shared_ptr<DegreesVector> &degreesVector
        ) override;

        std::unordered_set<int> getInverseComponent(
                const std::unordered_set<int> &connectivityComponent
        ) const override;

    private:
        std::shared_ptr<AdjacencyMatrix<EdgeProbability>> _adjacencyMatrix;
        std::shared_ptr<DegreesVector> _degreesVector;

        inline int getLastVertexNum() const {
            return _adjacencyMatrix->getDimension() - 1;
        }

        inline void addEdgeInternal(int from, int to, const EdgeProbability &value) {
            _adjacencyMatrix->setAt(value, from, to);
            _adjacencyMatrix->setAt(value, to, from);
        }

        inline void removeEdgeInternal(int from, int to) {
            _adjacencyMatrix->setAt(EMPTY, from, to);
            _adjacencyMatrix->setAt(EMPTY, to, from);
        }

        inline void renumberPairVertexes(int vertexNum1, int vertexNum2) {
            if (vertexNum1 == vertexNum2) {
                return;
            }
            _adjacencyMatrix->renumberPairOfVertices(vertexNum1, vertexNum2);
            _degreesVector->renumberPairOfVertexes(vertexNum1, vertexNum2);
        }

        inline void removeLastVertex() {
            _adjacencyMatrix->removeLastVertex();
        }

        inline void restoreLastVertex() {
            _adjacencyMatrix->restoreLastVertex();
        }

        void enrichWithEdges(int edgeSourceVertex,
                             int vertexToEnrich,
                             const std::vector<grph::graph::EdgeNode<EdgeProbability>> &edges);

        void replaceMultiEdge(int fromVertex, int toVertex, const EdgeProbability &parallelEdgeValue);

        void setSubgraphWithoutChainMatrixWithDegreesVector(const Chain &chain);

        int findNeighbor(int vertexNum) const;

    private:
        class RemoveRandomEdgeActionURG : public RemoveRandomEdgeAction {
        public:
            explicit RemoveRandomEdgeActionURG(
                    UndirectedRandomGraph &graph,
                    const std::shared_ptr<DegreesVector> &degreesVectorBeforeAction,
                    Edge removedEdge,
                    EdgeProbability edgeRatioValue
            ) : RemoveRandomEdgeAction(degreesVectorBeforeAction),
                _graph(graph),
                _removedEdge(removedEdge),
                _edgeRatioValue(edgeRatioValue) {};

            void rollback() override;

            Edge getRemovedEdge() const override;

            double getEdgeProbability() const override;

        private:
            UndirectedRandomGraph &_graph;
            Edge _removedEdge;
            EdgeProbability _edgeRatioValue;
        };

        class PullEdgeActionURG : public PullEdgeAction {
        public:
            explicit PullEdgeActionURG(
                    UndirectedRandomGraph &graph,
                    const std::shared_ptr<DegreesVector> &degreesVectorBeforeAction,
                    int prevRemovedVertexNum,
                    int remainingVertexNum,
                    const EdgeProbability removedEdgeValue,
                    const std::vector<EdgeNode<EdgeProbability>> &remainingVertexSnapshot
            ) : PullEdgeAction(degreesVectorBeforeAction),
                _graph(graph),
                _prevRemovedVertexNum(prevRemovedVertexNum),
                _remainingVertexNum(remainingVertexNum),
                _removedEdgeValue(removedEdgeValue),
                _remainingVertexSnapshot(remainingVertexSnapshot) {};

            void rollback() override;

        private:
            void removeAllEdgesForRemainingVertex();

            void restoreEdgesFromSnapshot();

            UndirectedRandomGraph &_graph;
            const std::vector<EdgeNode<EdgeProbability>> _remainingVertexSnapshot;
            int _prevRemovedVertexNum;
            int _remainingVertexNum;
            EdgeProbability _removedEdgeValue;
        };

        class RemoveTrivialChainActionURG : public RemoveTrivialChainAction {
        public:
            explicit RemoveTrivialChainActionURG(
                    UndirectedRandomGraph &graph,
                    const std::shared_ptr<DegreesVector> &degreesVectorBeforeAction,
                    const Chain &trivialChain,
                    const EdgeProbability stEdgeValue,
                    double chainFactor,
                    int middleVertexNum
            ) : RemoveTrivialChainAction(degreesVectorBeforeAction),
                _graph(graph),
                _trivialChain(trivialChain),
                _stEdgeValue(stEdgeValue),
                _chainFactor(chainFactor),
                _middleVertexNum(middleVertexNum) {};

            void rollback() override;

            double getChainFactor() const override;

        private:
            UndirectedRandomGraph &_graph;
            const Chain &_trivialChain;
            const EdgeProbability _stEdgeValue;
            double _chainFactor;
            int _middleVertexNum;
        };
    };

}

#endif //GRPH_UNDIRECTEDRANDOMGRAPH_HPP
