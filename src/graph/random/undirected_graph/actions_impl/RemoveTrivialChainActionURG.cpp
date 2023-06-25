#include "../UndirectedRandomGraph.hpp"

using namespace grph::graph::random;

void UndirectedRandomGraph::RemoveTrivialChainActionURG::rollback() {
    _graph._degreesVector = RollbackAction::_degreesVectorBeforeAction;

    _graph.restoreLastVertex();
    _graph.renumberPairVertexes(_graph.getLastVertexNum(), _middleVertexNum);

    auto sVertex = _trivialChain.getHead();
    auto tVertex = _trivialChain.getTail();
    _graph.addEdgeInternal(sVertex, tVertex, _stEdgeValue);
}

double UndirectedRandomGraph::RemoveTrivialChainActionURG::getChainFactor() const {
    return _chainFactor;
}
