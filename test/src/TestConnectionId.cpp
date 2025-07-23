#include <QtNodes/Definitions>
#include <QtNodes/ConnectionIdUtils>

#include <catch2/catch.hpp>

using QtNodes::ConnectionId;
using QtNodes::NodeId;
using QtNodes::PortIndex;
using QtNodes::invertConnection;

TEST_CASE("ConnectionId basic functionality", "[core]")
{
    NodeId node1 = 1;
    NodeId node2 = 2;
    PortIndex port1 = 0;
    PortIndex port2 = 1;

    SECTION("ConnectionId creation and equality")
    {
        ConnectionId conn1{node1, port1, node2, port2};
        ConnectionId conn2{node1, port1, node2, port2};
        ConnectionId conn3{node2, port1, node1, port2};

        CHECK(conn1 == conn2);
        CHECK(conn1 != conn3);
        CHECK(conn2 != conn3);

        // Test individual fields
        CHECK(conn1.outNodeId == node1);
        CHECK(conn1.outPortIndex == port1);
        CHECK(conn1.inNodeId == node2);
        CHECK(conn1.inPortIndex == port2);
    }

    SECTION("ConnectionId inversion")
    {
        ConnectionId original{node1, port1, node2, port2};
        ConnectionId copy = original;

        invertConnection(copy);

        CHECK(copy.outNodeId == original.inNodeId);
        CHECK(copy.outPortIndex == original.inPortIndex);
        CHECK(copy.inNodeId == original.outNodeId);
        CHECK(copy.inPortIndex == original.outPortIndex);

        // Inverting again should restore original
        invertConnection(copy);
        CHECK(copy == original);
    }
}

TEST_CASE("ConnectionId edge cases", "[core]")
{
    SECTION("Same node, different ports")
    {
        ConnectionId conn{1, 0, 1, 1};
        CHECK(conn.outNodeId == conn.inNodeId);
        CHECK(conn.outPortIndex != conn.inPortIndex);
    }

    SECTION("Different nodes, same ports")
    {
        ConnectionId conn{1, 0, 2, 0};
        CHECK(conn.outNodeId != conn.inNodeId);
        CHECK(conn.outPortIndex == conn.inPortIndex);
    }

    SECTION("Maximum values")
    {
        ConnectionId conn{
            std::numeric_limits<NodeId>::max(),
            std::numeric_limits<PortIndex>::max(),
            std::numeric_limits<NodeId>::max() - 1,
            std::numeric_limits<PortIndex>::max() - 1
        };

        CHECK(conn.outNodeId == std::numeric_limits<NodeId>::max());
        CHECK(conn.outPortIndex == std::numeric_limits<PortIndex>::max());
        CHECK(conn.inNodeId == std::numeric_limits<NodeId>::max() - 1);
        CHECK(conn.inPortIndex == std::numeric_limits<PortIndex>::max() - 1);
    }
}
