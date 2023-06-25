#include <iostream>

#include "src/utils/parse/TGFParser.hpp"
#include "src/graph/criteria/ATR.hpp"

int main() {
    auto matrix = grph::utils::TGFParser::parseFile(
            "../Google_tests/integration_tests/test_graphs/example_1.tgf"
    );

    std::cout << "ATR: " << grph::graph::random::ATR::calculateATR(*matrix) << std::endl;


    // TODO проверка полного графа на 10 вершинах
//    auto matrix = grph::graph::AdjacencyMatrix<grph::graph::random::EdgeProbability>(10);
//
//    for (int lineIdx = 0; lineIdx < 10; ++lineIdx) {
//        for (int columnIdx = 0; columnIdx < 10; ++columnIdx) {
//            if (lineIdx != columnIdx) {
//                matrix.setAt(0.5, lineIdx, columnIdx);
//            }
//        }
//    }
}
