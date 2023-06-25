#pragma once

#include <vector>
#include <set>
#include <list>
#include <memory>

#include "../common/AdjacencyMatrix.hpp"
#include "../../exceptions/graph/exceptions.h"
#include "../common/DegreesVector.hpp"
#include "../common/types.hpp"
#include "actions/Actions.hpp"
#include "Chain.hpp"

namespace grph::graph::random {

    class IRandomGraph {
    public:
        virtual int getNumVertexes() const = 0;

        virtual int getNumEdges() const = 0;

        virtual int getVertexDegree(int vertexNum) const = 0;

        virtual EdgeProbability at(int firstVertexNum, int secondVertexNum) const = 0;

        virtual bool containsHangingVertex() const = 0;

        virtual EdgeProbability removeHangingVertexWithLowestIndex() = 0;

        virtual int findFirstVertexThatDegreeEqualTwo() const = 0;

        virtual bool containsChain() const = 0;

        virtual double removeChain(const Chain &chain) = 0;

        virtual std::shared_ptr<RemoveRandomEdgeAction> removeRandomEdge() = 0;

        virtual std::shared_ptr<PullEdgeAction> pullEdge(int remainingVertexNum, int vertexToRemoveNum) = 0;

        virtual std::shared_ptr<RemoveTrivialChainAction> removeTrivialChain(const Chain &trivialChain) = 0;

        virtual bool isTree() const = 0;

        virtual bool isCycle() const = 0;

        virtual double calculateTreeATR() const = 0;

        virtual double calculateCycleATR() const = 0;

        virtual const std::shared_ptr<AdjacencyMatrix<EdgeProbability>> &getAdjacencyMatrix() const = 0;

        virtual const std::shared_ptr<DegreesVector> &getDegreesVector() const = 0;

        virtual std::shared_ptr<AdjacencyMatrix<EdgeProbability>> getSubgraphAdjacencyMatrix(
                const std::unordered_set<int> &subgraphVertexes
        ) const = 0;

        virtual void setMatrixAndVector(
                const std::shared_ptr<AdjacencyMatrix<EdgeProbability>> &matrix,
                const std::shared_ptr<DegreesVector> &degreesVector
        ) = 0;

        virtual std::unordered_set<int> getInverseComponent(
                const std::unordered_set<int> &connectivityComponent
        ) const = 0;

    };

}
