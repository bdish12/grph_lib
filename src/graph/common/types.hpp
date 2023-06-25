#ifndef GRPH_TYPES_HPP
#define GRPH_TYPES_HPP

#include <sstream>

#define VERTEX_NOT_FOUND (-1)
#define TRIVIAL_CHAIN_VERTEX_LENGTH 3
#define EMPTY 0

namespace grph::graph {

    /// Always: from < to
    struct Edge {
    public:
        int _from;
        int _to;

        Edge(int from, int to) : _from(from), _to(to) {
            if (_from == _to) {
                throw std::runtime_error("Error: invalid edge (from == to)");
            } else if (_from > _to) {
                std::swap(_from, _to);
            }
        };

        Edge(const Edge &edge) = default;

        int maxVertex() const {
            return _to;
        }

        int minVertex() const {
            return _from;
        }
    };

    template<typename VertexesRatioType>
    struct EdgeNode {
    private:
        const Edge edge;
        const VertexesRatioType edgeValue;

    public:
        explicit EdgeNode(const Edge &edge, const VertexesRatioType &edgeValue)
                : edge(edge), edgeValue(edgeValue) {};

        int oppositeEnd(int vertexNum) const {
            if (vertexNum != edge._from && vertexNum != edge._to) {
                std::cout << vertexNum << std::endl;
                std::cout << "From: " << edge._from << " To:" << edge._to << std::endl;
                throw std::runtime_error("ERROR: invalid opposite vertex");
            }
            return vertexNum == edge._from
                   ? edge._to
                   : edge._from;
        }

        VertexesRatioType value() const {
            return edgeValue;
        }
    };

    namespace random {

        struct empty {
        };

        typedef double EdgeProbability;

    }

}
#endif //GRPH_TYPES_HPP
