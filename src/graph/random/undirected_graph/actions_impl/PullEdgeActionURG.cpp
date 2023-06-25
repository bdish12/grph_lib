#include "../UndirectedRandomGraph.hpp"

using namespace grph::graph::random;

void UndirectedRandomGraph::PullEdgeActionURG::rollback() {
    this->removeAllEdgesForRemainingVertex();
    this->restoreEdgesFromSnapshot();

    _graph.restoreLastVertex();
    auto lastVertexNum = _graph._adjacencyMatrix->getDimension() - 1;
    _graph._adjacencyMatrix->renumberPairOfVertices(_prevRemovedVertexNum,
                                                    lastVertexNum);
    _graph.addEdgeInternal(_prevRemovedVertexNum, _remainingVertexNum, _removedEdgeValue);

    _graph._degreesVector = RollbackAction::_degreesVectorBeforeAction;
}

void UndirectedRandomGraph::PullEdgeActionURG::removeAllEdgesForRemainingVertex() {
        _graph._adjacencyMatrix->zeroLine(_remainingVertexNum);
        _graph._adjacencyMatrix->zeroColumn(_remainingVertexNum);
}

void UndirectedRandomGraph::PullEdgeActionURG::restoreEdgesFromSnapshot() {
    for (const auto &edgeSnapshot: _remainingVertexSnapshot) {
        _graph.addEdgeInternal(_remainingVertexNum,
                               edgeSnapshot.oppositeEnd(_remainingVertexNum),
                               edgeSnapshot.value());
    }
}
