#ifndef GRPH_ADJACENCYMATRIX_HPP
#define GRPH_ADJACENCYMATRIX_HPP

#include <memory>
#include <vector>
#include <set>
#include <iostream>

#include "types.hpp"

namespace grph::graph {

    template<typename VertexesRatioType = bool>
    class AdjacencyMatrix {
    public:
        explicit AdjacencyMatrix(const int dimension)
                : INITIAL_DIMENSION(dimension),
                  _curDimension(dimension) {
            this->_matrix = std::shared_ptr<VertexesRatioType[]>(new VertexesRatioType[dimension * dimension],
                                                                 std::default_delete<VertexesRatioType[]>());
            memset(_matrix.get(),
                   0,
                   sizeof(VertexesRatioType) * INITIAL_DIMENSION * INITIAL_DIMENSION);
        }

        explicit AdjacencyMatrix(const int dimension, const VertexesRatioType matrix[])
                : INITIAL_DIMENSION(dimension),
                  _curDimension(dimension) {
            this->_matrix = std::shared_ptr<VertexesRatioType[]>(new VertexesRatioType[dimension * dimension],
                                                                 std::default_delete<VertexesRatioType[]>());

            for (int lineIdx = 0; lineIdx < INITIAL_DIMENSION; ++lineIdx) {
                memcpy(reinterpret_cast<char *>(_matrix.get()) + calcLineFirstByteIndex(lineIdx),
                       &matrix[lineIdx * INITIAL_DIMENSION],
                       sizeof(VertexesRatioType) * INITIAL_DIMENSION);
            }
        }

        void print() const {
            std::cout << "Matrix:" << std::endl;
            for (int lineIdx = 0; lineIdx < _curDimension; ++lineIdx) {
                for (int columnIdx = 0; columnIdx < _curDimension; ++columnIdx) {
                    std::cout << at(lineIdx, columnIdx) << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "---------" << std::endl;
        }

        bool isConnected(int firstVertexNum, int secondVertexNum) const {
            return _matrix[firstVertexNum * INITIAL_DIMENSION + secondVertexNum] != 0;
        }

        int getDimension() const {
            return _curDimension;
        }

        VertexesRatioType at(int lineIndex, int columnIndex) const {
            if (isOutOfBounds(lineIndex, columnIndex)) {
                throw std::runtime_error("Out of bounds:");
            }
            return _matrix[lineIndex * INITIAL_DIMENSION + columnIndex];
        }

        VertexesRatioType setAt(VertexesRatioType value, int lineIndex, int columnIndex) {
            if (isOutOfBounds(lineIndex, columnIndex)) {
                throw std::runtime_error("Out of bounds");
            }
            auto prevValue = _matrix[lineIndex * INITIAL_DIMENSION + columnIndex];
            _matrix[lineIndex * INITIAL_DIMENSION + columnIndex] = value;
            return prevValue;
        }

        std::vector<EdgeNode<VertexesRatioType>> getOutgoingEdges(int fromVertexNum) {
            std::vector<EdgeNode<VertexesRatioType>> outgoingEdges;
            for (int columnIdx = 0; columnIdx < _curDimension; ++columnIdx) {
                if (columnIdx != fromVertexNum && isConnected(fromVertexNum, columnIdx)) {
                    outgoingEdges.push_back(
                            EdgeNode(
                                    {fromVertexNum, columnIdx},
                                    at(fromVertexNum, columnIdx)
                            )
                    );
                }
            }
            return outgoingEdges;
        }

        void renumberPairOfVertices(int firstVertexNum, int secondVertexNum) {
            if (firstVertexNum == secondVertexNum) {
                return;
            }

            this->swapLines(firstVertexNum, secondVertexNum);

            // TODO: add OpenMP
            // swap columns
            for (int lineIdx = 0; lineIdx < _curDimension; ++lineIdx) {
                std::swap(_matrix[lineIdx * INITIAL_DIMENSION + firstVertexNum],
                          _matrix[lineIdx * INITIAL_DIMENSION + secondVertexNum]);
            }
        }

        int findNeighborWithLowestNumFor(int vertexNum,
                                         const std::set<int> &excludedVerticesSet = std::set < int > ()) const {
            for (int columnIndex = 0; columnIndex < _curDimension; ++columnIndex) {
                if (_matrix[vertexNum * INITIAL_DIMENSION + columnIndex] != 0
                    && !excludedVerticesSet.contains(columnIndex)) {
                    return columnIndex;
                }
            }
            return VERTEX_NOT_FOUND;
        }

        int findNeighborWithBiggestNumFor(int vertexNum,
                                          const std::set<int> &excludedVerticesSet = std::set < int > ()) const {
            for (int columnIndex = _curDimension - 1; columnIndex >= 0; --columnIndex) {
                if (_matrix[vertexNum * INITIAL_DIMENSION + columnIndex] != EMPTY
                    && !excludedVerticesSet.contains(columnIndex)) {
                    return columnIndex;
                }
            }
            return VERTEX_NOT_FOUND;
        }

        std::pair<Edge, VertexesRatioType> removeHangingVertex(int hangingVertex, int neighborVertex) {
            if (hangingVertex >= _curDimension || hangingVertex != _curDimension - 1) {
                throw std::runtime_error("removeHangingVertex");
            }
            auto ratioValue = at(hangingVertex, neighborVertex);
            decrementDimension();
            return std::make_pair(Edge{hangingVertex, neighborVertex}, ratioValue);
        }

        void restoreRemovedHangingVertex() {
            this->incrementDimension();
        }

        VertexesRatioType removeEdge(const Edge &edgeToRemove) {
            setAt(EMPTY, edgeToRemove._from, edgeToRemove._to);
            return setAt(EMPTY, edgeToRemove._to, edgeToRemove._from);;
        }

        void restoreEdge(const Edge &edgeToRestore, VertexesRatioType ratioVal) {
            setAt(ratioVal, edgeToRestore._from, edgeToRestore._to);
            setAt(ratioVal, edgeToRestore._to, edgeToRestore._from);
        }

        inline void addEdge(int from, int to, const VertexesRatioType &value) {
            this->setAt(value, from, to);
            this->setAt(value, to, from);
        }

        std::shared_ptr<AdjacencyMatrix<VertexesRatioType>> copy() const {
            return std::shared_ptr<AdjacencyMatrix<VertexesRatioType>>(new AdjacencyMatrix(this->_matrix,
                                                                                           this->INITIAL_DIMENSION,
                                                                                           this->_curDimension));
        }

        void removeLastVertex() {
            decrementDimension(); // этого достаточно
        }

        void restoreLastVertex() {
            incrementDimension();
        }

        void zeroLine(int lineIndex) {
            memset(reinterpret_cast<char *>(_matrix.get()) + calcLineFirstByteIndex(lineIndex),
                   EMPTY,
                   calcCurMatrixLineLengthInBytes());
        }

        void zeroColumn(int columnIndex) {
            for (int lineIndex = 0; lineIndex < _curDimension; ++lineIndex) {
                setAt(EMPTY, lineIndex, columnIndex);
            }
        }

        void replaceMultiEdge(int fromVertex, int toVertex, const VertexesRatioType &parallelEdgeValue) {
            auto curEdgeValue = at(fromVertex, toVertex);
            auto newEdgeValue = curEdgeValue + parallelEdgeValue - curEdgeValue * parallelEdgeValue;
            this->setAt(newEdgeValue, fromVertex, toVertex);
            this->setAt(newEdgeValue, toVertex, fromVertex);
        }

        bool isEqual(const AdjacencyMatrix<VertexesRatioType> &other) const {
            if (this->_curDimension != other._curDimension) {
                return false;
            }

            for (int lineIdx = 0; lineIdx < _curDimension; ++lineIdx) {
                for (int columnIdx = 0; columnIdx < _curDimension; ++columnIdx) {
                    if (this->at(lineIdx, columnIdx) != other.at(lineIdx, columnIdx)) {
                        return false;
                    }
                }
            }
            return true;
        }

    private:
        AdjacencyMatrix(const std::shared_ptr<VertexesRatioType[]> &sourceMatrix,
                        int INITIAL_DIMENSION, int curDimension)
                : INITIAL_DIMENSION(INITIAL_DIMENSION), _curDimension(curDimension) {
            this->_matrix = std::shared_ptr<VertexesRatioType[]>(
                    new VertexesRatioType[INITIAL_DIMENSION * INITIAL_DIMENSION],
                    std::default_delete<VertexesRatioType[]>());

            memcpy(_matrix.get(),
                   sourceMatrix.get(),
                   INITIAL_DIMENSION * INITIAL_DIMENSION * sizeof(VertexesRatioType));
        }

        inline void decrementDimension() {
            if (_curDimension > 0) {
                --_curDimension;
            } else {
                throw std::runtime_error("error: cannot decrement dimension");
            }
        }

        inline void incrementDimension() {
            if (_curDimension < INITIAL_DIMENSION) {
                ++_curDimension;
            } else {
                throw std::runtime_error("error: cannot increment dimension");
            }
        }

        /// use with: reinterpret_cast<char *>
        inline int calcLineFirstByteIndex(int lineIndex) const {
            return lineIndex * INITIAL_DIMENSION * sizeof(VertexesRatioType);
        }

        inline int calcCurMatrixLineLengthInBytes() const {
            return _curDimension * sizeof(VertexesRatioType);
        }

        int findHangingVertexNeighbor(int vertexToRemove) const {
            for (int columnIndex = 0; columnIndex < _curDimension; ++columnIndex) {
                if (isConnected(vertexToRemove, columnIndex)) {
                    return columnIndex;
                }
            }
            return VERTEX_NOT_FOUND;
            // TODO: throw exception
        }

        // TODO: переписать на использование двумерного массива
        void swapLines(int firstLineIndex, int secondLineIndex) {
            const int lineLengthInBytes = sizeof(VertexesRatioType) * INITIAL_DIMENSION;

            auto buffer = new VertexesRatioType[INITIAL_DIMENSION];
            char *matrixBytesPtr = reinterpret_cast<char *>(_matrix.get());

            memcpy(buffer,
                   matrixBytesPtr + calcLineFirstByteIndex(firstLineIndex),
                   lineLengthInBytes);

            memcpy(matrixBytesPtr + calcLineFirstByteIndex(firstLineIndex),
                   matrixBytesPtr + calcLineFirstByteIndex(secondLineIndex),
                   lineLengthInBytes);

            memcpy(matrixBytesPtr + calcLineFirstByteIndex(secondLineIndex),
                   buffer,
                   lineLengthInBytes);

            delete[]buffer;
        }

        inline bool isOutOfBounds(int lineIndex, int columnIndex) const {
            return lineIndex * INITIAL_DIMENSION + columnIndex >= INITIAL_DIMENSION * INITIAL_DIMENSION;
        }

    private:
        std::shared_ptr<VertexesRatioType[]> _matrix;
        const int INITIAL_DIMENSION{};
        int _curDimension{};
    };

}

#endif //GRPH_ADJACENCYMATRIX_HPP
