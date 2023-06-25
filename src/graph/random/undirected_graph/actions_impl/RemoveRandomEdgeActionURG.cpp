#include "../UndirectedRandomGraph.hpp"

using namespace grph::graph::random;

double UndirectedRandomGraph::RemoveRandomEdgeActionURG::getEdgeProbability() const {
    return _edgeRatioValue;
}

grph::graph::Edge UndirectedRandomGraph::RemoveRandomEdgeActionURG::getRemovedEdge() const {
    return _removedEdge;
}

void UndirectedRandomGraph::RemoveRandomEdgeActionURG::rollback() {
    _graph._degreesVector = RollbackAction::_degreesVectorBeforeAction;

    _graph._adjacencyMatrix->restoreEdge(_removedEdge, _edgeRatioValue);
    _graph._degreesVector->restoreEdge(_removedEdge._from, _removedEdge._to);
}
