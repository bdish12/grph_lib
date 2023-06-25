#include "gtest/gtest.h"
#include <iostream>

#include "../../../../../src/graph/random/undirected_graph/UndirectedRandomGraph.hpp"
#include "../../../../../src/graph/random/undirected_graph/UndirectedRandomGraph.cpp"
#include "../../../../../src/graph/random/undirected_graph/actions_impl/PullEdgeActionURG.cpp"
#include "../../../../../src/graph/random/undirected_graph/actions_impl/RemoveRandomEdgeActionURG.cpp"
#include "../../../../../src/graph/random/undirected_graph/actions_impl/RemoveTrivialChainActionURG.cpp"
#include "../../../../../src/utils/parse/TGFParser.hpp"

using namespace grph::graph;

static random::EdgeProbability matrixAfterPull_4_5[25] = {
        0, 0.5, 0.5, 0, 0,
        0.5, 0, 0, 0.5, 0.5,
        0.5, 0, 0, 0, 0.75,
        0, 0.5, 0, 0, 0.75,
        0, 0.5, 0.75, 0.75, 0
};

static random::EdgeProbability matrixAfterPull_0_4[25] = {
        0, 0.75, 0.75, 0.5, 0.5,
        0.75, 0, 0, 0.5, 0,
        0.75, 0, 0, 0, 0.5,
        0.5, 0.5, 0, 0, 0.5,
        0.5, 0, 0.5, 0.5, 0
};

class PullEdgeFixture : public ::testing::Test {
public:
    PullEdgeFixture() :
            _matrix(grph::utils::TGFParser::parseFile(
                            "../../Google_tests/integration_tests/test_graphs/example_1.tgf"
                    )
            ) {}

protected:
    void SetUp() override {
        _graph = random::UndirectedRandomGraph(_matrix->copy());
    }

    random::UndirectedRandomGraph _graph;
    const std::shared_ptr<AdjacencyMatrix<random::EdgeProbability>> _matrix;
};

TEST_F(PullEdgeFixture, PullEdge_expectThrowError) {
    ASSERT_THROW(_graph.pullEdge(5, 4),
                 std::runtime_error);
}

TEST_F(PullEdgeFixture, PullEdge_4_5) {
    const auto initialMatrix = _graph.getAdjacencyMatrix()->copy();
    const auto initialDegreesVector = _graph.getDegreesVector();

    const auto &action = _graph.pullEdge(4, 5);
    const auto expectedPulledMatrix = AdjacencyMatrix<random::EdgeProbability>(5, matrixAfterPull_4_5);
    const auto expectedDegreesVector = DegreesVector::make(expectedPulledMatrix);


    ASSERT_EQ(5, _graph.getNumVertexes());
    ASSERT_TRUE(_graph.getAdjacencyMatrix()->isEqual(expectedPulledMatrix));
    ASSERT_TRUE(_graph.getDegreesVector()->isEqual(*expectedDegreesVector));

    action->rollback();
    ASSERT_EQ(6, _graph.getNumVertexes());
    ASSERT_TRUE(_graph.getAdjacencyMatrix()->isEqual(*initialMatrix));
    ASSERT_TRUE(_graph.getDegreesVector()->isEqual(*initialDegreesVector));
}

TEST_F(PullEdgeFixture, PullEdge_0_4) {
    const auto initialMatrix = _graph.getAdjacencyMatrix()->copy();
    const auto initialDegreesVector = _graph.getDegreesVector();

    const auto &action = _graph.pullEdge(0, 4);
    const auto expectedPulledMatrix = AdjacencyMatrix<random::EdgeProbability>(5, matrixAfterPull_0_4);
    const auto expectedDegreesVector = DegreesVector::make(expectedPulledMatrix);

    ASSERT_EQ(5, _graph.getNumVertexes());
    ASSERT_TRUE(_graph.getAdjacencyMatrix()->isEqual(expectedPulledMatrix));
    ASSERT_TRUE(_graph.getDegreesVector()->isEqual(*expectedDegreesVector));

    action->rollback();
    ASSERT_EQ(6, _graph.getNumVertexes());
    ASSERT_TRUE(_graph.getAdjacencyMatrix()->isEqual(*initialMatrix));
    ASSERT_TRUE(_graph.getDegreesVector()->isEqual(*initialDegreesVector));
}











