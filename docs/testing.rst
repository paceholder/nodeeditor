Testing
=======

The QtNodes library includes a comprehensive unit test suite built with Catch2.

Test Coverage
-------------

The test suite covers the following areas:

**Core Functionality (20 test cases, 216 assertions)**
  - AbstractGraphModel operations (node CRUD, connections)
  - AbstractGraphModel signal emissions (comprehensive signal testing)
  - DataFlowGraphModel functionality
  - NodeDelegateModelRegistry operations
  - Serialization (save/load JSON)
  - Undo system integration
  - Graphics scene management
  - Connection ID utilities

**Data Flow Testing (5 test cases, 46 assertions)**
  - Real data transfer between connected nodes using NodeDelegateModel pattern
  - Programmatic and interactive connection creation with data propagation
  - Multiple output connections (one-to-many data distribution)
  - Connection lifecycle testing (creation, data flow, disconnection)
  - Custom node delegate models with embedded widgets and signal handling

**Visual UI Interaction Testing (3 test cases, 5 assertions)**
  - Node movement and visual positioning
  - Connection creation by dragging between ports
  - Connection disconnection by dragging and deletion
  - Zoom and pan operations
  - Keyboard shortcuts (delete, undo)
  - Context menu interactions
  - Stress testing with rapid mouse movements and UI load

**Total: 28 test cases, 267 assertions**

**Infrastructure**
  - Complete AbstractGraphModel test implementation
  - Qt application setup utilities
  - Node delegate stubs for testing
  - UITestHelper namespace for UI interaction simulation
  - Virtual display testing with proper window exposure
  - Clean build system without internal dependencies

Running Tests
-------------

From the build directory:

.. code-block:: bash

    # Build tests
    make test_nodes
    
    # Run all tests
    ./bin/test_nodes
    
    # Run specific test categories
    ./bin/test_nodes "[core]"        # Core functionality
    ./bin/test_nodes "[signals]"     # Signal emission tests
    ./bin/test_nodes "[dataflow]"    # DataFlowGraphModel tests
    ./bin/test_nodes "[registry]"    # Registry tests
    ./bin/test_nodes "[serialization]"  # Save/load tests
    ./bin/test_nodes "[undo]"        # Undo system tests
    ./bin/test_nodes "[graphics]"    # Graphics tests
    ./bin/test_nodes "[ui]"          # UI interaction tests
    ./bin/test_nodes "[visual]"      # Visual UI tests
    ./bin/test_nodes "[stress]"      # Stress tests

Test Structure
--------------

Tests are organized in ``test/`` directory:

.. code-block::

    test/
    ├── CMakeLists.txt              # Build configuration
    ├── include/
    │   ├── ApplicationSetup.hpp    # Qt app setup
    │   ├── TestGraphModel.hpp      # Test graph model
    │   └── StubNodeDataModel.hpp   # Node delegate stub
    └── src/
        ├── TestAbstractGraphModel.cpp
        ├── TestAbstractGraphModelSignals.cpp
        ├── TestDataFlowGraphModel.cpp
        ├── TestNodeDelegateModelRegistry.cpp
        ├── TestBasicGraphicsScene.cpp
        ├── TestConnectionId.cpp
        ├── TestSerialization.cpp
        ├── TestUIInteraction.cpp
        └── TestUndoCommands.cpp

Test Categories
---------------

**Core Tests ([core])**
  - AbstractGraphModel basic functionality
  - Connection management
  - Node deletion with connections
  - ConnectionId utilities and edge cases

**Signal Tests ([signals])**
  - Signal emission verification for all AbstractGraphModel signals
  - Signal spy validation and argument type checking
  - Signal consistency with model state
  - Edge case signal behavior (invalid operations)
  - Complex operation signal ordering

**DataFlow Tests ([dataflow])**
  - DataFlowGraphModel operations
  - Node creation and validation
  - Connection possibility checks
  - Port bounds validation

**Registry Tests ([registry])**
  - NodeDelegateModelRegistry functionality
  - Model registration with categories
  - Lambda factory registration
  - Category associations

**Serialization Tests ([serialization])**
  - DataFlowGraphModel serialization
  - Individual node serialization
  - JSON save/load operations

**Undo System Tests ([undo])**
  - QUndoStack integration with BasicGraphicsScene
  - Manual undo/redo simulation
  - State tracking

**Graphics Tests ([graphics])**
  - BasicGraphicsScene functionality
  - Graphics undo/redo support
  - Scene management

**UI Interaction Tests ([ui], [visual], [stress])**
  - Node movement and visual positioning using UITestHelper namespace
  - Connection creation by dragging between ports
  - Connection disconnection by dragging from ports and deletion of selected connections
  - Zoom and pan operations with mouse wheel and drag
  - Keyboard shortcuts (delete key, Ctrl+Z undo)
  - Context menu interactions (right-click)
  - Stress testing with rapid mouse movements and memory load
  - Virtual display testing with proper window exposure handling

Key Features
------------

**Segfault Resolution**: Fixed critical infinite recursion in signal emission
that was causing stack overflow crashes during graphics system updates.

**API Modernization**: Updated from v2.x to v3.x Model-View architecture
with proper AbstractGraphModel implementation following QtNodes best practices.

**Clean Dependencies**: Removed internal header dependencies, using only
public APIs for better stability and maintainability.

**Signal Emission Testing**: Comprehensive verification of all AbstractGraphModel 
signals using QSignalSpy, ensuring proper signal emission for all operations 
including node creation/deletion, connection creation/deletion, and node updates.

**Comprehensive Coverage**: Tests all major functionality including node
management, connections, serialization, undo system, and graphics integration.

**Port Bounds Validation**: Added proper validation in ``connectionPossible()``
to ensure port indices are within valid ranges.

Building Tests
--------------

Tests are built automatically when ``BUILD_TESTING`` is enabled (default).

**Prerequisites:**
  - Catch2 testing framework
  - Qt6 (or Qt5 with appropriate configuration)
  - CMake 3.8+

**Configuration:**

.. code-block:: bash

    # Enable testing (default)
    cmake .. -DBUILD_TESTING=ON
    
    # Disable testing
    cmake .. -DBUILD_TESTING=OFF

**Build:**

.. code-block:: bash

    # Build library and tests
    make
    
    # Build only tests
    make test_nodes

Test Implementation Details
---------------------------

**TestGraphModel**: A complete implementation of ``AbstractGraphModel`` that provides:
  - Full node and connection management
  - Proper signal emission patterns
  - Serialization support
  - Integration with graphics systems

**UITestHelper**: A namespace providing utility functions for UI interaction testing:
  - ``simulateMousePress/Move/Release()`` - Low-level mouse event simulation
  - ``simulateMouseDrag()`` - High-level drag operation simulation
  - ``waitForUI(ms = 10)`` - Optimized UI event processing with 10ms default timing
  - Proper Qt event system integration for realistic UI testing

**Signal Safety**: The test model implements signal emission patterns that prevent
infinite recursion between the model and graphics system, following the approach
used in ``examples/simple_graph_model``.

**Mock Objects**: Comprehensive stub implementations for testing without external
dependencies, including ``StubNodeDataModel`` for node delegate testing.

Troubleshooting
---------------

**Common Issues:**

* **Catch2 not found**: Install Catch2 or disable testing with ``-DBUILD_TESTING=OFF``
* **Qt version conflicts**: Ensure consistent Qt5/Qt6 usage throughout build
* **Missing test binary**: Check that ``BUILD_TESTING`` is enabled in CMake configuration

**Performance Notes:**
  - Tests include Qt application setup overhead
  - Graphics tests may show Qt warnings about runtime directories (these are harmless)
  - UI tests use optimized 10ms timing for consistent performance
  - Full test suite typically completes in under 10 seconds
