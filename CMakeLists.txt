cmake_minimum_required(VERSION 3.23)
project(grph)

set(CMAKE_CXX_STANDARD 20)

# Project specification
add_executable(${PROJECT_NAME}
        main.cpp
        src/graph/common/AdjacencyMatrix.hpp
        src/exceptions/graph/exceptions.h
        src/graph/common/types.hpp
        src/graph/common/AdjacencyList.hpp
        src/graph/common/EdgesList.hpp
        src/utils/parse/GMLParser.hpp
        src/utils/parse/TGFParser.hpp
        src/graph/common/DegreesVector.hpp
        src/graph/random/undirected_graph/UndirectedRandomGraph.hpp
        src/graph/random/undirected_graph/UndirectedRandomGraph.hpp src/graph/random/IRandomGraph.hpp src/graph/criteria/ATR.hpp src/graph/random/actions/Actions.hpp src/graph/random/Chain.hpp src/graph/random/undirected_graph/UndirectedRandomGraph.cpp src/graph/random/undirected_graph/actions_impl/RemoveRandomEdgeActionURG.cpp src/graph/random/undirected_graph/actions_impl/PullEdgeActionURG.cpp src/graph/random/undirected_graph/actions_impl/RemoveTrivialChainActionURG.cpp)

add_subdirectory(Google_tests)

## Add nlohmann_json support to project
#target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# Use the global target
#target_link_libraries(${PROJECT_NAME} gtest::gtest)
