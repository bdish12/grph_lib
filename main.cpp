#include <iostream>

#include "src/utils/parse/TGFParser.hpp"
#include "src/graph/criteria/ATR.hpp"

int main() {
    auto matrix = grph::utils::TGFParser::parseFile(
            "../Google_tests/integration_tests/test_graphs/example_from_manual.tgf"
    );

    std::cout << "ATR: " << grph::graph::random::ATR::calculateATR(*matrix) << std::endl;
}
