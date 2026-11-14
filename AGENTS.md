# AGENTS.md

This file provides guidance to coding agents when working with code in this repository.

## Writing style

Write short, simple technical English. One idea per sentence. No filler or marketing words.
This applies to answers, code comments, commit messages and docs.

## Project overview

QtNodes is a Qt-based C++ library for building node graph editors. It separates graph data
(`AbstractGraphModel`) from visualization (`BasicGraphicsScene`/`GraphicsView`). It supports
generic graph editing and a data-flow paradigm with automatic propagation. It works with Qt5
(5.15+) and Qt6 (6.3+).

## Build and test

```bash
cmake -S . -Bbuild -DCMAKE_BUILD_TYPE=Release -DUSE_QT6=ON   # Qt6 (default); -DUSE_QT6=OFF for Qt5
cmake --build build
cmake --build build --target test_nodes                      # tests only
./build/bin/test_nodes                                       # all tests
./build/bin/test_nodes "[core]"                              # by Catch2 tag, e.g. [core] [graphics]
cd build && ctest --output-on-failure                        # via ctest (Linux headless: xvfb-run -a ctest ...)
```

CMake options: `BUILD_TESTING`, `BUILD_EXAMPLES`, `BUILD_DOCS`, `BUILD_SHARED_LIBS`, `USE_QT6`.

Tests use Catch2 v3 (downloaded if not found) in one executable, `test_nodes`. Helpers are in
`test/include/`: `TestGraphModel`, `UITestHelper`, `TestDataFlowNodes`, `ApplicationSetup`.

CI (`.github/workflows/cmake_build.yml`): Linux/gcc/Qt5, macOS/clang/Qt6, Windows/MSVC/Qt5 and
Qt6. Apple Clang builds with `-Werror`.

## Architecture

Model-View. The model is the single source of truth. The scene and view only observe model
signals.

- **Model state changes go through the model.** Change the graph with model calls or undo
  commands (`UndoCommands.hpp`). The graphics layer follows the model signals.
- `AbstractGraphModel`: pure virtual graph. Node and connection CRUD, data queries by
  `NodeRole`/`PortRole`. It can run headless, without a scene.
- `DataFlowGraphModel`: concrete model for the data-flow paradigm. It owns one
  `NodeDelegateModel` per node and propagates data through connections. Loops are disabled.
- `NodeDelegateModel`: node logic for `DataFlowGraphModel`. It defines ports, data types and
  embedded widgets, receives data in `setInData()` and signals `dataUpdated(PortIndex)`.
  Subclasses are registered in `NodeDelegateModelRegistry`.
- Graphics: `BasicGraphicsScene` (owns the `QUndoStack`), `DataFlowGraphicsScene` (JSON
  save/load, context menu), `GraphicsView` (zoom, pan, copy/paste, shortcuts).
- Customization by inheritance: `AbstractNodeGeometry`, `AbstractNodePainter`,
  `AbstractConnectionPainter`.
- Styling: JSON styles via `NodeStyle`, `ConnectionStyle`, `GraphicsViewStyle`, managed by the
  `StyleCollection` singleton.
- Serialization: the `Serializable` interface, `save()`/`load()` with `QJsonObject`.

Core types: `NodeId` and `PortIndex` are `unsigned int` (`InvalidNodeId` is the maximum
value). `PortType` is `In`, `Out` or `None`. `ConnectionId` is
`{outNodeId, outPortIndex, inNodeId, inPortIndex}`.

Directories: `include/QtNodes/internal/` (public headers), `src/`, `test/src/` and
`test/include/`, `examples/`.

## Code style

Enforced by `.clang-format`.

- 4 spaces, no tabs, 100 columns.
- Brace on a new line after class, struct and function. Same line after control statements.
- Pointer alignment right (`int *p`). No namespace indentation.
- `#pragma once` in all headers. Everything is in `namespace QtNodes`.
- Members are prefixed with `_` (`_value`).
- `QT_NO_KEYWORDS` is defined. Use `Q_EMIT`, `Q_SIGNALS`, `Q_SLOTS`, `Q_FOREACH`.

## C++ standard

The sources use C++17, and Qt 6 requires it. The standard is declared only in the
`QT_NODES_DEVELOPER_DEFAULTS` block of `CMakeLists.txt`. Do not set it for consumers. Projects
that include QtNodes choose their own standard.

## Tests and docs

- Test titles state the rule they prove in words.
- A bug fix comes with a regression test that fails without the fix.
- Update `docs/` and `README.rst` when a change alters what they describe.
- Do not silently change recorded behavior or settle an open design decision. Ask.

## Commits and releases

- One logical change per commit.
- No `Co-Authored-By` lines or other attribution trailers in commit messages.
- Do not push, tag or delete remote branches unless asked.
- Versions follow semver. A tag on a later commit must have a higher version than the tags
  before it. Never move or delete a published tag.
- Branches: `master` (latest dev state) and `v3`. Older versions are tags (`1.0`, `2.*`).
