Installation
============

Requirements
------------

- **Qt** 5.15+ or Qt 6.x
- **Qt SVG module** (for processing status icons)
- **CMake** 3.11+
- **C++ Compiler** with C++17 support (GCC 7+, Clang 5+, MSVC 2019+)
- **Catch2** (optional, for running tests)

On Ubuntu/Debian, install Qt SVG with:

.. code-block:: bash

   # For Qt6
   sudo apt install libqt6svg6

   # For Qt5
   sudo apt install libqt5svg5

Building from Source
--------------------

**1. Clone the repository**

.. code-block:: bash

   git clone https://github.com/paceholder/nodeeditor.git
   cd nodeeditor

**2. Create build directory and configure**

.. code-block:: bash

   mkdir build && cd build
   cmake ..

**3. Build**

.. code-block:: bash

   cmake --build .

CMake Options
-------------

.. list-table::
   :widths: 30 15 55
   :header-rows: 1

   * - Option
     - Default
     - Description
   * - ``USE_QT6``
     - ``ON``
     - Build with Qt6. Set to ``OFF`` for Qt5.
   * - ``BUILD_SHARED_LIBS``
     - ``ON``
     - Build as shared library. Set to ``OFF`` for static.
   * - ``BUILD_TESTING``
     - ``ON``
     - Build unit tests. Requires Catch2.
   * - ``BUILD_EXAMPLES``
     - ``ON``
     - Build example applications.

**Examples:**

.. code-block:: bash

   # Build with Qt5
   cmake .. -DUSE_QT6=OFF

   # Build static library
   cmake .. -DBUILD_SHARED_LIBS=OFF

   # Skip tests (if Catch2 not installed)
   cmake .. -DBUILD_TESTING=OFF

Using vcpkg
-----------

If you use vcpkg for dependency management:

.. code-block:: bash

   cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake

Integration into Your Project
-----------------------------

**Option 1: CMake subdirectory**

Add QtNodes as a subdirectory in your project:

.. code-block:: cmake

   add_subdirectory(external/nodeeditor)
   target_link_libraries(your_app QtNodes::QtNodes)

**Option 2: Installed library**

After running ``cmake --install .``:

.. code-block:: cmake

   find_package(QtNodes REQUIRED)
   target_link_libraries(your_app QtNodes::QtNodes)

Verifying Installation
----------------------

Run the calculator example to verify everything works:

.. code-block:: bash

   ./bin/calculator

.. image:: /_static/screenshots/calc-start.png
   :alt: Calculator example running successfully
   :width: 400px

Next Steps
----------

Continue to :doc:`quickstart` to build your first node graph.
