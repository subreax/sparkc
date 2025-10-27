#include <catch2/catch_test_macros.hpp>
#include "spark/common/alloc/LinearAllocator.h"

TEST_CASE("Linear allocator test", "[linear-allocator]") {
    LinearAllocator allocator("test", 16, true);
    REQUIRE(allocator.getUsedSize() == 0);
    REQUIRE(allocator.getFreeSize() == 16);
    REQUIRE(allocator.getCapacity() == 16);

    allocator.allocate(4);
    REQUIRE(allocator.getUsedSize() == 4);
    REQUIRE(allocator.getFreeSize() == 12);
    
    allocator.allocate(11);
    REQUIRE(allocator.getUsedSize() == 15);
    REQUIRE(allocator.getFreeSize() == 1);

    REQUIRE_THROWS(allocator.allocate(2));

    allocator.allocate(1);
    REQUIRE(allocator.getUsedSize() == 16);
    REQUIRE(allocator.getFreeSize() == 0);

    REQUIRE_THROWS(allocator.allocate(1));

    allocator.free(9);
    REQUIRE(allocator.getFreeSize() == 9);
    REQUIRE(allocator.getUsedSize() == 7);

    REQUIRE_THROWS(allocator.free(8));
    
    allocator.free(7);
    REQUIRE(allocator.getFreeSize() == 16);

    allocator.allocate(16);
    REQUIRE(allocator.getFreeSize() == 0);

    allocator.free(16);
    REQUIRE(allocator.getFreeSize() == 16);

    REQUIRE_THROWS(allocator.allocate(17));
}