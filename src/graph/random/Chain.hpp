#ifndef GRPH_CHAIN_HPP
#define GRPH_CHAIN_HPP

#include <list>
#include <vector>
#include <unordered_set>
#include "../common/types.hpp"

namespace grph::graph::random {

    class Chain {
    public:
        Chain() : _middleVertex(VERTEX_NOT_FOUND), _chainFactor(0.), _reducedChainValue(0.) {};

        explicit Chain(
                const std::list<int> &vertexesChain,
                double sumInverseP,
                double pProduct,
                int middleVertex
        ) : _chainVertexes(vertexesChain.begin(), vertexesChain.end()),
            _vertexesToRemove(_chainVertexes.begin(), _chainVertexes.end()),
            _middleVertex(middleVertex) {
            if (_chainVertexes.empty()) {
                throw std::runtime_error("ERROR: Invalid chain creation");
            }

            _vertexesToRemove.erase(_chainVertexes.front());
            _vertexesToRemove.erase(_chainVertexes.back());

            int numChainEdges = _chainVertexes.size() - 1;
            _chainFactor = pProduct * (sumInverseP - numChainEdges + 1);
            _reducedChainValue = this->isCycle()
                                 ? EMPTY
                                 : 1 / (sumInverseP - numChainEdges + 1);
        };

        unsigned long size() const {
            return this->isCycle()
                   ? _chainVertexes.size() - 1
                   : _chainVertexes.size();
        }

        bool isTrivialChain() const {
            return this->size() <= TRIVIAL_CHAIN_VERTEX_LENGTH;
        }

        bool empty() const {
            return _chainVertexes.empty();
        }

        double getChainFactor() const {
            return _chainFactor;
        }

        double getReducedChainValue() const {
            return _reducedChainValue;
        }

        int getHead() const {
            return _chainVertexes.front();
        }

        int getTail() const {
            return _chainVertexes.back();
        }

        int getMiddleVertex() const {
            return _middleVertex;
        }

        const std::unordered_set<int> &getVertexesToRemove() const {
            return _vertexesToRemove;
        }

        bool isCycle() const {
            return !_chainVertexes.empty()
                   && _chainVertexes[0] == _chainVertexes[_chainVertexes.size() - 1];
        }

    private:
        std::vector<int> _chainVertexes;
        std::unordered_set<int> _vertexesToRemove;
        int _middleVertex{};
        double _chainFactor{};
        double _reducedChainValue{};
    };

}

#endif //GRPH_CHAIN_HPP
