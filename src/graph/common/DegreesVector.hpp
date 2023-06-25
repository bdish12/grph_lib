#ifndef GRPH_DEGREESVECTOR_HPP
#define GRPH_DEGREESVECTOR_HPP

#include <vector>
#include <stack>
#include <map>
#include <algorithm>

#include "types.hpp"
#include "AdjacencyMatrix.hpp"

namespace grph::graph {

    class DegreesVector {
    public:
        static std::shared_ptr<DegreesVector> make(
                const std::shared_ptr<AdjacencyMatrix<random::EdgeProbability>> &adjacencyMatrix
        ) {
            return std::make_shared<DegreesVector>(DegreesVector(adjacencyMatrix));
        }

        static std::shared_ptr<DegreesVector> make(const AdjacencyMatrix<random::EdgeProbability> &adjacencyMatrix) {
            return std::make_shared<DegreesVector>(DegreesVector(adjacencyMatrix));
        }

        int findFirstVertexThatDegreeEqualTwo() const {
            for (int i = _beginIndex; i <= _endIndex; ++i) {
                if (_degreesVector[i]._degree == 2) {
                    return _degreesVector[i]._vertexNum;
                } else if (_degreesVector[i]._degree > 2) {
                    return VERTEX_NOT_FOUND;
                }
            }
            return VERTEX_NOT_FOUND;
        };

        bool containsHangingVertex() const {
            return _degreesVector[_beginIndex]._degree == 1;
        }

        int getHangingVertexWithLowestIndex() const {
            if (_degreesVector[_beginIndex]._degree != 1) {
                throw std::runtime_error("getHangingVertexWithLowestIndex");
            }
            return _degreesVector[_beginIndex]._vertexNum;
        }

        void removeHangingVertexWithLowestIndex(int neighborVertex) {
            _beginIndex++;
            auto neighborVertexNodeIdx = _mappingVector[neighborVertex];
            _degreesVector[neighborVertexNodeIdx]._degree--;
            siftLeft(neighborVertexNodeIdx);
        }

        void restoreHangingVertexWithLowestIndex(int neighborVertex) {
            _beginIndex++;
            auto neighborVertexNodeIdx = _mappingVector[neighborVertex];
            _degreesVector[neighborVertexNodeIdx]._degree++;
            siftRight(neighborVertexNodeIdx);
        }

        int getVertexDegree(int vertexNum) const {
            return _degreesVector[_mappingVector[vertexNum]]._degree;
        }

        int getVertexWithLowestDegree() const {
            return _degreesVector[_beginIndex]._vertexNum;
        }

        int getNumEdges() const {
            int degreesSum = 0;
            for (int i = _beginIndex; i <= _endIndex; ++i) {
                degreesSum += _degreesVector[i]._degree;
            }
            return degreesSum / 2;
        }

        bool isCycle() const {
            return _degreesVector[_beginIndex]._degree == 2
                   && _degreesVector[_endIndex]._degree == 2;
        }

        void removeEdge(int from, int to) {
            _degreesVector[_mappingVector[from]]._degree--;
            siftLeft(_mappingVector[from]);

            _degreesVector[_mappingVector[to]]._degree--;
            siftLeft(_mappingVector[to]);
        }

        void restoreEdge(int from, int to) {
            _degreesVector[_mappingVector[from]]._degree++;
            siftRight(_mappingVector[from]);

            _degreesVector[_mappingVector[to]]._degree++;
            siftRight(_mappingVector[to]);
        }

        void renumberPairOfVertexes(int vertexNum1, int vertexNum2) {
            if (vertexNum1 == vertexNum2) {
                return;
            }

            _degreesVector[_mappingVector[vertexNum1]]._vertexNum = vertexNum2;
            _degreesVector[_mappingVector[vertexNum2]]._vertexNum = vertexNum1;

            std::swap(_mappingVector[vertexNum1],
                      _mappingVector[vertexNum2]);
        }

        bool isEqual(const DegreesVector &other) {
            if (this->_beginIndex != other._beginIndex
                || this->_endIndex != other._endIndex
                || this->_mappingVector != other._mappingVector) {
                return false;
            }

            for (int i = 0; i < this->_degreesVector.size(); ++i) {
                if (!this->_degreesVector[i].isEqual(other._degreesVector[i])) {
                    return false;
                }
            }
            return true;
        }

        void print() const {
            std::cout << "DegreesVector:" << std::endl;
            for (auto &node: _degreesVector) {
                std::cout << node._degree << " " << node._vertexNum << std::endl;
             }
            std::cout << "MappingVector:" << std::endl;
            for (auto &key: _mappingVector) {
                std::cout << key << " " << _mappingVector[key] << std::endl;
            }
            std::cout << "-------------" << std::endl;
        }

    private:
        struct DegreeNode {
            int _vertexNum;
            int _degree;

            explicit DegreeNode(int vertexNum) : _vertexNum(vertexNum), _degree(0) {};

            bool isLess(const DegreeNode &other) const {
                return (this->_degree < other._degree)
                       || (this->_degree == other._degree && this->_vertexNum < other._vertexNum);
            }

            bool isMore(const DegreeNode &other) const {
                return (this->_degree > other._degree)
                       || (this->_degree == other._degree && this->_vertexNum > other._vertexNum);
            }

            bool isEqual(const DegreeNode &other) const {
                return this->_vertexNum == other._vertexNum
                       && this->_degree == other._degree;
            }
        };

        int _beginIndex; // cur begin at degreesVector
        int _endIndex; // [beginIndex, endIndex]
        std::vector<DegreeNode> _degreesVector; // вершины, отсортированные по возрастанию своих степеней
        std::vector<int> _mappingVector; // map: (cur-graph-state) [vertexNum] --> [index] in degreesVector

        explicit DegreesVector(const std::shared_ptr<AdjacencyMatrix<random::EdgeProbability>> &adjacencyMatrix)
                : _beginIndex(0),
                  _endIndex(adjacencyMatrix->getDimension() - 1),
                  _mappingVector(std::vector<int>(adjacencyMatrix->getDimension())) {

            for (int vertexIndex = 0; vertexIndex < adjacencyMatrix->getDimension(); ++vertexIndex) {
                _degreesVector.emplace_back(vertexIndex);
                for (int columnIndex = 0; columnIndex < adjacencyMatrix->getDimension(); ++columnIndex) {
                    if (adjacencyMatrix->isConnected(vertexIndex, columnIndex)) {
                        _degreesVector[vertexIndex]._degree++;
                    }
                }
            }

            std::sort(_degreesVector.begin(), _degreesVector.end(),
                      [](const DegreeNode &n1, const DegreeNode &n2) {
                          if (n1._degree < n2._degree) {
                              return true;
                          } else if (n1._degree == n2._degree) {
                              return n1._vertexNum < n2._vertexNum;
                          } else {
                              return false;
                          }
                      });

            // TODO: OpenMP?
            for (int i = 0; i < _degreesVector.size(); ++i) {
                _mappingVector[_degreesVector[i]._vertexNum] = i;
            }
        }

        explicit DegreesVector(const AdjacencyMatrix<random::EdgeProbability> &adjacencyMatrix)
                : _beginIndex(0),
                  _endIndex(adjacencyMatrix.getDimension() - 1),
                  _mappingVector(std::vector<int>(adjacencyMatrix.getDimension())) {

            for (int vertexIndex = 0; vertexIndex < adjacencyMatrix.getDimension(); ++vertexIndex) {
                _degreesVector.emplace_back(vertexIndex);
                for (int columnIndex = 0; columnIndex < adjacencyMatrix.getDimension(); ++columnIndex) {
                    if (adjacencyMatrix.isConnected(vertexIndex, columnIndex)) {
                        _degreesVector[vertexIndex]._degree++;
                    }
                }
            }

            std::sort(_degreesVector.begin(), _degreesVector.end(),
                      [](const DegreeNode &n1, const DegreeNode &n2) {
                          if (n1._degree < n2._degree) {
                              return true;
                          } else if (n1._degree == n2._degree) {
                              return n1._vertexNum < n2._vertexNum;
                          } else {
                              return false;
                          }
                      });

            // TODO: OpenMP?
            for (int i = 0; i < _degreesVector.size(); ++i) {
                _mappingVector[_degreesVector[i]._vertexNum] = i;
            }
        }

        void siftLeft(int nodeIndex) {
            int cureNode = nodeIndex;
            int leftNode = nodeIndex - 1;
            while (leftNode >= _beginIndex && _degreesVector[cureNode].isLess(_degreesVector[leftNode])) {
                swapNodes(cureNode, leftNode);
            }
        }

        void siftRight(int nodeIndex) {
            int cureNode = nodeIndex;
            int rightNode = nodeIndex + 1;
            while (rightNode <= _endIndex && _degreesVector[cureNode].isMore(_degreesVector[rightNode])) {
                swapNodes(cureNode, rightNode);
            }
        }

        void swapNodes(int nodeIdx1, int nodeIdx2) {
            std::swap(_mappingVector[_degreesVector[nodeIdx1]._vertexNum],
                      _mappingVector[_degreesVector[nodeIdx2]._vertexNum]);

            std::swap(_degreesVector[nodeIdx1], _degreesVector[nodeIdx2]);
        }

    };

}

#endif //GRPH_DEGREESVECTOR_HPP
