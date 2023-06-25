#ifndef GRPH_TGFPARSER_HPP
#define GRPH_TGFPARSER_HPP

#include <iostream>
#include <fstream>

#include "../../graph/common/AdjacencyMatrix.hpp"
#include "../../graph/common/types.hpp"

namespace grph::utils {

    class TGFParser {
    public:
        static graph::AdjacencyMatrix<graph::random::EdgeProbability> parseFile(std::istream &istream) {
            std::string dimension;
            std::getline(istream, dimension);

            graph::AdjacencyMatrix<graph::random::EdgeProbability> adjacencyMatrix(std::stoi(dimension));

            std::string nextLine;
            while (std::getline(istream, nextLine)) {
                std::stringstream ss(nextLine);
                int from, to;
                double probability;
                ss >> from >> to >> probability;

                adjacencyMatrix.setAt(probability, from - 1, to - 1);
                adjacencyMatrix.setAt(probability, to - 1, from - 1);
            }

            return adjacencyMatrix;
        }

        static std::shared_ptr<graph::AdjacencyMatrix<graph::random::EdgeProbability>> parseFile(
                const std::string &fileName
        ) {
            std::ifstream ifs(fileName);
            std::string dimension;
            std::getline(ifs, dimension);

            const auto &matrix =
                    std::make_shared<graph::AdjacencyMatrix<graph::random::EdgeProbability>>(
                            std::stoi(dimension)
                    );

            std::string nextLine;
            while (std::getline(ifs, nextLine)) {
                std::stringstream ss(nextLine);
                int from, to;
                double probability;
                ss >> from >> to >> probability;

                matrix->setAt(probability, from - 1, to - 1);
                matrix->setAt(probability, to - 1, from - 1);
            }
            return matrix;
        }
    };

}

#endif //GRPH_TGFPARSER_HPP
