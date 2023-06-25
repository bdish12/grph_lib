//#ifndef GRPH_EDGESLIST_HPP
//#define GRPH_EDGESLIST_HPP
//
//#include <vector>
//#include <map>
//
//#include "AdjacencyMatrix.hpp"
//#include "types.hpp"
//
//namespace grph::graph {
//
//    template<typename MetaInfoType = random::empty>
//    struct EdgeNode {
//        int from, to;
//        MetaInfoType metaInfo;
//
//        EdgeNode(const Edge &edge, const MetaInfoType &metaInfo) : from(edge.from), to(edge.to), metaInfo(metaInfo) {};
//
//        EdgeNode(int from, int to, const MetaInfoType &metaInfo) : from(from), to(to), metaInfo(metaInfo) {};
//    };
//
//    template<typename VertexesRatioType = bool>
//    class EdgesList {
//    public:
//        explicit EdgesList(const AdjacencyMatrix<VertexesRatioType> &adjacencyMatrix) {
//            this->numVertexes = adjacencyMatrix.getDimension();
//
//            for (int lineIndex = 0; lineIndex < adjacencyMatrix.getDimension(); ++lineIndex) {
//                for (int columnIndex = 0; columnIndex < adjacencyMatrix.getDimension(); ++columnIndex) {
//                    if (adjacencyMatrix.isConnected(lineIndex, columnIndex)) {
//                        Edge edge(lineIndex, columnIndex);
//                        edges.push_back(EdgeNode(edge, adjacencyMatrix.at(lineIndex, columnIndex)));
//                    }
//                }
//            }
//        }
//
//        void renumberVertexes(const std::map<int, int> &mapping) {
//            // TODO: add OpenMP
//            for (auto &edge: edges) {
//                edge.from = mapping.contains(edge.from)
//                            ? mapping.at(edge.from)
//                            : edge.from;
//                edge.to = mapping.contains(edge.to)
//                          ? mapping.at(edge.to)
//                          : edge.to;
//            }
//        }
//
//        AdjacencyMatrix<VertexesRatioType> toAdjacencyMatrix() const {
//            AdjacencyMatrix<VertexesRatioType> adjacencyMatrix(this->numVertexes);
//            // TODO: add OpenMP
//            for (auto &edge: edges) {
//                // TODO: пересмотреть
//                adjacencyMatrix.setAt(edge.metaInfo, edge.from.num, edge.to.num);
//            }
//            return adjacencyMatrix;
//        }
//
//    private:
//        std::vector<EdgeNode<VertexesRatioType>> edges;
//        int numVertexes{};
//    };
//
//}
//
//#endif //GRPH_EDGESLIST_HPP
