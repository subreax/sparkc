#include <catch2/catch_test_macros.hpp>
#include "sparkc/common/cfg/Graph.h"

TEST_CASE("Graph test", "[cfg]") {
    Graph<int> graph(5);
    for (int i = 0; i < 5; i++) {
        graph[i] = i;
    }

    graph.connect(0, 1);
    graph.connect(1, 2);
    graph.connect(1, 3);
    graph.connect(2, 4);
    graph.connect(3, 4);

    REQUIRE(graph.isConnected(0, 1));
    REQUIRE(graph.isConnected(1, 2));
    REQUIRE(graph.isConnected(1, 3));
    REQUIRE(graph.isConnected(2, 4));
    REQUIRE(graph.isConnected(3, 4));

    REQUIRE_THROWS(graph.isConnected(3, 5));
    REQUIRE_THROWS(graph.getNode(-1));
    REQUIRE_THROWS(graph.getNode(5));
    REQUIRE_THROWS(graph.setNode(-1, 0));
    REQUIRE_THROWS(graph.setNode(5, 0));
    REQUIRE_THROWS(graph[-1]);
    REQUIRE_THROWS(graph[5]);

    auto successors = graph.successors(1);
    REQUIRE(successors.hasNext());
    REQUIRE(successors.nextNode() == 2);
    REQUIRE(successors.hasNext());
    REQUIRE(successors.nextNode() == 3);
    REQUIRE_FALSE(successors.hasNext());
    REQUIRE_THROWS(successors.next());

    auto predecessors = graph.predecessors(4);
    REQUIRE(predecessors.hasNext());
    REQUIRE(predecessors.nextNode() == 2);
    REQUIRE(predecessors.hasNext());
    REQUIRE(predecessors.nextNode() == 3);
    REQUIRE_FALSE(predecessors.hasNext());
    REQUIRE_THROWS(predecessors.next());
}