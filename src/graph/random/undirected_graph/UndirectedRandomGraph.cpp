#include "UndirectedRandomGraph.hpp"

using namespace grph::graph::random;

int UndirectedRandomGraph::getNumVertexes() const {
    return _adjacencyMatrix->getDimension();
}

int UndirectedRandomGraph::getVertexDegree(int vertexNum) const {
    return _degreesVector->getVertexDegree(vertexNum);
}

const std::shared_ptr<grph::graph::DegreesVector> &UndirectedRandomGraph::getDegreesVector() const {
    return _degreesVector;
}

const std::shared_ptr<grph::graph::AdjacencyMatrix<EdgeProbability>> &UndirectedRandomGraph::getAdjacencyMatrix()
const {
    return _adjacencyMatrix;
}

EdgeProbability UndirectedRandomGraph::at(int firstVertexNum, int secondVertexNum) const {
    return _adjacencyMatrix->at(firstVertexNum, secondVertexNum);
}

bool UndirectedRandomGraph::containsHangingVertex() const {
    return _degreesVector->containsHangingVertex();
}

EdgeProbability UndirectedRandomGraph::removeHangingVertexWithLowestIndex() {
    auto hangingVertexNum = _degreesVector->getHangingVertexWithLowestIndex();
    this->renumberPairVertexes(hangingVertexNum, getLastVertexNum());

    hangingVertexNum = getLastVertexNum();
    auto neighborVertex = this->findNeighbor(hangingVertexNum);

    _degreesVector->removeHangingVertexWithLowestIndex(neighborVertex);
    auto removedEdgeWithRatio = _adjacencyMatrix->removeHangingVertex(hangingVertexNum,
                                                                      neighborVertex);
    return removedEdgeWithRatio.second;
}

void UndirectedRandomGraph::restoreHangingVertexWithLowestIndex(int prevRemovedVertexNum) {
    _adjacencyMatrix->restoreRemovedHangingVertex();
    this->renumberPairVertexes(getLastVertexNum(), prevRemovedVertexNum);

    auto neighborVertex = this->findNeighbor(prevRemovedVertexNum);
    _degreesVector->restoreHangingVertexWithLowestIndex(neighborVertex);
}

int UndirectedRandomGraph::findFirstVertexThatDegreeEqualTwo() const {
    return _degreesVector->findFirstVertexThatDegreeEqualTwo();
}

bool UndirectedRandomGraph::containsChain() const {
    return _degreesVector->findFirstVertexThatDegreeEqualTwo() != VERTEX_NOT_FOUND;
}

double UndirectedRandomGraph::removeChain(const Chain &chain) {
    setSubgraphWithoutChainMatrixWithDegreesVector(chain);
    return chain.getChainFactor();
}

int UndirectedRandomGraph::getNumEdges() const {
    return _degreesVector->getNumEdges();
}

bool UndirectedRandomGraph::isTree() const {
    return _degreesVector->getNumEdges() == (_adjacencyMatrix->getDimension() - 1);
}

bool UndirectedRandomGraph::isCycle() const { // только при условии связности графа
    return _degreesVector->isCycle();
}

bool UndirectedRandomGraph::isConnected(int vertex1, int vertex2) const {
    return _adjacencyMatrix->isConnected(vertex1, vertex2);
}

double UndirectedRandomGraph::calculateTreeATR() const {
    double product = 1;
    for (int lineIdx = 0; lineIdx < getNumVertexes(); ++lineIdx) {
        for (int columnIdx = 0; columnIdx < lineIdx; ++columnIdx) {
            if (isConnected(lineIdx, columnIdx)) {
                product *= _adjacencyMatrix->at(lineIdx, columnIdx);
            }
        }
    }
    return product;
}

double UndirectedRandomGraph::calculateCycleATR() const {
    double product = 1;
    double sum = 0;
    for (int lineIdx = 0; lineIdx < getNumVertexes(); ++lineIdx) {
        for (int columnIdx = 0; columnIdx < lineIdx; ++columnIdx) {
            if (isConnected(lineIdx, columnIdx)) {
                double p = _adjacencyMatrix->at(lineIdx, columnIdx);
                product *= p;
                sum += (1 - p) / p;
            }
        }
    }
    return product * (1 + sum);
}

std::shared_ptr<RemoveRandomEdgeAction> UndirectedRandomGraph::removeRandomEdge() {
    auto curDegreesVector = _degreesVector;

    auto vertexWithLowestDegree = _degreesVector->getVertexWithLowestDegree();
    auto neighborWithBiggestNum = _adjacencyMatrix->findNeighborWithBiggestNumFor(vertexWithLowestDegree);
    auto edgeToRemove = Edge{vertexWithLowestDegree, neighborWithBiggestNum};

    auto ratioValue = _adjacencyMatrix->removeEdge(edgeToRemove);
    _degreesVector->removeEdge(edgeToRemove._from, edgeToRemove._to);

    return std::shared_ptr<RemoveRandomEdgeAction>(
            new RemoveRandomEdgeActionURG(
                    *this,
                    curDegreesVector,
                    edgeToRemove,
                    ratioValue)
    );
}

std::shared_ptr<PullEdgeAction> UndirectedRandomGraph::pullEdge(int remainingVertexNum, int vertexToRemoveNum) {
    if (remainingVertexNum >= vertexToRemoveNum) {
        throw std::runtime_error("ERROR(pullEdge): left biggest vertex when try to pull edge");
    }

    auto removedEdgeValue = at(vertexToRemoveNum, remainingVertexNum);
    this->removeEdgeInternal(remainingVertexNum, vertexToRemoveNum);

    auto remainingVertexSnapshot = _adjacencyMatrix->getOutgoingEdges(remainingVertexNum);
    if (vertexToRemoveNum != getLastVertexNum()) {
        this->renumberPairVertexes(vertexToRemoveNum, getLastVertexNum());
    }

    this->enrichWithEdges(getLastVertexNum(),
                          remainingVertexNum,
                          _adjacencyMatrix->getOutgoingEdges(getLastVertexNum()));
    this->removeLastVertex();
    auto curDegreesVector = this->_degreesVector;
    this->_degreesVector = DegreesVector::make(this->_adjacencyMatrix);

    return std::shared_ptr<PullEdgeAction>(
            new PullEdgeActionURG(
                    *this,
                    curDegreesVector,
                    vertexToRemoveNum,
                    remainingVertexNum,
                    removedEdgeValue,
                    remainingVertexSnapshot
            )
    );
}

std::shared_ptr<RemoveTrivialChainAction> UndirectedRandomGraph::removeTrivialChain(const Chain &trivialChain) {
    if (!trivialChain.isTrivialChain()) {
        throw std::runtime_error("ERROR(removeTrivialChain): Is not trivial chain");
    }

    auto curDegreesVector = _degreesVector;

    auto sVertex = trivialChain.getHead();
    auto tVertex = trivialChain.getTail();
    auto stEdgeValue = _adjacencyMatrix->at(sVertex, tVertex);

    auto middleVertex = trivialChain.getMiddleVertex();

    if (!trivialChain.isCycle()) {
        if (stEdgeValue != EMPTY) {
            replaceMultiEdge(sVertex, tVertex, trivialChain.getReducedChainValue());
        } else {
            addEdgeInternal(sVertex, tVertex, trivialChain.getReducedChainValue());
        }
    }
    renumberPairVertexes(middleVertex, getLastVertexNum());
    removeLastVertex();

    this->_degreesVector = DegreesVector::make(this->_adjacencyMatrix);

    return std::shared_ptr<RemoveTrivialChainAction>(
            new RemoveTrivialChainActionURG(
                    *this,
                    curDegreesVector,
                    trivialChain,
                    stEdgeValue,
                    trivialChain.getChainFactor(),
                    middleVertex
            )
    );
}

std::shared_ptr<grph::graph::AdjacencyMatrix<EdgeProbability>> UndirectedRandomGraph::getSubgraphAdjacencyMatrix(
        const std::unordered_set<int> &subgraphVertexes
) const {
    int mappedVertexNum = 0;
    std::unordered_map<int, int> vertexesMapping;
    for (const auto subgraphVertex: subgraphVertexes) {
        vertexesMapping[subgraphVertex] = mappedVertexNum;
        ++mappedVertexNum;
    }

    auto subgraphMatrix = std::make_shared<AdjacencyMatrix < EdgeProbability>>
    (subgraphVertexes.size());
    for (const auto subgraphVertex: subgraphVertexes) {
        for (int graphVertex = 0; graphVertex < this->_adjacencyMatrix->getDimension(); ++graphVertex) {
            if (subgraphVertexes.contains(graphVertex)) {
                subgraphMatrix->setAt(at(subgraphVertex, graphVertex),
                                      vertexesMapping[subgraphVertex],
                                      vertexesMapping[graphVertex]); // TODO: добавить симметрию?
            }
        }
    }
    return subgraphMatrix;
}

void UndirectedRandomGraph::setMatrixAndVector(
        const std::shared_ptr<graph::AdjacencyMatrix<EdgeProbability>> &matrix,
        const std::shared_ptr<graph::DegreesVector> &degreesVector
) {
    this->_adjacencyMatrix = matrix;
    this->_degreesVector = degreesVector;
}

std::unordered_set<int> UndirectedRandomGraph::getInverseComponent(
        const std::unordered_set<int> &connectivityComponent
) const {
    std::unordered_set<int> inverseComponent;
    for (int vertexNum = 0; vertexNum < getNumVertexes(); ++vertexNum) {
        if (!connectivityComponent.contains(vertexNum)) {
            inverseComponent.insert(vertexNum);
        }
    }
    return inverseComponent;
}

void UndirectedRandomGraph::replaceMultiEdge(int fromVertex, int toVertex, const EdgeProbability &parallelEdgeValue) {
    _adjacencyMatrix->replaceMultiEdge(fromVertex, toVertex, parallelEdgeValue);
}

void UndirectedRandomGraph::setSubgraphWithoutChainMatrixWithDegreesVector(const Chain &chain) {
    if (chain.isTrivialChain()) {
        this->removeTrivialChain(chain);
        return;
    }

    std::unordered_set<int> remainingVertexes;
    for (int v = 0; v < this->_adjacencyMatrix->getDimension(); ++v) {
        if (!chain.getVertexesToRemove().contains(v)) {
            remainingVertexes.insert(v);
        }
    }

    int mappedVertexNum = 0;
    std::unordered_map<int, int> vertexesMapping;
    for (const auto subgraphVertex: remainingVertexes) {
        vertexesMapping[subgraphVertex] = mappedVertexNum;
        ++mappedVertexNum;
    }

    std::shared_ptr<AdjacencyMatrix < EdgeProbability>>
    subMatrix = std::make_shared<AdjacencyMatrix < EdgeProbability>>(remainingVertexes.size());
    for (const auto subgraphVertex: remainingVertexes) {
        for (int graphVertex = 0; graphVertex < this->_adjacencyMatrix->getDimension(); ++graphVertex) {
            if (remainingVertexes.contains(graphVertex)) {
                subMatrix->setAt(at(subgraphVertex, graphVertex),
                                 vertexesMapping[subgraphVertex],
                                 vertexesMapping[graphVertex]); // TODO: добавить симметрию?
            }
        }
    }

    if (!chain.isCycle()) {
        auto mappedS = vertexesMapping[chain.getHead()];
        auto mappedT = vertexesMapping[chain.getTail()];
        if (at(chain.getHead(), chain.getTail()) != EMPTY) {
            subMatrix->replaceMultiEdge(mappedS, mappedT, chain.getReducedChainValue());
        } else {
            subMatrix->addEdge(mappedS, mappedT, chain.getReducedChainValue());
        }
    }

    this->_adjacencyMatrix = subMatrix;
    this->_degreesVector = DegreesVector::make(this->_adjacencyMatrix);
}

int UndirectedRandomGraph::findNeighbor(int vertexNum) const {
    for (int columnIdx = 0; columnIdx < getNumVertexes(); ++columnIdx) {
        if (isConnected(vertexNum, columnIdx)) {
            return columnIdx;
        }
    }
    return EMPTY;
}

void UndirectedRandomGraph::enrichWithEdges(int edgeSourceVertex,
                                            int vertexToEnrich,
                                            const std::vector<grph::graph::EdgeNode<EdgeProbability>> &edges) {
    for (const auto &edgeNode: edges) {
        if (_adjacencyMatrix->at(vertexToEnrich, edgeNode.oppositeEnd(edgeSourceVertex)) != EMPTY) {
            replaceMultiEdge(vertexToEnrich,
                             edgeNode.oppositeEnd(edgeSourceVertex),
                             edgeNode.value());
        } else {
            _adjacencyMatrix->setAt(edgeNode.value(),
                                    vertexToEnrich,
                                    edgeNode.oppositeEnd(edgeSourceVertex));
            _adjacencyMatrix->setAt(edgeNode.value(),
                                    edgeNode.oppositeEnd(edgeSourceVertex),
                                    vertexToEnrich);
        }
    }
}
