Installation
============

Requirements
------------

- **Qt** 5.15+ or Qt 6.x
- **Qt SVG module** (for processing status icons)
- **CMake** 3.13+
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
     - Build as shared library. Set to ``OFF`` for static. See
       `Shared and static library`_.
   * - ``BUILD_TESTING``
     - ``ON`` or ``OFF``
     - Build unit tests. Requires Catch2. ``ON`` when QtNodes is the top-level project, ``OFF``
       when it is added with ``add_subdirectory``.
   * - ``BUILD_EXAMPLES``
     - ``ON`` or ``OFF``
     - Build example applications. Same default as ``BUILD_TESTING``.

**Examples:**

.. code-block:: bash

   # Build with Qt5
   cmake .. -DUSE_QT6=OFF

   # Build static library
   cmake .. -DBUILD_SHARED_LIBS=OFF

   # Skip tests (if Catch2 not installed)
   cmake .. -DBUILD_TESTING=OFF

Shared and static library
-------------------------

Choose the library type with ``BUILD_SHARED_LIBS``. The default is ``ON`` (shared).

.. code-block:: bash

   cmake .. -DBUILD_SHARED_LIBS=OFF

- QtNodes defines ``NODE_EDITOR_SHARED`` or ``NODE_EDITOR_STATIC`` for you. The definition is
  public, so your target gets it when you link ``QtNodes::QtNodes``. This works with
  ``add_subdirectory`` and with ``find_package``. In an installed package the choice is fixed
  by the build that you installed.
- Do not define ``NODE_EDITOR_SHARED``, ``NODE_EDITOR_STATIC`` or ``NODE_EDITOR_EXPORTS``
  yourself. If both ``SHARED`` and ``STATIC`` are defined, the compiler stops with
  ``Cannot link against shared and static simultaneously.``
- A static QtNodes does not make Qt static. QtNodes links Qt ``Core``, ``Widgets``, ``Gui`` and
  ``OpenGL`` publicly, so your application still needs Qt.
- With ``add_subdirectory`` or ``FetchContent``, set ``BUILD_SHARED_LIBS`` before QtNodes is
  added. A plain ``set()`` works. The cache form and ``-DBUILD_SHARED_LIBS=OFF`` work too.

.. code-block:: cmake

   set(BUILD_SHARED_LIBS OFF)
   add_subdirectory(external/nodeeditor)

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

If you installed to a custom prefix, pass it with ``-DCMAKE_PREFIX_PATH=<prefix>``.

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
